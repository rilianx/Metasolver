/*
 * Space.h
 *
 *  Created on: 01-06-2017
 *      Author: ignacio
 */

#ifndef SPACE_H_
#define SPACE_H_

#include <iostream>

namespace clp {

class Block;

/**
 * Caja alineada a los ejes (coordenadas enteras): [xmin,xmax) x [ymin,ymax) x [zmin,zmax)
 */
struct AABB {
	long xmin, ymin, zmin, xmax, ymax, zmax;

	AABB() : xmin(0), ymin(0), zmin(0), xmax(0), ymax(0), zmax(0) {}

	AABB(long x1, long y1, long z1, long x2, long y2, long z2) :
		xmin(x1), ymin(y1), zmin(z1), xmax(x2), ymax(y2), zmax(z2) {}

	long getL() const { return xmax-xmin; }
	long getW() const { return ymax-ymin; }
	long getH() const { return zmax-zmin; }
	long getVolume() const { return getL()*getW()*getH(); }
	long getSurface() const { return 2*(getL()*getW()+getL()*getH()+getW()*getH()); }

	/** interseccion de volumen no nulo */
	bool strict_intersects(const AABB& b) const {
		return !( xmax<=b.xmin || b.xmax<=xmin ||
				  ymax<=b.ymin || b.ymax<=ymin ||
				  zmax<=b.zmin || b.zmax<=zmin );
	}

	/** interseccion de cajas cerradas (incluye contacto) */
	bool touches(const AABB& b) const {
		return xmin<=b.xmax && b.xmin<=xmax &&
			   ymin<=b.ymax && b.ymin<=ymax &&
			   zmin<=b.zmax && b.zmin<=zmax;
	}

	/** *this contiene a b */
	bool contains(const AABB& b) const {
		return xmin<=b.xmin && ymin<=b.ymin && zmin<=b.zmin &&
			   xmax>=b.xmax && ymax>=b.ymax && zmax>=b.zmax;
	}
};

inline std::ostream& operator <<(std::ostream& os, const AABB& a){
	return os << "(" << a.xmin << "," << a.ymin << "," << a.zmin << ") ; ("
			<< a.xmax << "," << a.ymax << "," << a.zmax << ")";
}

/**
 * Bloque ubicado dentro del contenedor
 */
struct PlacedBlock : public AABB {
	PlacedBlock(long x, long y, long z, long l, long w, long h, const Block* block) :
		AABB(x, y, z, x+l, y+w, z+h), block(block) {}
	const Block* block;
};

/**
 * Espacio libre maximal del contenedor.
 * Se guarda la esquina de anclaje (la mas cercana a una esquina del contenedor)
 * y la distancia manhattan desde ella a esa esquina del contenedor.
 */
class Space : public AABB {
public:
	Space(const AABB& a, long L, long W, long H) : AABB(a), volume(a.getVolume()) {
		if(origin_anchor){
			anchor[0]=anchor[1]=anchor[2]=0;
			manhattan_distance=xmin+ymin+zmin;
			return;
		}
		long dist=0;
		if(xmin <= L - xmax){ dist+=xmin; anchor[0]=0; }
		else { dist+=L - xmax; anchor[0]=1; }

		if(ymin <= W - ymax){ dist+=ymin; anchor[1]=0; }
		else { dist+=W - ymax; anchor[1]=1; }

		if(zmin <= H - zmax){ dist+=zmin; anchor[2]=0; }
		else { dist+=H - zmax; anchor[2]=1; }

		manhattan_distance=dist;
	}

	/** posicion de un bloque de dimensiones l,w,h ubicado en la esquina de anclaje */
	void get_location(long l, long w, long h, long& x, long& y, long& z) const {
		x = (!anchor[0])? xmin : xmax-l;
		y = (!anchor[1])? ymin : ymax-w;
		z = (!anchor[2])? zmin : zmax-h;
	}

	/**
	 * Orden de seleccion de espacios: menor distancia manhattan,
	 * luego mayor volumen y luego orden lexicografico.
	 */
	bool operator<(const Space& sp2) const {
		if(manhattan_distance != sp2.manhattan_distance)
			return manhattan_distance < sp2.manhattan_distance;
		if(volume != sp2.volume) return volume > sp2.volume;
		if(xmin!=sp2.xmin) return xmin<sp2.xmin;
		if(ymin!=sp2.ymin) return ymin<sp2.ymin;
		if(zmin!=sp2.zmin) return zmin<sp2.zmin;
		if(getW()!=sp2.getW()) return getW()<sp2.getW();
		if(getL()!=sp2.getL()) return getL()<sp2.getL();
		return getH()<sp2.getH();
	}

	long volume;
	long manhattan_distance;
	bool anchor[3];

	//si es true, todos los espacios se anclan en la esquina mas cercana al origen
	//(la carga queda agrupada en torno al origen)
	static bool origin_anchor;
};

} /* namespace clp */

#endif /* SPACE_H_ */
