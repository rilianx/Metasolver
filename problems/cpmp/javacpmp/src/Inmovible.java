/*
 * 	Esta Clase fue Creada con motivo de facilitar la implementación
 * 	y representación de un container en especifico que se necesite
 * 	para el cálculo de calcularLowerBound, Maximos, Restricciones , etc.
 *
 * */

public class Inmovible{
    public
    int Valor;
    int Pos_J;
    int Pos_I;

    public
    Inmovible(int Valor , int i , int j) {
        this.Valor = Valor;
        this.Pos_I = i;
        this.Pos_J = j;
    }

    void Show() {
        System.out.println("No se movera el valor: " + this.Valor + " ubicado en i: "+ this.Pos_I + " j: "+ this.Pos_J);
    }
}