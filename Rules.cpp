#include "Rules.h"
#include <queue>
#include <set>
std::map <usi, usi> Occured_States;

void LexemeRules::Add_Automatas(unsigned int a) {
	this->Int_Automatas += a;
}



void LexemeRules::AddRules(std::string RULE, std::string Terminal) {
	RegParser reg;
	LexemeLinkedList* ns = reg.ParseReg(RULE);

	this->Add_Automatas(ns->GetLength()*3 + 1);

	if (this->LexemeHead == nullptr) {
		this->LexemeHead = new Lexemes;
		this->LexemeHead->Terminal = Terminal;
		this->LastLexeme = this->LexemeHead;
		this->LexemeHead->ConstructCurrentLexeme(ns);
	}
	else {
		Lexemes* NewLexeme = new Lexemes;
		NewLexeme->Terminal = Terminal;
		NewLexeme->ConstructCurrentLexeme(ns);
		this->LastLexeme->SetNextLexeme(NewLexeme);
		this->LastLexeme = NewLexeme;
	}
}

unsigned int LexemeRules::GetEstimatedAutomatas()
{
	return this->Int_Automatas;
}

BoundedAutomatas LexemeRules::Construct_DFA()
{
	BoundedAutomatas atm;
	atm.Allocate(this->GetEstimatedAutomatas());
	Lexemes* CurLex = this->LexemeHead;
	CurLex->Get_Current_LinkedList()->ConstructAutomations(&atm, CurLex->Terminal);
	
	atm = NFAtoDFA(&atm);
	//first construstion

	CurLex = CurLex->Get_Next();
	usi Current_Automata = 0;
	usi p = 0;
	while (CurLex != nullptr) {
		BoundedAutomatas atmtemp;
		atmtemp.Allocate(CurLex->Get_Current_LinkedList()->GetLength()* 3 + 1);
		CurLex->Get_Current_LinkedList()->ConstructAutomations(&atmtemp, CurLex->Terminal);
		atmtemp = NFAtoDFA(&atmtemp);
		p = AppendTwoAutomatas(&atm, &atmtemp,p);
		CurLex = CurLex->Get_Next();
	}
	return atm;
}

void LexemeRules::SetLexemeHead(Lexemes* Hd) {
	if(Hd != nullptr)
		this->LexemeHead = Hd;
}

void Lexemes::SetNextLexeme(Lexemes* p)
{
	if (p != nullptr)
		this->Next_Lexeme = p;
}

LexemeLinkedList* Lexemes::Get_Current_LinkedList()
{
	return this->Current_Lexeme;
}

Lexemes* Lexemes::Get_Next()
{
	return this->Next_Lexeme;
}

void Lexemes::ConstructCurrentLexeme(LexemeLinkedList* ll)
{
	this->Current_Lexeme = ll;
}
void Recur_Append_ATMS(BoundedAutomatas* atm1, usi atm1pos, BoundedAutomatas* atm2, usi atm2pos, bool Is_OverLapped, usi * max_state_of_atm1) {
	bool OverLapped = Is_OverLapped;
	Automata* Automata2 = &atm2->Automatas[atm2pos];
	Automata* Automata1 = &atm1->Automatas[atm1pos];
	usi Trannum = Automata2->Get_Total_Transitions();
	usi Trannum1 = Automata1->Get_Total_Transitions();
	usi targetpos;
	if (!Is_OverLapped) { //not overlapped , simply add it
		for (unsigned int i = 0; i < Trannum; i++) {
			if (atm1->GetSize() < *max_state_of_atm1) {
				std::cout << "Warning:BATM space exceed , abandon atm append";
				return ;
			}
			auto pt = Occured_States.find(Automata2->GetTrans(i).NextState);
			if (pt == Occured_States.end()) {
				*max_state_of_atm1 += 1;
				Occured_States.insert(std::make_pair(Automata2->GetTrans(i).NextState, *max_state_of_atm1));
				targetpos = *max_state_of_atm1;
				usi temp = *max_state_of_atm1;
				atm1->Automatas[atm1pos].SetTrans(targetpos, Automata2->GetTrans(i).TransCond);
				usi NxtState = Automata2->GetTrans(i).NextState;
				if (atm2->Automatas[NxtState].Get_Ter()) {//is ter
					atm1->Automatas[*max_state_of_atm1].SetTer(true, atm2->Automatas[NxtState].Get_Ter_Label());
				}
				Recur_Append_ATMS(atm1, temp, atm2, Automata2->GetTrans(i).NextState, false, max_state_of_atm1);
			}
			else {
				targetpos = pt->second;
				atm1->Automatas[atm1pos].SetTrans(targetpos, Automata2->GetTrans(i).TransCond);
				usi NxtState = Automata2->GetTrans(i).NextState;
				if (atm2->Automatas[NxtState].Get_Ter()) {//is ter
					atm1->Automatas[*max_state_of_atm1].SetTer(true, atm2->Automatas[NxtState].Get_Ter_Label());
				}
			}
			

		}
	}
	else {
		bool matchedfrag = false;
		usi matched_pos;
		for (unsigned int i = 0; i < Trannum; i++) {


			std::string str_trans = Automata2->GetTrans(i).TransCond;
			for (unsigned int j = 0; j < Trannum1; j++) {
				std::string str_trans1 = Automata1->GetTrans(j).TransCond;
				if (str_trans.compare(str_trans1) == 0) {
					matchedfrag = true;
					matched_pos = j;
					break;
				}
			}
			if (!matchedfrag) {//not overlapped
				if (atm1->GetSize() < *max_state_of_atm1) {
					std::cout << "Warning:BATM space exceed , abandon atm append";
					return;
				}
				auto pt = Occured_States.find(Automata2->GetTrans(i).NextState);
				if (pt == Occured_States.end()) {
					*max_state_of_atm1 += 1;
					Occured_States.insert(std::make_pair(Automata2->GetTrans(i).NextState, *max_state_of_atm1));
					targetpos = *max_state_of_atm1;
					usi temp = *max_state_of_atm1;
					atm1->Automatas[atm1pos].SetTrans(targetpos, Automata2->GetTrans(i).TransCond);
					usi NxtState = Automata2->GetTrans(i).NextState;
					if (atm2->Automatas[NxtState].Get_Ter()) {//is ter
						atm1->Automatas[*max_state_of_atm1].SetTer(true, atm2->Automatas[NxtState].Get_Ter_Label());
					}
					Recur_Append_ATMS(atm1, temp, atm2, Automata2->GetTrans(i).NextState, false, max_state_of_atm1);
				}
				else {
					targetpos = pt->second;
					atm1->Automatas[atm1pos].SetTrans(targetpos, Automata2->GetTrans(i).TransCond);
					usi NxtState = Automata2->GetTrans(i).NextState;
					if (atm2->Automatas[NxtState].Get_Ter()) {//is ter
						atm1->Automatas[*max_state_of_atm1].SetTer(true, atm2->Automatas[NxtState].Get_Ter_Label());
					}
				}
			}
			else {//have overlapped 
				usi NxtState = Automata2->GetTrans(i).NextState;
				if (atm2->Automatas[NxtState].Get_Ter()) {//is ter
					atm1->Automatas[Automata1->GetTrans(matched_pos).NextState].SetTer(true, atm2->Automatas[NxtState].Get_Ter_Label());
				}
				Occured_States.insert(std::make_pair(atm2pos, *max_state_of_atm1));
				auto pt = Occured_States.find(Automata2->GetTrans(i).NextState);
				if (pt == Occured_States.end()) {
					Occured_States.insert(std::make_pair(Automata2->GetTrans(i).NextState, *max_state_of_atm1));
					Recur_Append_ATMS(atm1, Automata1->GetTrans(matched_pos).NextState, atm2, Automata2->GetTrans(i).NextState, true, max_state_of_atm1);
				}
			}

		}
	}
}

usi AppendTwoAutomatas(BoundedAutomatas* atm1, BoundedAutomatas* atm2, usi max_state = 0)
{
	//traverse to get the maximun num of atm1's state
	std::set <unsigned int> FirstA;

	usi max_atm1_state = max_state;
	if (max_atm1_state == 0) {
		std::queue <usi> states;
		states.push(0);
		while (!states.empty()) {
			usi p = states.front();
			states.pop();
			Automata* CurAutomata = &atm1->Automatas[p];
			usi Trannum = CurAutomata->Get_Total_Transitions();
			for (unsigned int i = 0; i < Trannum; i++) {
				usi nxtstate = CurAutomata->GetTrans(i).NextState;
				auto fd = FirstA.find(nxtstate);
				if (fd == FirstA.end()) {
					states.push(nxtstate);
					FirstA.insert(nxtstate);
				}
				max_atm1_state = std::max(max_atm1_state, nxtstate);

			}
		}
	}
	//can be handle recursively
	Occured_States.clear();
	Recur_Append_ATMS(atm1, 0, atm2, 0, true,&max_atm1_state);
	return max_atm1_state;
}
