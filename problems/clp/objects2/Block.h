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


/**
 * For sorting the blocks in the map Block::all_blocks;
 */
struct block_order {
    bool operator()(const Block* b1, const Block* b2);
};


class Block : public Volume{
public:

	static int INCREMENTAL_ID;
	int id;

	/**
	 * Constructs a container of dimensions l,w,h
	 */
	Block(double l, double w, double h);

	virtual Block* clone(){
		id = INCREMENTAL_ID++;
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

    virtual long getOccupiedVolume() const {return occupied_volume;}

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
		occupied_volume(b.occupied_volume), n_boxes(b.n_boxes),
	 	spaces(new SpaceSet(*b.spaces, *this)), blocks(new AABBList(*b.blocks)) {	}


	double occupied_volume;

	static set<const Block*, block_order> all_blocks;


};

}

#endif /* BLOCK_H_ */
