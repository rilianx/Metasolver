/*
 * Container.cpp
 */

#include <algorithm>
#include "Container.h"

using namespace std;

namespace clp {

bool Space::origin_anchor=false;

void Container::insert(const Block& b, long x, long y, long z, long min_l, long min_w, long min_h){
	occupied_volume += b.getOccupiedVolume();

	PlacedBlock pb(x, y, z, b.getL(), b.getW(), b.getH(), &b);
	blocks.push_back(pb);

	//los espacios sin apoyo se vuelven a considerar
	spaces.insert(spaces.end(), dormant.begin(), dormant.end());
	dormant.clear();

	// los espacios que intersectan al bloque se reemplazan por los sub-espacios
	// resultantes de restarle el bloque (pieces); los que solo lo tocan (touching)
	// se mantienen, pero pueden contener a alguna pieza
	static vector<Space> pieces;
	static vector<int> touching;
	pieces.clear(); touching.clear();

	int r=0;
	auto add_piece = [&](long x1, long y1, long z1, long x2, long y2, long z2){
		if(x2-x1 >= min_l && y2-y1 >= min_w && z2-z1 >= min_h)
			pieces.push_back(Space(AABB(x1,y1,z1,x2,y2,z2), regions[r], r));
	};

	for(size_t i=0; i<spaces.size(); ){
		const Space& sp = spaces[i];
		if(!sp.touches(pb)) { i++; continue; }
		if(!sp.strict_intersects(pb)) { touching.push_back(i); i++; continue; }
		r=sp.region;

		if(pb.xmax < sp.xmax) add_piece(pb.xmax, sp.ymin, sp.zmin, sp.xmax, sp.ymax, sp.zmax);
		if(pb.ymax < sp.ymax) add_piece(sp.xmin, pb.ymax, sp.zmin, sp.xmax, sp.ymax, sp.zmax);
		if(pb.zmax < sp.zmax) add_piece(sp.xmin, sp.ymin, pb.zmax, sp.xmax, sp.ymax, sp.zmax);
		if(pb.xmin > sp.xmin) add_piece(sp.xmin, sp.ymin, sp.zmin, pb.xmin, sp.ymax, sp.zmax);
		if(pb.ymin > sp.ymin) add_piece(sp.xmin, sp.ymin, sp.zmin, sp.xmax, pb.ymin, sp.zmax);
		if(pb.zmin > sp.zmin) add_piece(sp.xmin, sp.ymin, sp.zmin, sp.xmax, sp.ymax, pb.zmin);

		//el ultimo espacio pasa a la posicion i (que se revisa a continuacion)
		remove_space(i);
	}

	// se eliminan las piezas no maximales: contenidas en un espacio que solo toca
	// al bloque o en otra pieza (los espacios no se contienen entre si, por lo que
	// los que solo tocan al bloque no pueden estar contenidos en una pieza)
	//orden estable por volumen decreciente (insercion: son pocas piezas)
	for(size_t i=1; i<pieces.size(); i++){
		Space sp = pieces[i];
		size_t j=i;
		for(; j>0 && pieces[j-1].volume < sp.volume; j--) pieces[j]=pieces[j-1];
		pieces[j]=sp;
	}

	size_t first_new = spaces.size();
	for(size_t j=0; j<pieces.size(); j++){
		const Space& pc = pieces[j];
		bool contained=false;
		for(int t : touching)
			if(spaces[t].contains(pc)) { contained=true; break; }
		for(size_t i=first_new; !contained && i<spaces.size(); i++)
			if(spaces[i].contains(pc)) contained=true;
		if(!contained) spaces.push_back(pc);
	}
}

} /* namespace clp */
