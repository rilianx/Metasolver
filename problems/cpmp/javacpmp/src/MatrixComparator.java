import java.util.Comparator;

public class MatrixComparator implements Comparator<Matrix> {

    @Override
    public int compare(Matrix x , Matrix y){
        if(x.getLower_Bound() + x.getCantidad_Movimientos() < y.getLower_Bound() + y.getCantidad_Movimientos())
            return -1;
        else if(x.getCantidad_Movimientos() + x.getLower_Bound() == y.getLower_Bound() + y.getCantidad_Movimientos()){
            if(x.getCantidad_Movimientos() < y.getCantidad_Movimientos())
                return -1;
            else
                return 1;
        }
        return 1;
    }
}
