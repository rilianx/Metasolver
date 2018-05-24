import java.util.*;
import java.awt.Toolkit;

public class CPMP {

    public static void main(String[] args) {

        //Ruta del Archivo a Abrir
        System.out.println("La ruta es " +System.getProperty("java.class.path"));
        String rutaArchivo = System.getProperty("java.class.path") + "\\Test/kappa.dat";

        //Mapa para Guardar las Matrices ya recorridas
        HashMap<Integer, Matrix> mapaMatrices = new HashMap<Integer, Matrix>();

        Comparator<Matrix> comparator = new MatrixComparator();

        //Priority Queue
        PriorityQueue<Matrix> colaPrioridad = new PriorityQueue<Matrix>(1, comparator);

        //Se Crea la Matriz Inicial Leida del Archivo
        Chronometer ch = new Chronometer();
        ch.start();


        Matrix matrizInicial = new Matrix(rutaArchivo,1);

        Matrix mejor = matrizInicial.Copy_Matrix();

        matrizInicial.Show_Matrix();

        Matrix matrizActual = null;
        //Se agrega la Matriz Inicial a la cola con Prioridad
        colaPrioridad.add(matrizInicial);

        int lowerBound = matrizInicial.getLower_Bound();

        while(colaPrioridad.size() > 0){
            matrizActual = colaPrioridad.remove();

            if(matrizActual.getLower_Bound() == 0){
                mejor =matrizActual.Copy_Matrix();
                break;
            }
            //Se generan todos los vecinos posibles de la Matriz Actual
            List<Matrix> matricesVecinas = matrizActual.Lista_Vecinos();

            //Se itera sobre los vecinos de la Matriz Actual
            for(int i = 0; i < matricesVecinas.size(); i++){
                //Si el vecino a  Matriz Actual no ha sido ya vista previamente entonces se accede
                if(mapaMatrices.get(matricesVecinas.get(i).hashCode()) == null){
                    //Se guarda en el mapa el vecino a la Matriz Actual
                    mapaMatrices.put(matricesVecinas.get(i).hashCode(),matricesVecinas.get(i));
                    //Si y solo si el lowerbound del vecino es menor o igual a 1.5* minimoLowerbound
                    colaPrioridad.add(matricesVecinas.get(i));
                }
            }
            //System.out.println(lowerBound + " ..." + matrizActual.getLower_Bound());
        }
        mejor.Show_Matrix();
        ch.stop();
        System.out.println(ch.getSeconds());

        return;

    }
}
