/*
 * BlockSet.h
 *
 *  Created on: 7 jul. 2017
 *      Author: iaraya
 */




#ifndef CLP_BLOCKSET_H_
#define CLP_BLOCKSET_H_
#include "clpStatekd.h"
#include "../../objects2/Vector3.h"
#include "../../clpState.h"
#include "../../evaluators/VCS_Function.h"
#include "kdNode.h"
#include <list>
#include <map>

using namespace std;

namespace clp {

class kd_block: public KdData{

public:
	kd_block(list<pair <double, const Block*> >& b) : b(b) {
		coordinates = new long[3];
		coordinates[0]=(*b.begin()).second->getX();
		coordinates[1]=(*b.begin()).second->getY();
		coordinates[2]=(*b.begin()).second->getZ();
	}

    virtual ~kd_block(){
    	delete[] coordinates;
    }

	virtual double eval() const{
		double eval=0;
		list<pair <double, const Block*> > ::iterator it=b.begin();
		for(;it!=b.end();it++){
			if(!is_constructible(*s, *it->second )) continue;
			Action* a=new clpAction(*it->second,*sp);
			KdNode::nn++;
			it->first=-vcs->eval_action(*s,*a);

			//cout << "ub:" << vcs->cs[0]* sqrt((double)vcs->cs[1]*(double)vcs->cs[2]/(double)vcs->cs[0]) << endl;
			//cout << "vol:" << it->second->getVolume() << endl;


			delete a;
			if(eval<-it->first)
				eval = -it->first;

		}
		return eval;
	}


	virtual double compute_ub(const long* minpoint, const long* maxpoint, long maxvol) const{

		long surface=0.0;
		double cs=0.5;

		long volume=maxvol; //maxpoint[0] * maxpoint[1] * maxpoint[2];
		

		if(maxpoint[0] == sp->getL()  || (maxpoint[1] == sp->getW() )
			 	|| (maxpoint[2] == sp->getH())) cs=1.0;
		
/*		
		if(maxpoint[0] >= sp->getL() - p*(double)maxpoint[0] || (maxpoint[1] >= sp->getW() - p*(double)maxpoint[1])
				|| (maxpoint[2] >= sp->getH()- p*(double)maxpoint[2])) cs=1.0;
*/
/*
		if(alpha>0.0){
			if(maxpoint[0] == sp->getL()) surface+=maxpoint[1] * maxpoint[2];
			if(maxpoint[1] == sp->getW()) surface+=maxpoint[0] * maxpoint[2];
			if(maxpoint[2] == sp->getH()) surface+=maxpoint[0] * maxpoint[1];
			if(surface>0.0)
				cs += 0.5* ((double) surface/ (double) (maxpoint[0] * maxpoint[1] + maxpoint[0] * maxpoint[2] + maxpoint[1] * maxpoint[2]));
		}
*/


		//Block b(sp->getL(), sp->getW(), sp->getH(), true);
		//vcs->CS_p(*s, b, *sp);

		//calcular superficie cubierta de espacio maxpoint
		//return 0.0625 * vcs->cs[0]* sqrt((double)vcs->cs[1]*(double)vcs->cs[2]/(double)vcs->cs[0]);


		if(cs==0.5 && alpha==4.0) return (double) volume * 0.0625;
		else if(alpha==0.0 || cs==1.0) return (double) volume;
		else return (double) volume * pow(cs,alpha);

	}


	static void set_state(const clpState_kd& st) {s=&st;}

	static void set_space(const Space& s) {sp=&s;}

	static void set_vcs(VCS_Function& v) {vcs=&v;}

	static void set_alpha(double a) {alpha=a;}

	static void set_p(double p2) {p=p2;}

	mutable list<pair <double, const Block*> > b;


private:
	static const clpState_kd* s;

	static const Space* sp;

	static VCS_Function* vcs;

	static double alpha;

	static double p;


};

class BlockSet {

public:
	BlockSet(const list<const Block*>& blocks){

		map<Vector3, list< pair<double, const Block*> > > c2l;
		list<const Block*>::const_iterator it=blocks.begin();
		for(;it!=blocks.end();it++)
			c2l[Vector3(**it)].push_back(make_pair(0.0,*it));

		vector<long*> coordinates;
		coordinates.resize(c2l.size());

		map<Vector3, list< pair<double, const Block*> > >::iterator it2=c2l.begin();
		list< const KdData* > kd_blocks;

		int i=0;
		for(;it2!=c2l.end();it2++,i++){
			coordinates[i] = new long[3];
			coordinates[i][0]=(*it2).first.getX();
			coordinates[i][1]=(*it2).first.getY();
			coordinates[i][2]=(*it2).first.getZ();

			kd_blocks.push_back(new kd_block(it2->second));
		}


		kdtree_root = KdNode::createKdTree(kd_blocks, coordinates, 3);




	}

	//void insert(const Block*); //test

	/**
	 * Search the n largest blocks larger than minspace and smaller than maxspace
	 */
	void search_blocks(const clpState_kd& state, const Space& sp,
			list<const Block*>& blocks, int n){

		kd_block::set_state(state);
		kd_block::set_space(sp);


		Vector3 max=sp.getDimensions();

		long minpoint[]={0,0,0};
		long maxpoint[]={max.getX(),max.getY(),max.getZ()};

		multimap<long, const KdData*> best_solutions;

		KdNode::nn=0;

		kdtree_root->search(minpoint, maxpoint, 3, best_solutions, n) ;

		//best_solutions.clear();

		//kdtree_root->search(minpoint, maxpoint, 3, best_solutions, n) ;


		//kdtree_root->searchKdTreeroot(query, maxpoint, 3, 0, n, data);

		list<pair <double, const Block*> > b;

		multimap<long, const KdData*>::iterator it=best_solutions.begin();
		for(;it!=best_solutions.end();it++){
			b.insert(b.end(),
					dynamic_cast<const kd_block*>(it->second)->b.begin(),
					dynamic_cast<const kd_block*>(it->second)->b.end());

		}
		b.sort();

		int j=0;
		for(auto pair_block : b){
			if(j==n) break;
			blocks.push_front(pair_block.second);
			j++;
		}





		//cout << KdNode::nn << ":" << blocks.size() << endl;
		//exit(0);

	}

private:
	KdNode* kdtree_root;

};


}



#endif /* CLP_BLOCKSET_H_ */
