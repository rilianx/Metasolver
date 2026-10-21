/*
 * Greedy.cpp
 *
 *  Created on: 01-06-2017
 *      Author: ignacio
 */

#include "Greedy.h"

namespace metasolver {

Greedy::~Greedy(){
	for(Run& r : runs)
		for(Action* a : r.actions) delete a;
}

list<State*> Greedy::next(list<State*>& S) {
	State& s= **S.begin();

	Action* action = best_action(s);

	if(action){
		s.transition(*action);
		delete action;
		return S;
	}else{
		S.clear();
		return S;
	}
}

void Greedy::replay(Record rec, State& s){
	while(rec.run>=0){
		const Run& r = runs[rec.run];
		for(size_t k=rec.pos; k<r.actions.size(); k++)
			s.transition(*r.actions[k]);
		rec = r.next;
	}
}

double Greedy::run(State& s, double tl, clock_t bt){
	if(s.get_hash()==0) return SearchStrategy::run(s, tl, bt);

	begin_time=bt;
	timelimit=tl;
	best_state=&s;

	Run r;
	r.next.run=-1; r.next.pos=0;
	vector<uint64_t> visited;
	bool timeout=false;

	while(true){
		if(timelimit != 0.0 && get_time()>timelimit){ timeout=true; break; }

		uint64_t h = s.get_hash();
		auto it = memory.find(h);
		if(it!=memory.end()){
			replay(it->second, s);
			r.next = it->second;
			break;
		}
		visited.push_back(h);

		Action* action = best_action(s);
		if(!action) break;
		s.transition(*action);
		r.actions.push_back(action);
	}

	//solo se recuerdan los greedy completos
	if(!timeout && memory.size() + visited.size() <= max_memory){
		int id = runs.size();
		for(size_t k=0; k<visited.size(); k++)
			memory[visited[k]] = Record{id, (int) k};
		runs.push_back(r);
	}else
		for(Action* a : r.actions) delete a;

	return s.get_value();
}

} /* namespace clp */
