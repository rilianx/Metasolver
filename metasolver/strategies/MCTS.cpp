/*
 * MCTS.cpp
 *
 *  Created on: 03-01-2018
 *      Author: investigador
 */

#include "MCTS.h"
namespace metasolver {

double Stimator::normal_cdf(double z){
using boost::math::normal; // typedef provides default type is double.
normal s;
return cdf(s,z);
}

double Stimator::z_value(double mean,double sd, double x){
	return (x-mean)/sd;
}
double Stimator::stimate_standart_deviation(double volume){

	return volume*slope+intercept;
}
double Stimator::probability_greater_than(double best,double mean,double sd){
	double z=z_value(best,mean,sd);
	cout<<"probability:"<<1-normal_cdf(z)<<endl;
	return 1-normal_cdf(z);
}

void Stimator::addPoint(vector<double> simulations, double volume){
	int N=simulations.size();
	using boost::math::chi_squared;
     using boost::math::quantile;
     using boost::math::complement;
     int n=simulations.size();
     double sd=standart_deviation(simulations);
     boost::math::chi_squared dist(N - 1);
     double upper_limit = sqrt((N - 1) * sd * sd /  quantile(dist, alpha / 2));
     pair<double,double> point;
     point=make_pair(volume,upper_limit);
     points.push_back(point);

}

void Stimator::update_coeficients(){
	int n=points.size();
	//variables for sums/sigma of xi,yi,xi^2,xiyi etc
    double xsum=0,x2sum=0,ysum=0,xysum=0;
    for (int i=0;i<n;i++){
    	xsum+=points[i].first;
    	ysum+=points[i].second;
    	x2sum+=pow(points[i].first,2);
    	xysum+=points[i].first*points[i].second;
    }
    slope=(n*xysum-xsum*ysum)/(n*x2sum-xsum*xsum);
    intercept=(x2sum*ysum-xsum*xysum)/(x2sum*n-xsum*xsum);
}

double Stimator::standart_deviation(vector<double> simulations){
	double sd=0;
	int size=simulations.size();
	double mean=meean(simulations);
	for(int i=0;i<size;i++){
		sd+=pow(simulations[i]-mean,2);
	}
	sd=sd/size;
	sd=sqrt(sd);
	return sd;
}

double Stimator::meean(vector <double> numbers){
	double mean=0;
	int size=numbers.size();
	for(int i =0; i< size; i++)
		mean+=numbers[i];

	mean=mean/size;
	return mean;
}


double MCTS::run(State& beginState,double tl,clock_t bt=clock()){
	begin_time=bt;
	timelimit=tl;
	root=new MctsNode(beginState);
	int expansions=2;
	best_node=root;
	vector <MctsNode*>expansiones;
	firstSimulations(root,bt,tl);
	 //node_queue.push();

	while(true){
		MctsNode* node=root;
		//Seleccion:
		//cout <<"seleccion"<<endl;
		if(node->hasChildren()){
			node = selection(root);
		}
		//cout <<"expansion"<<endl;

		//cout<<"Expansion"<<endl;

		if( !node->isTerminal()&&!node->isFullyExpanded()) {
			for(int i=0;i<expansions&&i<node->getNumActions();i++)
				 expansiones.push_back(node->expand());
		 }else{
			 break;
		 }
		 //cout <<"Simulacion"<<endl;

		//cout <<"simulacion"<<endl;

		 for(int i=0; i < expansions && i < node->getNumActions() ; i++){

	     node=expansiones[i];

    	 State& state_copy = *(node->getState()->clone());

         double volume = state_copy.get_value();
         double standart_deviation=stimator.stimate_standart_deviation(volume);

         double value = greedy->run(state_copy, timelimit, begin_time);
         //cout <<"Value:"<<value<<endl;
         cout <<"depth:"<<node->getDepth()<<endl;
         node->addSimul(value);
         pair <MctsNode*,double> aux_node;
         aux_node.first=node;
         aux_node.second=stimator.probability_greater_than(bestValue,node->getValue(),standart_deviation);
         nodes.push_back(aux_node);
         //value=node->getValue();

		 if(best_node->getValue()<value){
			 best_node=node;
			 best_state=&state_copy;
			 cout <<"new best value:"<<value<<endl;
			 reevaluation_of_nodes();
		 }
		 }
		 expansiones.clear();
         if(getTime()>timelimit) break;
	}

	return best_node->getValue();
}

MctsNode* MCTS::bestChild(MctsNode* node){
	float bestScore=-1;
	MctsNode* bestChildnode=NULL;
	int size=node->getNumChild();

	for(int i=0; i<size; i++ ){
		if(node->getChildI(i)->getValue()>bestScore){
			bestScore=node->getChildI(i)->getValue();
			bestChildnode=node->getChildI(i);
		}
	}
	return bestChildnode;
}

//retorna el mejor hijo basado en

MctsNode* MCTS::selection(MctsNode* root){
	double bestScore=-1;
	MctsNode* bestChildnode=NULL;
	int size=nodes.size();
	for(int i = 0; i< size ; i++){
		if(nodes[i].second>bestScore){
			bestChildnode=nodes[i].first;
			bestScore=nodes[i].second;
		}
	}
	return bestChildnode;
}
void MCTS::firstSimulations(MctsNode* root,double begin_time,double timelimit){
	State& state_copy = *(root->getState()->clone());
    double val;
    val= greedy->run(state_copy, timelimit, begin_time);
    root->addSimul(val);
    val = greedy->run(state_copy, timelimit, begin_time);
    root->addSimul(val);
    val = greedy->run(state_copy, timelimit, begin_time);
    root->addSimul(val);
    val = greedy->run(state_copy, timelimit, begin_time);
    root->addSimul(val);
}
void MCTS::reevaluation_of_nodes(){
	int size=nodes.size();
	for(int i = 0; i<size;i++){
        double volume = nodes[i].first->getState()->get_value();
        double standart_deviation=stimator.stimate_standart_deviation(volume);
		nodes[i].second=stimator.probability_greater_than(bestValue,nodes[i].first->getValue(),standart_deviation);
	}
}

} /* namespace metasolver */
