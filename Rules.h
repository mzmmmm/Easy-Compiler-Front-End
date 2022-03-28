#pragma once
#include "ReadReg.h"
#include <map>
class Lexemes {
	LexemeLinkedList* Current_Lexeme = nullptr;
	Lexemes* Next_Lexeme = nullptr;
public:
	std::string Terminal;
	void SetNextLexeme(Lexemes* p);
	LexemeLinkedList* Get_Current_LinkedList();
	Lexemes* Get_Next();
	void ConstructCurrentLexeme(LexemeLinkedList* ll);
};
class LexemeRules {
	unsigned int Int_Automatas;
	Lexemes* LexemeHead=nullptr;
	Lexemes* LastLexeme = nullptr;
public:
	void Add_Automatas(unsigned int num);//modify the estimated automatas num
	void SetLexemeHead(Lexemes* Head);
	void AddRules(std::string RULE, std::string Terminal);
	unsigned int GetEstimatedAutomatas();
	BoundedAutomatas Construct_DFA();
};
extern std::map <usi,usi> Occured_States; //A Midate Set used for Append atms
usi AppendTwoAutomatas(BoundedAutomatas *atm1, BoundedAutomatas* atm2,usi max_state); //append atm2 into atm1
//atm2 should be deleted
//atm1 should have enough space