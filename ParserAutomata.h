#pragma once

#include "FiniteAutomata.h"
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <queue>
#include <stack>

template <typename t1,typename t2>
struct Couple {
	t1 s1;
	t2 s2;
};
typedef Couple<std::string, std::string> Action;
typedef Couple<std::string, std::string> Production;
extern char NonTer_Identifier;
extern char NonTer_Ender;
extern char _epi;
class ParserAutomata {


	
public:
	usi statenum=0;
	std::vector<std::pair<Action, std::string>> ACTS;
	std::multimap <std::pair<std::string, std::string >
		, ParserAutomata* > GOTO;
	std::map <std::pair<std::string, std::string >
		, ParserAutomata* >* GOTO_d = new std::map <std::pair<std::string, std::string >
		, ParserAutomata* >;
	std::multimap <std::string, ParserAutomata*> GOTOMem;
	void NewGoto(std::string str, ParserAutomata** pr, std::string cond,bool is_d);
	void AddACT(Production Act, std::string Cond);


	//construction to a deterministic
	ParserAutomata* construct_nondeter_to_deter();
	//clear
	void clear_all_atms();
};

class Productions {
	int p;
	void Generate_Parse_Automatas_Recur(Production curpro, usi curpos, ParserAutomata* atm, std::set<std::string> firsts,
		std::map <std::pair<std::string, usi>, ParserAutomata* > * used_states );
public:
	//the head of each new state
	std::map <std::pair<std::string, usi>, ParserAutomata* >* all_atm_heads = new 
		std::map <std::pair<std::string, usi>, ParserAutomata* >;

	std::vector<Production> prod;
	std::vector<Production>begin_prods;
	std::multimap<std::string, Production> prod_index;//index for productions
	// s1 -> get all productions which left side is s1
	std::multimap<std::string, std::string> All_First;
	void Generate_All_First(); //Generate all single nonter first
	
	void New_Prod(std::string s1, std::string s2,bool is_begin=false);
	ParserAutomata* Generate_Parse_Automatas(); //LR1 parser
public:
    int getP() const;
    void setP(int p);

};

template <typename T>
class ParseNodes {
	std::vector<T*> fathers;
	std::vector<T*> sons;
public:
	std::vector<T*> getFathers() const;
	void setFathers(T* fathers);

	std::vector<T*> getSons() const;
	void setSons(T* sons);

};

template <typename T, typename U>
class ParserNodes_Map :public ParseNodes<ParserNodes_Map<T,U>> {
	std::map<T, U> Attributes;

public:
	U getAttributes(T k) const;
	void setAttributes(T attk, U attv);

};

class ParserTable {
	ParserAutomata* Head;
	std::string accept_state;
public:
    ParserAutomata* getHeadParser() const;
    void setHeadParser(ParserAutomata* HeadParser);

	ParserNodes_Map<std::string, std::string>* Parse(LexemeLinkedList * s);
    ParserAutomata* getHead() const;
    void setHead(ParserAutomata* Head);

    std::string getAcceptstate() const;
    void setAcceptstate(std::string acceptstate);

};


usi Get_Str_Depth(std::string str);
std::set<std::string> Find_First_Of(std::string, Productions* p);
