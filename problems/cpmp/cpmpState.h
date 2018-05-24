/*
 * cpmpState.h
 *
 *  Created on: 24 may. 2018
 *      Author: iaraya
 */

#include "State.h"
#include <functional>   // std::greater
#include <algorithm>    // std::sort

#ifndef CPMPSTATE_H_
#define CPMPSTATE_H_

using namespace metasolver;

/*
 *
 * 	Esta Clase fue creada con motivo de ayudar a implementar de forma
 * 	sencilla el cálculo del Lowerbound().
 *
 *
 * 	Calculo del GX del LowerBound en Base al Paper de Andreas Bortfeldt
 * 	A tree search procedure for the contianer pre-marshalling problem
 * 	doi: 10.1016/j.ejor.2011.10005
 *
 *
 * */

class GX{
private:
    int number;
    int demand;
    int cumulative_demand;
    int potential_supply;
    int cumulative_supply;
    int cumulative_demand_surplus;

public:
    /**
     * 	Constructor de GX
     * @param number
     */
    GX(int number) : number(number), demand(demand), cumulative_demand(cumulative_demand),
	potential_supply(potential_supply), cumulative_supply(cumulative_supply),
	cumulative_demand_surplus(cumulative_demand_surplus){

    }
}


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

	virtual ~cpmpState();

	   /**
	     * 	Constructor que recibe el Alto y Ancho
	     * 	Retorna una Matriz de Height x Width
	     * 	@param Height
	     * 	@param Width
	     */

	cpmpState(int Height , int Width) : Layout(Height, vector(Width,0)), Height(Height), Width(Width),
			Cantidad_Movimientos(0), Total_Containers(0), Well_Placed(0), Bad_Placed(0),
			Minimum_Bad_Placed(0), Movimientos(""), Lower_Bound(0){
	}


	cpmpState(string File) : Cantidad_Movimientos(0), Total_Containers(0), Well_Placed(0), Bad_Placed(0),
			Minimum_Bad_Placed(0), Movimientos(""), Lower_Bound(0){

		ifstream in(File.c_str());
		string line;
		getline(in,line); //number of instances

		std::stringstream ss(line);
		long l,w,h;
		ss >> l >> w >> h;





		int totalContainers = 0, width = 0;
		int maxHeight = 0;

		/*
		BufferedReader br = null;

		String line = "" , separador = " ";
		String[] separada;



		try{
			br = new BufferedReader(new FileReader(File));
			line = br.readLine();
			separada = line.split(separador);
			width = Integer.parseInt(separada[0]);
			totalContainers = Integer.parseInt(separada[1]);
			line = br.readLine();

			while(line != null){
				separada = line.split(separador);
				if(Integer.parseInt(separada[0])> maxHeight){

					maxHeight = Integer.parseInt(separada[0]);
				}
				line = br.readLine();
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
		}*/

		maxHeight = maxHeight+2;


		Layout = vector(maxHeight, vector(width,0));
		Height = maxHeight;
		Width = width;

		Total_Containers = totalContainers;

		/*
		try{
			br = new BufferedReader(new FileReader(File));
			line = br.readLine();
			line = br.readLine();
			int col = 0;
			while(line != null){
				int altura = maxHeight-1;
				Scanner n = new Scanner(line);
				line = br.readLine();
				n.nextInt();
				while(n.hasNext()){
					if(n.hasNextInt()){
						//System.out.println("Agregando en " + altura + " " + col);
						Layout[altura][col] = n.nextInt();
						altura--;
					}
				}
				n.close();
				col++;
			}
		}        catch (FileNotFoundException e) {
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
		}*/


		Initialize();

	}

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
            //Linea Width
            Line = br.readLine();
            int Width = Integer.parseInt(Line.split(Separador)[1].split(" ")[1]);
            //Linea Height
            Line = br.readLine();
            int Height = Integer.parseInt(Line.split(Separador)[1].split(" ")[1]);
            //Linea Total Containers
            Line = br.readLine();
            int Total = Integer.parseInt(Line.split(Separador)[1].split(" ")[1]);

            this.Layout = new int[Height][Width];
            this.Height = Height;
            this.Width = Width;
            this.Cantidad_Movimientos = 0;
            this.Total_Containers = 0;
            this.Well_Placed = 0;
            this.Bad_Placed = 0;
            this.Minimum_Bad_Placed = 0;
            this.Movimientos =  "";
            this.Total_Containers = Total;
            this.Lower_Bound = 0;


            for(int j = 0; j < Width; j++) {
                Line = br.readLine().split(Separador)[1];
                Scanner n = new Scanner(Line);
                int i = Height-1;
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
        this.Initialize();
    }*/


    /**
     * Calcula el máximo valor de container en la Matriz
     * @return
     * 		El valor del container más alto.
     */

    int Max_Value() {
        int i , j;
        int Max = 0;
        for(i = 0; i < Height; i++) {
            for(j = 0; j < Width; j++) {
                if(Layout[i][j] > Max) Max = Layout[i][j];
            }
        }
        return Max;
    }


    int hashCode() {
        int h = 0;
        int p = Max_Value();
        int i , j;
        for(i = 0; i < Height; i++) {
            for(j = 0; j < Width; j++) {
                h = (int) (h * p * 0.6180334 + Layout[i][j]);
            }
        }
        return h;
    }

    int getCantidad_Movimientos() {
        return Cantidad_Movimientos;
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
         cout << "Height: " << Height << " Width: " << Width << endl;
         System.out.println("LowerBound: " + this.Lower_Bound);
         System.out.println("Cantidad de Movimientos: " + this.Cantidad_Movimientos);
         System.out.println("Total de Containers: " + this.Total_Containers);
         System.out.println("Total Bien Ubicados: " + this.Well_Placed);
         System.out.println("Total Mal Ubicados : " + this.Bad_Placed);
         System.out.println("Minimo de Mal Ubicados: " + this.Minimum_Bad_Placed);
         System.out.println("Movimientos Realizados: " + this.Movimientos);
         Show_Matrix_Only();
     }

     /**
      * Muestra únicamente la Matriz, sin mostrar
      * los otros parámetros asociados como el Lowerbound.
      *
      * @param
      *
      * @return
      * 		No retorna nada
      *
      *  */

     void Show_Matrix_Only() const {
         for(int i = 0 ; i < Height; i++) {
             for(int j = 0; j < Width; j++) {
                 System.out.print(this.Layout[i][j] + "   ");
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

     bool Compare_To(cpmpState& A) {
         if (Height != A.Height || Width != A.Width) return false;
         else {
             for(int i = 0; i < Height; i++) {
                 for(int j = 0; j < Width; j++) {
                     if(Layout[i][j] != A.Layout[i][j]) return false;
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

	void Badly_Placed_Containers() {
		int Minimum = 9999;
		int Total = 0;
		int Local = 0;
		int Bottom = 0;
		int Bad = 0;
		int aux = 9999;
		for(int i=0; i<Width; i++) {
			Local = 0;
			Bottom = Layout[Height-1][i];
			Bad = 0;
			Minimum = 9999;
			for(int j = Height -1;j>=0;j--) {
				if(Layout[j][i] == 0) {break;}
				if(Layout[j][i] < Minimum ) {Minimum = Layout[j][i];}
				if(Layout[j][i] > Bottom || Layout[j][i] > Minimum){Bad=1;}
				if(Bad==1) {Local++;}
			}
			Total = Total + Local;
			if(Local < aux) {aux = Local;}
		}
		Minimum_Bad_Placed = aux;
		Bad_Placed = Total;
		Well_Placed = Total_Containers - Total;
	}

    /**
     * Verifica si un movimiento es posible de realizar
     * @param CI
     * @param CF
     * @return
     * 		Retorna (True/False) dependiendo si es posible realizar el movimiento
     */

    bool Possible_Movement(int CI ,int CF) {

        if(Layout[0][CF-1]!= 0 || CF > Width || Layout[Height-1][CI-1] == 0) return false;
        return true;

    }




	virtual State* clone() const{
		State* st=new cpmpState(*this);
		return st;
	}

    /**
      * Crea una Copia de la Matriz Orginal
      * @return
      * 		La Matriz Copiada.
      */
    cpmpState(cpmpState& st) : Layout(st.Layout), Height(st.Height), Width(st.Width),
			Cantidad_Movimientos(st.Cantidad_Movimientos), Total_Containers(st.Total_Containers),
			Well_Placed(st.Well_Placed), Bad_Placed(st.Bad_Placed),
			Minimum_Bad_Placed(st.Minimum_Bad_Placed), Movimientos(st.Movimientos),
			Lower_Bound(st.Lower_Bound)  {
     }








	/**
	 * The value of the objective function
	 */
	virtual double get_value() const{ return (double) Cantidad_Movimientos;}

	virtual void _transition(const Action& action) {
		cpmpAction *act = dynamic_cast<cpmpAction*>(&action);
		int CI=act->CI;
		int CF=act->CF;

        int rowi = 0, rowf=0, i , j;


        //Matrix aux = new Matrix(0,0);
        //aux = this.Copy_Matrix();

        while(true) {
            if(rowi+1 == Height || Layout[rowi][CI-1]!= 0) break;
            rowi++;
        }
        while(true) {
            if(rowf+1 == Height || Layout[rowf+1][CF-1] != 0) break;
            rowf++;
        }

        Layout[rowf][CF-1] = Layout[rowi][CI-1];
        Layout[rowi][CI-1] = 0;
        Movimientos = Movimientos + "\nC" + string(CI) + " -> C" + string(CF) + "";
        Cantidad_Movimientos++;
        Initialize();
	}

    /**
     * Revisa si una Columna se encuentra bien ordenada
     * @param Column
     * @return
     * 		Retorna (True/False) dependiendo si la Columna
     * 		está bien ordenada.
     */

    bool Check_Column(int Column) {
        int j;
        for(j=Height-1; j>=1; j--) {
            if(Layout[j][Column-1] < Layout[j-1][Column-1]) return false;
        }
        return true;
    }

    /**
     *	Genera una Lista con cada valor existente en la Matriz de Containers
     *
     *	No agrega repetidos en la Lista
     */

    set<int, std::greater<int> > Tier_Set(){
    	set<int, std::greater<int> > Tier;
    	for(int j = 0; j < Height ;j++)
    	    for(int k = 0; k < Width ; k++)
    	    	Tier.insert(Layout[j][k]);
        return Tier;
    }

    /**
      * 	Calculo del GX del LowerBound en Base al Paper de Andreas Bortfeldt
      * 	A tree search procedure for the contianer pre-marshalling problem
      * 	doi: 10.1016/j.ejor.2011.10005
      */

     int Calculate_GX() {
    	 set<int, std::greater<int> > Tier = Tier_Set();
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

         for(auto type:Tier){
             GX Auxiliar = new GX(type);
             Bad_Placed = 0;
             Supply = 0;
             for(i = 0; i < Width; i++) {
                 Minimum_Stack = 9999;
                 Bad = 0;
                 Bottom = Layout[Height-1][i];
                 Last_Item = Bottom;
                 for(j = Height-1; j>=0; j--) {
                     if(Layout[j][i] < Minimum_Stack) {Minimum_Stack = Layout[j][i];}
                     if(Layout[j][i] > Bottom || Layout[j][i] > Minimum_Stack) {Bad=1;}
                     if(Bad==1 && Layout[j][i] == type) {Bad_Placed++;Cumulative_Bad_Placed++;}
                     if(Bad==1 && Last_Item == type) {Supply++;Cumulative_Supply++;}
                     if(Bad==0) {Last_Item = Layout[j][i];}
                 }
             }
             Auxiliar.demand = Bad_Placed;
             Auxiliar.cumulative_demand = Cumulative_Bad_Placed;
             Auxiliar.potential_supply = Supply;
             Auxiliar.cumulative_supply  = Cumulative_Supply;
             GX_Items.push_back(Auxiliar);
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

     int LowerBound() {
         Badly_Placed_Containers();
         Lower_Bound = Minimum_Bad_Placed + Bad_Placed + Calculate_GX();
         return Lower_Bound;
     }

     /**
      * 	Inicializa los parámetros de la Matriz llamando
      * 	a la las funciones LowerBound y Badly_Placed_Containers
      */

     void Initialize() {
         LowerBound();
     }

     virtual void get_actions(list< Action* >& action) const {
         for(int i = 1; i <= Width; i++)
             for(int j=1; j <= Width; j++)
                 if(i!=j && Possible_Movement(i,j))
                 	 action.push_back(new cpmpAction(i,j));
     }


     virtual int get_lower_bound() {
         return Lower_Bound;
     }


private:

	vector<vector<int> > Layout;
    int Lower_Bound;
    int Height, Width;
    int Cantidad_Movimientos;
    int Total_Containers;
    int Well_Placed;
    int Bad_Placed;
    int Minimum_Bad_Placed;
    string Movimientos;



};

} /* namespace cpmp */

#endif /* CPMPSTATE_H_ */
