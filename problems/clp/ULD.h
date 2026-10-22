/*
 * ULD.h
 *
 * Unit Load Device (contenedor o pallet de carga aerea), segun las instancias de
 * Paquay et al. (2016, 2018): una caja L x W x H a la que se le pueden recortar
 * hasta cuatro esquinas en el plano x-z (para calzar con el fuselaje), con
 * capacidad de peso y desviacion permitida del centro de gravedad.
 */

#ifndef ULD_H_
#define ULD_H_

#include <vector>
#include <string>
#include <cstdlib>
#include "Space.h"

namespace clp {

/**
 * Recorte (semiplano) en el plano x-z: el espacio utilizable cumple nx*x + nz*z <= c
 */
struct Cut {
	long nx, nz, c;
};

class ULD {
public:
	int type;
	long L, W, H;
	double max_weight;
	double cost;
	//desviacion maxima del centro de gravedad respecto al centro (en cada eje)
	long dev[3];
	std::vector<Cut> cuts;
	//volumen utilizable (sin los recortes)
	double volume;

	/** el bloque [x1,x2] x [z1,z2] (en el plano x-z) cumple todos los recortes */
	bool inside(long x1, long z1, long x2, long z2) const {
		for(const Cut& f : cuts)
			if(f.nx*(f.nx>0? x2:x1) + f.nz*(f.nz>0? z2:z1) > f.c) return false;
		return true;
	}

	bool inside(const AABB& b) const { return inside(b.xmin, b.zmin, b.xmax, b.zmax); }

	/**
	 * Ajusta la posicion (x,z) de un bloque de largo l y alto h para que cumpla los
	 * recortes, desplazandolo en x (dentro de [xlo,xhi]) o, si allow_z, en z
	 * (dentro de [zlo,zhi]). Se elige el menor desplazamiento.
	 * Retorna false si no hay una posicion factible.
	 */
	bool adjust(long& x, long& z, long l, long h, long xlo, long xhi, long zlo, long zhi, bool allow_z) const {
		if(cuts.empty() || inside(x, z, x+l, z+h)) return true;

		long bx=0, bz=0;
		bool okx=shift(x, z, l, h, xlo, xhi, true, bx);
		bool okz=allow_z && shift(x, z, l, h, zlo, zhi, false, bz);
		if(!okx && !okz) return false;
		if(okx && (!okz || std::labs(bx-x) <= std::labs(bz-z))) x=bx;
		else z=bz;
		return true;
	}

	/** lee los ULDs del archivo uld.csv de Paquay et al. */
	static std::vector<ULD> read(const std::string& file);

private:

	static long floor_div(long a, long b){ long q=a/b; if((a%b!=0) && ((a<0)!=(b<0))) q--; return q; }
	static long ceil_div(long a, long b){ return -floor_div(-a, b); }

	/** posicion mas cercana en un eje (x si in_x, z si no) que cumple los recortes */
	bool shift(long x, long z, long l, long h, long lo, long hi, bool in_x, long& best) const {
		for(const Cut& f : cuts){
			//f.nx*X + f.nz*Z <= c, con X,Z la esquina del bloque que maximiza el lado izquierdo
			long a = in_x? f.nx : f.nz;
			long rest = in_x? f.nz*(f.nz>0? z+h:z) : f.nx*(f.nx>0? x+l:x);
			long ext = in_x? l : h;
			if(a==0){ if(rest > f.c) return false; continue; }
			if(a>0) hi = std::min(hi, floor_div(f.c - rest, a) - ext);
			else lo = std::max(lo, ceil_div(f.c - rest, a));
		}
		if(lo>hi) return false;
		long cur = in_x? x : z;
		best = std::min(std::max(cur, lo), hi);
		return true;
	}
};

} /* namespace clp */

#endif /* ULD_H_ */
