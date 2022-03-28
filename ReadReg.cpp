#include "ReadReg.h"


RegTuple ProcRegLines(std::string s,std::string delimiter) {
	size_t pos = 0;
	//std::string delimiter = "::=";
	std::string Token;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		Token = s.substr(0, pos);
		s.erase(0, pos + delimiter.length());
	}
	RegTuple TRPair = {
		Token,
		s,
	};
	return TRPair;
}

void Earse(std::string& string) {
	string.erase(std::remove_if(string.begin(), string.end(), std::isspace), string.end());
}

void ReadRegLines(std::string fn) {
	std::ifstream file(fn);
	std::string str;
	while (std::getline(file, str)) {
		RegTuple Tuple = ProcRegLines(str);
		Earse(Tuple.Token);
		Earse(Tuple.Reg);
	}

}

BoundedAutomatas SimpleAutomataFromLinkedList(LexemeLinkedList* p) { // allocate size
	unsigned int size = 1;
	while (p != nullptr) {
		if (p->GetStr().back() == '*') { // using simple thompson's algorithm
			size_t t = countnum(p->GetStr(), "*");
			size += 3 * (unsigned int)t;
		}
		else {
			size += 1;
		}
		p = p->Next();
	}
	BoundedAutomatas atm;
	atm.Allocate(size);
	return atm;
}


void LexemeLinkedList::SetStr2(std::string s)
{
	this->RegParttern2 = s;
}

void LexemeLinkedList::add_length(LinkedLength lg) {
	this->length += lg;
}

 void LexemeLinkedList::SetNext(LexemeLinkedList& Next) {
	this->next = &Next;
}

 void LexemeLinkedList::SetStr(std::string s) {
	this->RegParttern = s;
}

 std::string LexemeLinkedList::GetStr() {
	return this->RegParttern;
}
 std::string LexemeLinkedList::GetStr2() {
	 return this->RegParttern2;
 }

 LexemeLinkedList* LexemeLinkedList::Next() {
	if (this->next == nullptr) {
		std::cout << "You haven't allocate next yet"<<std::endl;
		return nullptr;
	}
	return this->next;
}

 LinkedLength LexemeLinkedList::GetLength()
 {
	 return this->length;
 }

 void LexemeLinkedList::ConstructAutomations(BoundedAutomatas* atm, std::string token) {
	LexemeLinkedList* p = this;
	unsigned int* ednum =& this->ednum;
	unsigned int* curnum = &this->curnum;
	unsigned int* stnum = &this->stnum;//0
	while (p != nullptr) {

		size_t ors = p->RegParttern.find('|');
		if (ors != std::string::npos) { // or case
			RegTuple Splited = ProcRegLines(p->RegParttern, "|"); // (string,string) , split it

			ConstructFromSingleLexeme(curnum, stnum, ednum, Splited.Token, atm);
			ConstructFromSingleLexeme(curnum, stnum, ednum, Splited.Reg, atm);
		}
		else {
			ConstructFromSingleLexeme(curnum, stnum, ednum, p->RegParttern, atm);
		}
		p = p->next;
		if (p != nullptr) {
			*stnum = *ednum;
			*ednum = *curnum + 1;
			*curnum = *curnum + 1;
		}
	}
	//ending needs a label!

	atm->Automatas[*ednum].SetTer(1, token);
}

 LexemeLinkedList* RegParser::ParseReg(std::string s) {
	size_t strpoint = 0;
	size_t lastlexemepoint = -1;
	if (s[strpoint] == '|' or s[strpoint] == '*') { // illegal case
		std::cout << "encounter an error while parsing Reg";
		return nullptr;
	}
	LexemeLinkedList* Last = new LexemeLinkedList;
	LexemeLinkedList* RecLast = Last;
	bool Recon = 0;
	while (strpoint < s.size()) {
		char BytesRead = s[strpoint];
		size_t ter = this->Terminals.find(s[strpoint]);
		if (ter != std::string::npos) { //encounter terminal
			if (ter == 0) { // * case , need to look back
				if (s[strpoint - 1] == '*' or s[strpoint - 1] == '|') {
					std::cout << "** |*  error";
					return nullptr;
				}
			}
			if (ter == 1) { // encounter ] case
				if (!Recon) {
					std::cout << "] Error";
					return nullptr;
				}
				else {
					Recon = 0;
				}
			}
			if (strpoint + 1 < s.size()) { // look ahead for one
				if (s[strpoint + 1] == '*' or s[strpoint + 1] == '|' or s[strpoint + 1] == '-') {
					strpoint++;
					continue;
				}
			}
			//accept case
			if (!Recon) {
				Last->SetStr(s.substr(lastlexemepoint + 1, strpoint - lastlexemepoint));
				if (strpoint + 1 != s.size()) { //not reach the last lexeme
					LexemeLinkedList* New = new LexemeLinkedList;
					Last->SetNext(*New);
					RecLast->add_length(1);
					Last = New;
				}
				lastlexemepoint = strpoint;
			}
		}
		else { // not terminal
			size_t nter = this->NonTerminals.find(s[strpoint]);
			if (nter == std::string::npos) {
				std::cout << "Not matched char";
				return nullptr;
			}
			if (nter == 0) { // | case
				if (s[strpoint - 1] == '|' or strpoint + 1 == s.size()) {
					std::cout << "| error";
					return nullptr;
				}
			}
			if (nter == 1) {
				// [ case
				if (Recon) {
					std::cout << "[[ error";
					return nullptr;
				}
				Recon = 1;
			}
			if (nter == 2) {
				// - case
				;
			}
		}
		strpoint++;
	}
	return RecLast;
}
