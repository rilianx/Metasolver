import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.*;


/*
 * 	La clase Principal del Programa, utilizada para Representar cada estado
 * 	posible de una Matriz de Containers.
 *
 * 	Incluye al menos 20 métodos propios para operar de forma sencilla.
 *
 * */
public class Matrix {

    private int[][] Layout;
    private int Lower_Bound;
    private int Height, Width;
    private int Cantidad_Movimientos;
    private int Total_Containers;
    private int Well_Placed;
    private int Bad_Placed;
    private int Minimum_Bad_Placed;
    private String Movimientos;

    /**
     *	Constructor Natural sin Parametros
     *	Retorna una Matriz 0x0
     *
     */

    public Matrix() {
        this.Layout = new int[0][0];
        this.Height = Height;
        this.Width = Width;
        this.Cantidad_Movimientos = 0;
        this.Total_Containers = 0;
        this.Well_Placed = 0;
        this.Bad_Placed = 0;
        this.Minimum_Bad_Placed = 0;
        this.Movimientos = "";
        this.Lower_Bound = 0;
    }


    /**
     * 	Constructor que recibe el Alto y Ancho
     * 	Retorna una Matriz de Height x Width
     * 	@param Height
     * 	@param Width
     */

    public Matrix(int Height , int Width) {
        this.Layout = new int[Height][Width];
        this.Height = Height;
        this.Width = Width;
        this.Cantidad_Movimientos = 0;
        this.Total_Containers = 0;
        this.Well_Placed = 0;
        this.Bad_Placed = 0;
        this.Minimum_Bad_Placed = 0;
        this.Movimientos =  "";
        this.Lower_Bound = 0;
    }


    public Matrix(String File, int tipoArchivo){

        BufferedReader br = null;

        String line = "" , separador = " ";
        String[] separada;

        int totalContainers = 0, width = 0;
        int maxHeight = 0;

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
        }

        maxHeight = maxHeight+2;
        this.Layout = new int[maxHeight][width];
        this.Height = maxHeight;
        this.Width = width;
        this.Cantidad_Movimientos = 0;
        this.Total_Containers = 0;
        this.Well_Placed = 0;
        this.Bad_Placed = 0;
        this.Minimum_Bad_Placed = 0;
        this.Movimientos =  "";
        this.Total_Containers = totalContainers;
        this.Lower_Bound = 0;

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
        }
        this.Initialize();

    }


    /**
     * 	Constructor que recibe la Ruta del Archivo
     * 	Retorna una Matriz Inicializada con todos sus valores
     * 	y Containers cargados del archivo .bay.
     * @param File
     */

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
    }

    @Override
    public int hashCode() {
        int h = 0;
        int p = this.Max_Value();
        int i , j;
        for(i = 0; i < this.Height; i++) {
            for(j = 0; j < this.Width; j++) {
                h = (int) (h * p * 0.6180334 + this.Layout[i][j]);
            }
        }
        return h;
    }

    public int getCantidad_Movimientos() {
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

    public void Show_Matrix() {
        System.out.println("Height: " + this.Height + " Width: " + this.Width);
        System.out.println("LowerBound: " + this.Lower_Bound);
        System.out.println("Cantidad de Movimientos: " + this.Cantidad_Movimientos);
        System.out.println("Total de Containers: " + this.Total_Containers);
        System.out.println("Total Bien Ubicados: " + this.Well_Placed);
        System.out.println("Total Mal Ubicados : " + this.Bad_Placed);
        System.out.println("Minimo de Mal Ubicados: " + this.Minimum_Bad_Placed);
        System.out.println("Movimientos Realizados: " + this.Movimientos);
        for(int i = 0 ; i < Height; i++) {
            for(int j = 0; j < Width; j++) {
                System.out.print(this.Layout[i][j] + " ");
            }
            System.out.println();
        }
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

    public void Show_Matrix_Only() {
        for(int i = 0 ; i < Height; i++) {
            for(int j = 0; j < Width; j++) {
                System.out.print(this.Layout[i][j] + "   ");
            }
            System.out.println();
        }
    }



    /**
     * Cambia el valor en la Matriz[i][j] por el valor dado como parametro
     * @param i (Posicion i)
     * @param j (Posicion j)
     * @param val (Valor a cambiar)
     * @return
     * 		No Retorna Nada
     */

    public void Set_Value(int i , int j , int val) {
        if(i < this.Height && j < this.Width) {
            this.Layout[i][j] = val;
        }
        else {
            System.out.println("Imposible");
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

    public boolean Compare_To(Matrix A) {
        if (this.Height != A.Height || this.Width != A.Width) return false;
        else {
            for(int i = 0; i < this.Height; i++) {
                for(int j = 0; j < this.Width; j++) {
                    if(this.Layout[i][j] != A.Layout[i][j]) return false;
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

    public void Badly_Placed_Containers() {
        int Minimum = 9999;
        int Total = 0;
        int Local = 0;
        int Bottom = 0;
        int Bad = 0;
        int aux = 9999;
        for(int i=0; i<this.Width; i++) {
            Local = 0;
            Bottom = this.Layout[Height-1][i];
            Bad = 0;
            Minimum = 9999;
            for(int j = this.Height -1;j>=0;j--) {
                if(this.Layout[j][i] == 0) {break;}
                if(this.Layout[j][i] < Minimum ) {Minimum = this.Layout[j][i];}
                if(this.Layout[j][i] > Bottom || this.Layout[j][i] > Minimum){Bad=1;}
                if(Bad==1) {Local++;}
            }
            Total = Total + Local;
            if(Local < aux) {aux = Local;}
        }
        this.Minimum_Bad_Placed = aux;
        this.Bad_Placed = Total;
        this.Well_Placed = this.Total_Containers - Total;

    }



    /**
     * Calcula el máximo valor de container en la Matriz
     * @return
     * 		El valor del container más alto.
     */

    public int Max_Value() {
        int i , j;
        int Max = 0;
        for(i = 0; i < this.Height; i++) {
            for(j = 0; j < this.Width; j++) {
                if(this.Layout[i][j] > Max) Max = this.Layout[i][j];
            }
        }
        return Max;
    }



    /**
     * Verifica si un movimiento es posible de realizar
     * @param CI
     * @param CF
     * @return
     * 		Retorna (True/False) dependiendo si es posible realizar el movimiento
     */

    public boolean Possible_Movement(int CI ,int CF) {

        if(this.Layout[0][CF-1]!= 0 || CF > this.Width || this.Layout[this.Height-1][CI-1] == 0) return false;
        return true;

    }



        /**
         * Crea una Copia de la Matriz Orginal
         * @return
         * 		La Matriz Copiada.
         */

        public Matrix Copy_Matrix() {
            Matrix M = new Matrix(this.Height,this.Width);
            M.Lower_Bound = this.Lower_Bound;
            M.Bad_Placed = this.Bad_Placed;
            M.Cantidad_Movimientos = this.Cantidad_Movimientos;
            M.Lower_Bound = this.Lower_Bound;
            M.Minimum_Bad_Placed = this.Minimum_Bad_Placed;
            M.Movimientos = this.Movimientos;
            M.Total_Containers = this.Total_Containers;
            M.Well_Placed = this.Well_Placed;
            int i , j;
            for(i = 0; i < this.Height; i++) {
                for(j=0; j< this.Width; j++) {
                    M.Layout[i][j] = this.Layout[i][j];
                }
            }
            M.Initialize();
            return M;
        }



    /**
     * Traslada un Container en la Matriz
     * @param CI
     * @param CF
     * @return
     * 		Retorna la Matriz nueva, aumenta la cantidad de movimientos y ademas
     * 		agrega a la lista de movimientos.
     */

    public Matrix Traslado_Container(int CI , int CF) {
        int rowi = 0, rowf=0, i , j;
        Matrix aux = new Matrix(0,0);
        aux = this.Copy_Matrix();
        String mov = "";
        while(true) {
            if(rowi+1 == this.Height || this.Layout[rowi][CI-1]!= 0) break;
            rowi++;
        }
        while(true) {
            if(rowf+1 == this.Height || this.Layout[rowf+1][CF-1] != 0) break;
            rowf++;
        }
        aux.Layout[rowf][CF-1] = aux.Layout[rowi][CI-1];
        aux.Layout[rowi][CI-1] = 0;
        mov = mov + "\nC" + String.valueOf(CI) + " -> C" + String.valueOf(CF) + "";
        aux.Movimientos = aux.Movimientos + mov;
        aux.Cantidad_Movimientos++;
        aux.Initialize();
        return aux;
    }



    /**
     * Trasla un Container de forma Inversa, para generar vecinos
     * a partir de soluciones parciales
     * @param CI
     * @param CF
     * @return
     */

    public Matrix Traslado_Container_Inverso(int CI, int CF) {
        int rowi = 0, rowf = 0, i , j;
        Matrix aux = new Matrix(0,0);
        aux = this.Copy_Matrix();
        String mov = "";
        while(true) {
            if(rowi+1==this.Height || aux.Layout[rowi][CI-1]!=0) break;
            rowi++;
        }
        while(true) {
            if(rowf+1==this.Height || aux.Layout[rowf+1][CF-1]!=0) break;
            rowf++;
        }

        aux.Layout[rowf][CF-1] = aux.Layout[rowi][CI-1];
        aux.Layout[rowi][CI-1] = 0;
        mov = "C"+String.valueOf(CF) + " -> C"+String.valueOf(CI) + " -- ";
        aux.Movimientos = mov + aux.Movimientos;
        aux.Cantidad_Movimientos++;
        aux.Initialize();
        return aux;

    }



    /**
     * Revisa si una Columna se encuentra bien ordenada
     * @param Column
     * @return
     * 		Retorna (True/False) dependiendo si la Columna
     * 		está bien ordenada.
     */

    public boolean Check_Column(int Column) {
        int j;
        for(j=this.Height-1; j>=1; j--) {
            if(this.Layout[j][Column-1] < this.Layout[j-1][Column-1]) return false;
        }
        return true;
    }



    /**
     * Revisa que tipo de movimiento es, en base a lo establecido
     * por la academia
     * @param CI
     * @param CF
     * @return
     * 		1 Si es BAD-GOOD
     * 		2 Si es GOOD-BAD
     * 		3 Si es BAD-BAD
     * 		4 Si es GOOD-GOOD
     */

    public int Good_Bad_Movement(int CI , int CF) {
        Matrix aux = new Matrix(0,0);
        aux = this.Copy_Matrix();
        aux = aux.Traslado_Container(CI, CF);
        boolean A = this.Check_Column(CI);
        boolean B = aux.Check_Column(CF);

        if(A == false && B == true ) return 1;
        else if(A == true && B == false) return 2;
        else if(A == false && B == false) return 3;
        else return 4;
    }



    /**
     *	Genera una Lista con cada valor existente en la Matriz de Containers
     *
     *	No agrega repetidos en la Lista
     */

    public List<Integer> Tier_List(){
        List<Integer> Tier = new ArrayList<>();
        int i , j , k , Validador;
        for( i = this.Max_Value(); i > 0; i--) {
            Validador = 1;
            for(j = 0; j < this.Height && Validador==1;j++) {
                for(k = 0; k < this.Width && Validador==1; k++) {
                    if(this.Layout[j][k] == i) {
                        Tier.add(i);
                        Validador=0;
                    }
                }
            }
        }
        return Tier;
    }



    /**
     * 	Calculo del GX del LowerBound en Base al Paper de Andreas Bortfeldt
     * 	A tree search procedure for the contianer pre-marshalling problem
     * 	doi: 10.1016/j.ejor.2011.10005
     */

    public int Calculate_GX() {
        List<Integer> Tier = this.Tier_List();
        List<GX> GX_Items = new ArrayList<>();

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

        for(Iterar = 0; Iterar < Tier.size(); Iterar++) {
            GX Auxiliar = new GX(Tier.get(Iterar));
            Bad_Placed = 0;
            Supply = 0;
            for(i = 0; i < this.Width; i++) {
                Minimum_Stack = 9999;
                Bad = 0;
                Bottom = this.Layout[this.Height-1][i];
                Last_Item = Bottom;
                for(j = this.Height-1; j>=0; j--) {
                    if(this.Layout[j][i] < Minimum_Stack) {Minimum_Stack = this.Layout[j][i];}
                    if(this.Layout[j][i] > Bottom || this.Layout[j][i] > Minimum_Stack) {Bad=1;}
                    if(Bad==1 && this.Layout[j][i] == Tier.get(Iterar)) {Bad_Placed++;Cumulative_Bad_Placed++;}
                    if(Bad==1 && Last_Item == Tier.get(Iterar)) {Supply++;Cumulative_Supply++;}
                    if(Bad==0) {Last_Item = this.Layout[j][i];}
                }
            }
            Auxiliar.demand = Bad_Placed;
            Auxiliar.cumulative_demand = Cumulative_Bad_Placed;
            Auxiliar.potential_supply = Supply;
            Auxiliar.cumulative_supply  = Cumulative_Supply;
            GX_Items.add(Auxiliar);
        }

        int min = -9999;
        for(Iterar = 0; Iterar < GX_Items.size(); Iterar++) {
            if(GX_Items.get(Iterar).cumulative_demand - GX_Items.get(Iterar).cumulative_supply > min) {min = GX_Items.get(Iterar).cumulative_demand - GX_Items.get(Iterar).cumulative_supply;}
        }
        return min;
    }



    /**
     * Calcula BX y GX del LowerBound
     * @return
     * 	LowerBound calculado
     */

    public int LowerBound() {
        this.Badly_Placed_Containers();
        this.Lower_Bound = this.Minimum_Bad_Placed + this.Bad_Placed + this.Calculate_GX();
        return this.Lower_Bound;
    }



    /**
     * 	Inicializa los parámetros de la Matriz llamando
     * 	a la las funciones LowerBound y Badly_Placed_Containers
     */

    public void Initialize() {
        this.LowerBound();
    }



    /**
     *	Se Genera una Lista con todos los Vecinos posibles a partir
     *	de la Matriz que llama a la funcion.
     *
     *	Se Consideran vecinos a todas aquellas configuraciones posibles
     *	de hacer a partir de un movimiento.
     * @return
     * 	Una Lista de Vecinos
     */

    public List<Matrix> Lista_Vecinos(List<Inmovible> Restricciones){
        List<Matrix> Vecinos = new ArrayList<>();
        int i , j;
        for(i=1;i<=this.Width;i++) {
            for(j=1;j<=this.Width;j++) {
                if(i!=j) {
                    if(this.Possible_Movement(i, j)) {
                        int valid = 1;
                        Matrix aux = this.Traslado_Container(i, j);
                        for(int k = 0; k < Restricciones.size(); k++) {
                            if(aux.Layout[Restricciones.get(k).Pos_I][Restricciones.get(k).Pos_J] != Restricciones.get(k).Valor) {
                                valid=0;
                                break;
                            }
                        }
                        if(valid==1) {Vecinos.add(aux);}
                    }
                }
            }
        }
        return Vecinos;
    }

    public List<Matrix> Lista_Vecinos(){
        List<Matrix> Vecinos = new ArrayList<>();
        for(int i = 1; i <= this.Width; i++){
            for(int j=1; j <= this.Width; j++){
                if(i!=j){
                    if(this.Possible_Movement(i,j)){
                        Matrix aux = this.Traslado_Container(i,j);
                        Vecinos.add(aux);
                    }
                }
            }
        }
        if(Vecinos.size()>0) return Vecinos;
        return null;
    }



    /**
     * 	Se Genera un Lista con todos los Vecinos Inversos posibles, es decir
     * 	que se generen de atras hacia adelante, y que además cumplan con la condición
     * 	de ser movimientos GOOD-BAD o BAD-BAD
     * @return
     * 	Una Lista de Vecinos
     */

    public List<Matrix> Lista_Vecinos_Inversos(){
        List<Matrix> Vecinos = new ArrayList<>();
        int i , j;
        for(i=1;i<=this.Width;i++) {
            for(j=1;j<=this.Width;j++) {
                if(i!=j) {
                    if(this.Possible_Movement(i, j)) {
                        if(this.Good_Bad_Movement(i, j) == 2 || this.Good_Bad_Movement(i, j) == 3) {
                            Matrix aux = this.Traslado_Container_Inverso(i, j);
                            Vecinos.add(aux);
                        }
                    }
                }
            }
        }
        return Vecinos;
    }



    /**
     * Verifica si el container ubicado en Pos_I , Pos_J esta bien ubicado
     * @param Pos_I
     * @param Pos_J
     * @return
     * Retorna (True/False) segun corresponda
     */

    public boolean Container_Bien_Ubicado(int Pos_I , int Pos_J) {
        int j;
        for(j=this.Height-1; j>Pos_I;j--) {
            if(this.Layout[j][Pos_J] < this.Layout[j-1][Pos_J]) return false;
        }
        return true;
    }



    /**
     *
     */

    public List<Inmovible> Analizar_Containers(){
        List<Inmovible> No_Mover = new ArrayList<>();
        int Repeat = 0 , Cont = 0;
        Matrix temporal = this.Copy_Matrix();

        int Aux;

        for(Repeat = 0; Repeat < this.Width && Cont < this.Width; Repeat++) {
            Aux = temporal.Max_Value();
            for(int i = this.Height-1; i>=0; i--) {
                for(int j = 0; j<this.Width;j++) {
                    if(temporal.Layout[i][j] == Aux) {
                        Cont++;
                        if(this.Container_Bien_Ubicado(i, j)) {
                            Inmovible Auxi = new Inmovible(temporal.Layout[i][j],i,j);
                            No_Mover.add(Auxi);
                        }
                        temporal.Layout[i][j] = 0;
                    }

                }
            }
        }
        return No_Mover;
    }


    /**
     * 	Retorna una Solucion a "mano" de la Matriz, la cual se encarga
     * 	de ordenar cada columna por separado.
     *
     *
     * Por Ejemplo:
     *
     * 	Inicialmente:
     * 		8	4	9
     * 		6	3	5
     * 		7	2	1
     *
     * 	Final:
     *
     * 		6	2	1
     * 		7	3	5
     * 		8	4	9
     *
     * @return
     * 	Solucion Generada
     */

    public Matrix Solucion_Ordenar_Columnas() {
        Matrix Temporal = this.Copy_Matrix();
        for(int j = 0; j < this.Width; j++) {
            for(int i = 0; i< this.Height; i++) {
                for(int k = i + 1; k < this.Height; k++) {
                    if(Temporal.Layout[i][j] > Temporal.Layout[k][j]) {
                        int aux = Temporal.Layout[i][j];
                        Temporal.Layout[i][j] = Temporal.Layout[k][j];
                        Temporal.Layout[k][j] = aux;
                    }
                }
            }
        }
        Temporal.Initialize();
        return Temporal;
    }



    /**
     * 	Retorna una Solucion a "mano" de la Matriz la cual apila en Columnas
     * 	los maximos valores de la Matriz
     *
     * 	Por Ejemplo:
     *
     * 	Inicialmente:
     * 		8	4	9
     * 		6	3	5
     * 		7	2	1
     *
     * 	Final:
     *
     * 		7	4	1
     * 		8	5	2
     * 		9	6	3
     * @return
     */

    public Matrix Solucion_Maximos() {

        Matrix Temporal = this.Copy_Matrix();
        Matrix Aux = this.Copy_Matrix();
        int i , j , k;
        float Contador = 0 , cont;
        int Promedio , Auxi;

        int I_Matrix = this.Height - 1 , J_Matrix = 0;

        for(j = 0; j < this.Width; j++) {
            cont = 0;
            for(i = this.Height - 1; i >=0; i--) {
                if(this.Layout[i][j] != 0) {cont++;}
                else {break;}
            }
            Contador = Contador + cont;
        }

        Promedio = Math.round(Contador/this.Width);

        for(i = 0; i < this.Height; i++) {
            for(j = 0; j < this.Width; j++) {
                Temporal.Layout[i][j] = 0;
                Aux.Layout[i][j] = this.Layout[i][j];
            }
        }

        for(k = 0; k < this.Total_Containers; k++) {
            Auxi = Aux.Max_Value();
            for(i = 0; i < this.Height; i++) {
                for(j = 0; j < this.Width; j++) {
                    if(Aux.Layout[i][j]==Auxi) {
                        Aux.Layout[i][j] = 0;
                        i = this.Height;
                        j = this.Width;
                        break;
                    }
                }
            }
            if(I_Matrix == this.Height-1-Promedio) {
                I_Matrix = this.Height-1;
                J_Matrix++;
            }
            Temporal.Layout[I_Matrix][J_Matrix] = Auxi;
            I_Matrix--;
        }
        Temporal.Initialize();
        return Temporal;
    }




    /**
     * 	Obtiene una Lista de los Containers Prioritarios a Mover
     * 	con el fin de poder generar restricciones si no las hubieran.
     *
     * 	Por Ejemplo:
     *
     * 	Inicial:
     *
     * 		8	4	9
     * 		6	7	5
     * 		1	2	1
     *
     * 	La Lista de Prioritarios a mover seria
     * 		8 en la Posicion [0][0]
     * 		7 en la Posicion [1][1]
     * 		9 en la Posicion [0][2]
     *
     */

    public List<Inmovible> Get_Priority_Containers(){
        int Max = this.Max_Value();
        int Cont = 0;
        boolean Valid = true;
        List<Inmovible> Lista = new ArrayList<>();
        Matrix Temporal = this.Copy_Matrix();
        for(int i = Max; i > 0 && Valid; i--) {
            for(int j = 0; j < this.Height && Valid; j++) {
                for(int k = 0; k < this.Width && Valid; k++) {
                    if(Temporal.Layout[j][k] == i) {
                        Inmovible Aux = new Inmovible(i,j,k);
                        Temporal.Layout[j][k] = 0;
                        Lista.add(Aux);
                        Cont++;
                    }
                    if(Cont == this.Width) {
                        Valid = false;
                    }
                }
            }
        }
        return Lista;
    }



    /**
     * 	Funcion que permite Acomodar la Matriz Inicial de Modo
     * 	de poder Generar Restricciones que permitan al Algoritmo
     * 	del Main poder resolver el CPMP
     * @return
     * 	Matriz Inicial Modificada
     */

    public Matrix Intentar_Restringir() {
        Matrix Aux = this.Copy_Matrix();
        Inmovible Max = Aux.Max_Get();
        int Drop_To = 0;
        int Objetivo = 0;
        List<Inmovible> Restricciones = Aux.Analizar_Containers();
        /*
         * 	Mientras no existan restricciones o no sean las mejores
         * */
        while(Restricciones.size() < 1 || (!Aux.Container_Bien_Ubicado(Max.Pos_I, Max.Pos_J) && !Aux.On_Top_Stack(Max))) {
            /*
             * 	Si es que Existen Restricciones pero no son las mejores entonces
             * 	se apilaran los containers en la Columna que contenga alguna restriccion
             * */
            if(Restricciones.size() > 0 && Restricciones.get(0).Pos_J != Max.Pos_J) {
                //Se Establece que la columna donde se apilaran las cosas es la de la restriccion
                Drop_To = Restricciones.get(0).Pos_J;
                int Cont = 9999;
                for(int i = 0; i < this.Width; i++) {
                    if(i != Drop_To && i != Max.Pos_J) {
                        if(Aux.Containers_Columna(i) < Cont) {
                            Cont = Aux.Containers_Columna(i);
                            //Se Intentara Dejar Vacia la Columna que tenga menos containers
                            Objetivo = i;
                        }
                    }
                }
            }
            /*
             * 	Si no Existen Restricciones entonces se apilaran los Containers
             * 	en aquella Columna que tenga el mejor container bien ubicado de alto valor
             * */
            else {
                System.out.println("CASO B");
                int Cont = 0;
                for(int i = 0; i < this.Width; i++) {
                    if(this.Layout[this.Height-1][i] > Cont && i != Max.Pos_J) {
                        Drop_To =i;
                        Cont = this.Layout[this.Height-1][i];
                    }
                }
                Cont = 9999;
                for(int i = 0; i < this.Width; i++) {
                    if(i != Drop_To && i != Max.Pos_J) {
                        if(Aux.Containers_Columna(i) < Cont) {
                            Cont = Aux.Containers_Columna(i);
                            Objetivo = i;
                        }
                    }
                }
            }

            /*
             * 	Mientras la Columna a Vaciar Tenga Containers, Se vaciaran
             * 	sobre la Columna a Apilar
             * */
            while(Aux.Containers_Columna(Objetivo) > 0) {
                Aux = Aux.Traslado_Container(Objetivo+1, Drop_To+1);
            }

            /*
             * 	Luego se intenta vaciar la Columna donde esta ubicado el Container mas alto
             * 	sobre la Columna a Apilar
             * */
            while(!Aux.On_Top_Stack(Max)) {
                Aux = Aux.Traslado_Container(Max.Pos_J+1,Drop_To+1);
            }

            //Por Fin se Mueve el Container deseado a una Columna Vacia
            Aux = Aux.Traslado_Container(Max.Pos_J+1, Objetivo+1);
            return Aux;
        }
        return Aux;
    }



    /**
     * 	Funcion que Retorna la Cantidad de Containers en una Columna
     * @param Columna
     * @return
     * 	Cantidad de Containers en Columna
     */

    public int Containers_Columna(int Columna) {
        int Contador = 0;
        for(int i = this.Height-1; i > 0; i--) {
            if(this.Layout[i][Columna] != 0) {
                Contador++;
            }
            else {
                break;
            }
        }
        return Contador;
    }



    /**
     * 	Funcion que Revisa si el Containers deseado se encuentra al Tope
     * 	de su Columna, por lo cual puede ser movido inmediatamente.
     * @param A
     * @return
     *		Retorna (True/False) según corresponda.
     */

    public boolean On_Top_Stack(Inmovible A) {
        int Num = 0;
        for(int i=this.Height-1; i > 0; i--) {
            if(this.Layout[i][A.Pos_J] != 0) {
                Num = this.Layout[i][A.Pos_J];
            }
            else {
                break;
            }
        }
        if(Num == A.Valor) return true;
        return false;
    }



    /**
     *	Funcion que Busca el Container con Mayor valor en la Matriz.
     *	Si uno o mas containers tiene el mismo valor, retornara solo
     *	aquel que se encuentre mas al tope de su respectivo stack.
     * @return
     */

    public Inmovible Max_Get(){
        Inmovible Aux = new Inmovible(0,0,0);
        for(int i = 0; i < this.Height; i++) {
            for(int j = 0; j < this.Width; j++) {
                if(this.Layout[i][j] > Aux.Valor) {
                    Aux.Valor = this.Layout[i][j];
                    Aux.Pos_I = i;
                    Aux.Pos_J = j;
                }
                if(this.Layout[i][j] == Aux.Valor) {
                    if(i < Aux.Pos_I) {
                        Aux.Pos_I = i;
                        Aux.Pos_J = j;
                    }
                }
            }
        }
        return Aux;
    }

    public int getLower_Bound() {
        return Lower_Bound;
    }

    public int[][] getLayout() {
        return Layout;
    }

    public void setLayout(int[][] layout) {
        Layout = layout;
    }

    public void setLower_Bound(int lower_Bound) {
        Lower_Bound = lower_Bound;
    }

    public int getHeight() {
        return Height;
    }

    public void setHeight(int height) {
        Height = height;
    }

    public int getWidth() {
        return Width;
    }

    public void setWidth(int width) {
        Width = width;
    }

    public void setCantidad_Movimientos(int cantidad_Movimientos) {
        Cantidad_Movimientos = cantidad_Movimientos;
    }

    public int getTotal_Containers() {
        return Total_Containers;
    }

    public void setTotal_Containers(int total_Containers) {
        Total_Containers = total_Containers;
    }

    public int getWell_Placed() {
        return Well_Placed;
    }

    public void setWell_Placed(int well_Placed) {
        Well_Placed = well_Placed;
    }

    public int getBad_Placed() {
        return Bad_Placed;
    }

    public void setBad_Placed(int bad_Placed) {
        Bad_Placed = bad_Placed;
    }

    public int getMinimum_Bad_Placed() {
        return Minimum_Bad_Placed;
    }

    public void setMinimum_Bad_Placed(int minimum_Bad_Placed) {
        Minimum_Bad_Placed = minimum_Bad_Placed;
    }

    public String getMovimientos() {
        return Movimientos;
    }

    public void setMovimientos(String movimientos) {
        Movimientos = movimientos;
    }
}

