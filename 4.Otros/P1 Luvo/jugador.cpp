#include "../Comportamientos_Jugador/jugador.hpp"
#include <iostream>
using namespace std;

int INF = 999999999;

Action ComportamientoJugador::think(Sensores sensors){

  if(sensors.reset) Reinicio();

  Action action;

  // ==========================================================================
  // ================================ ANALIZER ================================
  // ==========================================================================

  switch(last_action_){
    case actFORWARD:{
      if(!sensors.colision)
        switch(compass_){
          case 0:{
            --row_;
            --path_row_;
          }break; // north
          case 1:{
            ++col_;
            ++path_col_;
          }break; // east
          case 2:{
            ++row_;
            ++path_row_;
          }break; // south
          case 3:{
            --col_;
            --path_col_;
          }break; // west
        }
    }break;
    case actTURN_L:{
      compass_ = (compass_+3)%4; // (compass_-1)%4;
    }break;
    case actTURN_R:{
      compass_ = (compass_+1)%4;
    }break;
  }

  // ShowInfo(sensors);
  // ShowAction(action);

  if(sensors.terreno[0]=='K' and !well_located_){
    row_ = sensors.mensajeF;
    col_ = sensors.mensajeC;
    well_located_ = true;
  }

  if(well_located_){
    if(!relocated_){
      CopyAuxiliarToResultado();
      relocated_ = true;
    }
    UpdateMapaResultado(sensors);
  }
  else
    UpdateMapaAuxiliar(sensors);

  // ==========================================================================
  // ============================= ACTION CHOOSER =============================
  // ==========================================================================

  if(n_keys_ <= 2 and sensors.superficie[2] == '3'){
    action = CatchObject(sensors, '3');
  }
  else if(!have_bikini_ and sensors.superficie[2] == '1'){
    action = CatchObject(sensors, '1');
  }
  else if(!have_trainers_ and sensors.superficie[2] == '2'){
    action = CatchObject(sensors, '2');
  }
  else if(sensors.terreno[2] == 'A' and have_bikini_){
    action = SelectObject(sensors, '1');
  }
  else if(sensors.terreno[2] == 'B' and have_trainers_){
    action = SelectObject(sensors, '2');
  }
  else if(sensors.terreno[2] == 'D' and sensors.superficie[2] != '_' and n_keys_){
    action = SelectObject(sensors, '3');
    if(action == actFORWARD){
      action = actGIVE;
      --n_keys_;
    }
  }
  else if(Bump(sensors)){
    SetBump();
    action = actTURN_R;
  }
  else{
    action = DecideDirection();
  }

  last_action_ = action;

  path_map_[path_row_][path_col_] = iteration_++;
  return action;
}

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

void ComportamientoJugador::InicioJuego(){
  for(int i=0; i<100; ++i)
    for(int j=0; j<100; ++j)
      if((i<3 or i>96) or (j<3 or j>96)) mapaResultado[i][j] = 'P';
      else mapaResultado[i][j] = 'T';

  iteration_ = 1;
}

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

void ComportamientoJugador::Reinicio(){
  // cout << "Reinicio\n";
  row_ = col_ = 99;
  path_row_ = path_col_ = 99; // row and col from path_map_;
  compass_ = 0;  // 0-N / 1-E / 2-S / 3-W
  well_located_ = relocated_ = false;
  last_action_ = actIDLE;
  have_trainers_ = have_bikini_ = false;
  n_keys_ = 0;

  for(int i=0; i<200; ++i)
    for(int j=0; j<200; ++j){
      path_map_[i][j] = 0;
      aux_map_[i][j] = '?';
    }

  for(int i=0; i<4; ++i)
    surroundings_[i] = 0;
}

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

Action ComportamientoJugador::SelectObject(Sensores sensors, char obj){
  Action action = actIDLE;

  if(sensors.objetoActivo == obj)   action = actFORWARD;
  else
    if(sensors.objetoActivo == '_') action = actPOP;
    else                            action = actPUSH;

  return action;
}

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

Action ComportamientoJugador::CatchObject(Sensores sensors, char obj){
  Action action = actIDLE;

  if(sensors.objetoActivo == '_'){
    action = actPICKUP;
    switch(obj){
      case '1': have_bikini_ = true; break;
      case '2': have_trainers_ = true; break;
      case '3': ++n_keys_; break;
    }
  }
  else action = actPUSH;

  return action;
}

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

bool ComportamientoJugador::Bump(Sensores sensors, int loc /* = 2 */){
  return !((sensors.terreno[loc]=='T' or
  sensors.terreno[loc]=='S' or
  sensors.terreno[loc]=='K' or
  sensors.terreno[loc]=='D') and
  sensors.superficie[loc]=='_');
}

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

void ComportamientoJugador::SetBump(){
  switch(compass_){
    case 0:{
      path_map_[path_row_-1][path_col_] = INF;
    }break;
    case 1:{
      path_map_[path_row_][path_col_+1] = INF;
    }break;
    case 2:{
      path_map_[path_row_+1][path_col_] = INF;
    }break;
    case 3:{
      path_map_[path_row_][path_col_-1] = INF;
    }break;
  }
}

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

void ComportamientoJugador::UpdateSurroundings(){
  switch(compass_){
    case 0:{                // north
      surroundings_[0] = path_map_[path_row_-1][path_col_];
      surroundings_[1] = path_map_[path_row_][path_col_+1];
      surroundings_[2] = path_map_[path_row_+1][path_col_];
      surroundings_[3] = path_map_[path_row_][path_col_-1];
    }break;
    case 1:{                // east
      surroundings_[0] = path_map_[path_row_][path_col_+1];
      surroundings_[1] = path_map_[path_row_+1][path_col_];
      surroundings_[2] = path_map_[path_row_][path_col_-1];
      surroundings_[3] = path_map_[path_row_-1][path_col_];
    }break;
    case 2:{                // south
      surroundings_[0] = path_map_[path_row_+1][path_col_];
      surroundings_[1] = path_map_[path_row_][path_col_-1];
      surroundings_[2] = path_map_[path_row_-1][path_col_];
      surroundings_[3] = path_map_[path_row_][path_col_+1];
    }break;
    case 3:{                // west
      surroundings_[0] = path_map_[path_row_][path_col_-1];
      surroundings_[1] = path_map_[path_row_-1][path_col_];
      surroundings_[2] = path_map_[path_row_][path_col_+1];
      surroundings_[3] = path_map_[path_row_+1][path_col_];
    }break;
  }
}

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

Action ComportamientoJugador::DecideDirection(){
  Action decision = actIDLE;
  UpdateSurroundings();

  int front = surroundings_[0];
  int right = surroundings_[1];
  int left = surroundings_[3];

  if(front <= right and front <= left)
    decision = actFORWARD;
  else if(right <= front and right <= left)
    decision = actTURN_R;
  else
    decision = actTURN_L;

  return decision;
}

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

void ComportamientoJugador::UpdateMapaResultado(Sensores sensors){
  mapaResultado[row_][col_] = sensors.terreno[0];

  switch(compass_){
    case 0:{        // N O R T H
      mapaResultado[row_-1][col_-1] = sensors.terreno[1];
      mapaResultado[row_-1][col_] = sensors.terreno[2];
      mapaResultado[row_-1][col_+1] = sensors.terreno[3];

      mapaResultado[row_-2][col_-2] = sensors.terreno[4];
      mapaResultado[row_-2][col_-1] = sensors.terreno[5];
      mapaResultado[row_-2][col_] = sensors.terreno[6];
      mapaResultado[row_-2][col_+1] = sensors.terreno[7];
      mapaResultado[row_-2][col_+2] = sensors.terreno[8];

      mapaResultado[row_-3][col_-3] = sensors.terreno[9];
      mapaResultado[row_-3][col_-2] = sensors.terreno[10];
      mapaResultado[row_-3][col_-1] = sensors.terreno[11];
      mapaResultado[row_-3][col_] = sensors.terreno[12];
      mapaResultado[row_-3][col_+1] = sensors.terreno[13];
      mapaResultado[row_-3][col_+2] = sensors.terreno[14];
      mapaResultado[row_-3][col_+3] = sensors.terreno[15];
    }break;
    case 1:{      // E A S T
      mapaResultado[row_-1][col_+1] = sensors.terreno[1];
      mapaResultado[row_][col_+1] = sensors.terreno[2];
      mapaResultado[row_+1][col_+1] = sensors.terreno[3];

      mapaResultado[row_-2][col_+2] = sensors.terreno[4];
      mapaResultado[row_-1][col_+2] = sensors.terreno[5];
      mapaResultado[row_][col_+2] = sensors.terreno[6];
      mapaResultado[row_+1][col_+2] = sensors.terreno[7];
      mapaResultado[row_+2][col_+2] = sensors.terreno[8];

      mapaResultado[row_-3][col_+3] = sensors.terreno[9];
      mapaResultado[row_-2][col_+3] = sensors.terreno[10];
      mapaResultado[row_-1][col_+3] = sensors.terreno[11];
      mapaResultado[row_][col_+3] = sensors.terreno[12];
      mapaResultado[row_+1][col_+3] = sensors.terreno[13];
      mapaResultado[row_+2][col_+3] = sensors.terreno[14];
      mapaResultado[row_+3][col_+3] = sensors.terreno[15];
    }break;
    case 2:{      // S O U T H
      mapaResultado[row_+1][col_+1] = sensors.terreno[1];
      mapaResultado[row_+1][col_] = sensors.terreno[2];
      mapaResultado[row_+1][col_-1] = sensors.terreno[3];

      mapaResultado[row_+2][col_+2] = sensors.terreno[4];
      mapaResultado[row_+2][col_+1] = sensors.terreno[5];
      mapaResultado[row_+2][col_] = sensors.terreno[6];
      mapaResultado[row_+2][col_-1] = sensors.terreno[7];
      mapaResultado[row_+2][col_-2] = sensors.terreno[8];

      mapaResultado[row_+3][col_+3] = sensors.terreno[9];
      mapaResultado[row_+3][col_+2] = sensors.terreno[10];
      mapaResultado[row_+3][col_+1] = sensors.terreno[11];
      mapaResultado[row_+3][col_] = sensors.terreno[12];
      mapaResultado[row_+3][col_-1] = sensors.terreno[13];
      mapaResultado[row_+3][col_-2] = sensors.terreno[14];
      mapaResultado[row_+3][col_-3] = sensors.terreno[15];
    }break;
    case 3:{      // W E S T
      mapaResultado[row_+1][col_-1] = sensors.terreno[1];
      mapaResultado[row_][col_-1] = sensors.terreno[2];
      mapaResultado[row_-1][col_-1] = sensors.terreno[3];

      mapaResultado[row_+2][col_-2] = sensors.terreno[4];
      mapaResultado[row_+1][col_-2] = sensors.terreno[5];
      mapaResultado[row_][col_-2] = sensors.terreno[6];
      mapaResultado[row_-1][col_-2] = sensors.terreno[7];
      mapaResultado[row_-2][col_-2] = sensors.terreno[8];

      mapaResultado[row_+3][col_-3] = sensors.terreno[9];
      mapaResultado[row_+2][col_-3] = sensors.terreno[10];
      mapaResultado[row_+1][col_-3] = sensors.terreno[11];
      mapaResultado[row_][col_-3] = sensors.terreno[12];
      mapaResultado[row_-1][col_-3] = sensors.terreno[13];
      mapaResultado[row_-2][col_-3] = sensors.terreno[14];
      mapaResultado[row_-3][col_-3] = sensors.terreno[15];
    }break;
  }
}

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

void ComportamientoJugador::UpdateMapaAuxiliar(Sensores sensors){
  aux_map_[path_row_][path_col_] = sensors.terreno[0];

  switch(compass_){
    case 0:{        // N O R T H
      aux_map_[path_row_-1][path_col_-1] = sensors.terreno[1];
      aux_map_[path_row_-1][path_col_] = sensors.terreno[2];
      aux_map_[path_row_-1][path_col_+1] = sensors.terreno[3];

      aux_map_[path_row_-2][path_col_-2] = sensors.terreno[4];
      aux_map_[path_row_-2][path_col_-1] = sensors.terreno[5];
      aux_map_[path_row_-2][path_col_] = sensors.terreno[6];
      aux_map_[path_row_-2][path_col_+1] = sensors.terreno[7];
      aux_map_[path_row_-2][path_col_+2] = sensors.terreno[8];

      aux_map_[path_row_-3][path_col_+3] = sensors.terreno[9];
      aux_map_[path_row_-3][path_col_+2] = sensors.terreno[10];
      aux_map_[path_row_-3][path_col_+1] = sensors.terreno[11];
      aux_map_[path_row_-3][path_col_] = sensors.terreno[12];
      aux_map_[path_row_-3][path_col_-1] = sensors.terreno[13];
      aux_map_[path_row_-3][path_col_-2] = sensors.terreno[14];
      aux_map_[path_row_-3][path_col_-3] = sensors.terreno[15];
    }break;
    case 1:{      // E A S T
      aux_map_[path_row_-1][path_col_+1] = sensors.terreno[1];
      aux_map_[path_row_][path_col_+1] = sensors.terreno[2];
      aux_map_[path_row_+1][path_col_+1] = sensors.terreno[3];

      aux_map_[path_row_-2][path_col_+2] = sensors.terreno[4];
      aux_map_[path_row_-1][path_col_+2] = sensors.terreno[5];
      aux_map_[path_row_][path_col_+2] = sensors.terreno[6];
      aux_map_[path_row_+1][path_col_+2] = sensors.terreno[7];
      aux_map_[path_row_+2][path_col_+2] = sensors.terreno[8];

      aux_map_[path_row_-3][path_col_+3] = sensors.terreno[9];
      aux_map_[path_row_-2][path_col_+3] = sensors.terreno[10];
      aux_map_[path_row_-1][path_col_+3] = sensors.terreno[11];
      aux_map_[path_row_][path_col_+3] = sensors.terreno[12];
      aux_map_[path_row_+1][path_col_+3] = sensors.terreno[13];
      aux_map_[path_row_+2][path_col_+3] = sensors.terreno[14];
      aux_map_[path_row_+3][path_col_+3] = sensors.terreno[15];
    }break;
    case 2:{      // S O U T H
      aux_map_[path_row_+1][path_col_+1] = sensors.terreno[1];
      aux_map_[path_row_+1][path_col_] = sensors.terreno[2];
      aux_map_[path_row_+1][path_col_-1] = sensors.terreno[3];

      aux_map_[path_row_+2][path_col_+2] = sensors.terreno[4];
      aux_map_[path_row_+2][path_col_+1] = sensors.terreno[5];
      aux_map_[path_row_+2][path_col_] = sensors.terreno[6];
      aux_map_[path_row_+2][path_col_-1] = sensors.terreno[7];
      aux_map_[path_row_+2][path_col_-2] = sensors.terreno[8];

      aux_map_[path_row_+3][path_col_+3] = sensors.terreno[9];
      aux_map_[path_row_+3][path_col_+2] = sensors.terreno[10];
      aux_map_[path_row_+3][path_col_+1] = sensors.terreno[11];
      aux_map_[path_row_+3][path_col_] = sensors.terreno[12];
      aux_map_[path_row_+3][path_col_-1] = sensors.terreno[13];
      aux_map_[path_row_+3][path_col_-2] = sensors.terreno[14];
      aux_map_[path_row_+3][path_col_-3] = sensors.terreno[15];
    }break;
    case 3:{      // W E S T
      aux_map_[path_row_+1][path_col_-1] = sensors.terreno[1];
      aux_map_[path_row_][path_col_-1] = sensors.terreno[2];
      aux_map_[path_row_-1][path_col_-1] = sensors.terreno[3];

      aux_map_[path_row_+2][path_col_-2] = sensors.terreno[4];
      aux_map_[path_row_+1][path_col_-2] = sensors.terreno[5];
      aux_map_[path_row_][path_col_-2] = sensors.terreno[6];
      aux_map_[path_row_-1][path_col_-2] = sensors.terreno[7];
      aux_map_[path_row_-2][path_col_-2] = sensors.terreno[8];

      aux_map_[path_row_+3][path_col_-3] = sensors.terreno[9];
      aux_map_[path_row_+2][path_col_-3] = sensors.terreno[10];
      aux_map_[path_row_+1][path_col_-3] = sensors.terreno[11];
      aux_map_[path_row_][path_col_-3] = sensors.terreno[12];
      aux_map_[path_row_-1][path_col_-3] = sensors.terreno[13];
      aux_map_[path_row_-2][path_col_-3] = sensors.terreno[14];
      aux_map_[path_row_-3][path_col_-3] = sensors.terreno[15];
    }break;
  }
}

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

void ComportamientoJugador::CopyAuxiliarToResultado(){
  for(int i=0; i<100; ++i)
    for(int j=0; j<100; ++j)
      if(aux_map_[path_row_ - row_ + i][path_col_ - col_ + j] != '?')
        mapaResultado[i][j] = aux_map_[path_row_ - row_ + i][path_col_ - col_ + j];
}

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

void ComportamientoJugador::ShowInfo(Sensores sensors){
  cout << "Terreno: ";
  for (int i=0; i<sensors.terreno.size(); i++)
    cout << sensors.terreno[i];
  cout << endl;

  cout << "Superficie: ";
  for (int i=0; i<sensors.superficie.size(); i++)
    cout << sensors.superficie[i];
  cout << endl;

  cout << "Colisión: " << sensors.colision << endl;
  cout << "Mochila: " << sensors.mochila << endl;
  cout << "Reset: " << sensors.reset << endl;
  cout << "Vida: " << sensors.vida << endl;
  cout << "objetoActivo: " << sensors.objetoActivo << endl;
  cout << endl;
}

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

void ComportamientoJugador::ShowAction(Action action){
  switch(action){
    case actFORWARD:
      cout << "action: actFORWARD\n";
    break;
    case actTURN_R:
      cout << "action: actTURN_R\n";
    break;
    case actTURN_L:
      cout << "action: actTURN_L\n";
    break;
    case actIDLE:
      cout << "action: actIDLE\n";
    break;
    case actPICKUP:
      cout << "action: actPICKUP\n";
    break;
    case actPUTDOWN:
      cout << "action: actPUTDOWN\n";
    break;
    case actPUSH:
      cout << "action: actPUSH\n";
    break;
    case actPOP:
      cout << "action: actPOP\n";
    break;
    case actGIVE:
      cout << "action: actGIVE\n";
    break;
    case actTHROW:
      cout << "action: actTHROW\n";
    break;
  }
}

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

int ComportamientoJugador::interact_(Action action, int valor){
  return false;
}
