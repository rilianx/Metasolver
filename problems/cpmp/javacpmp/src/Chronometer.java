/*
 * 	Clase Chronometer utilizada para medir el tiempo que demora el algoritmo
 * 	en encontrar una solución al  CPMP.
 *
 * 	Clase sacada de: http://silveiraneto.net/2008/03/15/simple-java-chronometer/
 * */

public class Chronometer{
    public long begin, end;

    public void start(){
        begin = System.currentTimeMillis();
    }

    public void stop(){
        end = System.currentTimeMillis();
    }

    public long getTime() {
        return end-begin;
    }

    public long getMilliseconds() {
        return end-begin;
    }

    public double getSeconds() {
        return (end - begin) / 1000.0;
    }

    public double getMinutes() {
        return (end - begin) / 60000.0;
    }

    public double getHours() {
        return (end - begin) / 3600000.0;
    }
}