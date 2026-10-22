/*
 * BoxShape.h
 *
 *  Created on: 2 jun. 2017
 *      Author: iaraya
 */

#ifndef BOXSHAPE_H_
#define BOXSHAPE_H_

namespace clp {

/**
 * Tipo de caja: dimensiones, orientaciones permitidas, peso y si es fragil.
 * Cada tipo se identifica por su indice (type) en la instancia.
 */
class BoxShape {
public:
	enum Orientation{LWH=0, LHW, WLH, WHL, HLW, HWL};

	BoxShape(int type, int id, long l, long w, long h, bool rot1, bool rot2, bool rot3,
			double weight=0.0, bool fragile=false) :
		type(type), id(id), l(l), w(w), h(h), weight(weight), fragile(fragile) {
		valid[LWH]=true;
		valid[WLH]=rot3;
		valid[WHL]=rot1;
		valid[HWL]=rot1;
		valid[HLW]=rot2;
		valid[LHW]=rot2;
	}

	bool is_valid(int o) const { return valid[o]; }

	long getL(int o) const {
		if(o==WLH || o==WHL) return w;
		if(o==HLW || o==HWL) return h;
		return l;
	}

	long getW(int o) const {
		if(o==WLH || o==HLW) return l;
		if(o==WHL || o==LHW) return h;
		return w;
	}

	long getH(int o) const {
		if(o==HLW || o==LHW) return w;
		if(o==HWL || o==WHL) return l;
		return h;
	}

	/** i-esima dimension (0:L, 1:W, 2:H) de la caja en la orientacion o */
	long get(int i, int o) const {
		switch(i){
			case 0: return getL(o);
			case 1: return getW(o);
			default: return getH(o);
		}
	}

	long getVolume() const { return l*w*h; }

	const int type;
	const int id;
	const long l, w, h;

	double weight;

	//no se puede ubicar nada sobre la caja
	bool fragile;

private:
	bool valid[6];
};

} /* namespace clp */

#endif /* BOXSHAPE_H_ */
