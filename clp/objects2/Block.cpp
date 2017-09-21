/*
 * Block.cpp
 *
 *  Created on: 01-06-2017
 *      Author: ignacio
 */

#include "Block.h"

#include <iostream>


using namespace std;

namespace clp {

bool Block::FSB=false;
set<const Block*, block_order> Block::all_blocks;

Block::Block(long l, long w, long h) : Volume(l,w,h),occupied_volume(0), n_boxes(0),
		spaces(new SpaceSet(*this)), blocks(new AABBList()) {
	spaces->insert(Space (*this));
}

Block::Block(const BoxShape& box, BoxShape::Orientation o) :
		Volume(box.getL(o), box.getW(o), box.getH(o)), occupied_volume(box.getVolume()),
		n_boxes(1), spaces(NULL), blocks(NULL) {

	nb_boxes[&box]=1;
};

Block::~Block() {
	if(spaces) delete spaces;
	if(blocks) delete blocks;
}



Block* Block::create_block(const BoxShape & b, BoxShape::Orientation o, bool fsb){
	if(fsb) return new Block_fsb(b,o);
	else return new Block(b,o);
}

void Block::insert(const Block& block, const Vector3& point, const Vector3 min_dim){
    //Se actualiza la cantidad de cajas
	map<const BoxShape*, int>::const_iterator it_nb;
    for(it_nb = block.nb_boxes.begin(); it_nb!=block.nb_boxes.end(); it_nb++){
    	nb_boxes[it_nb->first]+=it_nb->second;
    	n_boxes+=it_nb->second;
    }

    //Se actualiza el volumen ocupado
    occupied_volume += block.getOccupiedVolume();

	AABB b(point, &block);

    spaces->crop_volume(b,*this, min_dim);

    blocks->insert(b);
}


list<const Block* > create_new_blocks(const Block& b1, const Block& b2, double min_fr, const Vector3& max_dim){

	list<const Block*> blocks;

	for(int i=0; i<3; i++){
		long ll= max(b1.getL(),b2.getL());
		long ww= max(b1.getW(),b2.getW());
		long hh= max(b1.getH(),b2.getH());

		long x2=0, y2=0, z2=0;

		switch(i){
			case 0:
			  ll=(b1.getL()+b2.getL()); x2=b1.getL();  break;
			case 1:
			  ww=(b1.getW()+b2.getW()); y2=b1.getW(); break;
			case 2:
			  hh=(b1.getH()+b2.getH()); z2=b1.getH();
		}

		long vol= ll*ww*hh;

		if( ((double) (b1.occupied_volume+b2.occupied_volume) / (double) vol) >= min_fr && Vector3(ll,ww,hh) <= max_dim  ){

			Block* new_block;

			new_block=new Block(ll,ww,hh);
			new_block->insert(b1, Vector3(0,0,0));
			new_block->insert(b2, Vector3(x2,y2,z2));

			if(Block::all_blocks.find(new_block)==Block::all_blocks.end()){
				Block::all_blocks.insert(new_block);
				blocks.push_back(new_block);
			}else{
				delete new_block;
			}
		}
	}

	return blocks;
}

bool block_order::operator()(const Block* b1, const Block* b2) {
	if(b1->getOccupiedVolume() != b2->getOccupiedVolume())
		return (b1->getOccupiedVolume() < b2->getOccupiedVolume());

	if(b1->lex_lower(*b2)) return true;
	if(b2->lex_lower(*b1)) return false;

	if(b1->nb_boxes.size() != b2->nb_boxes.size())
		 return (b1->nb_boxes.size() < b2->nb_boxes.size());

	map<const BoxShape*, int>::const_iterator i, j;
	for(i = b1->nb_boxes.begin(), j = b2->nb_boxes.begin(); i != b1->nb_boxes.end(); ++i, ++j)
	{
		 if(i->first != j->first)
		   return (*(i->first) < *(j->first));
		 if(i->second!=j->second)
		   return (i->second < j->second);
	}

	return false; //they are equal
}


double double_rand(double min, double max){
	return min+(max-min)*((double)rand()/(double)RAND_MAX);
}

}

