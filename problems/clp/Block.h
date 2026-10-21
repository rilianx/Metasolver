/*
 * Block.h
 *
 *  Created on: 01-06-2017
 *      Author: ignacio
 */

#ifndef BLOCK_H_
#define BLOCK_H_

#include <vector>
#include <utility>
#include "BoxShape.h"

namespace clp {

/**
 * Bloque: conjunto de cajas que se ubica como una unidad.
 * Solo se guardan sus dimensiones, el volumen ocupado y la cantidad
 * de cajas de cada tipo (ordenadas por tipo).
 */
class Block {
public:
	/** bloque de una caja en la orientacion o */
	Block(const BoxShape& box, int o) : id(-1),
		l(box.getL(o)), w(box.getW(o)), h(box.getH(o)),
		occupied_volume(box.getVolume()), n_boxes(1) {
		boxes.push_back(std::make_pair(box.type, 1));
	}

	Block(long l, long w, long h) : id(-1), l(l), w(w), h(h), occupied_volume(0), n_boxes(0) {}

	long getL() const { return l; }
	long getW() const { return w; }
	long getH() const { return h; }
	long getVolume() const { return l*w*h; }
	long getOccupiedVolume() const { return occupied_volume; }

	/** cantidad de cajas del tipo t en el bloque */
	int count(int t) const {
		for(auto& p : boxes) if(p.first==t) return p.second;
		return 0;
	}

	bool fits(long L, long W, long H) const { return l<=L && w<=W && h<=H; }

	/**
	 * Genera bloques combinando bloques de una caja en las direcciones x, y, z
	 * (generador de bloques guillotina). Retorna a lo mas max_bl bloques.
	 * @param left cantidad disponible de cajas de cada tipo
	 * @param min_fr proporcion minima del volumen del bloque ocupada por cajas
	 */
	static void generate_blocks(std::vector<const Block*>& B, const std::vector<int>& left,
			double min_fr, int max_bl, long L, long W, long H);

	//indice del bloque en la lista de bloques de la instancia
	int id;

	long l, w, h;
	long occupied_volume;
	int n_boxes;
	std::vector< std::pair<int,int> > boxes;
};

} /* namespace clp */

#endif /* BLOCK_H_ */
