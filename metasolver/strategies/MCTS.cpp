/*
 * MCTS.cpp
 *
 *  Created on: 03-01-2018
 *      Author: investigador
 */

#include "MCTS.h"
namespace metasolver {

void Stimator::printPoints(){
	int size = points.size();
	cout <<"points"<<endl;
	for (int i = 0; i < size; i++){
		cout<< "volume: "<< points[i].first<<", upper: "<<points[i].second<<endl;
	}
}

double Stimator::getRSquared(){
	double sum_stimated=0;
	double sum_see=0;
	double sd_mean=0;
	double rsquared;
	int size=points.size();

	for(int i = 0;i<size;i++){
		sd_mean+=points[i].second;
	}
	sd_mean=sd_mean/size;
	for(int i=0;i<size;i++){
		sum_stimated+=pow(stimate_standart_deviation(points[i].first)-sd_mean,2);
		sum_see+=pow(points[i].second-sd_mean,2);
	}

	cout << "total sum stimated (sum (y*_i- mean))^2: "<<sum_stimated<<endl;
	cout << "total sum see (sum (y-mean))^2:"<< sum_see<<endl;

	rsquared = sum_stimated/sum_see;
	rsquared = 1 - rsquared;
	char c;
	cout << "rsquared: "<<rsquared<<endl;
	//cin >> c;
	return rsquared;
}

double Stimator::normal_cdf(double z) {
	using boost::math::normal;
	// typedef provides default type is double.
	normal s;
	double _cdf=cdf(s, z);
	//cout <<"cdf:"<< _cdf << endl;
	return _cdf;
}
double Stimator::getSlope() {
	//cout << "slope:"<< slope <<endl;
	return slope;
}

int Stimator::getNumPoints() {
	//cout << "slope:"<< slope <<endl;
	return points.size();
}
double Stimator::getVolI(int i) {
	//cout << "slope:"<< slope <<endl;
	return points[i].first;
}
double Stimator::getSdI(int i) {
	//cout << "slope:"<< slope <<endl;
	return points[i].second;
}
double Stimator::getIntercept() {
	//cout << "intercept:"<< intercept<<endl;
	return intercept;
}

void Stimator::restart(MctsNode* root) {
	//cout <<"restart regression "<<endl;
	points.clear();
	//addPoint(0, 1);
	//addPoint(root->get_sd(), root->get_num_simulations(), 0);
}

double Stimator::z_value(double mean, double sd, double x) {
	double z=(x - mean) / sd;
	//cout <<"z value: "<<z<<endl;
	return z;
}
double Stimator::stimate_standart_deviation(double volume) {
	double sd = volume * slope + intercept;
	//cout <<"slope, intercept: "<<slope << ", "<<intercept<<endl;
	//cout<<"sd = volume * slope + intercept="<<sd<<endl;
	return sd;
}
double Stimator::probability_greater_than(double best, double mean, double sd) {
	//cout<<"probability greater than "<<best<<"(best)"<<endl;
	//cout<<"mean: "<<mean<<" sd:"<<sd<<endl;
	double z = z_value(best, mean, sd);
	//cout<<"probability=1-normal_cdf(z)="<<1-normal_cdf(z)<<endl<<endl;
	return  normal_cdf(z);
}

void Stimator::addPoint(double sd, double num_simuls, double volume) {
	int N = num_simuls;
	//cout<<"simulations size:"<<N<<endl;
	using boost::math::chi_squared;
	using boost::math::quantile;
	using boost::math::complement;
	//double sd = standart_deviation(simulations);
	//cout<<"standart deviation of the simulations of this node:"<<sd<<endl;
	boost::math::chi_squared dist(N - 1);
	double upper_limit = sqrt((N - 1) * sd * sd / quantile(dist, alpha / 2));
	pair<double, double> point;
	//cout<<"upper limit:"<<upper_limit<<endl;
	point = make_pair(volume, upper_limit);
	points.push_back(point);
	//cout << "adding point to regression (volume, upper): "<<volume<<", "<< upper_limit<<endl;

	//update_coeficients();
}

void Stimator::addPoint( double volume,double upper) {

	pair<double, double> point;
	point = make_pair(volume, upper);
	points.push_back(point);
	//cout << "adding point to regression (volume, upper): "<<volume<<", "<< upper<<endl;
	//update_coeficients();
}

void Stimator::update_coeficients() {
	cout<<"updating coeficients of regression"<<endl;
	int n = points.size();
	//variables for sums/sigma of xi,yi,xi^2,xiyi etc
	cout <<"number of points: "<<n << endl;
	printPoints();
	double xsum = 0, x2sum = 0, ysum = 0, xysum = 0;
	for (int i = 0; i < n; i++) {
		xsum += points[i].first;
		ysum += points[i].second;
		x2sum += pow(points[i].first, 2);
		xysum += points[i].first * points[i].second;
	}
	cout<<"xsum: "<<xsum<<endl;
	cout<<"ysum: "<<ysum<<endl;
	cout<<"x2sum: "<<x2sum<<endl;
	cout<<"xysum: "<<xysum<<endl;
	slope = (n * xysum - xsum * ysum) / (n * x2sum - xsum * xsum);
	intercept = (x2sum * ysum - xsum * xysum) / (x2sum * n - xsum * xsum);
	cout <<"new regression: sd=vol*"<<slope<<" + "<<intercept<<endl;
}



double MCTS::run(State& beginState, double tl, clock_t bt = clock()) {

	bool echo=true;
	srand(time(NULL));

	begin_time = bt;
	timelimit = tl;
	root = new MctsNode(beginState);
	get_best_actions(*root->getState(),root->actions,50);

	int expansions = 1;
	best_node = root;
	vector<MctsNode*> expansiones;

	nodes.push_back(root);
	firstSimulations(root, bt, tl);

	while (true) {
		//iterationToTxt();
		pointsToTxt();
		iterationToTxt();
		if(echo){
		cout << "______________________" << endl;
		cout << "new iteration" << endl;
		cout << "______________________" << endl << endl;

		cout << "best node: " << best_node << endl;
		cout << "best node's depth: " << best_node->getDepth() <<endl;
		cout << "best node's mcts value:" << best_node->getMctsValue() << endl;
		cout << "best simulation value so far:\t" << bestValue << endl << endl;
		//Seleccion:
		cout << "selection" << endl;
		}
		MctsNode* node = root;

		/*while (node->hasChildren()){
			node=bestChild(node);
		}*/

		if (node->hasChildren()) {
			node = selection();
			node->addVisit();
			//while(node->getNumVisits()>9)
			//	node = selectRandom(root);

		}


		if(echo){
		cout << "selected node to expand:\t" << node << endl;
		cout << "number of simulations of this node:"
				<< node->get_num_simulations() << endl;
		cout << "mean of the simulations:\t" << node->getValue() << endl;
		cout << "standart deviation\t\t:" << node->get_sd() << endl;
		cout << "depth of the node:\t\t" << node->getDepth() << endl;
		cout << "number of children nodes\t:" << node->getNumChild() << endl;
		cout << "number of times visited\t:" << node->getNumVisits() << endl;
		cout << "actions left:\t\t\t\t" << node->getNumActions() << endl;
		cout << "probability of overcome best:" << node->getMctsValue() << endl;
		}

		//añadir hijos al arbol

		for (int i = 0; i < expansions && i < node->getNumActions(); i++){
			MctsNode* n=node->expand();
			get_best_actions(*n->getState(),n->actions,50);
			expansiones.push_back(n);
		}



		for (int i = 0; i < expansions && i < node->getNumActions(); i++) {

			node = expansiones[i];
			State& state_copy = *(node->getState()->clone());

			double volume = state_copy.get_value();



			double value = greedy->run(state_copy, timelimit, begin_time);
			node->addSimul(value);

			//MctsNode* aux_node;
			//aux_node=node;

			//if (node->get_num_simulations() < 3) {
			double standart_deviation = stimator.stimate_standart_deviation(
					volume);
			node->set_stimated_sd(standart_deviation);

			node->setMctsValue(
					stimator.probability_greater_than(bestValue,
							node->getValue(), node->getPonderatedSd()));

			nodes.push_back(node);
			parent_reevaluation(node);
			if(echo){
			cout << "added node: " << node << endl << endl;

			cout << "volume of this node" << volume << endl;

			cout << "simulation" << endl;
			cout << "value from simulation:" << value << endl << endl;
			cout << "stimating sd:" << endl;
			cout << "stimated standart deviation: " << standart_deviation<< endl;
			}

			if (bestValue < value) {
				bestValue = value;
				best_node = node;
				best_state = &state_copy;
				cout << "______________________" << endl;

				cout << "new best value founded in ("<< getTime()<<" s ):" << value << endl;
				cout << "______________________" << endl;
			}

			 stimator.printPoints();

			if(node->getParent()->get_num_simulations()>=3){
			reevaluation_of_nodes();
			}
		}
		expansiones.clear();
		if(echo){
		cout << "______________________" << endl;
		cout << "\tend of iteration" << endl;
		cout << "______________________" << endl;
		}
		it++;


		if (getTime() > timelimit){
			printRegresionConfig();
			break;
		}
	}

	return bestValue;
}



MctsNode* MCTS::bestChild(MctsNode* node) {
	float bestScore = -1;
	MctsNode* bestChildnode = NULL;
	int size = node->getNumChild();

	for (int i = 0; i < size; i++) {
		if (node->getChildI(i)->getMctsValue() > bestScore) {
			bestScore = node->getChildI(i)->getMctsValue();
			bestChildnode = node->getChildI(i);
		}
	}
	return bestChildnode;
}

//retorna el mejor hijo basado en

MctsNode* MCTS::selection() {
	//cout<<"selection"<<endl;
	double bestScore = -1;
	MctsNode* bestChildnode = NULL;
	int size = nodes.size();
	for (int i = 0; i < size; i++) {
		if (nodes[i]->getMctsValue() > bestScore && !nodes[i]->isTerminal()) {
			bestChildnode = nodes[i];
			bestScore = nodes[i]->getMctsValue();
		}
	}
	return bestChildnode;

}
/*MctsNode* MCTS::selectRandom(MctsNode* root) {
	MctsNode* random_node = NULL;
	int size = nodes.size();
	int index = rand() % size;
	random_node = nodes[index];
	while (random_node->isTerminal()) {
		index = rand() % size;
		random_node = nodes[index];
	}
	return random_node;
}*/
void MCTS::firstSimulations(MctsNode* root, double begin_time,
		double timelimit) {
	cout << "______________________" << endl;
	cout << "first simulations" << endl;
	cout << "______________________" << endl;
	double val;
	MctsNode* node = root;
	int expansions = 3;
	vector<MctsNode*> expansiones;
	cout << "root:"<<root<<endl;

	cout << "root num actions:"<<root->getNumActions()<<endl;

	//cout << "atributes:"<<endl;
	//cout << root->
//expande
	for (int i = 0; i < expansions && i < node->getNumActions(); i++){
		MctsNode* n=node->expand();
		get_best_actions(*n->getState(),n->actions,50);
		expansiones.push_back(n);
		cout<<"added node from root:"<<n<<endl;
		cout << " num actions:"<<n->getNumActions()<<endl;

	}

//simula

	for (int i = 0; i < expansions /*&& i < node->getNumActions()*/; i++) {
		node = expansiones[i];
		cout<<"simulating from: "<<node<<endl;
		cout << " num actions:"<<node->getNumActions()<<endl;

		State& state_copy = *(node->getState()->clone());
		val = greedy->run(state_copy, timelimit, begin_time);
		cout << "value from simulation: "<<val<<endl;
		node->addSimul(val);
		if (bestValue < val) {
			bestValue = val;
			best_node = node;
			best_state = &state_copy;
			cout << "new best node:" << best_node << endl;
			cout << "new best value founded:" << val << endl;
		}
		cout <<endl;
	}
cout <<"añade primeros puntos al estimador"<<endl;
	stimator.addPoint(1, 0); //volume = 1, sd = 0
	stimator.addPoint(root->get_sd(), root->get_num_simulations(), 0);
	stimator.update_coeficients();
cout <<"stimator created coeficients updated "<<endl;
	//slopes.push_back(stimator.getSlope());
	//intercepts.push_back(stimator.getIntercept());
//calcula el puntaje mcts para los nodos añadidos
	for (int i = 0; i < expansions && i < node->getNumActions(); i++) {
		node = expansiones[i];

		//pair <MctsNode*,double> aux_node;
		// aux_node.first=node;

		State& state_copy = *(node->getState()->clone());

		double volume = state_copy.get_value();
		double standart_deviation = stimator.stimate_standart_deviation(volume);
		cout << "stimated standart deviation: " << standart_deviation << endl;
		node->set_stimated_sd(standart_deviation);
		node->setMctsValue(
				stimator.probability_greater_than(bestValue, node->getValue(),
						node->getPonderatedSd()));

		cout << "adding node to list" << endl;
		nodes.push_back(node);
	}
	//reevaluation_of_nodes();
//calcular puntaje mcts para nodo raiz
	/**
	 * funcion bacan :)
	 */
	double standart_deviation = stimator.stimate_standart_deviation(0);

	root->set_stimated_sd(standart_deviation);

	root->setMctsValue(
			stimator.probability_greater_than(bestValue, root->getValue(),
					root->getPonderatedSd()));
	cout << "______________________" << endl;

	cout << "End of first Simulations" << endl;
	cout << "______________________" << endl;
	//iterationToTxt();
	it++;

}
void MCTS::reevaluation_of_nodes() {
	cout<<endl<<"reevaluation of nodes"<<endl;
	int size = nodes.size();
	//cout<<"adding points to stimator"<<endl;
	 stimator.restart(root);
	 //cout<<"number of nodes:"<<size<<endl;
	 for (int i = 0; i < size; i++) {
		 if (nodes[i]->get_num_simulations() >= 3){
		//cout <<nodes[i]<<"\t";
		 double volume = nodes[i]->getState()->get_value();
		 stimator.addPoint(nodes[i]->get_sd(),nodes[i]->get_num_simulations(), volume);
		 }
	 }

	 if(stimator.getNumPoints()<2){

	 stimator.addPoint(1,0);
	 }
	 stimator.update_coeficients();
	 //stimator.printPoints();

	 //cout <<" coeficients updated"<<endl;

	 //cout <<"reevaluation"<<endl;
	 for (int i = 0; i < size; i++) {
	 double volume = nodes[i]->getState()->get_value();

	 double standart_deviation = stimator.stimate_standart_deviation(
	 volume);

	 //cout <<"volume of this node"<<volume<<endl;
	 nodes[i]->set_stimated_sd(standart_deviation);
	 //cout<<"stimated sd established: "<<standart_deviation <<endl;
	 //cout<<"ponderated sd: "<<nodes[i]->getPonderatedSd()<<endl;
	 nodes[i]->setMctsValue(
	 stimator.probability_greater_than(bestValue,
	 nodes[i]->getValue(), nodes[i]->getPonderatedSd()));
	 //cout<<"mctsvalue established"<<endl;
	 }

}

void MCTS::parent_reevaluation(MctsNode* node) {
	cout<<"parent reevaluation"<<endl;

	MctsNode *parent = node->getParent();
	parent->setMctsValue(
			stimator.probability_greater_than(bestValue, parent->getValue(),
					parent->getPonderatedSd()));
	//cout<<"after Setting Mctsvalue"<<endl;
	double volume = node->getValue();

	//cout<<"after add point"<<endl;
}



void MCTS::iterationToTxt() {
	cout<<"printing iteration into file"<<endl;
ofstream points("problems/clp/mcts_out/regresion/points/"+std::to_string(it2)+"_points.json");
ofstream line("problems/clp/mcts_out/regresion/points/"+std::to_string(it2)+"_line.json");
ofstream rsquared("problems/clp/mcts_out/regresion/points/"+std::to_string(it2)+"_rsquared.json");
int size = stimator.getNumPoints();
points << "["<<endl;
for (int i = 0; i < size; i++) {
	/*double standart_deviation = nodes[i]->get_sd();
	double stimated_sd = nodes[i]->get_stimated_sd();
	double volume = nodes[i]->getState()->get_value();
	double num_simul = nodes[i]->get_num_simulations();*/

	double upper= stimator.getSdI(i);
	double volume= stimator.getVolI(i);
	//if(nodes[i]->hasChildren()&&upper!=0)

	points << "{\"x\":"<<volume << ",\"y\":" << upper << "}";
	if(i < size-1) points<<",";


	points << endl;
}

points << "]";
rsquared<<"[{\"rsquared\":"<<stimator.getRSquared() <<"}]"<<endl;

line << "[{\"slope\":"<<stimator.getSlope()<<",\"intercept\":"<<stimator.getIntercept()<<"}]"<<endl;
points.close();
line.close();
it2++;
}

void MCTS::printRegresionConfig(){
	ofstream config("problems/clp/mcts_out/regresion/config.json");
	config <<"{\"Max\":"<<it2<<"}"<<endl;
}

void MCTS::pointsToTxt() {

	ofstream myfile4("problems/clp/mcts_out/tree/flare"+std::to_string(it)+".json");
	int size = nodes.size();

	for (int i = 0; i < size; i++)
	nodes[i]->setId(i);

	MctsNode* node=root;
	dfsPrintChild(root,myfile4);
}

void MCTS::dfsPrintChild(MctsNode* node, ofstream& file){
	file << "{ "<<endl;
	file<<"\t \"name\":\""<<node->getId()<<"\",";
	file<<"\t \"parent\":\""<<node->getParent() <<"\",";
	file<<"\t \"value\":\""<<node->getValue() <<"\",";
	file<<"\t \"sd\":\""<<node->get_sd() <<"\",";
	file<<"\t \"mcts_value\":\""<<node->getMctsValue()<<"\",";
	file<<"\t \"stimated_sd\":\""<<node->get_stimated_sd()<<"\",";
	file<<"\t \"ponderated_sd\":\""<<node->getPonderatedSd()<<"\",";
	file<<"\t \"depth\":\""<<node->getDepth()<<"\",";
	file<<"\t \"num_visit\":\""<<node->getNumVisits()<<"\"";

	if(node->get_num_simulations()>0){
		file<< "\t,\"simulations\":["<<endl;

		for(int i = 0 ; i < node->get_num_simulations();i++){

			file<<node->simulations[i];
			if(i < node->get_num_simulations()-1)
				file<< ",";
		}
		file<<"]";

	}

	if(node->hasChildren()){
		file<< "\t,\"children\":["<<endl;
		for(int i = 0 ; i < node->getNumChild();i++){
			dfsPrintChild(node->getChildI(i),file);
			if(i < node->getNumChild()-1)
			    file<<","<<endl;
		}
		file<<"]";
	}
	file<<"}"<<endl;
}



} /* namespace metasolver */
