#pragma once
#include <fstream>
#include <string>
#include <iostream>
#include "FiniteAutomata.h"
#include "Utils.h"

struct RegTuple {
	std::string Token;
	std::string Reg;
};

RegTuple ProcRegLines(std::string s, std::string delimiter="::=");
void ReadRegLines(std::string fn);


class RegParser {
private:
	std::string NonTerminals = "|[-\\";
	std::string Terminals = "*]=+-/%abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789{};";
	const int lookahead = 1; // must look ahead for 1 character
public:
	LexemeLinkedList* ParseReg(std::string s);
};
BoundedAutomatas SimpleAutomataFromLinkedList(LexemeLinkedList* p);