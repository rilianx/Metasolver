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
	double sd=volume*slope+intercept;
	cout<<"stimated standart deviation:"<<sd<<endl;
	return volume*slope+intercept;
}
double Stimator::probability_greater_than(double best,double mean,double sd){
	cout<<"probability greater than "<<best<<endl;
	double z=z_value(best,mean,sd);
	cout<<"probability:"<<1-normal_cdf(z)<<endl;
	return 1-normal_cdf(z);
}

void Stimator::addPoint(vector<double> simulations, double volume){
	cout << "adding point to regression"<<endl;
	int N=simulations.size();
	cout<<"simulations size:"<<N<<endl;
	using boost::math::chi_squared;
     using boost::math::quantile;
     using boost::math::complement;
     double sd=standart_deviation(simulations);
     //cout<<"standart deviation of the first simulations of the root node:"<<sd<<endl;
     boost::math::chi_squared dist(N - 1);
     double upper_limit = sqrt((N - 1) * sd * sd /  quantile(dist, alpha / 2));
     pair<double,double> point;
     //cout<<"upper limit:"<<upper_limit<<endl;
     point=make_pair(volume,upper_limit);
     points.push_back(point);
     update_coeficients();
}

void Stimator::addPoint(double upper, double volume){

     pair<double,double> point;
     point=make_pair(volume,upper);
     points.push_back(point);
     //update_coeficients();
}



void Stimator::update_coeficients(){
	cout<<"updating coeficients of regression"<<endl;
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
    cout <<"new regression: sd=vol*"<<slope<<" + "<<intercept<<endl;
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
	srand (time(NULL));

	begin_time=bt;
	timelimit=tl;
	root=new MctsNode(beginState);
	int expansions=1;
	best_node=root;
	vector <MctsNode*>expansiones;
	firstSimulations(root,bt,tl);

	while(true){
		cout<<"new iteration"<<endl;
		MctsNode* node=root;
		//Seleccion:
		//cout <<"selection"<<endl;
		if(node->hasChildren()){
			node = selection(root);
			node->addVisit();
			//while(node->getNumVisits()>9)
			//	node = selectRandom(root);

		}
		cout<<"selected node to expand:"<<node<<endl;
		cout<<"number of simulations of this node:"<<node->get_num_simulations()<<endl;
		if( !node->isTerminal()&&!node->isFullyExpanded()) {
			for(int i=0;i<expansions&&i<node->getNumActions();i++)
				 expansiones.push_back(node->expand());
		 }else{
			break;
			//node = selectRandom(root);

		 }


		 for(int i=0; i < expansions && i < node->getNumActions() ; i++){

	     node=expansiones[i];
	     cout<<"added node: "<<node<<endl;
    	 State& state_copy = *(node->getState()->clone());



         double value = greedy->run(state_copy, timelimit, begin_time);
         node->addSimul(value);
         pair <MctsNode*,double> aux_node;
         aux_node.first=node;


         double volume = state_copy.get_value();

         if(node->get_num_simulations()<3){
			 double standart_deviation=stimator.stimate_standart_deviation(volume);
			 cout<<"stimated standart deviation: "<<standart_deviation<<endl;
			 aux_node.second=stimator.probability_greater_than(bestValue,node->getValue(),standart_deviation);
		 }else{
			 double standart_deviation= node->get_sd();
			 aux_node.second=stimator.probability_greater_than(bestValue,node->getValue(),standart_deviation);
			 stimator.addPoint(node->simulations,volume);
         }
         nodes.push_back(aux_node);
		 reevaluation_of_nodes();

			 if(bestValue<value){
				 bestValue=value;
				 best_node=node;
				 best_state=&state_copy;
				 cout <<"new best value founded:"<<value<<endl;
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
	cout<<"selection"<<endl;
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
MctsNode* MCTS::selectRandom(MctsNode* root){
	MctsNode* random_node=NULL;
	int size=nodes.size();
	int index = rand()%size;
	random_node=nodes[index].first;
	while(random_node->isTerminal()){
		index = rand()%size;
		random_node=nodes[index].first;
	}
	return random_node;
}
void MCTS::firstSimulations(MctsNode* root,double begin_time,double timelimit){
	cout<<"______________________"<<endl;
	cout<<"first simulations"<<endl;
	cout<<"______________________"<<endl;
    double val;
    MctsNode* node=root;
    int expansions=3;
	vector <MctsNode*>expansiones;

    for(int i=0;i<expansions&&i<node->getNumActions();i++)
    				 expansiones.push_back(node->expand());

	 for(int i=0; i < expansions && i < node->getNumActions() ; i++){
	     node=expansiones[i];
	     State& state_copy = *(node->getState()->clone());
	     val= greedy->run(state_copy, timelimit, begin_time);
	     node->addSimul(val);
		 if(bestValue<val){
			 bestValue=val;
			 best_node=node;
			 best_state=&state_copy;
			 cout <<"new best value founded:"<<val<<endl;
		 }
	 }

    stimator.addPoint(0,1);
    stimator.addPoint(root->simulations,0);

	 for(int i=0; i < expansions && i < node->getNumActions() ; i++){
	     node=expansiones[i];

         pair <MctsNode*,double> aux_node;
         aux_node.first=node;

         State& state_copy = *(node->getState()->clone());

         double volume = state_copy.get_value();
         double standart_deviation=stimator.stimate_standart_deviation(volume);
         cout<<"stimated standart deviation: "<<standart_deviation<<endl;
         aux_node.second=stimator.probability_greater_than(bestValue,node->getValue(),standart_deviation);

		 cout<<"adding node to list"<<endl;
         nodes.push_back(aux_node);
	 }
	 //reevaluation_of_nodes();
		cout<<"______________________"<<endl;
		cout<<"End of first Simulations"<<endl;
		cout<<"______________________"<<endl;

}
void MCTS::reevaluation_of_nodes(){
	cout<<"reevaluation of nodes"<<endl;
	int size=nodes.size();
	for(int i = 0; i<size;i++){
        double volume = nodes[i].first->getState()->get_value();
        if(nodes[i].first->get_num_simulations()<3){
        	cout<<"not enough simulations, stimating"<<endl;
			double standart_deviation=stimator.stimate_standart_deviation(volume);
			nodes[i].second=stimator.probability_greater_than(bestValue,nodes[i].first->getValue(),standart_deviation);
			stimator.addPoint(nodes[i].first->simulations,volume);
        }else{
        	cout<<"enough simulations"<<endl;
            double standart_deviation=nodes[i].first->get_sd();
    		nodes[i].second=stimator.probability_greater_than(bestValue,nodes[i].first->getValue(),standart_deviation);
    		stimator.addPoint(nodes[i].first->simulations,volume);
        }
    }
}

} /* namespace metasolver */
