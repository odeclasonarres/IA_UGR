#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"
#include "stdlib.h"
using namespace std;

class ComportamientoJugador : public Comportamiento{

  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size){
      // Inicializar Variables de Estado
      fil=99;
      col=99;
      brujula=0;
      ultimaAccion=actIDLE;
      girar_derecha=false;
      bien_situado=false;
    }

    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);

    int interact(Action accion, int valor);


    ComportamientoJugador * clone(){return new ComportamientoJugador(*this);}

  private:
    // Declarar Variables de Estado
    int fil, col, brujula, ciclo,llaves,filProv, colProv;
    Action ultimaAccion;
    bool girar_derecha, bien_situado, bikini, zapatillas;
    char mapaResultado[200][200], mapaAux[200][200];

    void Iniciar();
    void Reiniciar();
    void Guardar();
    void ActualizarMapa(Sensores sensores, bool mapa);
    Action CogerObjeto(Sensores sensores, char objeto);
    Action UsarObjeto(Sensores sensores, char objeto);
};


#endif
filProv
