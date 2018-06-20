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

public class GX{
    private
    int number;
    int demand;
    int cumulative_demand;
    int potential_supply;
    int cumulative_supply;
    int cumulative_demand_surplus;

    public
    /**
     * 	Constructor de GX
     * @param number
     */
    GX(int number) {
        this.number = number;
        this.demand = 0;
        this.cumulative_demand = 0;
        this.potential_supply = 0;
        this.cumulative_supply = 0;
        this.cumulative_demand_surplus = 0;
    }
}