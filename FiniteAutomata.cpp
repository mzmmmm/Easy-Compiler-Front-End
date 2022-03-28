#include "FiniteAutomata.h"
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <queue>
#include <set>

std::string a_z = "abcdefghijklmnopqrstuvwxyz";
std::string A_Z = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
std::string zero_nine = "0123456789";
void Automata::Allocate(unsigned int size) { // can be dynamically allocated
	if (this->size > 0) {
		CSPair* tem = new CSPair[this->size];
		unsigned int i = 0;
		while (i < this->size) {
			tem[i] = this->transitions[i];
			i++;
		}
		delete[] this->transitions;
		this->transitions = new CSPair[this->size + size];
		i = 0;
		while (i < this->size) {
			this->transitions[i] = tem[i];
			i++;

		}
		delete[] tem;
		this->size = this->size + size;
	}
	else {
		this->transitions = new CSPair[size];
		this->size = size;
	}
}
int Automata::SetTrans(unsigned int NS, std::string COND) {
	if (this->transpos >= this->size) {
		std::cout << "Automata Size Out of memory.Automated Reallocated 10 trans"<<std::endl;
		this->Allocate(10);
	}
	this->transitions[this->transpos] = CSPair{ NS,COND };
	this->transpos++;
	return 1;
}
void BoundedAutomatas::Allocate(unsigned int size) { // can be dynamically allocated
	if (this->size > 0) {
		Automata* tem = new Automata[this->size];
		unsigned int i = 0;
		while (i < this->size) {
			tem[i] = this->Automatas[i];
			i++;
		}
		delete[] this->Automatas;
		this->Automatas = new Automata[this->size + size];
		 i = 0;
		while (i < this->size) {
			this->Automatas[i] = tem[i];
			i++;
		}
		delete[] tem;
		this->size = this->size + size;
	}
	else {
		this->size = size;
		this->Automatas = new Automata[size];
	}
}
bool Automata::Get_Ter() {
	return this->is_ter;
}
std::string Automata::Get_Ter_Label() {
	return this->terlabel;
}
void Automata::SetTer(bool p,std::string s) {
	this->is_ter = p;
	this->terlabel = s;
}
CSPair Automata::GetTrans(unsigned int num) {
	return this->transitions[num];
}
unsigned Automata::GetSize() {
	return this->size;
}
unsigned int Automata::Get_Total_Transitions()
{
	return this->transpos;
}
void ConstructFromSingleLexeme(unsigned int * curnum, unsigned int* stnum, unsigned int * ednum,std::string str, BoundedAutomatas* atm) { // automata for single lexeme , thompson's algo
//stnum:Automata that needs to be transited , ednum:target automata , curnum:max num of Automata index
	Automata * curm = &atm->Automatas[*stnum];
	int inisize = curm->GetSize();
	if (str[0] != '\\' && str.back() == '*') { // * case
		str.pop_back();
		curm->Allocate(2);
		curm->SetTrans(*curnum+1, "_epi");
		curm->SetTrans(*ednum, "_epi");
		curm =& atm->Automatas[*curnum + 1];
		curm->Allocate(1);
		curm->SetTrans(*curnum + 2, str);
		curm =& atm->Automatas[*curnum + 2];
		curm->Allocate(2);
		curm->SetTrans(*curnum + 1, "_epi");
		curm->SetTrans(*ednum, "_epi");
		
		*curnum = *curnum + 2;
	}
	else {//simply concate
		curm->Allocate(1);
		curm->SetTrans(*ednum, str);
	}
	//| case need to be proceed earlier
}
unsigned int BoundedAutomatas::GetSize() {
	return this->size;
}


//shit hill
BoundedAutomatas NFAtoDFA(BoundedAutomatas* atm) { //by default , 0 is the start set
	BoundedAutomatas ret;
	ret.Allocate(atm->GetSize()); 


	/* warning : sometimes dfa size is larger than nfa , currently there is no such procedure*/


	std::map <std::set<unsigned int>, unsigned int> powersets;
	std::map <unsigned int, unsigned int> lookups;//lookups for duplicated states;
	std::map <unsigned int, std::string> lookupters;//lookups for terminate name
	unsigned int An = 0; //Marker , for new DFA

	std::queue <unsigned int> que;//find first power set
	std::set <unsigned int> FirstA;
	bool Contains_End ;
	std::string ter_name;
	Contains_End = false;
	FirstA.insert(0);
	que.push(0);
	lookups.insert(std::make_pair(0, 0)); // initial state
	std::vector <NFATranPair> TransformTable;
	while (!que.empty()) {
		Automata TranAt = atm->Automatas[que.front()];
		if (TranAt.Get_Ter() == true) {
			lookupters.insert(std::make_pair(0, TranAt.Get_Ter_Label()));
			ret.Automatas[0].SetTer(true, TranAt.Get_Ter_Label());
		}
		unsigned int i = 0;
		while (i < TranAt.GetSize()) {
			CSPair pr = TranAt.GetTrans(i);
			if (pr.TransCond == "_epi") {
				que.push(pr.NextState);
				FirstA.insert(pr.NextState);
			}
			else {
				if(pr.TransCond != "")
				TransformTable.push_back({ 0, pr.NextState, pr.TransCond });
			}
			i++;
		}
		que.pop();
	}
	std::queue <std::set<unsigned int>> poweriter;//power sets using for bfs
	std::queue <std::vector<NFATranPair>> itertrans;//transition of every power sets
	
	powersets.insert(std::make_pair(FirstA, 0));
	poweriter.push(FirstA); FirstA.clear();
	itertrans.push(TransformTable);
	while (!poweriter.empty()) {

		std::set<unsigned int> pr = poweriter.front();
		std::vector<NFATranPair> tr = itertrans.front();
		auto iter = powersets.find(pr);
		unsigned cur_num = -1;
		if (iter == powersets.end()) {
			std::cout << "error ps";
			//need something
		}
		else {
			cur_num = iter->second;
			if (iter->first.size() == 1) {
				for (auto p : iter->first) {
					unsigned int getstate = p;
					if (atm->Automatas[p].Get_Ter()) {
						ret.Automatas[cur_num].SetTer(true, atm->Automatas[p].Get_Ter_Label());
					}
				}
			}
			if (Contains_End) {
				ret.Automatas[cur_num].SetTer(true, ter_name);
			}
			auto c = lookupters.find(cur_num);
			if (c != lookupters.end()) {
				ret.Automatas[cur_num].SetTer(true, c->second);
			}
			Contains_End = false;
		}
		for (auto ptr = tr.begin(); ptr < tr.end(); ptr++) { //get all transitions
			std::set<unsigned int> subpowerset;
			subpowerset.insert((*ptr).To);
			std::vector<NFATranPair> subnfapair;
			que.push((*ptr).To);  // subpset queue
			unsigned int front = que.front();
			auto iter = lookups.find(que.front());
			if (iter != lookups.end()) { // already in lookup groups
				auto st = powersets.find(pr);
				if (st == powersets.end()) {
					std::cout << "something wrong during powesets construction";
				}
				int suc = ret.Automatas[st->second].SetTrans(iter->second, (*ptr).Cond);
				if (!suc) {
					ret.Automatas[st->second].Allocate(3);
					ret.Automatas[st->second].SetTrans(iter->second, (*ptr).Cond);
				}
				que.pop();
			}
			else {
				An++;
				//lookups.insert(std::make_pair(que.front(),++An))
				while (!que.empty()) { // generating next structure
					Automata TranAt = atm->Automatas[que.front()];
					if (atm->Automatas[que.front()].Get_Ter()) {
						ret.Automatas[An].SetTer(true, atm->Automatas[que.front()].Get_Ter_Label());
						lookupters.insert(std::make_pair(An, atm->Automatas[que.front()].Get_Ter_Label()));
						//ter_name = atm->Automatas[que.front()].Get_Ter_Label();
					}
					que.pop();

					unsigned int i = 0;
					while (i < TranAt.GetSize()) {
						CSPair cpr = TranAt.GetTrans(i);
						if (cpr.TransCond == "_epi") {
							if (atm->Automatas[cpr.NextState].Get_Ter()) {
								Contains_End = true;
								lookupters.insert(std::make_pair(An, atm->Automatas[cpr.NextState].Get_Ter_Label()));
								ter_name = atm->Automatas[cpr.NextState].Get_Ter_Label();
							}
							subpowerset.insert(cpr.NextState);
							que.push(cpr.NextState);
						}
						else {
							if(cpr.NextState != NULLPAIR)
								subnfapair.push_back({ An, cpr.NextState, cpr.TransCond });

						}
						i++;
					}

				}
				auto fd = powersets.find(subpowerset);
				if (fd == powersets.end()) { // new powerset
					powersets.insert(std::make_pair(subpowerset, An));
					lookups.insert(std::make_pair(front, An));
					poweriter.push(subpowerset);
					itertrans.push(subnfapair);
					int suc = ret.Automatas[cur_num].SetTrans(An, (*ptr).Cond);
					if (!suc) {
						ret.Automatas[cur_num].Allocate(3);
						ret.Automatas[cur_num].SetTrans(An, (*ptr).Cond);
					}
				}
				else {//already exists state
					lookups.insert(std::make_pair(front, fd->second));
					int suc = ret.Automatas[cur_num].SetTrans(fd->second, (*ptr).Cond);
					if (!suc) {
						ret.Automatas[cur_num].Allocate(3);
						ret.Automatas[cur_num].SetTrans(fd->second, (*ptr).Cond);
					}
				}
			}

		}
		itertrans.pop();
		poweriter.pop();
	}
	//ret automata construction

	return ret;
}

bool Match_Char(char m, std::string s) {
	if (s[0] != '[') {
		if (s[0] == '\\') {
			if (m == s[1])
				return true;
			else
				return false;
		}
		if (m == s[0])
			return true;
		else
			return false;
	}
	
	else {
		std::string actual;
		if (s.find("a-z") != std::string::npos)
			actual.append(a_z);
		if (s.find("A-Z") != std::string::npos)
			actual.append(A_Z);
		if (s.find("0-9") != std::string::npos)
			actual.append(zero_nine);
		if (actual.empty())
			return false;
		if (actual.find(m) != std::string::npos)
			return true;
		else
			return false;
	}
}

LexemeLinkedList * Generate_From_DFA(std::string orig_str,BoundedAutomatas dfa){
	LexemeLinkedList* p = new LexemeLinkedList;
	LexemeLinkedList* last = p;
	usi current_state=0;
	usi current_string = 1;
	usi last_succeed = 1;
	usi length = 0;
	bool matched_cur;
	for (std::string::iterator it = orig_str.begin(); it != orig_str.end(); ++it) {
		if (*it == ' ' or *it == '\n' or *it == '\t') {
			if (!dfa.Automatas[current_state].Get_Ter()) {
				std::cout << std::endl << "Lexeme Analysis Error: At :" << current_string << std::endl;
				return nullptr;
			}
			auto it2 = *(it + 1);
			if (it2 == ' ' or it2 == '\n' or it2 == '\t') {
				current_string++;
				continue;
			}
			last->SetStr(dfa.Automatas[current_state].Get_Ter_Label());
			last->SetStr2(orig_str.substr(last_succeed - 1, length));
			length = 0;
			last_succeed = current_string+1;
			current_state = 0;
			if ((it + 1) != orig_str.end()) {
				LexemeLinkedList* new_list = new LexemeLinkedList;
				last->SetNext(*new_list);
				last = new_list;
			}
			current_string++;
			continue;
		}
		matched_cur = false;
		Automata curatm = dfa.Automatas[current_state];
		usi trans_size = curatm.Get_Total_Transitions();
		for (usi i = 0; i <trans_size; i++) {
			CSPair p = curatm.GetTrans(trans_size-1-i);
			if (Match_Char(*it, p.TransCond)) {
				current_state = p.NextState;
				matched_cur = true;
				break;
			}
		}
		if (!matched_cur) {
			std::cout << std::endl << "Lexeme Analysis Error: At :" << current_string << std::endl;
			return nullptr;
		}
		current_string++;
		length++;
	}
	if (matched_cur) {
		last->SetStr(dfa.Automatas[current_state].Get_Ter_Label());
		last->SetStr2(orig_str.substr(last_succeed-1));
	}
	return p;
}