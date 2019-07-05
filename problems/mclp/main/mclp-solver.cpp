/*
 * main_clp.cpp
 *
 *  Created on: 29 jun. 2017
 *      Author: iaraya
 */

#include <iostream>
#include <fstream>
#include <unistd.h>

#include <dirent.h>
#include <malloc.h>
#include <iterator>

#include "args.hxx"
//#include "objects/State.cpp"
#include "mclp-state.h"
#include "BlockSet.h"
#include "Greedy.h"
#include "DoubleEffort.h"
#include "GlobalVariables.h"
#include "BSG.h"

bool global::TRACE = false;

using namespace std;


void dfsPrintChild(const State* node, ofstream& file){
	file << "{ "<<endl;
	file<<"\t \"name\":\""<<node->get_id()<<"\",";
	file<<"\t \"parent\":\""<<node->get_parent()->get_id() <<"\",";
	file<<"\t \"value\":\""<<node->get_value() <<"\",";
	file<<"\t \"sd\":\"\",";
	file<<"\t \"mcts_value\":\"\",";
	file<<"\t \"stimated_sd\":\"\",";
	file<<"\t \"ponderated_sd\":\"\",";
	file<<"\t \"depth\":\"\",";
	file<<"\t \"num_visit\":\"\"";
	file<< "\t,\"simulations\":[]"<<endl;


	if(!node->get_children().empty()){
		file<< "\t,\"children\":["<<endl;
		for(auto c:node->get_children()){
			dfsPrintChild(c,file);
			if(c!=node->get_children().back())
			    file<<","<<endl;
		}
		file<<"]";
	}
	file<<"}"<<endl;
}

void pointsToTxt(State* root, int it) {
	ofstream scp("problems/clp/tree_plot/flare"+std::to_string(it)+".json");
	dfsPrintChild(root,scp);
}

string findInto(string current_directory, string find_directory){
	DIR* dir;
	struct dirent *ent;
	dir = opendir(current_directory.c_str());
	if(dir != NULL){
		while((ent = readdir(dir)) != NULL){
			if(((string)ent->d_name).compare(find_directory) == 0){
				//break;
				closedir(dir);
				//cout << current_directory + find_directory << endl;
				return current_directory + find_directory;
			}
			if(((string)ent->d_name).find(".") == string::npos){
				findInto(current_directory + "/" + ent->d_name, find_directory);
				//cout << ent->d_name << endl;
				//cout << current_directory << endl;
				//cout << endl;
			}
		}
	}
	closedir(dir);
	//cout << current_directory + find_directory << endl;
	return "";
}

string findOut(string current_directory, string find_directory){
	DIR* dir;
	struct dirent *ent;
	current_directory += "/../";
	dir = opendir(current_directory.c_str());
	for(int i = 0; i < 5; i++){
		while((ent = readdir(dir)) != NULL){
			if(((string)ent->d_name).compare(find_directory) == 0){
				closedir(dir);
				//cout << current_directory + find_directory << endl;
				return current_directory + find_directory +"/";
			}
		}
		current_directory += "../";
		dir = opendir((current_directory).c_str());

	}
	closedir(dir);
	return "";
}

string findDirectory(string current_directory, string find_directory){
	string find = findInto(current_directory, find_directory);
	if(!find.empty()) return find;
	else {
		find = findOut(current_directory, find_directory);
		if(!find.empty()) return find;
	}
	return "";
}

/*
//Obtiene la ubicacion actual desde la raiz
char cwd[2048];
if (getcwd(cwd, sizeof(cwd)) != NULL) {
	printf("Current working dir: %s\n", cwd);
} else {
	perror("getcwd() error");
}*/

void exportToTxtSCP(list < pair <double, set<int>> >* bins,
		set<int>* used_boxes, long int nb_boxes){

	string path = findDirectory(".", "GRASP-SCP"), filename = "bins_scp" + to_string(getpid()) + ".txt";

	if(path.empty()){
		cout << "El directorio no existe.\n" << endl;
		exit(0);
	}
	cout << "Resultados almacenados en " << path << filename << endl;
	ofstream scp (path + filename);

	if (scp.is_open()){
		long int total_boxes = 0;

		//Bins quantity
		scp << " " << bins->size() << " ";

		//Boxes quantity
		scp << nb_boxes << "\n";

		//Matrix cost by boxes
		long int cont = 0;
		for(auto bin: *bins){
			if(cont >= 12){
				scp << "\n";
				cont = 0;
			}
			scp << " 1";
			cont += 1;
		}
		scp << "\n";

		//Boxes quantity in a set and then sets boxes
		for(auto bin: *bins){
			scp << " " << bin.second.size() << "\n";
			for(auto box: bin.second){
				scp << " " << box + 1;
			}
			scp << "\n";
		}

		scp.close();
	} else cout << "Unable to open file" << endl;
}

void run_GRASP_SCP(){

	string path = findDirectory(".", "GRASP-SCP"), filename = "bins_scp" + to_string(getpid()) + ".txt";
	if(path.empty()){
		cout << "El directorio no existe.\n" << endl;
		exit(0);
	}
	const string MAX_TIME = "10";
	const string SEED = "1";
	string run = string(path + "GRASP-SCP " + path + filename + " ") + MAX_TIME + string(" ") + SEED;

	FILE *p = popen(run.c_str(), "r");

	if(p != NULL) {
		cout << endl;
		cout << run << endl;

		cout << "running GRASP-SCP" << endl;
		cout << "Time: " << MAX_TIME << endl;
		cout << "Seed: " << SEED << endl;

		char output[100];
		string str;
		vector <string> line;

		while(fgets(output, sizeof(output), p) != NULL) { }

		str = output;
		string delimiter = " ";
		size_t pos = 0;
		string token;
		while ((pos = str.find(delimiter)) != string::npos) {
			token = str.substr(0, pos);
			str.erase(0, pos + delimiter.length());
			line.push_back(token);
		}
		for(int i = 1; i < line.size(); i++)
			cout << line[i] << " " << str;
	} else {
		perror("Unable to open file");
	}
	pclose(p);

	//FILE * scp = fopen((path + filename).c_str(), "w");
	remove((path + filename).c_str());
}

/**
 * Si son iguales retorna true
 */
bool compare_sets(set<int> set1, set<int> set2){

	if(set1.size() == set2.size()){
		set<int>::iterator it1 = set1.begin(), it2 = set2.begin();
		while(it1 != set1.end() && it2 != set2.end()){
			if(*it1 == *it2)
				return true;
			it1++;
			it2++;
		}
	}
	return false;
}


/*Clonar estado inicial
Aplicar Greedy y obtener contenedor
Verificar si el contenedor ya existe, si no agregarlo a lista de contenedores (bins)
Reducir peso de las cajas utilizadas en el contenedor
Volver a 1*/

int solve(Greedy* gr, BSG *bsg, mclpState* s0, int nbins, double pdec){
	mclpState::initalize_priorities();
	list < pair <double, set<int>> > bins;
	set<int> used_boxes;
	set<int> inserted_bins;
	int box_quantity = 0;

	for(int i=0; i < nbins || (s0->nb_left_boxes.size() > used_boxes.size()); i++){
		//copia el estado base
		mclpState& s_copy= *dynamic_cast<mclpState*>(s0->clone());

		//usa greedy para llenar contenedor
		double eval=bsg->run(s_copy);

		dynamic_cast<const mclpState*>(bsg->get_best_state())->update_volumes(pdec,
				&dynamic_cast<const mclpState*>(bsg->get_best_state())->cont->nb_boxes);

		//se almacena el bin en el conjunto
		for(auto box: dynamic_cast<const mclpState*>(bsg->get_best_state())->cont->nb_boxes){
			inserted_bins.insert(box.first->get_id());
			used_boxes.insert(box.first->get_id());
			//cout << box.first->get_id() + 1 << ", ";
		}
		//cout << endl;

		bool insert_bin = true;
		for(auto bin: bins){
			//Descomentar si se requiere que eval y bin.first sean iguales para realizar la busqueda de duplicados
			//if(eval == bin.first){
				//cout << eval << " = " << bin.first << endl;
				if(compare_sets(inserted_bins, bin.second) == true){
					//cout << "set1 = set2" << endl;
					insert_bin = false;
				}
			//}
		}
		if(insert_bin && !inserted_bins.empty())
			bins.push_back(make_pair(eval, inserted_bins));
		inserted_bins.clear();

	}

	cout << "used_boxes" << endl;


	for(auto bin: bins){
		cout << bin.first << " ";
		for(auto box: bin.second)
			cout << box << " ";
		cout << endl;
	}

	exportToTxtSCP(&bins, &used_boxes, s0->nb_left_boxes.size());
	run_GRASP_SCP();

	return bins.size();

}

int main(int argc, char** argv){

	args::ArgumentParser parser("********* BSG-CLP *********.", "BSG Solver for CLP.");
	args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
	args::ValueFlag<int> _inst(parser, "int", "Instance", {'i'});
	args::Flag _rotate(parser, "rotate", "rotations are allowed?", {"rot"});
	args::ValueFlag<int> _nboxes(parser, "int", "Number of boxes for each type", {"nboxes"});
	args::ValueFlag<int> _nbins(parser, "int", "Number of bins to be generated", {"nbins"});
	args::ValueFlag<double> _pdec(parser, "int", "Priority decreasing ratio", {"pdec"});
	args::ValueFlag<double> _min_fr(parser, "double", "Minimum volume occupied by a block (proportion)", {"min_fr"});
	args::ValueFlag<int> _maxtime(parser, "int", "Timelimit", {'t', "timelimit"});
	args::ValueFlag<int> _seed(parser, "int", "Random seed", {"seed"});
	args::ValueFlag<double> _alpha(parser, "double", "Alpha parameter", {"alpha"});
	args::ValueFlag<double> _beta(parser, "double", "Beta parameter", {"beta"});
	args::ValueFlag<double> _gamma(parser, "double", "Gamma parameter", {"gamma"});
	args::ValueFlag<double> _delta(parser, "double", "Delta parameter", {"delta"});
	args::ValueFlag<double> _p(parser, "double", "p parameter", {'p'});
	args::Flag _plot(parser, "double", "plot tree", {"plot"});
	//args::Flag fsb(parser, "fsb", "full-support blocks", {"fsb"});
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

	double alpha=3.0, beta=2.0, gamma=0.5, delta=1.0, p=0.04, pdec=0.8;
	int nbins=1000;
	int nboxes=1;
	if(_maxtime) maxtime=_maxtime.Get();
	if(_alpha) alpha=_alpha.Get();
	if(_beta) beta=_beta.Get();
	if(_gamma) gamma=_gamma.Get();
	if(_delta) delta=_delta.Get();
	if(_p) p=_p.Get();
	if(_nboxes) nboxes=_nboxes.Get();
	if(_pdec) pdec=_pdec.Get();
	if(_nbins) nbins=_nbins.Get();

	int seed=(_seed)? _seed.Get():1;
	srand(seed);

	global::TRACE = trace;

 cout << "cargando la instancia..." << endl;

//a las cajas se les inicializan sus pesos en 1

	cout << "***** Creando el contenedor ****" << endl;
	cout << "Instance:" << inst+1 << endl;
	cout << "min_fr:" << min_fr << endl;
	cout << "Maxtime:" << maxtime << endl;
	cout << "Rotations allowed:" << _rotate << endl;
	cout << "N_bins:" << nbins << endl;
	cout << "Decreasing ratio (priority):" << pdec << endl;

    mclpState* s0 = new_mstate(file,inst, min_fr, 10000, _rotate, nboxes);

    cout << "n_blocks:"<< s0->get_n_valid_blocks() << endl;

    clock_t begin_time=clock();


    VCS_Function* vcs = new VCS_Function(s0->nb_left_boxes, *s0->cont,
        alpha, beta, gamma, p, delta, 0.0, 0.0);

	cout << "greedy" << endl;
	Greedy *gr = new Greedy (vcs);


	cout << "bsg" << endl;
	BSG *bsg= new BSG(vcs,*gr, 4, 0.0, 0, _plot);
	bsg->trace=false;


   	int bins=solve(gr, bsg, s0, nbins, pdec);

	//if(_plot){
	//   pointsToTxt(&s_copy, 0);
	//   system("firefox problems/clp/tree_plot/index.html");
	//}

}
