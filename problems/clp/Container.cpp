/*
 * Container.cpp
 */

#include <algorithm>
#include "Container.h"

using namespace std;

namespace clp {

void Container::insert(const Block& b, long x, long y, long z, long min_l, long min_w, long min_h){
	occupied_volume += b.getOccupiedVolume();

	PlacedBlock pb(x, y, z, b.getL(), b.getW(), b.getH(), &b);
	blocks.push_back(pb);

	// se eliminan los espacios que tocan al bloque, los que lo intersectan se
	// reemplazan por los sub-espacios resultantes de restarle el bloque
	vector<Space> new_spaces;
	auto add_piece = [&](long x1, long y1, long z1, long x2, long y2, long z2){
		if(x2-x1 >= min_l && y2-y1 >= min_w && z2-z1 >= min_h)
			new_spaces.push_back(Space(AABB(x1,y1,z1,x2,y2,z2), L, W, H));
	};

	for(size_t i=0; i<spaces.size(); ){
		const Space& sp = spaces[i];
		if(!sp.touches(pb)) { i++; continue; }

		if(sp.strict_intersects(pb)){
			if(pb.xmax < sp.xmax) add_piece(pb.xmax, sp.ymin, sp.zmin, sp.xmax, sp.ymax, sp.zmax);
			if(pb.ymax < sp.ymax) add_piece(sp.xmin, pb.ymax, sp.zmin, sp.xmax, sp.ymax, sp.zmax);
			if(pb.zmax < sp.zmax) add_piece(sp.xmin, sp.ymin, pb.zmax, sp.xmax, sp.ymax, sp.zmax);
			if(pb.xmin > sp.xmin) add_piece(sp.xmin, sp.ymin, sp.zmin, pb.xmin, sp.ymax, sp.zmax);
			if(pb.ymin > sp.ymin) add_piece(sp.xmin, sp.ymin, sp.zmin, sp.xmax, pb.ymin, sp.zmax);
			if(pb.zmin > sp.zmin) add_piece(sp.xmin, sp.ymin, sp.zmin, sp.xmax, sp.ymax, pb.zmin);
		}else
			new_spaces.push_back(sp);

		remove_space(i);
	}

	// se eliminan los espacios no maximales (contenidos en otro)
	stable_sort(new_spaces.begin(), new_spaces.end(),
			[](const Space& a, const Space& b){ return a.volume > b.volume; });

	vector<bool> removed(new_spaces.size(), false);
	for(size_t i=0; i<new_spaces.size(); i++){
		if(removed[i]) continue;
		for(size_t j=i+1; j<new_spaces.size(); j++)
			if(!removed[j] && new_spaces[i].contains(new_spaces[j])) removed[j]=true;
		spaces.push_back(new_spaces[i]);
	}
}

} /* namespace clp */
