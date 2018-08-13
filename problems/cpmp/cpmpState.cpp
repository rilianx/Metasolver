/*
 * cpmpState.cpp
 *
 *  Created on: 24 may. 2018
 *      Author: iaraya
 */

#include "cpmpState.h"
#include <iostream>
#include <sstream>

namespace cpmp {

cpmpState::cpmpState(string File) : cantidadMovimientos(0), totalContainers(0), bienUbicados(0), malUbicados(0),
		minimoMalUbicados(0), movimientos(""), lowerBound(0){

    totalContainers = 0 , width = 0;

    //Tamano Maximo encontrado
    int maxHeight = 0;
    int primerNumeroLinea;

    string line;

    ifstream myfile;

    myfile.open(File);

    /*
     *  La Primera lectura del archivo se encargara de encontrar los siguientes valores.
     *  - Cantidad de Stacks
     *  - Cantidad de Containers
     *  - Maxima altura que hay en un Stack
     * */

    if(myfile.is_open()) {

        //Primero se obtiene el ancho de la Matriz
        getline(myfile, line, ' ');

        width = stoi(line);
        //Luego se obtiene la cantidad total de Containers
        getline(myfile, line);

        totalContainers = stoi(line);

        //Skip First Line
        //getline(myfile,line);

        while(getline(myfile,line)){
            primerNumeroLinea = stoi(line.substr(0,line.find(' ')));
            if(primerNumeroLinea > maxHeight){
                maxHeight = primerNumeroLinea;
            }
            //cout << line << '\n';
        }
        myfile.close();
    }
    else{
        cout << "Imposible abrir el archivo";
    }


	maxHeight = maxHeight+2;

	layout = vector<vector<int> >(maxHeight, vector<int>(width,0));
	height = maxHeight;

	myfile.open(File);

	if(myfile.is_open()){

		int col = 0;
		getline(myfile,line);
		while(getline(myfile,line)){
			std::stringstream ss(line);

			int altura = maxHeight-1;
			std::string token;
			getline(ss,token, ' ');
			while(getline(ss,token, ' ')){
				layout[altura][col] = stoi(token);
				altura--;
			}


			col++;
		}


	}else{
	    cout << "No se pudo abrir el archivo";
	}
    inicializar();
}

} /* namespace cpmp */
