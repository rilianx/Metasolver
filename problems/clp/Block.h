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
 *
 * Los bloques de la instancia se guardan en un arreglo contiguo (ordenado por id)
 * y las cantidades de cajas de todos ellos en otro, para que los recorridos sobre
 * la lista de bloques validos accedan a la memoria en forma secuencial.
 */
class Block {
public:
	typedef std::pair<int,int> BoxCount;

	/** cantidades de cajas por tipo (rango dentro del arreglo comun) */
	struct Boxes {
		const BoxCount* first;
		const BoxCount* last;
		const BoxCount* begin() const { return first; }
		const BoxCount* end() const { return last; }
		size_t size() const { return last-first; }
	};

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
	 * Genera los bloques de la instancia: primero los bloques de una caja (en cada
	 * orientacion permitida) y luego los que resultan de combinarlos en las direcciones
	 * x, y, z (generador de bloques guillotina). Retorna a lo mas max_bl bloques.
	 * @param left cantidad disponible de cajas de cada tipo
	 * @param min_fr proporcion minima del volumen del bloque ocupada por cajas
	 */
	static std::vector<const Block*> generate_blocks(const std::vector<const BoxShape*>& types,
			const std::vector<int>& left, double min_fr, int max_bl, long L, long W, long H);

	//indice del bloque en la lista de bloques de la instancia
	int id;

	long l, w, h;
	long occupied_volume;
	int n_boxes;
	Boxes boxes;

private:
	static std::vector<Block> all;
	static std::vector<BoxCount> all_boxes;
};

} /* namespace clp */

#endif /* BLOCK_H_ */
