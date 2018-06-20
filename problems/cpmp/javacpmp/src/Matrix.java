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

    private int[][] layout;
    private int lowerBound;
    private int height, width;
    private int cantidadMovimientos;
    private int totalContainers;
    private int bienUbicados;
    private int malUbicados;
    private int minimoMalUbicados;
    private String movimientos;

    /**
     *	Constructor Natural sin Parametros
     *	Retorna una Matriz 0x0
     *
     */

    public Matrix() {
        this.layout = new int[0][0];
        this.height = height;
        this.width = width;
        this.cantidadMovimientos = 0;
        this.totalContainers = 0;
        this.bienUbicados = 0;
        this.malUbicados = 0;
        this.minimoMalUbicados = 0;
        this.movimientos = "";
        this.lowerBound = 0;
    }


    /**
     * 	Constructor que recibe el Alto y Ancho
     * 	Retorna una Matriz de height x width
     * 	@param height
     * 	@param width
     */

    public Matrix(int height , int width) {
        this.layout = new int[height][width];
        this.height = height;
        this.width = width;
        this.cantidadMovimientos = 0;
        this.totalContainers = 0;
        this.bienUbicados = 0;
        this.malUbicados = 0;
        this.minimoMalUbicados = 0;
        this.movimientos =  "";
        this.lowerBound = 0;
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
        this.layout = new int[maxHeight][width];
        this.height = maxHeight;
        this.width = width;
        this.cantidadMovimientos = 0;
        this.totalContainers = 0;
        this.bienUbicados = 0;
        this.malUbicados = 0;
        this.minimoMalUbicados = 0;
        this.movimientos =  "";
        this.totalContainers = totalContainers;
        this.lowerBound = 0;

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
                        layout[altura][col] = n.nextInt();
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
        this.Initialize();
    }

    @Override
    public int hashCode() {
        int h = 0;
        int p = this.Max_Value();
        int i , j;
        for(i = 0; i < this.height; i++) {
            for(j = 0; j < this.width; j++) {
                h = (int) (h * p * 0.6180334 + this.layout[i][j]);
            }
        }
        return h;
    }

    public int getCantidadMovimientos() {
        return cantidadMovimientos;
    }

    /**
     *	Mostrar todos los datos relativos a la configuración como su actual
     *	Lowerbound , Cantidad de movimientos Realizados , Secuencia de movimientos Realizados
     *	Y Además muestra la configuración actual.
     *
     *
     *	@param
     *  @return
     *  	No Retorna Nada
     */

    public void Show_Matrix() {
        System.out.println("height: " + this.height + " width: " + this.width);
        System.out.println("LowerBound: " + this.lowerBound);
        System.out.println("Cantidad de movimientos: " + this.cantidadMovimientos);
        System.out.println("Total de Containers: " + this.totalContainers);
        System.out.println("Total Bien Ubicados: " + this.bienUbicados);
        System.out.println("Total Mal Ubicados : " + this.malUbicados);
        System.out.println("Minimo de Mal Ubicados: " + this.minimoMalUbicados);
        System.out.println("movimientos Realizados: " + this.movimientos);
        for(int i = 0 ; i < height; i++) {
            for(int j = 0; j < width; j++) {
                System.out.print(this.layout[i][j] + " ");
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

    public void showOnlyMatrix() {
        for(int i = 0 ; i < height; i++) {
            for(int j = 0; j < width; j++) {
                System.out.print(this.layout[i][j] + "   ");
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
        if(i < this.height && j < this.width) {
            this.layout[i][j] = val;
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

    public boolean compareTo(Matrix A) {
        if (this.height != A.height || this.width != A.width) return false;
        else {
            for(int i = 0; i < this.height; i++) {
                for(int j = 0; j < this.width; j++) {
                    if(this.layout[i][j] != A.layout[i][j]) return false;
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
        for(int i=0; i<this.width; i++) {
            Local = 0;
            Bottom = this.layout[height-1][i];
            Bad = 0;
            Minimum = 9999;
            for(int j = this.height -1;j>=0;j--) {
                if(this.layout[j][i] == 0) {break;}
                if(this.layout[j][i] < Minimum ) {Minimum = this.layout[j][i];}
                if(this.layout[j][i] > Bottom || this.layout[j][i] > Minimum){Bad=1;}
                if(Bad==1) {Local++;}
            }
            Total = Total + Local;
            if(Local < aux) {aux = Local;}
        }
        this.minimoMalUbicados = aux;
        this.malUbicados = Total;
        this.bienUbicados = this.totalContainers - Total;

    }



    /**
     * Calcula el máximo valor de container en la Matriz
     * @return
     * 		El valor del container más alto.
     */

    public int Max_Value() {
        int i , j;
        int Max = 0;
        for(i = 0; i < this.height; i++) {
            for(j = 0; j < this.width; j++) {
                if(this.layout[i][j] > Max) Max = this.layout[i][j];
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

        if(this.layout[0][CF-1]!= 0 || CF > this.width || this.layout[this.height-1][CI-1] == 0) return false;
        return true;

    }



        /**
         * Crea una Copia de la Matriz Orginal
         * @return
         * 		La Matriz Copiada.
         */

        public Matrix Copy_Matrix() {
            Matrix M = new Matrix(this.height,this.width);
            M.lowerBound = this.lowerBound;
            M.malUbicados = this.malUbicados;
            M.cantidadMovimientos = this.cantidadMovimientos;
            M.lowerBound = this.lowerBound;
            M.minimoMalUbicados = this.minimoMalUbicados;
            M.movimientos = this.movimientos;
            M.totalContainers = this.totalContainers;
            M.bienUbicados = this.bienUbicados;
            int i , j;
            for(i = 0; i < this.height; i++) {
                for(j=0; j< this.width; j++) {
                    M.layout[i][j] = this.layout[i][j];
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
            if(rowi+1 == this.height || this.layout[rowi][CI-1]!= 0) break;
            rowi++;
        }
        while(true) {
            if(rowf+1 == this.height || this.layout[rowf+1][CF-1] != 0) break;
            rowf++;
        }
        aux.layout[rowf][CF-1] = aux.layout[rowi][CI-1];
        aux.layout[rowi][CI-1] = 0;
        mov = mov + "\nC" + String.valueOf(CI) + " -> C" + String.valueOf(CF) + "";
        aux.movimientos = aux.movimientos + mov;
        aux.cantidadMovimientos++;
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
            if(rowi+1==this.height || aux.layout[rowi][CI-1]!=0) break;
            rowi++;
        }
        while(true) {
            if(rowf+1==this.height || aux.layout[rowf+1][CF-1]!=0) break;
            rowf++;
        }

        aux.layout[rowf][CF-1] = aux.layout[rowi][CI-1];
        aux.layout[rowi][CI-1] = 0;
        mov = "C"+String.valueOf(CF) + " -> C"+String.valueOf(CI) + " -- ";
        aux.movimientos = mov + aux.movimientos;
        aux.cantidadMovimientos++;
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
        for(j=this.height-1; j>=1; j--) {
            if(this.layout[j][Column-1] < this.layout[j-1][Column-1]) return false;
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
        boolean B = aux.comprobarColumna(CF);

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
            for(j = 0; j < this.height && Validador==1;j++) {
                for(k = 0; k < this.width && Validador==1; k++) {
                    if(this.layout[j][k] == i) {
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
        int malUbicados;
        int Cumulative_Bad_Placed = 0;
        int Supply;
        int Cumulative_Supply=0;
        int Bottom;
        int Last_Item;

        for(Iterar = 0; Iterar < Tier.size(); Iterar++) {
            GX Auxiliar = new GX(Tier.get(Iterar));
            malUbicados = 0;
            Supply = 0;
            for(i = 0; i < this.width; i++) {
                Minimum_Stack = 9999;
                Bad = 0;
                Bottom = this.layout[this.height-1][i];
                Last_Item = Bottom;
                for(j = this.height-1; j>=0; j--) {
                    if(this.layout[j][i] < Minimum_Stack) {Minimum_Stack = this.layout[j][i];}
                    if(this.layout[j][i] > Bottom || this.layout[j][i] > Minimum_Stack) {Bad=1;}
                    if(Bad==1 && this.layout[j][i] == Tier.get(Iterar)) {malUbicados++;Cumulative_Bad_Placed++;}
                    if(Bad==1 && Last_Item == Tier.get(Iterar)) {Supply++;Cumulative_Supply++;}
                    if(Bad==0) {Last_Item = this.layout[j][i];}
                }
            }
            Auxiliar.demand = malUbicados;
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
        this.lowerBound = this.minimoMalUbicados + this.malUbicados + this.calcularGX();
        return this.lowerBound;
    }



    /**
     * 	Inicializa los parámetros de la Matriz llamando
     * 	a la las funciones LowerBound y containersMalUbicados
     */

    public void Initialize() {
        this.calcularLowerBound();
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
        for(i=1;i<=this.width;i++) {
            for(j=1;j<=this.width;j++) {
                if(i!=j) {
                    if(this.Possible_Movement(i, j)) {
                        int valid = 1;
                        Matrix aux = this.Traslado_Container(i, j);
                        for(int k = 0; k < Restricciones.size(); k++) {
                            if(aux.layout[Restricciones.get(k).Pos_I][Restricciones.get(k).Pos_J] != Restricciones.get(k).Valor) {
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
        for(int i = 1; i <= this.width; i++){
            for(int j=1; j <= this.width; j++){
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
        for(i=1;i<=this.width;i++) {
            for(j=1;j<=this.width;j++) {
                if(i!=j) {
                    if(this.verificarMovimiento(i, j)) {
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
        for(j=this.height-1; j>Pos_I;j--) {
            if(this.layout[j][Pos_J] < this.layout[j-1][Pos_J]) return false;
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

        for(Repeat = 0; Repeat < this.width && Cont < this.width; Repeat++) {
            Aux = temporal.Max_Value();
            for(int i = this.height-1; i>=0; i--) {
                for(int j = 0; j<this.width;j++) {
                    if(temporal.layout[i][j] == Aux) {
                        Cont++;
                        if(this.Container_Bien_Ubicado(i, j)) {
                            Inmovible Auxi = new Inmovible(temporal.layout[i][j],i,j);
                            No_Mover.add(Auxi);
                        }
                        temporal.layout[i][j] = 0;
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
        for(int j = 0; j < this.width; j++) {
            for(int i = 0; i< this.height; i++) {
                for(int k = i + 1; k < this.height; k++) {
                    if(Temporal.layout[i][j] > Temporal.layout[k][j]) {
                        int aux = Temporal.layout[i][j];
                        Temporal.layout[i][j] = Temporal.layout[k][j];
                        Temporal.layout[k][j] = aux;
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

        int I_Matrix = this.height - 1 , J_Matrix = 0;

        for(j = 0; j < this.width; j++) {
            cont = 0;
            for(i = this.height - 1; i >=0; i--) {
                if(this.layout[i][j] != 0) {cont++;}
                else {break;}
            }
            Contador = Contador + cont;
        }

        Promedio = Math.round(Contador/this.width);

        for(i = 0; i < this.height; i++) {
            for(j = 0; j < this.width; j++) {
                Temporal.layout[i][j] = 0;
                Aux.layout[i][j] = this.layout[i][j];
            }
        }

        for(k = 0; k < this.totalContainers; k++) {
            Auxi = Aux.Max_Value();
            for(i = 0; i < this.height; i++) {
                for(j = 0; j < this.width; j++) {
                    if(Aux.layout[i][j]==Auxi) {
                        Aux.layout[i][j] = 0;
                        i = this.height;
                        j = this.width;
                        break;
                    }
                }
            }
            if(I_Matrix == this.height-1-Promedio) {
                I_Matrix = this.height-1;
                J_Matrix++;
            }
            Temporal.layout[I_Matrix][J_Matrix] = Auxi;
            I_Matrix--;
        }
        Temporal.inicializar();
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
        int Max = this.maxValue();
        int Cont = 0;
        boolean Valid = true;
        List<Inmovible> Lista = new ArrayList<>();
        Matrix Temporal = this.Copy_Matrix();
        for(int i = Max; i > 0 && Valid; i--) {
            for(int j = 0; j < this.height && Valid; j++) {
                for(int k = 0; k < this.width && Valid; k++) {
                    if(Temporal.layout[j][k] == i) {
                        Inmovible Aux = new Inmovible(i,j,k);
                        Temporal.layout[j][k] = 0;
                        Lista.add(Aux);
                        Cont++;
                    }
                    if(Cont == this.width) {
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
                for(int i = 0; i < this.width; i++) {
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
                for(int i = 0; i < this.width; i++) {
                    if(this.layout[this.height-1][i] > Cont && i != Max.Pos_J) {
                        Drop_To =i;
                        Cont = this.layout[this.height-1][i];
                    }
                }
                Cont = 9999;
                for(int i = 0; i < this.width; i++) {
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
        for(int i = this.height-1; i > 0; i--) {
            if(this.layout[i][Columna] != 0) {
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
        for(int i=this.height-1; i > 0; i--) {
            if(this.layout[i][A.Pos_J] != 0) {
                Num = this.layout[i][A.Pos_J];
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
        for(int i = 0; i < this.height; i++) {
            for(int j = 0; j < this.width; j++) {
                if(this.layout[i][j] > Aux.Valor) {
                    Aux.Valor = this.layout[i][j];
                    Aux.Pos_I = i;
                    Aux.Pos_J = j;
                }
                if(this.layout[i][j] == Aux.Valor) {
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
        return lowerBound;
    }

    public int[][] getLayout() {
        return layout;
    }

    public void setLayout(int[][] layout) {
        layout = layout;
    }

    public void setLower_Bound(int lower_Bound) {
        lowerBound = lower_Bound;
    }

    public int getHeight() {
        return height;
    }

    public void setHeight(int height) {
        height = height;
    }

    public int getWidth() {
        return width;
    }

    public void setWidth(int width) {
        width = width;
    }

    public void setCantidad_Movimientos(int cantidad_Movimientos) {
        cantidadMovimientos = cantidad_Movimientos;
    }

    public int getTotal_Containers() {
        return totalContainers;
    }

    public void setTotal_Containers(int total_Containers) {
        totalContainers = total_Containers;
    }

    public int getWell_Placed() {
        return bienUbicados;
    }

    public void setWell_Placed(int well_Placed) {
        bienUbicados = well_Placed;
    }

    public int getBad_Placed() {
        return malUbicados;
    }

    public void setBad_Placed(int bad_Placed) {
        malUbicados = bad_Placed;
    }

    public int getMinimum_Bad_Placed() {
        return minimoMalUbicados;
    }

    public void setMinimum_Bad_Placed(int minimum_Bad_Placed) {
        minimoMalUbicados = minimum_Bad_Placed;
    }

    public String getMovimientos() {
        return movimientos;
    }

    public void setMovimientos(String movimientos) {
        movimientos = movimientos;
    }
}

