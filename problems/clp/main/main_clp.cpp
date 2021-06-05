/*
 * main_clp.cpp
 *
 *  Created on: 29 jun. 2017
 *      Author: iaraya
 */

#include <iostream>
#include <fstream>
#include "args.hxx"
//#include "objects/State.cpp"
#include "clpState.h"
#include "BlockSet.h"
#include "VCS_Function.h"
#include "SpaceSet.h"
#include "Greedy.h"
#include "DoubleEffort.h"
#include "GlobalVariables.h"
#include "BSG.h"


bool global::TRACE = false;

using namespace std;
//structs de prueba
struct caja
{
	double peso;
	double volumen;
};


struct entrega
{
	caja cajas[3];	
};

struct tu
{
	list<entrega> entregas;
	double pesoMax = 115.9;
	double volMax = 271.9;
};

struct delivery
{
	list<BoxShape> boxes; //lista de cajas de la entrega
	int type; //id de la entrega
};



//funciones de prueba
bool hay_peso_volumen(double,double,double,double);
double pesoEntrega(entrega *);
double volumenEntrega(entrega *);
void addArrayCaja(caja *, int *, caja *, int);
void mostraArrayCaja(caja *, int);
void arraycero(caja *,int);
caja* extraerCajas(caja *, int, double, double);
//void nuevos_peso_volumen()

double bsg_solve(list<BoxShape*>& lb, long L, long W, long H, double Wmax, 
		double min_fr, double alpha, double beta, double gamma, double p, double delta, double r){
	//Se se pasa lb a un mapa y se envia a solucionarlo
	map<BoxShape*, int> lbmapa; 
	for(auto i : lb){
		if (lbmapa.find(i) == lbmapa.end()) lbmapa.insert({i,1});
		else lbmapa[i]++;
	}
	
	//se crea nodo raiz
	clpState* s0 = new_state(L, W, H, Wmax, lbmapa);
	s0->general_block_generator(min_fr, 10000, *s0->cont);
	s0->singlebox_blocks = new AABBList(); //for keeping boxes
	
	VCS_Function* vcs = new VCS_Function(s0->nb_left_boxes, *s0->cont,
 	alpha, beta, gamma, p, delta, 0.0, r);


	cout << "greedy" << endl;
	SearchStrategy *gr = new Greedy (vcs);

	cout << "bsg" << endl;
	BSG *bsg= new BSG(vcs,*gr, 8, 0.0, 0, false);

	cout << "running" << endl;

	clock_t begin_time=clock();
	double eval=bsg->run(*s0, 9999, begin_time) ;
	//identificar los contenedores que entraron en el tu --> S
	cout << "eval: " << eval << endl;

	const clpState* sbest = dynamic_cast<const clpState*> (bsg->get_best_state());
	map<const BoxShape*, int> lbmapa2  = sbest->nb_left_boxes;
	cout << lbmapa2.size() << endl;

	
	lb.clear();

	for (auto b : lbmapa2){
		const BoxShape* box = b.first;
		int n = b.second;
		for(int i=0; i<n; i++) lb.push_back((BoxShape*) box);
	} 		

	/*
	for(AABB aabb = sbest->cont->blocks->top(); sbest->cont->blocks->has_next(); aabb = sbest->cont->blocks->next()){
		cout << "block:" << aabb << endl;
		for(auto aabb2 :aabb.getBlock()->aabb_bloxs){
			AABB aabb3(aabb2.getMins()+aabb.getMins(), aabb2.getBlock());
			cout << "-->" << aabb3 << endl;
			if(aabb3.getZmin()==120){
				for(auto aabb4 : sbest->get_adjacent_aabbs(aabb3, clpState::adj_type::DOWN,0))
					cout << "  ---->down" << aabb4 << endl;
			}
		}

	}*/





	

	return sbest->cont->getOccupiedVolume();
}

list<BoxShape*> get_delivery_boxes(list<BoxShape*>& all_boxes, int d){
	list<BoxShape*> lr;
	for (auto  b : all_boxes)
		if (b->get_type() == d) 
			lr.push_back(b);

	return lr;
}

int main(int argc, char** argv){


	args::ArgumentParser parser("********* BSG-CLP *********.", "BSG Solver for CLP.");
	args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
	args::ValueFlag<int> _inst(parser, "int", "Instance", {'i'});
	args::ValueFlag<string> _format(parser, "string", "Format: (BR, BRw, 1C, BRpc)", {'f'});
	args::ValueFlag<double> _min_fr(parser, "double", "Minimum volume occupied by a block (proportion)", {"min_fr"});
	args::ValueFlag<int> _maxtime(parser, "int", "Timelimit", {'t', "timelimit"});
	args::ValueFlag<int> _seed(parser, "int", "Random seed", {"seed"});
	args::ValueFlag<double> _alpha(parser, "double", "Alpha parameter", {"alpha"});
	args::ValueFlag<double> _beta(parser, "double", "Beta parameter", {"beta"});
	args::ValueFlag<double> _gamma(parser, "double", "Gamma parameter", {"gamma"});
	args::ValueFlag<double> _delta(parser, "double", "Delta parameter", {"delta"});
	args::ValueFlag<double> _p(parser, "double", "p parameter", {'p'});
	args::Flag _show_layout(parser, "layout", "Show the layout of the boxes in the best found solution", {"show_layout"});
	args::Flag _plot(parser, "double", "plot tree", {"plot"});


	args::Flag fsb(parser, "fsb", "full-support blocks", {"fsb"});
	args::Flag trace(parser, "trace", "Trace", {"trace"});
	args::Positional<std::string> _file(parser, "instance-set", "The name of the instance set");


	cout.precision(8);
	try
	{
		parser.ParseCLI(argc, argv);

	}
	catch (args::Help&)
	{
		std::cout << parser;
		return 0;
	}
	catch (args::ParseError& e)
	{
		std::cerr << e.what() << std::endl;
		std::cerr << parser;
		return 1;
	}
	catch (args::ValidationError& e)
	{
		std::cerr << e.what() << std::endl;
		std::cerr << parser;
		return 1;
	}

	string file=_file.Get();
	int inst=(_inst)? _inst.Get():0;
	double min_fr=(_min_fr)? _min_fr.Get():0.98;
	int maxtime=(_maxtime)? _maxtime.Get():100;

	double alpha=4.0, beta=1.0, gamma=0.2, delta=1.0, p=0.04, maxtheta=0.0;
	if(_maxtime) maxtime=_maxtime.Get();
	if(_alpha) alpha=_alpha.Get();
	if(_beta) beta=_beta.Get();
	if(_gamma) gamma=_gamma.Get();
	if(_delta) delta=_delta.Get();
	if(_p) p=_p.Get();


	string format="BR";
	if(_format) format=_format.Get();

	clpState::Format f;
	if(format=="BR")
		f=clpState::BR;
	else if(format=="BRw")
		f=clpState::BRw;
	else if(format=="BRpc")
		f=clpState::BRpc;
	else if(format=="1C")
		f=clpState::_1C;

	int seed=(_seed)? _seed.Get():1;
	srand(seed);

	global::TRACE = trace;


	cout << "***** Creando el contenedor ****" << endl;
	cout << "File("<< format <<"): " << file << endl;
	cout << "Instance:" << inst+1 << endl;
	cout << "min_fr:" << min_fr << endl;
	cout << "Maxtime:" << maxtime << endl;

	double r=0.0;

    Block::FSB=fsb;
	Space::bottom_up=true;
    //clpState* s0 = new_state(file,inst, min_fr, 10000, f);

	long L, W, H;
	double Wmax;

	//En este mapa se almacenan todas las cajas de la instancia junto a su cantidad
	map<BoxShape*, int> boxes;
	// se lee la instancia
	read_instance(L, W, H, Wmax, boxes, file, inst, f);
	clpState* s0;
	list<BoxShape*> lista; // lista de cajas
	
	//for usado para recorrer el mapa boxes, de aqui se sacan las cajas
	for (auto b : boxes){
		BoxShape* box = b.first;
		int n = b.second;
		
		//cout <<"tipo: " <<box->get_type() << endl;
		int tipo = box->get_type();	
		box->getVolume();
		box->get_weight();
		for(int i=0; i<n; i++) lista.push_back(box);
	}
	//for (auto i : lista){	cout << "box en lista: " << i->get_type() << endl;	}

	//Ordena la lista con las cajas usando el campo type
	lista.sort([] (BoxShape* a, BoxShape* b) {
		return a->get_type() < b->get_type();
	});
	
	//for (auto i : lista){	cout << "box en lista ordenada: " << i->get_type() << endl;	}
	int cont = 0;
	bool rvacio = false; //para controlar el loop 1
	bool dloop = true; //para controlar el loop 2
	
	//id del primer y ultimo cliente (d y dMax)
	int d = lista.front()->get_type();
	//cout << "Primer type en la lista: " << d << endl;
	int dMax = lista.back()->get_type();
	//cout << "Ultimo type en la lista: " << dMax << endl;

	int tu = 0;
	
	
	int bins = 0;

	double pesomax = Wmax;
	double volmax = L*W*H;
	cout << "peso y volumen maximos: " << pesomax << " , " << volmax << endl;
	
	//se obtiene lista de cajas del cliente d
	list<BoxShape*> lr = get_delivery_boxes(lista,d);
	cout << "Total boxes: " << lista.size() << " boxes" << endl;
	cout << "Delivery " << d << ":" << lr.size() << " boxes" << endl;
	
	//cajas candidatas para ir en el contenedor (selecccionadas)
	list<BoxShape*> lb;
	//peso y volumen de cajas selccionadas
	double pe=0.0;
	double ve=0.0;
	while (dloop){ //itera por los clientes		

		while(!lr.empty()){
			BoxShape* b = lr.front(); lr.pop_front();
			lb.push_back(b);
			pe = pe + b->get_weight();
			ve = ve + b->getVolume();
			
			//cajas en lb sobre pasan el peso o volumen máximo del contenedor
			if (pe > pesomax || ve > volmax || (lr.empty() && d==dMax) ) {
				cout << "Peso cajas: " << (double)pe/(double)pesomax ;
				cout << "\tVolumen cajas: " << (double)ve/(double)volmax << endl;

				//se resuelve el problema para el contenedor actual
				double vol=bsg_solve(lb, L, W, H, Wmax, min_fr, alpha, beta, gamma, p, delta, r);
				bins++; //contenedores
				cout << "remaining boxes: " << lb.size() << endl;
				pe=0.0;
				ve=0.0;
				for(auto b : lb){
					pe = pe + b->get_weight();
					ve = ve + b->getVolume();
				}
			}
			
		}

		d = d+1;
		if(d>dMax) dloop=false;
		else{
			lr=get_delivery_boxes(lista,d);
			cout << "Delivery " << d << ":" << lr.size() << " boxes" << endl;
		}

		
	}

	cout << "TUs: " << bins << endl;


	/*
	if(f==clpState::BRpc){
		if(dynamic_cast<const clpState*>(de->get_best_state())!=NULL){
			cout << "multidrop:" << dynamic_cast<const clpState*>(de->get_best_state())->multidrop() << endl;
			cout << "angle:" << dynamic_cast<const clpState*>(de->get_best_state())->loadbalanceA() << endl;
			cout << "completeShipment:" << dynamic_cast<const clpState*>(de->get_best_state())->completeshipment() << endl;
		}
	}*/
	




}




bool hay_peso_volumen(double px,double vx,double p,double v){

	if ((px > p) && (vx > v))
	{
		return true;
	}
	

	return false;
}

double pesoEntrega(entrega *e){
	double p = 0.0;
	for (int i = 0; i < 3; i++)
	{
		p = p + e->cajas[i].peso;
		//cout << "peso entrega e " << i << " : " << e->cajas[i].peso << endl;
		//cout << "nuevo peso: " << p << endl;
	}
	
	return p;

}
double volumenEntrega(entrega *e){
	double p = 0.0;
	for (int i = 0; i < 3; i++)
	{	
		//cout << "En funcion volumen caja " << i << " : " << e->cajas[i].volumen << endl;  
		p = p + e->cajas[i].volumen;

	}
	
	return p;

}
/*
void addArrayCaja(caja * a, int * ai, caja * b, int bi){

	for (int i = 0; i < bi; i++)
	{
		a[ai] = b[bi];
	}
	cout << "Se actualizo " << i+1 << " a "; 
	ai = ai + bi;
	cout << ai << endl;
	
}
*/
void mostraArrayCaja(caja * a, int b){
	//cout << "----------------------funcion------------------------ " << endl;
	for (int i = 0; i < b; i++)
	{
		//cout << "Jalea Funcion" << endl;
		cout << "Peso caja " << i+1 << " : " << a[i].peso;  
	
		cout << "\t \t Volumen caja " << i+1 << " : " << a[i].volumen << endl;
	}
	
}

void arraycero(caja *caja,int size){

	for (int i = 0; i <= size; i++)
	{
		caja->peso = 0.0;
		caja->volumen = 0.0;
		
	}	
}

caja* extraerCajas(caja * c, int size, double peso, double vol){
	caja caja[size];
	for (int i = 0; i < size; i++)
	{
		caja[i] = c[i];
	}
	
	return caja;
}