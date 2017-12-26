
#include "Chromosome.h"

Chromosome::Chromosome(list<Gen>& genes) : genes(genes){ }

Chromosome* Chromosome::copy() {
	return new Chromosome(genes);
}

/*
 * TODO: Operador de mutacion
 * Mutates the chromosome
 */
Chromosome* Chromosome::mutate(){
	Chromosome* c_new = copy();

	//modifica sus genes de acuerdo a alguna probabilidad
	return c_new;
}

/**
 * TODO: Entrecruza los Cromosomas
 */
Chromosome* Chromosome::crossover(Chromosome* c2){
	Chromosome* c_new = copy();

	//cruzar this con c2
	//Elegir gen aleatorio de cada cromosoma para obtener punto de division.
	/*
	 * Elegir aleatoriamente un indice para los dos cromosomas.
	 * Guardar indice.
	 */
	int c2_gen_index = rand() % c2->genes.size();
	int new_gen_index = rand() % c_new->genes.size();

	//Intercambiar genes entre los cromosomas en funcion al indice de division.
	std::list<Gen>::iterator itc2;
	for(itc2 = c2->genes.begin(); itc2 < c2_gen_index; ++itc2){ }

	std::list<Gen>::iterator itnew;
	for(itnew = c_new->genes.begin(); itnew < new_gen_index; ++itnew){ }

	++itc2;
	++itnew;
	c_new->genes.splice(itnew,c2->genes,itc2,c2->genes.end());

	//Se eliminan los genes repetidos y se agregan los faltantes.
	/* En los cromosomas obtenidos:
	 * Eliminar los datos repetidos
	 * Agregar datos faltantes.
	 */
	c_new->genes.unique();
// Falta agregar elementos faltantes
	return c_new;
}

/**
 * TODO: Agrega un Gen al Cromosoma aleatoriamente
 */
void Chromosome::add_rnd_gene(Chromosome* c){
    //se elige un tipo de pieza aleatorio
    int gen_index = rand() % c->genes.size(); //utilizado para obtener el gen aleatorio
    std::list<Gen> gen_candidato;	//utilizado para guardar genes del mismo tipo
    std::list<Gen>::iterator it;	//iterador de un cromosoma G_x
    std::list<Gen>::iterator it2;	//iterador de un cromosoma G_y, luego se utiliza para los genes candidatos

    for(it = c->genes.begin(); it < gen_index; ++it){ }

    //elegir aleatoriamente un gen G_y del mismo tipo al seleccionado anteriormente
    for(it2 = c->genes.begin(); it2 != c->genes.end(); ++it2){
    	if(it2->type == it->type && it2->n < it->n)	//el numero de piezas debe ser menor al del cromosoma seleccionado
    		gen_candidato.push_back((Gen)it2);
    }

    gen_index = rand() % gen_candidato.size(); //iterador utilizado para seleccionar un genes candidatos aleatoriamente
    for(it2 = gen_candidato.begin(); it2 < gen_index; ++it2){ }

    Gen gene = new Gen(it->type,it2->n - it->n,0);	//crear un nuevo gen

    //elegir punto aleatorio del cromosoma para ser agregado
    gen_index = rand % c->genes.size();
    for(it2 = c->genes.begin(); it2 < gen_index; ++it2){ }

    //aregar y mover los demás genes hacia la derecha
    c->genes.emplace(it,(Gen)it2);
}

/**
 * TODO: Elimina un Gen al Cromosoma aleatoriamente
 */
void Chromosome::remove_rnd_gene(Chromosome* c){
	//elegir tipo de gen G_x candidato para la eliminacion
    int gen_index = rand() % c->genes.size(); //utilizado para obtener el gen aleatorio
    std::list<Gen> gen_candidato;	//utilizado para guardar genes del mismo tipo
    std::list<Gen>::iterator it;	//iterador de un cromosoma G_x
    std::list<Gen>::iterator it2;	//iterador de un cromosoma G_y, luego se utiliza para los genes candidatos

    for(it = c->genes.begin(); it < gen_index; ++it){ }

    //elegir aleatoriamente un gen G_y del mismo tipo al seleccionado anteriormente
    for(it2 = c->genes.begin(); it2 != c->genes.end(); ++it2){
    	if(it2->type == it->type && it2->n < it->n)	//el numero de piezas debe ser menor al del cromosoma seleccionado
    		gen_candidato.push_back((Gen)it2);
    }

    gen_index = rand() % gen_candidato.size(); //iterador utilizado para seleccionar un genes candidatos aleatoriamente
    for(it2 = gen_candidato.begin(); it2 < gen_index; ++it2){ }

    //a G_y se le suma la cantidad de piezas del gen G_x
    it2->n = it2->n+it->n;

    ////////////////////*ver si esta opcion se tomara en cuenta*////////////////////
    /*
    //en lo posible, la orientacion de las piezas deben ser distintas al momento de unir los genes
    if(it->n != it2->n)
    	it2->n=it2->n+it->n;
     */

	//se elimina G_x
	c->genes.remove((Gen)it);
}

/**
 * TODO: Cambia la orientación del Gen de un Cromosoma aleatoriamente
 */
void Chromosome::change_rnd_gene(Chromosome* c){
	const int number_orientation = 6;
	//se elige aleatoriamente un gen
    int gen_index = rand() % c->genes.size(); //utilizado para obtener el gen aleatorio
    std::list<Gen>::iterator it;
    for(it = c->genes.begin(); it < gen_index; ++it){ }

    //se elige aleatoriamente una orientacion del tipo de pieza
    int orientation = rand() % number_orientation; //orientacion aleatoria

    //realizar el cambio
    it->n = orientation;
}

