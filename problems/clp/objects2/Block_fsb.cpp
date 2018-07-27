/*
 * Blockfsb.cpp
 *
 *  Created on: 11 jul. 2017
 *      Author: iaraya
 */


#include "Block.h"

using namespace std;

namespace clp {

Block_fsb::Block_fsb(long l, long w, long h) : Block(l,w,h), pa_l(l), pa_w(w) { }

Block_fsb::Block_fsb(const BoxShape & b, BoxShape::Orientation o, double occupied_volume) : Block(b,o,occupied_volume) {
	pa_l=getL();
	pa_w=getW();
}

long Block_fsb::getPA_L() const{return pa_l;}
long Block_fsb::getPA_W() const{return pa_w;}


list<const Block* > Block_fsb::create_new_blocks(const Block* _b2, double min_fr, const Vector3& max_dim) const{

	const Block_fsb* b1=this;
	const Block_fsb* b2=dynamic_cast<const Block_fsb*>(_b2);

	//cout << "creating fsb blocks!" << endl;
	list<const Block*> blocks;

	for(int i=0; i<3; i++){
		long ll= max(b1->getL(),b2->getL());
		long ww= max(b1->getW(),b2->getW());
		long hh= max(b1->getH(),b2->getH());

		long x2=0, y2=0, z2=0;

		//cout << 3 << endl;
		switch(i){
			case 0:
			  if(b1->getH()!=b2->getH() || b1->pa_l!=b1->getL() || b2->pa_l!=b2->getL()) continue;
			  ll=(b1->getL()+b2->getL()); x2=b1->getL();  break;
			case 1:
			  if(b1->getH()!=b2->getH() || b1->pa_w!=b1->getW() || b2->pa_w!=b2->getW()) continue;
			  ww=(b1->getW()+b2->getW()); y2=b1->getW(); break;
			case 2:
			  if((b1->pa_w<b2->getW() || b1->pa_l<b2->getL()) && (b2->pa_w<b1->getW() || b2->pa_l<b1->getL())) continue;
			  hh=(b1->getH()+b2->getH()); z2=b1->getH();
		}


		long vol= ll*ww*hh;

		if( ((double) (b1->occupied_volume+b2->occupied_volume) / (double) vol) >= min_fr && Vector3(ll,ww,hh) <= max_dim  ){

			//TODO: Revisar que bloques construidos sean coherentes con packing areas definidas
			Block_fsb* new_block=new Block_fsb(ll,ww,hh);

			if(i!=2){
				new_block->insert(*b1, Vector3(0,0,0));
				new_block->insert(*b2, Vector3(x2,y2,z2));
			}

			switch(i){
			case 0:
				new_block->pa_l=ll;
				new_block->pa_w=min(b1->pa_w,b2->pa_w);
				break;
			case 1:
				new_block->pa_w=ww;
				new_block->pa_l=min(b1->pa_l,b2->pa_l);
				break;
			case 2:
				if(b1->pa_l >= b2->pa_l && b1->pa_w >= b2->pa_w){
					new_block->insert(*b1, Vector3(0,0,0));
					new_block->insert(*b2, Vector3(0,0,b1->getH()));
				}else{
					new_block->insert(*b2, Vector3(0,0,0));
					new_block->insert(*b1, Vector3(0,0,b2->getH()));
				}
				new_block->pa_l=min( b1->pa_l, b2->pa_l);
				new_block->pa_w=min( b1->pa_w, b2->pa_w);
				break;
			}

			if(Block::all_blocks.find(new_block)==Block::all_blocks.end()){
				Block::all_blocks.insert(new_block);
				blocks.push_back(new_block);
			}else
				delete new_block;



		}

	}

	return blocks;
}

void Block_fsb::insert(const Block& block, const Vector3& point, const Vector3 min_dim){
	const Block_fsb* block_fsb=dynamic_cast<const Block_fsb*>(&block);
    //Se actualiza la cantidad de cajas
	map<const BoxShape*, int>::const_iterator it_nb;
    for(it_nb = block.nb_boxes.begin(); it_nb!=block.nb_boxes.end(); it_nb++){
    	nb_boxes[it_nb->first]+=it_nb->second;
    	n_boxes+=it_nb->second;
    }

    //Se actualiza el volumen ocupado
    occupied_volume += block.getOccupiedVolume();
    total_weight += block.getTotalWeight();

	AABB b(point, &block);

    AABB b1(b.getXmin(), b.getYmin(), b.getZmin(), b.getXmax(), b.getYmax(), getH());

    spaces->crop_volume(b1, *this, min_dim);

    //TODO: revisar concordancia entre packing areas y los bloques
	AABB b2(b.getXmin(), b.getYmin(), b.getZmax(), b.getXmin()+block_fsb->getPA_L(), b.getYmin()+block_fsb->getPA_W(), getH());
	spaces->insert(Space(b2, *this));

    blocks->insert(b);
}

} /* namespace clp */
