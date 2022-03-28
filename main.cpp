#include "ReadReg.h"
#include "Rules.h"
#include "ParserAutomata.h"
int main() {
	/*RegParser reg;
	std::string st = "[a-z]b[a-zA-Z]*09|10|11[a-z]|[A-Z]*";
	//std::string st = "[a-z]|[A-Z]*";
	LexemeLinkedList * ns = reg.ParseReg(st);
	BoundedAutomatas atm = SimpleAutomataFromLinkedList(ns);
	ns->ConstructAutomations(&atm, "a");
	BoundedAutomatas dfs = NFAtoDFA(&atm);
	std::cout << "reg parse done";*/
	LexemeRules p;
	

	p.AddRules("[A-Za-z][0-9A-Za-z]*","ident");
	p.AddRules("[0-9][0-9]*","intconst");
	p.AddRules("+=","asop");
	p.AddRules("-=","asop");
	p.AddRules("\\*=", "asop");
	p.AddRules("=", "asop");
	p.AddRules("/=","asop");
	p.AddRules("%=","asop");
	p.AddRules("+","binop");
	p.AddRules("-","binop_minus");

	//remember to modify back to binop!
	p.AddRules("\\*","binop");
	p.AddRules("return","return");

	p.AddRules("/","binop");
	p.AddRules("%","binop");
	p.AddRules("id", "id");
	p.AddRules("{", "{");
	//p.AddRules("(", ")");
	//p.AddRules(")", "(");
	p.AddRules("}", "}");
	p.AddRules(";", ";");
	BoundedAutomatas atm = p.Construct_DFA();
	//std::string str_mt = "* * id = id";
	
	//strictly devided by space (need to fix!!!)
	std::string str_mt = "{ ABC = 3 ; CD = ABC ; AD = 321 ; AD += 600 ; return E ; }";

	LexemeLinkedList* s = Generate_From_DFA(str_mt, atm);
	
	//parser
	Productions pds;
	//pds.New_Prod("<S'>", "<S>", true);
	//pds.New_Prod("<S>", "<C><C>");
	//pds.New_Prod("<C>", "c<C>");
	//pds.New_Prod("<C>", "d");
	// 
	//ctrl + k + c can fast annotate selected

	//pds.New_Prod("<S'>", "<S>", true);
	//pds.New_Prod("<S>", "<L>asop<R>");
	//pds.New_Prod("<S>", "<R>");
	//pds.New_Prod("<L>", "binop<R>");
	//pds.New_Prod("<L>", "id");
	//pds.New_Prod("<R>", "<L>");


	pds.New_Prod("<Prog>", "<Prog'>", true);
	pds.New_Prod("<Prog'>", "{<stmts><return><exp><;>}");
	pds.New_Prod("<Prog'>", "{<return><exp><;>}");
	pds.New_Prod("<stmts>", "<L><LE>");
	pds.New_Prod("<LE>", "<ident><asop><exp><;>");
	pds.New_Prod("<L>", "#"); // # = epi
	pds.New_Prod("<L>", "<stmts>");
	//cons
	pds.New_Prod("<ident>", "ident");
	pds.New_Prod("<asop>", "asop");
	pds.New_Prod("<binop>", "binop");
	pds.New_Prod("<binop_minus>", "binop_minus");
	pds.New_Prod("<intconst>", "intconst");
	pds.New_Prod("<return>", "return");
	pds.New_Prod("<;>", ";");

	pds.New_Prod("<exp>", "(<exp>)");
	pds.New_Prod("<exp>", "<intconst>");
	pds.New_Prod("<exp>", "<ident>");
	pds.New_Prod("<exp>", "<binop_minus><expd>");
	pds.New_Prod("<exp>", "<exp><binop><expd>");
	pds.New_Prod("<exp>", "<exp><binop_minus><expd>");

	pds.New_Prod("<expd>", "(<expd>)");
	pds.New_Prod("<expd>", "<intconst>");
	pds.New_Prod("<expd>", "<ident>");
	pds.New_Prod("<expd>", "<binop_minus><expd>");








	pds.Generate_All_First();
	ParserAutomata * all_atms =  pds.Generate_Parse_Automatas();
	//by this way,  we get a nondeterministic parsing atms 
	// 
	

	//(needs implement) clear all_atms using delete to avoid memory leak!
	ParserAutomata* new_atms = all_atms->construct_nondeter_to_deter();
	
	ParserTable pt;
	pt.setHead(new_atms);
	pt.setAcceptstate("<Prog>");
	ParserNodes_Map<std::string, std::string>*  tree_head = pt.Parse(s);

	return 0;
}

