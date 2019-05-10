/*
 * Block.h
 *
 *  Created on: 01-06-2017
 *      Author: ignacio
 */

#ifndef BLOCK_H_
#define BLOCK_H_


#include "../containers/AABBContainer.h"
#include <list>
#include <map>
#include "AABBList.h"
#include "BoxShape.h"
#include "Space.h"
#include "SpaceSet.h"
#include "Volume.h"

using namespace std;

namespace clp {

class Space;
class Block;
class Block_fsb;


double double_rand(double min=0.0, double max=1.0);

/**
 * For sorting the blocks in the map Block::all_blocks;
 */
struct block_order {
    bool operator()(const Block* b1, const Block* b2);
};


class Block : public Volume{
public:

	static bool FSB;

	/**
	 * Constructs a container of dimensions l,w,h
	 */
	Block(long l, long w, long h);

	Block(long l, long w, long h, bool fake) : Volume(l,w,h), n_boxes(1),
			spaces(NULL), blocks(NULL), total_weight(0.0)  {

		occupied_volume = getVolume();
	}

	virtual Block* clone(){
		return new Block(*this);
	}

	/**
	 * Constructs a single-box block
	 */
	Block(const BoxShape & b, BoxShape::Orientation o,double occupied_volume);

	virtual ~Block();

	/**
	 * Crea una lista de a lo mas 3 bloques juntando los bloques b1 y b2
	 * Bloques son creados solo si  volumen_ocupado >= min_fr * volumen
	 */
	virtual list<const Block* > create_new_blocks(const Block *b2, double min_fr, const Vector3& max_dim) const;

    virtual void insert(const Block& b, const Vector3& p, const Vector3 min_dim=Vector3(0,0,0));

    virtual long getOccupiedVolume() const {
    	//if(MCLP){
    	occupied_volume=0;
    	for(auto box:nb_boxes)
    		occupied_volume+= box.first->getVolume()*box.second;
    	//}

    	return occupied_volume;
    }

    virtual double getTotalWeight() const {return total_weight;}

    virtual double getTotalPriority() const{
    	double priority=0;
    	for(auto box:nb_boxes)
    		priority+= box.first->get_priority()*box.second;
    	return priority;
    }

	bool verify_solution();

    void MatLab_print(double pause_time=0.0) const {
		cout << "axis equal" << endl;
		cout <<  "DrawCuboid([" << getL() << ";" << getW() << ";" << getH() <<"],["
			<< (double)getL()/2.0 << ";" <<
			getW()/2.0 << ";" <<
			getH()/2.0 << "],[0,0,0],[0 0 0], 0.0);" << endl;

		MatLab_printR();
    }

	map<const BoxShape*, int> nb_boxes;
	int n_boxes;


	AABBContainer<Space>* spaces;

	/**
    * List of sub-blocks located in the block
    */
	AABBContainer<AABB>* blocks;


protected:

	//only the clone function can use the copy constructor
	Block(const Block& b) : Volume(b.getL(),b.getW(),b.getH()),
		occupied_volume(b.occupied_volume), n_boxes(b.n_boxes), nb_boxes(b.nb_boxes),
	 	spaces(new SpaceSet(*b.spaces, *this)), blocks(new AABBList(*b.blocks)), total_weight(b.total_weight) {	}


    void MatLab_printR(int i=1, int j=1, double R=0.0, double G=0.0, double B=0.0, double alpha=1.0, Vector3 mins=Vector3(0,0,0)) const{
    	int ii=i;
    	if(i>1 && n_boxes==1)
    		MatLab_print(i,j,R,G,B, alpha, mins);
    	else{
       	   	const AABB* b=&blocks->top();
       	   	while(true){
       	   		if(i==1){
       	   			cout << "pause(1);" << endl;
       	   			R=double_rand(); G=double_rand(); B=double_rand(); ii++;
       	   		}
       	   		else j++;


       	   		b->getBlock()->MatLab_printR(ii,j,R,G,B, alpha, mins+b->getMins());
       	   		if(blocks->has_next()) b=&blocks->next();
       	   		else break;
       	   	}
    	}
    }

    void MatLab_print(int i, int j, double R, double G, double B, double alpha, Vector3 mins) const{
    	cout << "a(" << i << "," << j << ")=DrawCuboid([" << getL() << ";" << getW() << ";" << getH() <<"],["
             << (double) getL()/2.0 + (double) mins.getX() << ";" <<
                (double) getW()/2.0 + (double) mins.getY() << ";" <<
                (double) getH()/2.0 + (double) mins.getZ() << "],[0,0,0],["
             << R << " " << G << " " << B << "]," << alpha << ");" << endl;
    }

	mutable double occupied_volume;

	double total_weight;

	static set<const Block*, block_order> all_blocks;


};

class Block_fsb : public Block{
public:
	/**
	 * Constructs a container of dimensions l,w,h
	 */
	Block_fsb(long l, long w, long h);

	/**
	 * Constructs a single-box block
	 */
	Block_fsb(const BoxShape & b, BoxShape::Orientation o, double occupied_volume);

	virtual Block* clone(){
		return new Block_fsb(*this);
	}

	long getPA_L() const;
	long getPA_W() const;

	virtual void insert(const Block& block, const Vector3& point, const Vector3 min_dim=Vector3(0,0,0));

	virtual list<const Block* > create_new_blocks(const Block* b2, double min_fr, const Vector3& max_dim) const;

protected:
	//only the clone function can use the copy constructor
	Block_fsb(const Block_fsb& b) : Block(b), pa_l(b.pa_l), pa_w(b.pa_w) {	}

private:
	//usados en variante con restriccion fsb (full supported blocks)
	long pa_l;
	long pa_w;
};



}

#endif /* BLOCK_H_ */
