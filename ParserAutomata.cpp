#include "ParserAutomata.h"

char NonTer_Identifier = '<';
char NonTer_Ender = '>';
char _epi = '#';
char _ed = '$';
// # = _epi $ = end

//due to some ignorance , the parsing 
void ParserAutomata::NewGoto(std::string str, ParserAutomata** pr, std::string cond, bool is_d = false)
{
	if (is_d) {
		auto fd = this->GOTO.find(std::make_pair(str, cond));

		if (fd != this->GOTO.end())
			return;
		this->GOTO.insert(std::make_pair(std::make_pair(str, cond), *pr));
		this->GOTOMem.insert(std::make_pair(str, *pr));
		return;
	}
	if (*pr == nullptr)
		*pr = new ParserAutomata;

	auto p = this->GOTOMem.find(str);

	if (p == this->GOTOMem.end()) {

		this->GOTO.insert(std::make_pair(std::make_pair(str, cond), *pr));
		this->GOTOMem.insert(std::make_pair(str, *pr));
		return;
	}
	else {
		//*pr = p->second;

		//for (auto ck : s->GOTO) {
		//	(*pr)->GOTO.insert(,(*pr)->GOTOMem.find());
		//}

		this->GOTOMem.insert(std::make_pair(str, *pr));
		this->GOTO.insert(std::make_pair(std::make_pair(str, cond), *pr));
		return;
	}

}

void ParserAutomata::AddACT(Production Act, std::string Cond)
{
	this->ACTS.push_back(std::make_pair(Act, Cond));
	return;
}


//this algorithm requires to power expand all the path of a given graph ,
ParserAutomata* ParserAutomata::construct_nondeter_to_deter()
{
	// old - > new 
	std::map <std::set<unsigned int>, unsigned int> powersets; 
	//convert atm to usi
	std::map <ParserAutomata*, unsigned int> atm_to_usi;
	std::map < unsigned int,ParserAutomata*> usi_to_atm;
	//convert new atm to usi
	std::map <ParserAutomata*, unsigned int> atm_to_usi_new;
	std::map < unsigned int, ParserAutomata*> usi_to_atm_new;

	//maximum id currently
	usi id = 0;

	// max id of new atm
	usi id_new = 0;

	std::queue<std::set<unsigned int>> bfs;

	//
	std::set<usi> temp;
	temp.insert(0);
	bfs.push(temp);
	ParserAutomata* new_head = new ParserAutomata;

	new_head->ACTS.insert(new_head->ACTS.end(), this->ACTS.begin(), this->ACTS.end());
	powersets[temp] = id_new;
	atm_to_usi_new[new_head] = id_new;
	usi_to_atm_new[id_new] = new_head;
	id_new++;


	atm_to_usi[this] = id;
	usi_to_atm[id] = this;
	id++;
	//a map for current duplicated gotos
	while (!bfs.empty()) {

		std::set <usi> front = bfs.front();
		bfs.pop();


		auto cur_power_atm = powersets.find(front);
		if (cur_power_atm == powersets.end()) {
			std::cout << "Error Parser 51";
			return nullptr;
		}
		ParserAutomata * cur_power_parseratm = usi_to_atm_new[cur_power_atm->second];

		std::multimap <std::string, ParserAutomata*> dup_gotos;

		std::multimap<std::string, std::string> conditions;//get all conditions

		std::vector<std::pair<Action, std::string>> ACTS;

		for (auto power_atms : front) {
			auto atm = usi_to_atm.find(power_atms);//usi -> atm

			if (atm == usi_to_atm.end()) {
				std::cout << "Error Parser 51_2";
				return nullptr;
			}

			ParserAutomata* satm = atm->second;
			for (auto act : atm->second->ACTS)
				ACTS.push_back(act);

			for (auto gotos : satm->GOTO) {
				//deal with epi
				if (gotos.first.first[0] == _epi) {//epi , should add
					auto atm = atm_to_usi.find(gotos.second);
					usi corre_num;
					//not such a pair 
					if (atm == atm_to_usi.end()) {
						atm_to_usi[gotos.second] = id;
						usi_to_atm[id] = gotos.second;
						corre_num = id;
						id++;
					}
					else {
						corre_num = atm->second;
					}
					front.insert(corre_num);
				}

				conditions.insert(std::make_pair(gotos.first.first, gotos.first.second));
				dup_gotos.insert(std::make_pair(gotos.first.first, gotos.second));

			}

		}

		for (auto act : ACTS) {
			cur_power_parseratm->AddACT(act.first, act.second);
		}

		//finish generating dup_gotos
		std::multimap <std::string, ParserAutomata*>::iterator m_it, s_it;
		for (m_it = dup_gotos.begin(); m_it != dup_gotos.end(); m_it = s_it)
		{
			std::string theKey = (*m_it).first;

			

			auto keyRange = dup_gotos.equal_range(theKey);

			// Iterate over all map elements with key == theKey
			std::set<usi> constructed_sets;
			
			for (s_it = keyRange.first; s_it != keyRange.second; ++s_it)
			{
				auto atm = atm_to_usi.find(s_it->second);
				usi corre_num;
				//not such a pair 
				if (atm == atm_to_usi.end()) {
					atm_to_usi[s_it->second] = id;
					usi_to_atm[id] = s_it->second;
					corre_num = id;
					id++;
				}
				else {
					corre_num = atm->second;
				}

				constructed_sets.insert(corre_num);

			}

			//constructed_sets finished



			auto exists_power = powersets.find(constructed_sets);
			ParserAutomata* new_atm;
			if (exists_power == powersets.end()) {
				powersets[constructed_sets] = id_new;
				bfs.push(constructed_sets);

				new_atm = new ParserAutomata;


				atm_to_usi_new[new_atm] = id_new;
				usi_to_atm_new[id_new] = new_atm;
				id_new++;

			}
			else {
				new_atm = usi_to_atm_new[exists_power->second];

			}
			auto all_cond = conditions.equal_range(theKey);
			
			for (auto p = all_cond.first; p != all_cond.second; p++) {
				cur_power_parseratm->NewGoto(theKey, &new_atm, p->second,true);
			}

		}


	}
	


	return new_head;
}

void ParserAutomata::clear_all_atms()
{
}

bool Generate_First_Recursively(Production pd, usi curpos,
	std::multimap<std::string, std::string>* firsts,
	std::set< std::pair < std::string, std::string>>* itered, Productions* pds, bool force = false) {
	//true -> contain _epi , false: not contain epi


	if (itered->find(std::make_pair(pd.s1, pd.s2)) != itered->end()) { //already itered production
		if (!force)
			return false;
	}
	if (curpos >= pd.s2.length())
		return false;

	itered->insert(std::make_pair(pd.s1, pd.s2));

	if (pd.s2[curpos] == NonTer_Identifier) {
		std::string sbr = pd.s2.substr(curpos);
		size_t pos = sbr.find(NonTer_Ender);
		if (pos == std::string::npos)
			std::cout << "Grammar Error missing >";
		std::string pdc = pd.s2.substr(curpos, pos + 1);
		auto all_prods = pds->prod_index.equal_range(pdc);
		std::string tempname;
		bool contains_epi = false;
		for (auto it = all_prods.first; it != all_prods.second; it++) {
			bool epi = Generate_First_Recursively(it->second, 0, firsts, itered, pds);
			tempname = it->first;
			if (epi) {
				contains_epi = true;
			}
		}
		auto all_temps = firsts->equal_range(tempname);
		for (auto temp = all_temps.first; temp != all_temps.second; temp++) {
			bool add = true;
			auto checks = firsts->equal_range(pd.s1);
			for (auto p = checks.first; p != checks.second; p++) {
				if (p->second == temp->second) {
					add = false;
					break;
				}
			}
			if (add) {
				firsts->insert(std::make_pair(pd.s1, temp->second));
			}
		}
		if (contains_epi) {
			Generate_First_Recursively(pd, pos + 1, firsts, itered, pds, force = true);
		}

	}
	else {

		std::string t;
		std::string sbr = pd.s2.substr(curpos);
		size_t pos = sbr.find(NonTer_Identifier);
		if (pos == std::string::npos) {
			t = sbr;
		}
		else {
			t = pd.s2.substr(curpos, pos );
		}
		auto checks = firsts->equal_range(pd.s1);
		bool ins = true;
		for (auto p = checks.first; p != checks.second; p++) {
			if (p->second == t) {
				ins = false;
				break;
			}
		}

		if(ins)
		firsts->insert(std::make_pair(pd.s1, t));

		if (pd.s2[curpos] == _epi) {
			return true;
		}

	}
	return false;

}

void Productions::Generate_Parse_Automatas_Recur(Production curpro,
	usi curpos, ParserAutomata* atm, std::set <std::string> firsts,
	std::map <std::pair<std::string, usi>, ParserAutomata* >* used_states)
{
	//reach the end
	if (curpos >= curpro.s2.length()) {
		std::string conc_f;
		for (auto fir : firsts) {
			conc_f += fir;
		}
		std::string concated_str = curpro.s1 + "->" + curpro.s2 + "_fir:" + conc_f;

		auto itered_state = used_states->find(std::make_pair(concated_str, curpos));
		if (itered_state == used_states->end()) {
			for (auto fir : firsts)
				atm->AddACT(curpro, fir);
			used_states->insert(std::make_pair(std::make_pair(concated_str, curpos), atm));
		}

		return;
	}
	std::string s = curpro.s2;

	


	// non ter
	if (s[curpos] == NonTer_Identifier) {
		std::string sbr = s.substr(curpos);
		size_t pos = sbr.find(NonTer_Ender);
		if (pos == std::string::npos)
			std::cout << "Grammar Error missing >";
		std::string pdc = s.substr(curpos, pos + 1);
		std::string find_this_first;
		std::set <std::string> duplicated_firsts;
		if ((curpos + 1) == curpro.s2.length()) {
			for (auto fir : firsts) {
				auto tp = Find_First_Of(fir, this);
				duplicated_firsts.insert(tp.begin(), tp.end());
			}
		}
		else {
			if (sbr.length() <= (pos + 1)) {
				for (auto fir : firsts) {
					auto tp = Find_First_Of(fir, this);
					duplicated_firsts.insert(tp.begin(), tp.end());
				}
			}
			//not reach the end
			else {

				std::string temp = sbr.substr(pos + 1);
				if (temp.length() == 0) {
					for (auto fir : firsts) {
						temp += fir;
						auto tp = Find_First_Of(temp, this);
						duplicated_firsts.insert(tp.begin(), tp.end());
					}
				}
				else if (temp[0] == NonTer_Identifier) {
					size_t temp_pos = temp.find(NonTer_Ender);
					if (temp_pos == std::string::npos)
						std::cout << "Grammar Error missing >";
					std::string temp_pdc = temp.substr(0, temp_pos + 1);
					for (auto fir : firsts) {
						temp_pdc += fir;
						auto tp = Find_First_Of(temp_pdc, this);
						duplicated_firsts.insert(tp.begin(), tp.end());
					}

				}
				//ter
				else {
					duplicated_firsts = Find_First_Of(temp, this);
				}
			}
		}

		//finished getting the firsts vector
		std::string conc_f;
		for (auto fir : firsts) {
			conc_f += fir;
		}
		std::string concated_str = curpro.s1 + "->" + curpro.s2 + "_fir:" + conc_f;
		//used_states map concated_str to the next atm it should be expanded
		auto itered_state = used_states->find(std::make_pair(concated_str, curpos));

		
		//a new state is to be created
		if (itered_state == used_states->end()) {
			ParserAutomata* new_atm = nullptr;
			for (auto fir : firsts) {
				atm->NewGoto(pdc, &new_atm, fir);
			}
			used_states->insert(std::make_pair(std::make_pair(concated_str, curpos), new_atm));


			auto all_temps = this->prod_index.equal_range(pdc);
			for (auto temp = all_temps.first; temp != all_temps.second; temp++) {
				Generate_Parse_Automatas_Recur(
					temp->second, 0, atm, duplicated_firsts, used_states);
			}
			Generate_Parse_Automatas_Recur(curpro, curpos + pos + 1, new_atm, firsts, used_states);


		}
		//find in used_states
		else {
			ParserAutomata* atm_exi = itered_state->second;


			for (auto fir : firsts) {
				atm->NewGoto(pdc, &atm_exi, fir);
			}

			//check whether in heads
			auto hd_find = this->all_atm_heads->find(std::make_pair(concated_str, curpos));
			if (hd_find == this->all_atm_heads->end()) {
				this->all_atm_heads->insert(
					std::make_pair(std::make_pair(concated_str, curpos), atm_exi)
				);
				auto all_temps = this->prod_index.equal_range(pdc);
				for (auto temp = all_temps.first; temp != all_temps.second; temp++) {
					Generate_Parse_Automatas_Recur(
						temp->second, 0, atm, duplicated_firsts, used_states);
				}
			}
			else {
				//Note: decimate duplicate!!
				if (hd_find->second != atm) {
					for (auto gts : hd_find->second->GOTO) {
						atm->NewGoto(gts.first.first, &gts.second, gts.first.second,true);
					}
				}
			}
			Generate_Parse_Automatas_Recur(curpro, curpos + pos + 1, atm_exi, firsts, used_states );

		}

	}
	//reach a ter
	else {
		std::string conc_f;
		for (auto fir : firsts) {
			conc_f += fir;
		}
		std::string concated_str = curpro.s1 + "->" + curpro.s2 + "_fir:" + conc_f;
		auto itered_state = used_states->find(std::make_pair(concated_str, curpos));
		

		std::string temp;
		std::string sbr = curpro.s2.substr(curpos);
		size_t pos = sbr.find(NonTer_Identifier);
		if (pos == std::string::npos) {
			pos = sbr.length();
			temp = sbr;
		}
		else {
			temp = curpro.s2.substr(curpos, pos);
		}
		if (itered_state != used_states->end()) {
			ParserAutomata* atm_exi = itered_state->second;

			

			for (auto fir : firsts)
				atm->NewGoto(temp, &atm_exi, fir);
			return;
		}
		std::set <std::string> duplicated_firsts;
		//
		size_t apos;
		std::string f;
		if (curpos + 1 == curpro.s2.length()) {
			for (auto fir : firsts)
				duplicated_firsts.insert(fir);
			apos = curpos + 1;
		}
		else if (curpro.s2[curpos + 1] != NonTer_Identifier) {
			f += curpro.s2[curpos + 1];
			duplicated_firsts.insert(f);
			apos = curpos + 1;
		}
		//needs calc the first
		else {
			std::string sbr = s.substr(curpos);
			size_t pos = sbr.find(NonTer_Ender);
			apos = pos;
			if (pos == std::string::npos)
				std::cout << "Grammar Error missing >";
			for (auto fir : firsts) {


				std::string pdc = s.substr(curpos + 1, pos) + fir;
				auto tp = Find_First_Of(pdc, this);
				duplicated_firsts.insert(tp.begin(), tp.end());
			}
		}
		//finished duplicate firsts

		ParserAutomata* new_atm = nullptr;
		for (auto fir : firsts)
			atm->NewGoto(temp, &new_atm, fir);
		used_states->insert(std::make_pair(std::make_pair(concated_str, curpos), new_atm));

		Generate_Parse_Automatas_Recur(curpro, curpos +pos, new_atm, firsts, used_states);

		if (itered_state == used_states->end()) {

		}
		//already exists
		else {
			std::cout << "Error Parser 208";
		}

	}
}

void Productions::Generate_All_First()
{
	//should be solved recursively(can handle cyclic case)
	std::set < std::pair < std::string, std::string> > itered;
	std::multimap<std::string, std::string> firsts;
	for (auto p : this->prod) {
		itered.clear();
		Generate_First_Recursively(p, 0, &firsts, &itered, this);
	}

	this->All_First = firsts;
	return;
}




void Productions::New_Prod(std::string s1, std::string s2, bool is_begin)
{
	Production p = { s1,s2 };
	this->prod.push_back(p);
	this->prod_index.insert(std::make_pair(s1, p));
	if (is_begin)
		this->begin_prods.push_back(p);
	return;
}


ParserAutomata* Productions::Generate_Parse_Automatas()
{
	usi sz = this->begin_prods.size();
	if (sz == 0)
		return nullptr;
	ParserAutomata* atm_head = new ParserAutomata[sz];
	usi c = 0;
	for (auto it : this->begin_prods) {
		// map Production+curpos -> automata address
		std::set <std::string > p;
		p.insert("$");
		std::map <std::pair<std::string, usi>, ParserAutomata* > used_states;
		Generate_Parse_Automatas_Recur(it, 0, &atm_head[c], p, &used_states);
		c++;
	}

	return atm_head;
}

std::set<std::string> Find_First_Of(std::string s, Productions* p)
{
	std::set<std::string> str;

	size_t curpos = 0;
	if (s[curpos] == NonTer_Identifier) {
		std::string sbr = s.substr(curpos);
		size_t pos = sbr.find(NonTer_Ender);
		if (pos == std::string::npos)
			std::cout << "Grammar Error missing >";
		std::string pdc = s.substr(curpos, pos - curpos + 1);
		bool contains_epi = false;
		auto all_temps = p->All_First.equal_range(pdc);
		for (auto temp = all_temps.first; temp != all_temps.second; temp++) {
			str.insert(temp->second);
			if (temp->second == "$") {
				contains_epi = true;
			}
		}
		if (contains_epi) {
			std::string sbr2 = s.substr(pos + 1);
			std::set <std::string> str2 = Find_First_Of(sbr2, p);
			str.insert(str2.begin(), str2.end());
		}
	}
	else {
		std::string t;
		std::string sbr = s.substr(curpos);
		size_t pos = sbr.find(NonTer_Identifier);
		if (pos == std::string::npos) {
			t = sbr;
		}
		else {
			t = s.substr(curpos, pos);
		}
		str.insert(t);
	}
	return str;

}


ParserAutomata* ParserTable::getHeadParser() const
{
	return Head;
}

void ParserTable::setHeadParser(ParserAutomata* Head)
{
	this->Head = Head;
}

template <class T,class U>
//reduce Nodestack and generate new Nodes for the tree,T->Attr Name , U-> Attr val
// return a concated str for checking
U Parser_Reduce(std::stack<ParserNodes_Map<T,U>* > * NodesStack,
	T New_Attr_Name,U New_Attr_Val , usi depth,T checked_attr) {

	ParserNodes_Map<T, U>* new_fa = new ParserNodes_Map<T, U>;
	new_fa->setAttributes(New_Attr_Name, New_Attr_Val);
	U ck;
	for (usi i = 0; i < depth; i++) {
		ParserNodes_Map<T, U>* top = NodesStack->top();
		NodesStack->pop();
		ck.insert(0,top->getAttributes(checked_attr));
		top->setFathers(new_fa);
		new_fa->setSons(top);
	}
	NodesStack->push(new_fa);
	return ck;
}

usi Get_Str_Depth(std::string str) {
	if (str[0] == _epi)
		return 0;

	usi curpos = 0;
	usi depth = 0;
	while (curpos < str.length()) {
		std::string sbr = str.substr(curpos);
		if (str[curpos] == NonTer_Identifier) {
			
			size_t pos = sbr.find(NonTer_Ender);
			depth++;
			curpos = curpos + pos + 1;
		}
		else {
			depth++;
			size_t sub_pos = sbr.find(NonTer_Identifier);
			if (sub_pos == std::string::npos) {
				sub_pos = sbr.length();
			}
			curpos = curpos + sub_pos ;
		}
	}
	return depth;
}

//Parser a specific str , using stack, got the tree nodes
//0~n , from right to left(0 -> right most son , n -> left most son)
ParserNodes_Map<std::string, std::string>* ParserTable::Parse(LexemeLinkedList* s)
{
	ParserAutomata* hd = this->getHeadParser();
	LexemeLinkedList* cur_lk = s;

	std::string curstr;
	//lr1 look ahead for 1 
	//str1 = token , str2 = orig
	std::string lookahead;
	if (s->Next() == nullptr) {
		lookahead = "$";
	}
	else {
		lookahead += s->Next()->GetStr();
	}
	//stack for parser
	std::stack<ParserAutomata*> ParserStack;
	//stack for nodes
	std::stack<ParserNodes_Map<std::string, std::string>* > NodesStack;

	ParserNodes_Map<std::string, std::string>* fr_node = new ParserNodes_Map<std::string, std::string>;
	fr_node->setAttributes("Ident", s->GetStr());
	fr_node->setAttributes("Orig", s->GetStr2());
	NodesStack.push(fr_node);
	ParserStack.push(hd);

	bool first = true;

	bool parse_failed = false;

	//flags of epi
	bool empty_flag = false;

	//flags of failed
	//bool Reduce_failed = false;
	//bool Shift_failed = false;

	while (1) {
		ParserAutomata* stack_top = ParserStack.top();
		bool reduced = false;
		
		//Get All Acts

		for (auto p : stack_top->ACTS) {
			//check empty flag to avoid dead loop

			//Do the reductions
			if (p.second == lookahead) {

				if (p.first.s2[0] == _epi ) {
					if(!empty_flag)
						continue;

					else {
						empty_flag = false;
					}

				}
				

				
				usi Depth = Get_Str_Depth(p.first.s2);

				

				//temp , can slow perf
				std::stack<ParserNodes_Map<std::string, std::string>* > dupNodesStack=NodesStack;
				
				std::string ck = Parser_Reduce<std::string, std::string>
					(&dupNodesStack, "Ident", p.first.s1, Depth, "Ident");

				

				if (p.first.s2[0] != _epi and ck != p.first.s2) {
					//Reduce_failed = true;
				}
				else {
					reduced = true;
					Parser_Reduce<std::string, std::string>
						(&NodesStack, "Ident", p.first.s1, Depth, "Ident");
					for (usi i = 0; i < Depth; i++)
						ParserStack.pop();

					if (p.first.s1 == this->accept_state) {
						std::cout << "Parse Success!";
						return NodesStack.top();
					}
				}
				
				break;
			}
		}
		//not reduced , try shift
		if (!reduced) {
			//top of the ongoing node stack
			bool suc = false;
			ParserNodes_Map<std::string, std::string>* top_node = NodesStack.top();
			for (auto gotos : stack_top->GOTO) {
				if (first) {
					NodesStack.pop();
					delete top_node;
					first = false;
					break;
				}

				if (gotos.first.first[0] != NonTer_Identifier)
					continue;
				if (gotos.first.first == top_node->getAttributes("Ident")) {
					ParserStack.push(gotos.second);
					
					//ParserNodes_Map<std::string, std::string>* nodew = new ParserNodes_Map<std::string, std::string>;

					//if (gotos.first.first[0] != NonTer_Identifier) {

					//	nodew->setAttributes("Ident", cur_lk->GetStr());
					//	nodew->setAttributes("Orig", cur_lk->GetStr2());

					//	if (cur_lk->Next() == nullptr) {
					//		lookahead = "$";
					//	}
					//	else {
					//		std::string temp; temp += cur_lk->Next()->GetStr()[0];
					//		lookahead = temp;

					//	}
					//	cur_lk = cur_lk->Next();


					//	NodesStack.push(nodew);
					//}
					//else {
					//	nodew->setAttributes("Ident", gotos.first.first);
					//}



					suc = true;





					break;
				}

			}
			if (!suc) {

				//not succeed , then check the linked list
				bool find_lk_list = false;
				if (cur_lk != nullptr) {
					for (auto gotos : stack_top->GOTO) {
						//get it attributed!


						if (gotos.first.first == cur_lk->GetStr()) {
							ParserNodes_Map<std::string, std::string>* nodew = new ParserNodes_Map<std::string, std::string>;


							nodew->setAttributes("Ident", cur_lk->GetStr());
							nodew->setAttributes("Orig", cur_lk->GetStr2());

							if (cur_lk->Next() == nullptr) {
								lookahead = "$";
							}
							else {
								std::string temp; temp += cur_lk->Next()->GetStr();
								lookahead = temp;

							}
							cur_lk = cur_lk->Next();

							ParserStack.push(gotos.second);
							find_lk_list = true;
							NodesStack.push(nodew);
							break;
						}
					}
					if (!find_lk_list) {

						//then try empty op again!
						if (!empty_flag ) {
							empty_flag = true;
							continue;
						}

						std::cout << "Parser Error: When trying to shift" << top_node->getAttributes("Ident");
						return nullptr;
					}
				}
			}
		}
	}
}
template <typename T>
std::vector<T*> ParseNodes<T>::getFathers() const
{
    return fathers;
}
template <typename T>
void ParseNodes<T>::setFathers(T* fathers)
{
    this->fathers.push_back(fathers);
}

template <typename T>
std::vector<T*> ParseNodes<T>::getSons() const
{
    return sons;
}
template <typename T>
void ParseNodes<T>::setSons(T* sons)
{
    this->sons.push_back(sons);
}

template <typename T, typename U>
U ParserNodes_Map<T,U>::getAttributes(T k) const
{
    return Attributes.find(k)->second;
}
template <typename T, typename U>
void ParserNodes_Map<T,U>::setAttributes(T attk,U attv)
{

	//type checked recommended

	this->Attributes.insert(std::make_pair(attk,attv));
}


ParserAutomata* ParserTable::getHead() const
{
    return Head;
}

void ParserTable::setHead(ParserAutomata* Head)
{
    this->Head = Head;
}

std::string ParserTable::getAcceptstate() const
{
    return accept_state;
}

void ParserTable::setAcceptstate(std::string acceptstate)
{
    accept_state = acceptstate;
}

