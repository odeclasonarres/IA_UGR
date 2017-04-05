#include "../Comportamientos_Jugador/jugador.hpp"
#include <iostream>
#include "stdlib.h"
using namespace std;


Action ComportamientoJugador::think(Sensores sensores){
	Action accion;
	if(sensores.reset){
		Reiniciar();
	}

	//Orientar y analizar entorno
	switch (ultimaAccion){
		case actFORWARD:
			switch (brujula){
				case 0: // Norte
					fil--;
					filProv--;
				break;
				case 1: // Este
					col++;
					colProv--;
				break;
				case 2: // Sur
					fil++;
					filProv++;
				break;
				case 3: // Oeste
					col--;
					colProv--;
				break;
			}
		break;
		case actTURN_L:
			brujula = (brujula+3)%4;
		break;
		case actTURN_R:
			brujula = (brujula+1)%4;
		break;
	}


	if ((sensores.terreno[0]=='K') && !bien_situado){
		fil = sensores.mensajeF;
		co l= sensores.mensajeC;
		bien_situado = true;
	}

	if (bien_situado) {
		Guardar();
		ActualizarMapa(sensores,true);
	}else{
		ActualizarMapa(sensores,false);
	}

	//Actuar

	if(!bikini && sensores.superficie[2] == '1'){
    accion = CogerObjeto(sensores, '1');
  } else if(!zapatillas && sensores.superficie[2] == '2'){
    accion = CogerObjeto(sensores, '2');
  } else if(sensores.terreno[2] == 'A' && bikini){
    accion = CogerObjeto(sensores, '1');
  }else if(llaves <= 2 && sensores.superficie[2] == '3'){
    accion = CogerObjeto(sensores, '3');
  } else if(sensores.terreno[2] == 'B' && zapatillas){
    accion = UsarObjeto(sensores, '2');
  } else if(sensores.terreno[2] == 'D' && sensores.superficie[2] != '_' && llaves){
    accion = UsarObjeto(sensores, '3');
    if(accion == actFORWARD){
			llaves--;
      accion = actGIVE;
    }
  } else if(sensores.superficie[2]=='?'){
    accion = actFORWARD;
  } else{
    accion = actTURN_R;
  }


	//Recordar
	ultimaAccion=accion;
	return accion;
	/*
	cout << "Terreno: ";
	for (int i=0; i<sensores.terreno.size(); i++)
		cout << sensores.terreno[i];
	cout << endl;

	cout << "Superficie: ";
	for (int i=0; i<sensores.superficie.size(); i++)
		cout << sensores.superficie[i];
	cout << endl;

	cout << "Colisión: " << sensores.colision << endl;
	cout << "Mochila: " << sensores.mochila << endl;
	cout << "Reset: " << sensores.reset << endl;
	cout << "Vida: " << sensores.vida << endl;
	cout << "Fila: " << sensores.mensajeF << endl;
	cout << "Columna: " << sensores.mensajeC << endl;
	cout << "objetoActivo: " << sensores.objetoActivo << endl;
	cout << endl;
	*/
}

int ComportamientoJugador::interact(accion accion, int valor){
  return false;
}

//Borra las variables, menos las casillas del mapa de las que sabe la posicion bien
void ComportamientoJugador::Reiniciar(){
	ultimaAccion = actIDLE;
	fil = 99;
	colProv=99;
	col = 99;
	colProv=99;
  brujula=0;
	llaves=0;
	zapatillas=false;
	bikini=false;
	bien_situado=false;

	for(int i=0; i<200; ++i){
		for(int j=0; j<200; ++j){
			mapaAux[i][j] = '?';
		}
	}
}

//Inicializa variables
void ComportamientoJugador::Iniciar(){
	ciclo = 1;
	ultimaAccion = actIDLE;
	fil = 99;
	filProv=99;
	col = 99;
	colProv=99;
	brujula=0;
	llaves=0;
	zapatillas=false;
	bikini=false;
	bien_situado=false;
	movido = false;
	for(int i=0; i<200; ++i){
		for(int j=0; j<200; ++j){
			mapaAux[i][j] = '?';
		}
	}
	for(int i=0; i<100; ++i){
    for(int j=0; j<100; ++j){
      if((i<3 or i>96) or (j<3 or j>96)){
				mapaResultado[i][j] = 'P';
			}else{
				mapaResultado[i][j] = '?';
			}
		}
	}
}

//Traslada del mapa provisional al definitivo
void ComportamientoJugador::Guardar(){
  for(int i=0; i<100; ++i){
    for(int j=0; j<100; ++j){
      if(mapaAux[filProv - fil + i][colProv -  + j] != '?'){
        mapaResultado[i][j] = mapaAux[filProv - fil + i][colProv - col + j];
			}
		}
	}
}

//Recoge objeto de la superficie
Action ComportamientoJugador::CogerObjeto(Sensores sensores, char objeto){
  Action retorno = actIDLE;
  if(sensores.objetoActivo == '_'){
    retorno = actPICKUP;
    switch(obj){
      case '1':{
				bikini=true;
			}break;
      case '2': {
				zapatillas=true;
			}break;
      case '3': {
				llaves++;
			}break;
    }
  }
  else{
		retorno = actPUSH;
	}
  return retorno;
}

Action ComportamientoJugador::UsarObjeto(Sensores sensores, char objeto){
  Action retorno = actIDLE;
  if(sensores.objetoActivo == objeto){
		retorno = actFORWARD;
	}else if(sensores.objetoActivo == '_'){
		retorno = actPOP;
	}else{
		retorno = actPUSH;
  }
	return retorno;
}

//Comprueba los elementos cercanos
void ComportamientoJugador::ActualizarMapa(Sensores sensores,bool mapa){
	if (mapa) {
		mapaResultado[fil_][col_] = sensores.terreno[0];
	  switch(compass_){
	    case 0:{        // norte
	      mapaResultado[fil_-1][col_-1] = sensores.terreno[1];
	      mapaResultado[fil_-1][col_] = sensores.terreno[2];
	      mapaResultado[fil_-1][col_+1] = sensores.terreno[3];

	      mapaResultado[fil_-2][col_-2] = sensores.terreno[4];
	      mapaResultado[fil_-2][col_-1] = sensores.terreno[5];
	      mapaResultado[fil_-2][col_] = sensores.terreno[6];
	      mapaResultado[fil_-2][col_+1] = sensores.terreno[7];
	      mapaResultado[fil_-2][col_+2] = sensores.terreno[8];

	      mapaResultado[fil_-3][col_-3] = sensores.terreno[9];
	      mapaResultado[fil_-3][col_-2] = sensores.terreno[10];
	      mapaResultado[fil_-3][col_-1] = sensores.terreno[11];
	      mapaResultado[fil_-3][col_] = sensores.terreno[12];
	      mapaResultado[fil_-3][col_+1] = sensores.terreno[13];
	      mapaResultado[fil_-3][col_+2] = sensores.terreno[14];
	      mapaResultado[fil_-3][col_+3] = sensores.terreno[15];
	    }break;
	    case 1:{      // este
	      mapaResultado[fil_-1][col_+1] = sensores.terreno[1];
	      mapaResultado[fil_][col_+1] = sensores.terreno[2];
	      mapaResultado[fil_+1][col_+1] = sensores.terreno[3];

	      mapaResultado[fil_-2][col_+2] = sensores.terreno[4];
	      mapaResultado[fil_-1][col_+2] = sensores.terreno[5];
	      mapaResultado[fil_][col_+2] = sensores.terreno[6];
	      mapaResultado[fil_+1][col_+2] = sensores.terreno[7];
	      mapaResultado[fil_+2][col_+2] = sensores.terreno[8];

	      mapaResultado[fil_-3][col_+3] = sensores.terreno[9];
	      mapaResultado[fil_-2][col_+3] = sensores.terreno[10];
	      mapaResultado[fil_-1][col_+3] = sensores.terreno[11];
	      mapaResultado[fil_][col_+3] = sensores.terreno[12];
	      mapaResultado[fil_+1][col_+3] = sensores.terreno[13];
	      mapaResultado[fil_+2][col_+3] = sensores.terreno[14];
	      mapaResultado[fil_+3][col_+3] = sensores.terreno[15];
	    }break;
	    case 2:{      //sur
	      mapaResultado[fil_+1][col_+1] = sensores.terreno[1];
	      mapaResultado[fil_+1][col_] = sensores.terreno[2];
	      mapaResultado[fil_+1][col_-1] = sensores.terreno[3];

	      mapaResultado[fil_+2][col_+2] = sensores.terreno[4];
	      mapaResultado[fil_+2][col_+1] = sensores.terreno[5];
	      mapaResultado[fil_+2][col_] = sensores.terreno[6];
	      mapaResultado[fil_+2][col_-1] = sensores.terreno[7];
	      mapaResultado[fil_+2][col_-2] = sensores.terreno[8];

	      mapaResultado[fil_+3][col_+3] = sensores.terreno[9];
	      mapaResultado[fil_+3][col_+2] = sensores.terreno[10];
	      mapaResultado[fil_+3][col_+1] = sensores.terreno[11];
	      mapaResultado[fil_+3][col_] = sensores.terreno[12];
	      mapaResultado[fil_+3][col_-1] = sensores.terreno[13];
	      mapaResultado[fil_+3][col_-2] = sensores.terreno[14];
	      mapaResultado[fil_+3][col_-3] = sensores.terreno[15];
	    }break;
	    case 3:{      //oeste
	      mapaResultado[fil_+1][col_-1] = sensores.terreno[1];
	      mapaResultado[fil_][col_-1] = sensores.terreno[2];
	      mapaResultado[fil_-1][col_-1] = sensores.terreno[3];

	      mapaResultado[fil_+2][col_-2] = sensores.terreno[4];
	      mapaResultado[fil_+1][col_-2] = sensores.terreno[5];
	      mapaResultado[fil_][col_-2] = sensores.terreno[6];
	      mapaResultado[fil_-1][col_-2] = sensores.terreno[7];
	      mapaResultado[fil_-2][col_-2] = sensores.terreno[8];

	      mapaResultado[fil_+3][col_-3] = sensores.terreno[9];
	      mapaResultado[fil_+2][col_-3] = sensores.terreno[10];
	      mapaResultado[fil_+1][col_-3] = sensores.terreno[11];
	      mapaResultado[fil_][col_-3] = sensores.terreno[12];
	      mapaResultado[fil_-1][col_-3] = sensores.terreno[13];
	      mapaResultado[fil_-2][col_-3] = sensores.terreno[14];
	      mapaResultado[fil_-3][col_-3] = sensores.terreno[15];
	    }break;
	  }
	}else{
		mapaAuxiliar[filaProv_][colProv_] = sensores.terreno[0];
	  switch(compass_){
	    case 0:{
	      mapaAuxiliar[filaProv_-1][colProv_-1] = sensores.terreno[1];
	      mapaAuxiliar[filaProv_-1][colProv_] = sensores.terreno[2];
	      mapaAuxiliar[filaProv_-1][colProv_+1] = sensores.terreno[3];

	      mapaAuxiliar[filaProv_-2][colProv_-2] = sensores.terreno[4];
	      mapaAuxiliar[filaProv_-2][colProv_-1] = sensores.terreno[5];
	      mapaAuxiliar[filaProv_-2][colProv_] = sensores.terreno[6];
	      mapaAuxiliar[filaProv_-2][colProv_+1] = sensores.terreno[7];
	      mapaAuxiliar[filaProv_-2][colProv_+2] = sensores.terreno[8];

	      mapaAuxiliar[filaProv_-3][colProv_+3] = sensores.terreno[9];
	      mapaAuxiliar[filaProv_-3][colProv_+2] = sensores.terreno[10];
	      mapaAuxiliar[filaProv_-3][colProv_+1] = sensores.terreno[11];
	      mapaAuxiliar[filaProv_-3][colProv_] = sensores.terreno[12];
	      mapaAuxiliar[filaProv_-3][colProv_-1] = sensores.terreno[13];
	      mapaAuxiliar[filaProv_-3][colProv_-2] = sensores.terreno[14];
	      mapaAuxiliar[filaProv_-3][colProv_-3] = sensores.terreno[15];
	    }break;
	    case 1:{
	      mapaAuxiliar[filaProv_-1][colProv_+1] = sensores.terreno[1];
	      mapaAuxiliar[filaProv_][colProv_+1] = sensores.terreno[2];
	      mapaAuxiliar[filaProv_+1][colProv_+1] = sensores.terreno[3];

	      mapaAuxiliar[filaProv_-2][colProv_+2] = sensores.terreno[4];
	      mapaAuxiliar[filaProv_-1][colProv_+2] = sensores.terreno[5];
	      mapaAuxiliar[filaProv_][colProv_+2] = sensores.terreno[6];
	      mapaAuxiliar[filaProv_+1][colProv_+2] = sensores.terreno[7];
	      mapaAuxiliar[filaProv_+2][colProv_+2] = sensores.terreno[8];

	      mapaAuxiliar[filaProv_-3][colProv_+3] = sensores.terreno[9];
	      mapaAuxiliar[filaProv_-2][colProv_+3] = sensores.terreno[10];
	      mapaAuxiliar[filaProv_-1][colProv_+3] = sensores.terreno[11];
	      mapaAuxiliar[filaProv_][colProv_+3] = sensores.terreno[12];
	      mapaAuxiliar[filaProv_+1][colProv_+3] = sensores.terreno[13];
	      mapaAuxiliar[filaProv_+2][colProv_+3] = sensores.terreno[14];
	      mapaAuxiliar[filaProv_+3][colProv_+3] = sensores.terreno[15];
	    }break;
	    case 2:{
	      mapaAuxiliar[filaProv_+1][colProv_+1] = sensores.terreno[1];
	      mapaAuxiliar[filaProv_+1][colProv_] = sensores.terreno[2];
	      mapaAuxiliar[filaProv_+1][colProv_-1] = sensores.terreno[3];

	      mapaAuxiliar[filaProv_+2][colProv_+2] = sensores.terreno[4];
	      mapaAuxiliar[filaProv_+2][colProv_+1] = sensores.terreno[5];
	      mapaAuxiliar[filaProv_+2][colProv_] = sensores.terreno[6];
	      mapaAuxiliar[filaProv_+2][colProv_-1] = sensores.terreno[7];
	      mapaAuxiliar[filaProv_+2][colProv_-2] = sensores.terreno[8];

	      mapaAuxiliar[filaProv_+3][colProv_+3] = sensores.terreno[9];
	      mapaAuxiliar[filaProv_+3][colProv_+2] = sensores.terreno[10];
	      mapaAuxiliar[filaProv_+3][colProv_+1] = sensores.terreno[11];
	      mapaAuxiliar[filaProv_+3][colProv_] = sensores.terreno[12];
	      mapaAuxiliar[filaProv_+3][colProv_-1] = sensores.terreno[13];
	      mapaAuxiliar[filaProv_+3][colProv_-2] = sensores.terreno[14];
	      mapaAuxiliar[filaProv_+3][colProv_-3] = sensores.terreno[15];
	    }break;
	    case 3:{
	      mapaAuxiliar[filaProv_+1][colProv_-1] = sensores.terreno[1];
	      mapaAuxiliar[filaProv_][colProv_-1] = sensores.terreno[2];
	      mapaAuxiliar[filaProv_-1][colProv_-1] = sensores.terreno[3];

	      mapaAuxiliar[filaProv_+2][colProv_-2] = sensores.terreno[4];
	      mapaAuxiliar[filaProv_+1][colProv_-2] = sensores.terreno[5];
	      mapaAuxiliar[filaProv_][colProv_-2] = sensores.terreno[6];
	      mapaAuxiliar[filaProv_-1][colProv_-2] = sensores.terreno[7];
	      mapaAuxiliar[filaProv_-2][colProv_-2] = sensores.terreno[8];

	      mapaAuxiliar[filaProv_+3][colProv_-3] = sensores.terreno[9];
	      mapaAuxiliar[filaProv_+2][colProv_-3] = sensores.terreno[10];
	      mapaAuxiliar[filaProv_+1][colProv_-3] = sensores.terreno[11];
	      mapaAuxiliar[filaProv_][colProv_-3] = sensores.terreno[12];
	      mapaAuxiliar[filaProv_-1][colProv_-3] = sensores.terreno[13];
	      mapaAuxiliar[filaProv_-2][colProv_-3] = sensores.terreno[14];
	      mapaAuxiliar[filaProv_-3][colProv_-3] = sensores.terreno[15];
	    }break;
	  }
	}
}
