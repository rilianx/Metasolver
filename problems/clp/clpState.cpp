/*
 * clpState.cpp
 *
 *  Created on: 4 jul. 2017
 *      Author: iaraya
 */

#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>

#include "BoxShape.h"
#include "clpState.h"

using namespace std;
using namespace metasolver;

namespace clp {


void clpState::get_actions(list< Action* >& actions) const{
	list<const Block*>::const_iterator it;

    const Space* sp=NULL;

    //cout << valid_blocks.size() << endl;

	while(cont->spaces->size()>0 && actions.size()==0){


	    sp=&cont->spaces->top();
	    //cout << "spaces:" << cont->spaces->size() << endl;

		for(it = valid_blocks.begin();it!=valid_blocks.end();it++)
			if(**it <= sp->getDimensions()) actions.push_back(new clpAction(**it,*sp));

		//the space is removed
		if(actions.size()==0){
			//cout << "to delete: " << *sp << ";" << sp << endl;
			cont->spaces->pop();
		}
	}
}


void clpState::compute_supports(map<const AABB*,int>& n_supports, set<const AABB*>& zero_support_aabb){
	set<const AABB*> visited;

    //we calculate the number of supported blocks for each block
    const AABB* block = &cont->blocks->top();
    int i=0;
    while(true){
        visited.insert(block);
     //   original_order[block]=i++;

        if(n_supports.find(block)==n_supports.end()) zero_support_aabb.insert(block);

        list<const AABB*> adj = cont->blocks->get_intersected_objects(*block);
        for( auto aabb : adj )
            if(visited.find(aabb)==visited.end()) n_supports[aabb]++;

        if(cont->blocks->has_next()) block = &cont->blocks->next();
        else break;
    }
}

void clpState::update_supports(const AABB* block,
		map<const AABB*,int>& n_supports, set<const AABB*>& zero_support_aabb){

	zero_support_aabb.erase(block);
    list<const AABB*> adj = cont->blocks->get_intersected_objects(*block);
    for( auto aabb : adj )
        if(n_supports.find(aabb)!=n_supports.end()) {
            n_supports[aabb]--;
            if(n_supports[aabb]==0) {
                zero_support_aabb.insert(aabb);
                n_supports.erase(aabb);
            }
        }
}

State* clpState::create_neighbor(State* s0){
	clpState* s1=(clpState*) s0->clone();

	int n=path.size()/2;
	//we recover the partial state between s0 and best_state
	for(int j=0; j<n; j++)
		s1->transition(*s1->next_action(*this));

	list<const Block*> boxes;
	for(auto block:s1->valid_blocks)
		if(block->n_boxes==1) boxes.push_back(block);

	s1->valid_blocks=boxes;

	return s1;
}

bool clpState::left=true;
int clpState::shuffle_path() {

    //number of support blocks for each block
    map<const AABB*,int> n_supports;

    //set of blocks with no supports
    set<const AABB*> zero_support_aabb;

    compute_supports(n_supports, zero_support_aabb);

    list<const Action*> new_path;

    const AABB* block = &cont->blocks->top();
    //se genera el path colocando todos los bloques de la izquierda de cont
    while(true){
    	if(((left && block->getXmin() <= cont->getL() - block->getXmax())||
    			(!left && block->getXmin() > cont->getL() - block->getXmax())) &&
    			zero_support_aabb.find(block)!=zero_support_aabb.end()){


    		 new_path.push_back(new clpAction(*block, *cont));

    		 update_supports(block, n_supports, zero_support_aabb);
    	}

        if(cont->blocks->has_next()) block = &cont->blocks->next();
        else break;
    }

    int i=new_path.size();

    block = &cont->blocks->top();
     //se coloca el resto de loc bloques en el path
     while(true){
     	if(zero_support_aabb.find(block)!=zero_support_aabb.end()){

     		 new_path.push_back(new clpAction(*block, *cont));

     		 update_supports(block, n_supports, zero_support_aabb);
     	}

         if(cont->blocks->has_next()) block = &cont->blocks->next();
         else break;
     }

    while(!path.empty()){
		delete path.front();
		path.pop_front();
	}

    path=new_path;

    left= !left;

    return i;
}

/* TODO: Creacion aleatoria reproducible de pesos para cajas
long rand(long seed)
{
  long a = 1103515245;
  long m = 2<<32;
  long c = 12345;

  seed = (a * seed + c) % m;
  return seed;
}*/

double clpState::weight_of_allboxes=0.0;


clpState* new_state(string file, int i, double min_fr, int max_bl, clpState::Format f){

  clpState::weight_of_allboxes=0.0;

	ifstream in(file.c_str());
	string line;
	if(f==clpState::BR || f==clpState::BRw){
		getline(in,line); //number of instances
	}

	clpState *s=NULL;

	for(int inst=0;inst<=i; inst++){
		string line;

		if(f==clpState::BR || f==clpState::BRw){
			getline(in, line ); //n_inst best_sol?
		}

		getline(in, line); //L W H

		if(inst==i){
			std::stringstream ss(line);
			long l,w,h;
			ss >> l >> w >> h;
			cout << l << " " <<  w << " " << h << endl;
			if(f==clpState::_1C) {l*=10; w*=10; h*=10;}
			s= new clpState((Block::FSB)? new Block_fsb(l,w,h):new Block(l,w,h));
		}

		if(f==clpState::_1C){
			getline(in, line );
			std::stringstream ss1(line);
			ss1 >> clpState::weight_of_allboxes;
		}


		getline(in, line); //types of boxes

		std::stringstream ss0(line);
		int nb_types;
		ss0 >> nb_types;

		//se lee el archivo de entrada
		//Objetos BoxType, guardan los datos para cada tipo de cajas: dimensiones (w x l x h)
		//y restricciones de rotación
		//En el objeto clp se agregan los tipos de cajas y el número de elementos que hay de cada tipo

		//clpState::weight_of_allboxes=0.0;

		for(int j=0;j<nb_types;j++){
			getline(in, line );


			int n, id;
			long l,h,w;
			double weight = 1.0;
			double vol;
			bool rot1, rot2, rot3;
			std::stringstream ss1(line);

			if(f==clpState::BR){

				ss1 >> id >> l >> rot1 >> w >> rot2 >> h >> rot3 >> n;

				cout << l << " " << w << " " << h << endl;
				vol=l*h*w;
			}if(f==clpState::BRw){
				ss1 >> id >> l >> rot1 >> w >> rot2 >> h >> rot3 >> n >> weight;
				vol=l*h*w;
			}else if(f==clpState::_1C){
				double ll,hh,ww;
				ss1 >> ll >> rot1 >> ww >> rot2 >> hh >> rot3 >> weight >> n;
				ll*=10; ww*=10; hh*=10;
				l = ceil(ll);
				w = ceil(ww);
				h = ceil(hh);
				vol=ll*hh*ww;
			}



			if(inst==i){

				BoxShape* boxt=new BoxShape(id, l, w, h, rot1, rot2, rot3, weight);

				if (f==clpState::BR || f==clpState::BRw) clpState::weight_of_allboxes += weight*(double) n;

				s->nb_left_boxes.insert(make_pair(boxt,n));
				for(int o=0; o<6; o++){
					if(boxt->is_valid((BoxShape::Orientation) o)){
						if(!Block::FSB)
							s->valid_blocks.push_back(new Block(*boxt,(BoxShape::Orientation) o, vol));
						else
							s->valid_blocks.push_back(new Block_fsb(*boxt,(BoxShape::Orientation) o, vol));
					}
				}
				//cout << "Wmax:" << clpState::weight_of_allboxes << endl;
			}
		}

	}

	s->update_min_dim();
	cout << s->mindim << endl;
	s->generate_optimal_solutions(*s->cont);

	cout << "Test" << endl;
	s->general_block_generator(min_fr, max_bl, *s->cont);
	cout << "Test2" << endl;

	cout << "Test3" << endl;
	return s;
}


lint clpState::solve(solution ***sols, const Vector3& max_dim){
	lint xyz = (max_dim.getX())*(max_dim.getY())*(max_dim.getZ());
	solution opt;
	for(lint j=0; j<xyz; j++){
		int z=j%max_dim.getZ()+1;
		lint jj=j/(max_dim.getZ());
		int y=jj%max_dim.getY()+1;
		jj/=(max_dim.getY());
		int x=jj+1;
		if(sols[x][y][z].value!=-1) continue;


	    opt.value = 0;
	    opt.choice_dim = -1;
	    opt.choice_cut = -1;
	    opt.base_box = -1;



		if(x< mindim.getX() || y< mindim.getY() || z<mindim.getZ()){
			sols[x][y][z]=opt;
			continue;
		}

		long opt_value=-1;
		bool add=false;

	    for(auto p:sols_x[y][z]){
	    	int i=p.first;
	    	auto up = sols_x[y][z].upper_bound(x-i);

	    	long vol2=0;
	    	if(up != sols_x[y][z].begin() ){
	    		up--;
	    		vol2=up->second;
	    	}


	    	lint res = p.second + vol2;
	    	if(res >= opt_value ){
	    		if (res == p.second || res == vol2) add=false;
	    		else if(res>opt_value) add=true;
	    		opt_value=res;
	    	}else continue;
	    }

	    for(auto p:sols_y[x][z]){
	    	int i=p.first;
	    	auto up = sols_y[x][z].upper_bound(y-i);
	    	long vol2=0;
	    	if(up != sols_y[x][z].begin() ){
	    		up--;
	    		vol2=up->second;
	    	}

	    	lint res = p.second + vol2;
	    	if(res >= opt_value ){
	    		if (res == p.second || res == vol2) add=false;
	    		else if(res>opt_value) add=true;
	    		opt_value=res;
	    	}
	    }

	    for(auto p:sols_z[x][y]){
	    	int i=p.first;
	    	auto up = sols_z[x][y].upper_bound(z-i);
	    	long vol2=0;
	    	if(up != sols_z[x][y].begin() ){
	    		up--;
	    		vol2=up->second;
	    	}


	    	lint res = p.second + vol2;
	    	if(res >= opt_value ){
	    		if (res == p.second || res == vol2) add=false;
	    		else if(res>opt_value) add=true;
	    		opt_value=res;
	    	}
	    }

	    if(add && opt_value>-1){
			sols[x][y][z].value=opt_value;
			cout << "(" << x<< "," << y<< "," <<z << "):" << opt_value << endl ;
            sols_x[y][z].insert(make_pair(x,opt_value));
            sols_y[x][z].insert(make_pair(y,opt_value));
            sols_z[x][y].insert(make_pair(z,opt_value));
	    }



	}

	return 0;

}


void clpState::get_boxes(const solution ***sols, int a, int b, int c, int *ids, int *n_ids){
    solution sol = sols[a][b][c];
    // This is a base case, a given box
    if(sol.base_box!=-1){
        ids[*n_ids] = sol.base_box;
        *n_ids += 1;
        return;
    }
    // No more cuts:
    if(sol.choice_dim==-1) return;
    // Recursive call
    if(sol.choice_dim==0){
        get_boxes(sols,sol.choice_cut,b,c,ids,n_ids);
        get_boxes(sols,a-sol.choice_cut,b,c,ids,n_ids);
    }
    else if(sol.choice_dim==1){
        get_boxes(sols,a,sol.choice_cut,c,ids,n_ids);
        get_boxes(sols,a,b-sol.choice_cut,c,ids,n_ids);
    }
    else if(sol.choice_dim==2){
        get_boxes(sols,a,b,sol.choice_cut,ids,n_ids);
        get_boxes(sols,a,b,c-sol.choice_cut,ids,n_ids);
    }
}


void clpState::generate_optimal_solutions(const Vector3& max_dim){
    // Scan problem dimensions
    int s_x=max_dim.getX(),s_y=max_dim.getY(),s_z=max_dim.getZ();
    // Scan boxes
    int n_boxes=valid_blocks.size();
    cout << n_boxes << endl;
    int *boxes = new int[3*n_boxes];
    int p=0;
    for(auto b:valid_blocks){
        boxes[3*p+0] = b->getL();
        boxes[3*p+1] = b->getW();
        boxes[3*p+2] = b->getH();
        p++;
    }
    // Initialize matrix of precomputed problems

    sols = new solution**[s_x+1];
    for(int i=0;i<=s_x;i++){
        sols[i] = new solution*[s_y+1];
        for(int j=0;j<=s_y;j++){
            sols[i][j] = new solution[s_z+1];
            for(int k=0;k<=s_z;k++){
                sols[i][j][k].value = -1;
                sols[i][j][k].choice_dim = -1;
                sols[i][j][k].choice_cut = -1;
                sols[i][j][k].base_box = -1;
            }
        }
    }

    sols_z = new map<int,long>*[s_x+1];
    for(int i=0;i<=s_x;i++)
    	sols_z[i] = new map<int,long>[s_y+1];

    sols_x = new map<int,long>*[s_y+1];
    for(int i=0;i<=s_y;i++)
    	sols_x[i] = new map<int,long>[s_z+1];

    sols_y = new map<int,long>*[s_x+1];
    for(int i=0;i<=s_x;i++)
    	sols_y[i] = new map<int,long>[s_z+1];


    // Base cases (NOTE: curiously size 0 is not really used)
    for(int i=0;i<=s_x;i++) sols[i][0][0].value = 0;
    for(int j=0;j<=s_y;j++) sols[0][j][0].value = 0;
    for(int k=0;k<=s_z;k++) sols[0][0][k].value = 0;
    sols[1][1][1].value = 0;
    int max_max_boxes = 0; // Limit for the number of possible boxes.
    for(int p=0;p<n_boxes;p++){
        int a = boxes[3*p+0];
        int b = boxes[3*p+1];
        int c = boxes[3*p+2];
        long volume = a*b*c;
        if(a<=s_x && b<=s_y && c<=s_z){
            sols[a][b][c].value = volume;
            sols[a][b][c].base_box = p;

            sols_x[b][c].insert(make_pair(a,volume));
            sols_y[a][c].insert(make_pair(b,volume));
            sols_z[a][b].insert(make_pair(c,volume));
        }

        // Check if with this boxes more are possible:
        int max_boxes = ((lint)s_x*(lint)s_y*(lint)s_z)/volume;
        if(max_max_boxes<max_boxes) max_max_boxes = max_boxes;
    }
    // Free boxes
    free(boxes);
    // Get optimal value
    //lint res = solve_cut(sols,s_x,s_y,s_z);
    lint res = solve(sols,max_dim);

    printf("value: %lld\n",res);
    exit(0);
    // Get the chosen boxes
    int n_indexes = 0;
    int *indexes = new int[max_max_boxes];
    get_boxes((const solution ***)sols,s_x,s_y,s_z,indexes,&n_indexes);
    printf("indices: ");
    for(int i=0;i<n_indexes;i++){
        printf("%d ",indexes[i]);
    }
    printf("\n");
    free(indexes);


}

void clpState::general_block_generator(double min_fr, int max_bl, const Vector3& max_dim){

    list<const Block*>& B=valid_blocks;
    list<const Block*> P=B;
	while(B.size()<max_bl){
		list<const Block*>  N;
		list<const Block*> :: iterator itP=P.begin();
	    int new_elems=0;
	    for(;itP!=P.end() && B.size()+new_elems<max_bl; itP++){
	    	list<const Block*> :: iterator itB=B.begin();
	    	for(;itB!=B.end() && B.size()+new_elems<max_bl ; itB++){

	    		//cout << "new_blocks" << endl;
				list<const Block*> newB = (*itP)->create_new_blocks(*itB, min_fr, max_dim);


				list<const Block*>:: iterator itNew=newB.begin();
				for(;itNew!=newB.end();itNew++){
				   if(is_constructible(*this,**itNew) && **itNew <= *cont){
					  int NoldSize=N.size();
					  N.push_back(*itNew);
					  new_elems++;

					  if(B.size()+new_elems>=max_bl) break;
				   }else { /*delete *itNew;*/ }
				}
	    	}
	    }
		if(N.size()==0) break;
		B.insert(B.end(),N.begin(),N.end());
		P=N;
	}
}



void clpState::update_min_dim(){
	map<const BoxShape*, int>::const_iterator it= nb_left_boxes.begin();
	for(;it!=nb_left_boxes.end();it++){
		if(it->second >0){
			for(int o=0; o<6; o++)
				if(it->first->is_valid((BoxShape::Orientation) o)){
					if(mindim.getX() > it->first->getL((BoxShape::Orientation) o) )
						mindim.setX(it->first->getL((BoxShape::Orientation) o));

					if(mindim.getY() > it->first->getW((BoxShape::Orientation) o) )
						mindim.setY(it->first->getW((BoxShape::Orientation) o));

					if(mindim.getZ() > it->first->getH((BoxShape::Orientation) o) )
						mindim.setZ(it->first->getH((BoxShape::Orientation) o));
				}
		}
	}
}



void clpState::_transition(const Action& action) {
	const clpAction& act = *dynamic_cast<const clpAction*> (&action);

	const Block& b= act.block;

	//se actualizan las cajas restantes
	bool update=false;
	map<const BoxShape*, int>::const_iterator it= b.nb_boxes.begin();
	for(;it!=b.nb_boxes.end();it++){
		nb_left_boxes[it->first] -= it->second;
		if(nb_left_boxes[it->first]==0) update=true;
	}
	//if(update) update_min_dim();

	//se inserta el bloque en el contenedor
	cont->insert(b, act.space.get_location(b), mindim);

	//se actualizan los bloques validos
	update_valid_blocks();
}

bool is_constructible(const clpState& s, const Block& b){
	map<const BoxShape*, int>::const_iterator it= b.nb_boxes.begin();
	for(;it!=b.nb_boxes.end();it++)
		if(s.nb_left_boxes.at(it->first) < it->second)
			return false;

	return true;
}


}
