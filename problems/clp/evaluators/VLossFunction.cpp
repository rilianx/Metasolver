/*
 * VLossFunction.cpp
 *
 *  Created on: 2 jun. 2017
 *      Author: iaraya
 */

#include "VLossFunction.h"
#include "../clpState.h"
#include <map>

#include "../objects2/BoxShape.h"
#include "../objects2/Vector3.h"

using namespace std;

namespace clp {

VLossFunction::VLossFunction(const std::map<const BoxShape*, int>& nb_boxes, Vector3& dims, double beta,
		double delta, double f, double r) : ActionEvaluator (r), beta(beta), delta(delta), f(f){
	// TODO Auto-generated constructor stub

	mL= new long[dims.getX()+1];
    mW= new long[dims.getY()+1];
    mH= new long[dims.getZ()+1];

	listL= new std::set<const BoxShape*>[dims.getX()+1];
    listW= new std::set<const BoxShape*>[dims.getY()+1];
    listH= new std::set<const BoxShape*>[dims.getZ()+1];

    solveKnapsack(nb_boxes, dims);

}

VLossFunction::~VLossFunction() {
	 delete[] mL;
	 delete[] mH;
	 delete[] mW;
	 delete[] listL;
	 delete[] listH;
	 delete[] listW;
}

double VLossFunction::eval_action(const State& s,  const Action &a){
	const Block& b = dynamic_cast<const clpAction*>(&a)->block;
	const Space& sp =dynamic_cast<const clpAction*>(&a)->space;

    long resL=sp.getL() - b.getL();
    long resW=sp.getW() - b.getW();
    long resH=sp.getH() - b.getH();

    if(resL<0 || resW<0 || resH<0) return -1.0;

	double loss=(beta>0.0)?
			Loss(dynamic_cast<const clpState*>(&s)->nb_left_boxes, b, sp) : 0.0;

	double vol=(delta>0.0)? (double) b.getOccupiedVolume(): 1.0;

/*
			((double) b.getOccupiedVolume()/
					double(dynamic_cast<const clpState*>(&s)->cont.getVolume())) : 1.0;*/

	return ( delta * log(vol) + beta * log(1.0-loss) + f * log(b.getTotalWeight()) );
}

double VLossFunction::Loss(const std::map<const BoxShape*, int>& nb_boxes, const Block& block, const Space& free_space){
   long resL=free_space.getL() - block.getL();
   long resW=free_space.getW() - block.getW();
   long resH=free_space.getH() - block.getH();

   long lossL=resL, lossW=resW, lossH=resH;
   //cout << lossL << "  " << lossW << " " <<  lossH << endl;
   long maxL = compute_maxX(nb_boxes, block, lossL, resL, mL, listL);
   long maxW = compute_maxX(nb_boxes, block, lossW, resW, mW, listW);
   long maxH = compute_maxX(nb_boxes, block, lossH, resH, mH, listH);


   long vloss=(mL[free_space.getL()]*mW[free_space.getW()]*mH[free_space.getH()]) -
                  ((free_space.getL()-lossL)*(free_space.getW() - lossW)*(free_space.getH()-lossH));
   return (double) vloss / (double) (mL[free_space.getL()]*mW[free_space.getW()]*mH[free_space.getH()]);
}



void VLossFunction::solveKnapsack(const std::map<const BoxShape*, int>& nb_boxes, Vector3& dims){

	compute_mX(nb_boxes, dims.getX(), mL, listL,  0);
	compute_mX(nb_boxes ,dims.getY(), mW, listW,  1);
	compute_mX(nb_boxes ,dims.getZ(), mH, listH,  2);

}

long VLossFunction::compute_maxX(const std::map<const BoxShape*, int>& nb_boxes, const Block& block,
		long& lossX, long resX, long* mX,std::set<const  BoxShape*>* listX){
   long maxX = mX[resX];
   while(maxX>0){
	 for(std::set<const BoxShape*>::const_iterator it = listX[maxX].begin();it!=listX[maxX].end();it++){
		// cout << **it << endl;
		int n=0;
		if(block.nb_boxes.find(*it) != block.nb_boxes.end() ) n = block.nb_boxes.at(*it);

		if(nb_boxes.at(*it)-n>0) {
			   lossX = resX-maxX;
			   maxX=0;
			   break;
		}
	}

	if(maxX > 0)
	  maxX = mX[maxX-1];

  }
   return maxX;
}

void VLossFunction::compute_mX(const std::map<const BoxShape*, int>& nb_boxes, int X, long *mX,
		std::set<const BoxShape*>* listX,  int dim){
	 bool flag[X+1]; flag[0]=true;

	 for(int i=1; i<=X; i++) {
			flag[i]=false;
			listX[i].clear();
	 }

	 map<const BoxShape*,int>::const_iterator it_nb;

	 for(it_nb = nb_boxes.begin(); it_nb!=nb_boxes.end(); it_nb++){
		 int n=it_nb->second;

		 if(n > 0){
			  for(int ii=0; ii<3; ii++){

				  int x=0;
				  switch(ii){
						case 0:
							x=int((*it_nb).first->get(dim, BoxShape::LWH));
						break;
						case 1:
							if((*it_nb).first->is_valid(BoxShape::WHL))
								x=int((*it_nb).first->get(dim, BoxShape::WHL));
						  break;
						case 2:
							if((*it_nb).first->is_valid(BoxShape::HLW))
								x=int((*it_nb).first->get(dim, BoxShape::HLW));
						  break;
						default:  break;
				  }
				  if(x==0) continue;

				 for(int i=0; i<=X-x; i++){
					  if(!flag[i]) continue;
					  for(int j=min(n,(X-i)/x); j>=1; j--){
						 flag[i+x*j]=true;
						 //cout << i+x*j << endl;
						 listX[i+x*j].insert((*it_nb).first);
					  }
				 }

			 }

		}
	}

	for(int i=0; i<=X; i++){
	   if(flag[i]) mX[i]=i;
	   else mX[i]=mX[i-1];
	}
}



} /* namespace clp */
