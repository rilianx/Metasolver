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
#include <algorithm>

#include "objects2/BoxShape.h"
#include "clpState.h"

using namespace std;
using namespace metasolver;

namespace clp {


void clpState::get_actions(list< Action* >& actions, int nsample) const{
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
	

	if(nsample!=0 && actions.size() > nsample){
		// probabilidad de eliminar acción actual
		double p = double(actions.size() - nsample) / double(actions.size());
		for(auto it=actions.begin(); it!=actions.end(); ){
			if(p < (double)rand()/(double)RAND_MAX ){
				auto it2=it; it++;
				delete *it2;
				actions.erase(it2);
				if (actions.size() == nsample) return;
			}else it++;
			if(it==actions.end()) it=actions.begin();
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
double clpState::profit_of_allboxes=0.0;
double clpState::density_of_allboxes=0.0;
double clpState::square_density_of_allboxes=0.0;
double clpState::Wmax=0.0;
int clpState::nb_boxes=0;
map <int,int> clpState::nb_boxes_by_type;

clpState* new_state(long L, long W, long H, double Wmax, map<BoxShape*, int>& boxes){
	clpState::weight_of_allboxes=0.0;
	clpState *s= new clpState(new Block(L,W,H));
	clpState::Wmax=Wmax;

	for (auto boxp : boxes){
		BoxShape* boxt = boxp.first;
		int n = boxp.second;
		int vol = boxt->getVolume();


		clpState::weight_of_allboxes += boxt->get_weight()*(double) n;
		clpState::profit_of_allboxes += boxt->get_profit()*(double) n;
		clpState::density_of_allboxes += (boxt->get_weight()/((vol)/1000000.0))* (double) n;
		clpState::square_density_of_allboxes += pow((boxt->get_weight()/((vol)/1000000.0)),2) * (double) n;
		clpState::nb_boxes += n;
		clpState::nb_boxes_by_type[boxt->get_type()] +=n;

		s->nb_left_boxes.insert(make_pair(boxt,n));
		for(int o=0; o<6; o++){
			if(boxt->is_valid((BoxShape::Orientation) o)){
				if(!Block::FSB)
					s->valid_blocks.push_back(new Block(*boxt,(BoxShape::Orientation) o, vol));
				else
					s->valid_blocks.push_back(new Block_fsb(*boxt,(BoxShape::Orientation) o, vol));
			}
		}
	}

	s->update_min_dim();
	return s;

}

void read_instance(long& L, long& W, long& H, double& Wmax, map<BoxShape*, int>& boxes,
				 string file, int i, clpState::Format f, clpState::FormatP fp){
	
	ifstream in(file.c_str());
	string line;
	if(f==clpState::BR || f==clpState::BRw || f==clpState::BRwp  || f==clpState::BRpc){
		getline(in,line); //number of instances
		
	}


	for(int inst=0;inst<=i; inst++){
		string line;

		if(f==clpState::BR || f==clpState::BRw || f==clpState::BRwp || f==clpState::BRpc){
			getline(in, line ); //n_inst random_seed
		}

		getline(in, line); //L W H

		if(inst==i){
			std::stringstream ss(line);
			ss >> L >> W >> H;
		}

		if(f==clpState::_1C || f==clpState::BRwp || f==clpState::BRpc){
			getline(in, line );
			std::stringstream ss1(line);
			ss1 >> Wmax;
			
		}


		getline(in, line); //types of boxes

		std::stringstream ss0(line);
		int nb_types;
		ss0 >> nb_types;

		//se lee el archivo de entrada
		//Objetos BoxType, guardan los datos para cada tipo de cajas: dimensiones (w x l x h)
		//y restricciones de rotación
		//En el objeto clp se agregan los tipos de cajas y el número de elementos que hay de cada tipo


		for(int j=0;j<nb_types;j++){
			getline(in, line );


			int n, id;
			long l,h,w;
			double ll,hh,ww;
			double weight = 1.0;
			double profit = 1.0;
			double vol;
			bool rot1, rot2, rot3;

			std::stringstream ss1(line);

			//Practical Constraints
			int type = 1; // tipo de caja (por contenido)
			double supported_weight_factor = 1.0; // peso que soporta: w*supported_weight_factor
			double v_stability; //factor de estabilidad vertical
			double h_stability; //factor de estabilidad horizontal
			int id_client =0;

			if(f==clpState::BR){

				ss1 >> id >> l >> rot1 >> w >> rot2 >> h >> rot3 >> n;

				//cout << l << " " << w << " " << h << endl;
				vol=l*h*w;
			}else if(f==clpState::BRw){
				ss1 >> id >> l >> rot1 >> w >> rot2 >> h >> rot3 >> n >> weight;
				vol=l*h*w;
			}else if(f==clpState::BRwp){
				ss1 >> id >> l >> rot1 >> w >> rot2 >> h >> rot3 >> n >> weight >> profit;
				if(fp==clpState::ALL_ONE) profit = 1;
				else if(fp==clpState::WEIGHT) profit = weight;
				vol=l*h*w;
			}else if(f==clpState::BRpc){
				ss1 >> id >> l >> rot1 >> w >> rot2 >> h >> rot3 >> n >> weight >> type >> supported_weight_factor >> h_stability >> h_stability >> v_stability >> id_client;
				vol=l*h*w;
			}
			else if(f==clpState::_1C){

				ss1 >> ll >> rot1 >> ww >> rot2 >> hh >> rot3 >> weight >> n;
				if(fp==clpState::ALL_ONE) profit = 1;
				profit = weight;
				vol=ll*hh*ww;
			}

			if(inst==i){
				BoxShape* boxt;
				if(f==clpState::_1C) boxt=new BoxShape(id, ll, ww, hh, rot1, rot2, rot3, weight, profit);
				else if(f==clpState::BRpc){
					boxt=new BoxShape(id, l, w, h, rot1, rot2, rot3, weight, profit, 
					type, supported_weight_factor*w, h_stability, v_stability, id_client);
				}else boxt=new BoxShape(id, l, w, h, rot1, rot2, rot3, weight, profit);

				boxes[boxt]=n;

				if(fp==clpState::WEIGHT)
					clpState::profit_of_allboxes = clpState::Wmax;
			}
		}

	}
}

clpState* new_state(string file, int i, double min_fr, int max_bl, clpState::Format f, clpState::FormatP fp){

  clpState::weight_of_allboxes=0.0;

	ifstream in(file.c_str());
	string line;
	if(f==clpState::BR || f==clpState::BRw || f==clpState::BRwp  || f==clpState::BRpc){
		getline(in,line); //number of instances
		
	}

	clpState *s=NULL;

	for(int inst=0;inst<=i; inst++){
		string line;

		if(f==clpState::BR || f==clpState::BRw || f==clpState::BRwp || f==clpState::BRpc){
			getline(in, line ); //n_inst random_seed
		}

		getline(in, line); //L W H

		if(inst==i){
			std::stringstream ss(line);
			long l,w,h;
			ss >> l >> w >> h;
			cout << l << " " <<  w << " " << h << endl;
			//if(f==clpState::_1C) {l*=10; w*=10; h*=10;}
			s= new clpState((Block::FSB)? new Block_fsb(l,w,h):new Block(l,w,h));
		}

		if(f==clpState::_1C || f==clpState::BRwp || f==clpState::BRpc){
			getline(in, line );
			std::stringstream ss1(line);
			ss1 >> clpState::Wmax;
			//cout << clpState::Wmax << endl;
			
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
			double ll,hh,ww;
			double weight = 1.0;
			double profit = 1.0;
			double vol;
			bool rot1, rot2, rot3;

			std::stringstream ss1(line);

			//Practical Constraints
			int type = 1; // tipo de caja (por contenido)
			double supported_weight_factor = 1.0; // peso que soporta: w*supported_weight_factor
			double v_stability; //factor de estabilidad vertical
			double h_stability; //factor de estabilidad horizontal
			double id_client;

			if(f==clpState::BR){

				ss1 >> id >> l >> rot1 >> w >> rot2 >> h >> rot3 >> n;

				//cout << l << " " << w << " " << h << endl;
				vol=l*h*w;
			}else if(f==clpState::BRw){
				ss1 >> id >> l >> rot1 >> w >> rot2 >> h >> rot3 >> n >> weight;
				vol=l*h*w;
			}else if(f==clpState::BRwp){
				ss1 >> id >> l >> rot1 >> w >> rot2 >> h >> rot3 >> n >> weight >> profit;
				if(fp==clpState::ALL_ONE) profit = 1;
				else if(fp==clpState::WEIGHT) profit = weight;
				vol=l*h*w;
			}else if(f==clpState::BRpc){
				ss1 >> id >> l >> rot1 >> w >> rot2 >> h >> rot3 >> n >> weight >> type >> supported_weight_factor >> h_stability >> h_stability >> v_stability >> id_client;
				vol=l*h*w;
			}
			else if(f==clpState::_1C){

				ss1 >> ll >> rot1 >> ww >> rot2 >> hh >> rot3 >> weight >> n;
				if(fp==clpState::ALL_ONE) profit = 1;
				profit = weight;
				vol=ll*hh*ww;
			}



			if(inst==i){
				//cout <<  id << l << rot1 << w << rot2 << h << rot3 << n << weight;
				//cout << type << supported_weight_factor << h_stability << h_stability << v_stability;
				BoxShape* boxt;
				if(f==clpState::_1C) boxt=new BoxShape(id, ll, ww, hh, rot1, rot2, rot3, weight, profit);
				else if(f==clpState::BRpc){
					boxt=new BoxShape(id, l, w, h, rot1, rot2, rot3, weight, profit, 
					type, supported_weight_factor*w, h_stability, v_stability, id_client);
				}else boxt=new BoxShape(id, l, w, h, rot1, rot2, rot3, weight, profit);

				clpState::weight_of_allboxes += weight*(double) n;
				clpState::profit_of_allboxes += profit*(double) n;
				clpState::density_of_allboxes += (weight/((l*w*h)/1000000.0))* (double) n;
				clpState::square_density_of_allboxes += pow((weight/((l*w*h)/1000000.0)),2) * (double) n;
				clpState::nb_boxes += n;
				clpState::nb_boxes_by_type[type] +=n;

				if(fp==clpState::WEIGHT)
					clpState::profit_of_allboxes = clpState::Wmax;

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

	//cout << "Test" << endl;
	s->update_min_dim();
	s->general_block_generator(min_fr, max_bl, *s->cont);
	//cout << "Test2" << endl;
	//cout << "Test3" << endl;
	return s;
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
				list<const Block*> newB = (*itP)->create_new_blocks(*itB, min_fr, max_dim, clpState::Wmax);


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



//MOMENTO 2
//TODO: Generar bloques por cliente
/*
void clpState::general_block_generator(double min_fr, int max_bl, const Vector3& max_dim){
	//while para cada cliente
	//en cada iteracion B debería inicializarse con bloques unitarios de cliente respectivo

	//Se extrae en lista2 todos los clientes
	list<int> lista2;
	for(auto bl : valid_blocks){
		map<const BoxShape*, int> nb_boxes = bl->nb_boxes;
		//for usado para recorrer el mapa nb_boxes
		for (auto b : nb_boxes){
			int n = b.second;
			int id = b.first->get_type();
			for(int i=0; i<n; i++) lista2.push_back(id);
		}
	}
	//Ordena la lista de clientes en orden ascendente
	lista2.sort();
	//Obtengo el primer y ultimo cliente
	int first_cl = lista2.front();
	int last_cl = lista2.back();
	list<const Block*> B2;

	bool loop = true;
	while (loop){ //itera por los clientes

		//se agrupan bloques pertenecientes a un cliente en client_blocks
		list<const Block*> client_blocks;
		for(auto bl : valid_blocks){
			auto iter = bl->nb_boxes.begin();
			if(iter->first->get_type() == first_cl){
				client_blocks.push_back(bl);
			}
		}

		list<const Block*>& B=client_blocks; //bloques con 1 caja dentro, pertenecientes al cliente d
		list<const Block*> P=B;
		//max_bl era generalmente 10000 bloques, ahora debe ser 1 para generar un unico bloque por cada conjunto
		//de bloques de un cliente
		//Guardar cada B en un B2 por cada ciclo, luego guardar en valid_blocks
		while(B.size()<max_bl){
			list<const Block*>  N;
			list<const Block*> :: iterator itP=P.begin();
			int new_elems=0;
			//se tratan de juntar pares bloques
			for(;itP!=P.end() && B.size()+new_elems<max_bl; itP++){
				list<const Block*> :: iterator itB=B.begin();
				for(;itB!=B.end() && B.size()+new_elems<max_bl ; itB++){

					//cout << "new_blocks" << endl;
					//se intentan crear  bloques juntando itP con itB (hasta 3 bloques)
					//min_fr: porcentaje mínimo que debe tener el bloque ocupado (98%, 100%).
					//TODO: dejar bloque más pesado abajo
					list<const Block*> newB = (*itP)->create_new_blocks(*itB, min_fr, max_dim, clpState::Wmax);


					list<const Block*>:: iterator itNew=newB.begin();
					for(;itNew!=newB.end();itNew++){
					if(is_constructible(*this,**itNew) && **itNew <= *cont){
						int NoldSize=N.size();
						N.push_back(*itNew);
						new_elems++;

						if(B.size()+new_elems>=max_bl) break;
					}else { //delete *itNew; }
					}
				}
			}
			if(N.size()==0) break;
			B.insert(B.end(),N.begin(),N.end());
			P=N;
		}

		first_cl = first_cl+1;
		if(first_cl>last_cl) loop=false;

		B2.insert(B2.begin(),B.begin(),B.end());

	}
	valid_blocks = B2;
}
*/


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

	//Filling the singlebox_blocks
	if(singlebox_blocks){
		list<const AABB*> aabbs;
		get_singlebox_AABBs(aabbs, &b,act.space.get_location(b));
		for(auto aabb:aabbs){
		   singlebox_blocks->insert(*aabb);
	  }
	}
}

bool is_constructible(const clpState& s, const Block& b){
	map<const BoxShape*, int>::const_iterator it= b.nb_boxes.begin();
	for(;it!=b.nb_boxes.end();it++)
		if(s.nb_left_boxes.at(it->first) < it->second)
			return false;

	return true;
}


}
