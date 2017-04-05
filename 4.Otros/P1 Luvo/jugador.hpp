#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"
#include "stdlib.h"
using namespace std;

class ComportamientoJugador : public Comportamiento{

  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size){
      InicioJuego();
      Reinicio();
    }

    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}
    Action think(Sensores sensores);
    int interact_(Action action, int valor);
    ComportamientoJugador * clone(){return new ComportamientoJugador(*this);}

  private:
    int row_, col_, compass_,
        path_row_, path_col_,
        iteration_;
    Action last_action_;
    bool well_located_, relocated_,
        have_trainers_, have_bikini_;
    int n_keys_;
    int path_map_[200][200];
    char aux_map_[200][200];
    int surroundings_[4];

    void InicioJuego();
    void Reinicio();
    Action SelectObject(Sensores sensors, char obj);
    Action CatchObject(Sensores sensors, char obj);
    bool Bump(Sensores sensors, int loc = 2);
    void SetBump();
    void UpdateSurroundings();
    Action DecideDirection();
    void UpdateMapaResultado(Sensores sensors);
    void UpdateMapaAuxiliar(Sensores sensors);
    void CopyAuxiliarToResultado();
    void ShowInfo(Sensores sensors);
    void ShowAction(Action action);
};


#endif
