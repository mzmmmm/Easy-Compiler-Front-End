#pragma once
#include <string>
#define NULLPAIR -1

typedef unsigned int usi;
typedef unsigned int LinkedLength;



extern std::string a_z, A_Z, zero_nine;
struct NFATranPair { //trans pair using when NFA 2 DFA
	unsigned int From;
	unsigned int To;
	std::string Cond;
};
struct CSPair {
	unsigned int NextState = NULLPAIR;// no such path
	std::string TransCond;
};
class Automata {
	CSPair* transitions;
	std::string terlabel = "None";
	unsigned int transpos = 0;//used transitions
	unsigned int size = 0; // transition limit
	bool is_ter = 0;
public:
	bool Get_Ter();
	std::string Get_Ter_Label();
	CSPair GetTrans(unsigned int num);
	unsigned int GetSize();
	unsigned int Get_Total_Transitions();
	void SetTer(bool p,std::string s);
	void Allocate(unsigned int size); //allocate trans
	int SetTrans(unsigned int NS, std::string COND); // _epi is reserverd , means unconditional trans
};
class BoundedAutomatas {
	unsigned int size=0;
public:
	Automata* Automatas = nullptr;
	unsigned int GetSize();
	void Allocate(unsigned int size);//allocate size
};
class LexemeLinkedList {
	std::string RegParttern;
	std::string RegParttern2;
	LinkedLength length = 1;
	LexemeLinkedList* next = nullptr;
public:
	usi ednum = 1;
	usi stnum = 0;
	usi curnum = 1;
	void SetStr2(std::string s);
	void add_length(LinkedLength lg);
	void SetNext(LexemeLinkedList& Next);
	void SetStr(std::string s);
	std::string GetStr();
	std::string GetStr2();
	LexemeLinkedList* Next();
	LinkedLength GetLength();
	void ConstructAutomations(BoundedAutomatas* atm, std::string token);
};
void ConstructFromSingleLexeme(unsigned int* curnum, unsigned int * stnum, unsigned int* ednum, std::string str, BoundedAutomatas* atm);
BoundedAutomatas NFAtoDFA(BoundedAutomatas* atm); // convert a NFA TO DFA

bool Match_Char(char m, std::string s);

LexemeLinkedList* Generate_From_DFA(std::string orig_str, BoundedAutomatas dfa);
