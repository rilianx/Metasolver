/*
 * cpmpState.h
 *
 *  Created on: 24 may. 2018
 *      Author: iaraya
 */

#include "State.h"
#include <functional>   // std::greater
#include <algorithm>// std::sort
#include <vector>
#include <fstream>
#include <set>

#ifndef CPMPSTATE_H_
#define CPMPSTATE_H_

using namespace metasolver;
using std::vector;

//Viendo si funciona profesor
//Segundo test

/*
 *
 * 	Esta Clase fue creada con motivo de ayudar a implementar de forma
 * 	sencilla el cálculo del Lowerbound().
 *
 *
 * 	Calculo del GX del calcularLowerBound en Base al Paper de Andreas Bortfeldt
 * 	A tree search procedure for the contianer pre-marshalling problem
 * 	doi: 10.1016/j.ejor.2011.10005
 *
 *
 * */

class GX{
public:
    int number;
    int demand;
    int cumulative_demand;
    int potential_supply;
    int cumulative_supply;
    int cumulative_demand_surplus;


    /**
     * 	Constructor de GX
     * @param number
     */
    GX(int number) : number(number), demand(0), cumulative_demand(0),
	potential_supply(0), cumulative_supply(0),
	cumulative_demand_surplus(0){
    }
};


class cpmpAction : public Action{
public:
	cpmpAction(int CI, int CF) : CI(CI), CF(CF) { }


	virtual Action* clone() const{ return new cpmpAction(*this); }

	const int CI;
	const int CF;

};

namespace cpmp {

class cpmpState : public State {
public:

	virtual ~cpmpState() { };


	cpmpState(const cpmpState& from){
	    height = from.height;
	    width = from.width;
	    layout = from.layout;
	    cantidadMovimientos = from.cantidadMovimientos;
	    totalContainers = from.totalContainers;
	    bienUbicados = from.bienUbicados;
	    malUbicados = from.malUbicados;
	    minimoMalUbicados = from.minimoMalUbicados;
	    movimientos = from.movimientos;
	    lowerBound = from.lowerBound;
	    containerMaximo = from.containerMaximo;
	}


	cpmpState(string File);

    /**
     * 	Constructor que recibe la Ruta del Archivo
     * 	Retorna una Matriz Inicializada con todos sus valores
     * 	y Containers cargados del archivo .bay.
     * @param File
     */

	/*
    public Matrix(String File) {
        String Line , Separador = ":";

        BufferedReader br = null;

        try {
            br = new BufferedReader(new FileReader(File));

            //Skip First Line
            Line = br.readLine();
            //Linea width
            Line = br.readLine();
            int width = Integer.parseInt(Line.split(Separador)[1].split(" ")[1]);
            //Linea height
            Line = br.readLine();
            int height = Integer.parseInt(Line.split(Separador)[1].split(" ")[1]);
            //Linea Total Containers
            Line = br.readLine();
            int Total = Integer.parseInt(Line.split(Separador)[1].split(" ")[1]);

            this.layout = new int[height][width];
            this.height = height;
            this.width = width;
            this.cantidadMovimientos = 0;
            this.totalContainers = 0;
            this.bienUbicados = 0;
            this.malUbicados = 0;
            this.minimoMalUbicados = 0;
            this.movimientos =  "";
            this.totalContainers = Total;
            this.lowerBound = 0;


            for(int j = 0; j < width; j++) {
                Line = br.readLine().split(Separador)[1];
                Scanner n = new Scanner(Line);
                int i = height-1;
                while(n.hasNext()) {
                    if(n.hasNextInt()) {
                        this.Set_Value(i, j, n.nextInt());
                        i--;
                    }
                }
                n.close();
            }
        }
        catch (FileNotFoundException e) {
            e.printStackTrace();
        }
        catch (IOException e) {
            e.printStackTrace();
        }
        finally {
            if(br != null) {
                try {
                    br.close();
                }
                catch(IOException e) {
                    e.printStackTrace();
                }
            }
        }
        this.inicializar();
    }*/


    /**
     * Calcula el máximo valor de container en la Matriz
     * @return
     * 		El valor del container más alto.
     */

    int maxValue() const {
        int i , j;
        int Max = 0;
        for(i = 0; i < height; i++) {
            for(j = 0; j < width; j++) {
                if(layout[i][j] > Max) Max = layout[i][j];
            }
        }
        return Max;
    }


    virtual int hashCode() const {
        int h = 0;
        int p = maxValue();
        int i , j;
        for(i = 0; i < height; i++) {
            for(j = 0; j < width; j++) {
                h = (int) (h * p * 0.6180334 + layout[i][j]);
            }
        }
        return h;
    }

    int getCantidadMovimientos() {
        return cantidadMovimientos;
    }

    /**
      *	Mostrar todos los datos relativos a la configuración como su actual
      *	Lowerbound , Cantidad de Movimientos Realizados , Secuencia de Movimientos Realizados
      *	Y Además muestra la configuración actual.
      *
      *
      *	@param
      *  @return
      *  	No Retorna Nada
      */

    virtual void print() const {
         //cout << "height: " << height << " width: " << width << endl;
         //cout << "Lower Bound: " << lowerBound << endl;
         cout << "Cantidad de Movimientos: " << cantidadMovimientos << endl;
         //cout << "Total de Containers: " << totalContainers << endl;
         //cout << "Total Bien Ubicados: " << bienUbicados << endl;
         //cout << "Total Mal Ubicados: " <<  malUbicados << endl;
         //cout << "Minimo de  Mal Ubicados: " << minimoMalUbicados << endl;
         //cout << "Movimientos Realizados: " << movimientos << endl;
        //showOnlyMatrix();
     }



     void showOnlyMatrix() const {
         for(int i = 0 ; i < height; i++) {
             for(int j = 0; j < width; j++) {
                 cout << layout[i][j] << " ";
             }
             cout << endl;
         }
     }

     /**
      *
      *	Compara si la Matriz es idéntica a la Matriz A. Para ser
      *	consideradas idénticas deben tener el mismo tamaño y además
      *	cada subtérmino [i][j] debe ser igual.
      *
      *	@param
      *	@return
      *		Retorna (True/False)
      * */

     virtual bool compareTo(const State &s) const  {
         const cpmpState& A = *dynamic_cast<const cpmpState*> (&s);
         if (height != A.height || width != A.width) return false;
         else {
             for(int i = 0; i < height; i++) {
                 for(int j = 0; j < width; j++) {
                     if(layout[i][j] != A.layout[i][j]) return false;
                 }
             }
             return true;
         }
     }



     /**
         * Calcula y Modifica la Cantidad de Containers bien ubicados y mal ubiados.
         *
         * @param
         * @return
         * 		No Retorna Nada.
         */

	void containersMalUbicados() {
		int Minimum = 9999;
		int Total = 0;
		int Local = 0;
		int Bottom = 0;
		int Bad = 0;
		int aux = 9999;
		for(int i=0; i<width; i++) {
			Local = 0;
			Bottom = layout[height-1][i];
			Bad = 0;
			Minimum = 9999;
			for(int j = height -1;j>=0;j--) {
				if(layout[j][i] == 0) {break;}
				if(layout[j][i] < Minimum ) {Minimum = layout[j][i];}
				if(layout[j][i] > Bottom || layout[j][i] > Minimum){Bad=1;}
				if(Bad==1) {Local++;}
			}
			Total = Total + Local;
			if(Local < aux) {aux = Local;}
		}
		minimoMalUbicados = aux;
		malUbicados = Total;
		bienUbicados = totalContainers - Total;
	}

    /**
     * Verifica si un movimiento es posible de realizar
     * @param CI
     * @param CF
     * @return
     * 		Retorna (True/False) dependiendo si es posible realizar el movimiento
     */

    bool verificarMovimiento(int CI, int CF) const {

        if(layout[0][CF-1]!= 0 || CF > width || layout[height-1][CI-1] == 0) return false;
        return true;

    }




	virtual State* clone() const{
		State* st= new cpmpState(*this);
		return st;
	}

    /**
      * Crea una Copia de la Matriz Orginal
      * @return
      * 		La Matriz Copiada.
      */
    cpmpState(cpmpState& st) : layout(st.layout), height(st.height), width(st.width),
			cantidadMovimientos(st.cantidadMovimientos), totalContainers(st.totalContainers),
			bienUbicados(st.bienUbicados), malUbicados(st.malUbicados),
			minimoMalUbicados(st.minimoMalUbicados), movimientos(st.movimientos),
			lowerBound(st.lowerBound),containerMaximo(st.containerMaximo)  {
     }

	/**
	 * The value of the objective function
	 */
	virtual double get_value() const{ return  (double) -1*(cantidadMovimientos + lowerBound);}

	virtual void _transition(const Action& action) {
		const cpmpAction *act = dynamic_cast<const cpmpAction*>(&action);
		int CI=act->CI;
		int CF=act->CF;

        int rowi = 0, rowf=0, i , j;


        //Matrix aux = new Matrix(0,0);
        //aux = this.Copy_Matrix();

        while(true) {
            if(rowi+1 == height || layout[rowi][CI-1]!= 0) break;
            rowi++;
        }
        while(true) {
            if(rowf+1 == height || layout[rowf+1][CF-1] != 0) break;
            rowf++;
        }

        layout[rowf][CF-1] = layout[rowi][CI-1];
        layout[rowi][CI-1] = 0;
        movimientos = movimientos + "\nC" + to_string(CI) + " -> C" + to_string(CF) + "";
        cantidadMovimientos++;
        inicializar();
	}

    /**
     * Revisa si una Columna se encuentra bien ordenada
     * @param Column
     * @return
     * 		Retorna (True/False) dependiendo si la Columna
     * 		está bien ordenada.
     */

    bool comprobarColumna(int Column) const {
        int j;
        for(j=height-1; j>=1; j--) {
            if(layout[j][Column-1] < layout[j-1][Column-1]) return false;
        }
        return true;
    }

    /**
     *	Genera una Lista con cada valor existente en la Matriz de Containers
     *
     *	No agrega repetidos en la Lista
     */

    set<int, std::greater<int> > getTierSet(){
    	set<int, std::greater<int> > Tier;
    	for(int j = 0; j < height ;j++)
    	    for(int k = 0; k < width ; k++)
    	    	Tier.insert(layout[j][k]);
        return Tier;
    }

    /**
      * 	Calculo del GX del LowerBound en Base al Paper de Andreas Bortfeldt
      * 	A tree search procedure for the contianer pre-marshalling problem
      * 	doi: 10.1016/j.ejor.2011.10005
      */

     int calcularGX() {
    	 set<int, std::greater<int> > Tier = getTierSet();
         list<GX> GX_Items;

         int Iterar;
         int Minimum_Stack;
         int Bad;
         int  i ,j;
         int Bad_Placed;
         int Cumulative_Bad_Placed = 0;
         int Supply;
         int Cumulative_Supply=0;
         int Bottom;
         int Last_Item;

         for(int type:Tier){
             GX* Auxiliar = new GX(type);
             Bad_Placed = 0;
             Supply = 0;
             for(i = 0; i < width; i++) {
                 Minimum_Stack = 9999;
                 Bad = 0;
                 Bottom = layout[height-1][i];
                 Last_Item = Bottom;
                 for(j = height-1; j>=0; j--) {
                     if(layout[j][i] < Minimum_Stack) {Minimum_Stack = layout[j][i];}
                     if(layout[j][i] > Bottom || layout[j][i] > Minimum_Stack) {Bad=1;}
                     if(Bad==1 && layout[j][i] == type) {Bad_Placed++;Cumulative_Bad_Placed++;}
                     if(Bad==1 && Last_Item == type) {Supply++;Cumulative_Supply++;}
                     if(Bad==0) {Last_Item = layout[j][i];}
                 }
             }
             Auxiliar->demand = Bad_Placed;
             Auxiliar->cumulative_demand = Cumulative_Bad_Placed;
             Auxiliar->potential_supply = Supply;
             Auxiliar->cumulative_supply  = Cumulative_Supply;
             GX_Items.push_back(*Auxiliar);
         }

         int min = -9999;
         for(auto item:GX_Items){
             if(item.cumulative_demand - item.cumulative_supply > min)
            	 min = item.cumulative_demand - item.cumulative_supply;
         }
         return min;
     }

     /**
      * Calcula BX y GX del LowerBound
      * @return
      * 	LowerBound calculado
      */

     int calcularLowerBound() {
         containersMalUbicados();
         lowerBound = minimoMalUbicados + malUbicados;
         return lowerBound;
     }

     /**
      * 	Inicializa los parámetros de la Matriz llamando
      * 	a la las funciones LowerBound y Badly_Placed_Containers
      */

     void inicializar() {
         calcularLowerBound();
     }

     virtual void get_actions(list< Action* >& action) const {
         if(lowerBound == 0)
             return;
         for(int i = 1; i <= width; i++)
             for(int j=1; j <= width; j++)
                 if(i!=j && verificarMovimiento(i, j))
                 	 action.push_back(new cpmpAction(i,j));
     }


     virtual int get_lower_bound() {
         return lowerBound;
     }

     int topColumn(int numeroColumna) const{
         for(int j=height-1; j>=1; j--){
             if(layout[j-1][numeroColumna] == 0)
                 return layout[j][numeroColumna];
         }
         return layout[0][numeroColumna];
     }




private:

	vector<vector<int>> layout;
    int lowerBound;
    int height, width;
    int cantidadMovimientos;
    int totalContainers;
    int bienUbicados;
    int malUbicados;
    int minimoMalUbicados;
    int containerMaximo;
    string movimientos;



};

} /* namespace cpmp */

#endif /* CPMPSTATE_H_ */
