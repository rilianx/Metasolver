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
	Block(double l, double w, double h);

	Block(long l, long w, long h, bool fake) : Volume(l,w,h), n_boxes(1),
			spaces(NULL), blocks(NULL), boxes(NULL), total_weight(0.0), total_profit(0.0) {

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
	virtual list<const Block* > create_new_blocks(const Block *b2, double min_fr, const Vector3& max_dim, double wmax=0.0) const;

    virtual void insert(const Block& b, const Vector3& p, const Vector3 min_dim=Vector3(0,0,0));

    virtual long getOccupiedVolume() const {return occupied_volume;}

    virtual double getTotalWeight() const {return total_weight;}

    virtual double getTotalProfit() const {return total_profit;}

	const BoxShape* get_box_shape() const {return nb_boxes.begin()->first;}

	bool verify_solution();

	//cajita
	map<const BoxShape*, int> nb_boxes;


	int n_boxes;


	AABBContainer<Space>* spaces;

	/**
    * List of sub-blocks located in the block
    */
	AABBList* blocks;

	/**
    * List of boxes located in the block
    */
    AABBList* boxes;



    void recursive_print(ostream& os, Vector3 mins=Vector3(0,0,0)) const{
    	if(n_boxes==1 && getVolume()==occupied_volume)
    		print(os, mins);
    	else{
       	   	const AABB* b=&blocks->top();
       	   	while(true){
       	   		b->getBlock()->recursive_print(os, mins+b->getMins());
       	   		if(blocks->has_next()) b=&blocks->next();
       	   		else break;
       	   	}
    	}
    }


protected:

	//only the clone function can use the copy constructor
	Block(const Block& b) : Volume(b.getL(),b.getW(),b.getH()),
		occupied_volume(b.occupied_volume), n_boxes(b.n_boxes),
	 	spaces(new SpaceSet(*b.spaces, *this)), blocks(new AABBList(*b.blocks)), boxes(new AABBList(*b.boxes)),
		total_weight(b.total_weight), total_profit(b.total_profit) {

			}




    void print(ostream& os, Vector3 mins) const{
		os << mins << "," << mins+*this << endl;
    }

	double occupied_volume;

	double total_weight;

	double total_profit;

	static set<const Block*, block_order> all_blocks;


};

ostream& operator<<(ostream& os, const Block& dt);


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
