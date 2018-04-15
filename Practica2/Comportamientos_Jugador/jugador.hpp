#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "../include/comportamientos/comportamiento.hpp"

#include <queue>
#include <list>
#include <iostream>
#include <map>
#include <unistd.h>


struct estado {
  int fila;
  int columna;
  int orientacion;


  bool operator==(estado b){
    return fila == b.fila && columna == b.columna;
  }
};

struct nodeestado {
  estado a;
  estado * father;
};

struct node {

  char c;
  int cont;
};

struct estadocomp  {
  bool operator() (const estado& lhs, const estado& rhs) const;
};

class ComportamientoJugador : public Comportamiento {
 public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size) {
      // Inicializar Variables de Estado
      fil = col = 99;
      brujula = 0; // 0: Norte, 1:Este, 2:Sur, 3:Oeste
      destino.fila = -1;
      destino.columna = -1;
      destino.orientacion = -1;
    }
    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR) {
      // Inicializar Variables de Estado
      fil = col = 99;
      brujula = 0; // 0: Norte, 1:Este, 2:Sur, 3:Oeste
      destino.fila = -1;
      destino.columna = -1;
      destino.orientacion = -1;
    }
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);
  ComportamientoJugador * clone(){return new ComportamientoJugador(*this);}

private:
  // Declarar Variables de Estado
  int fil, col, brujula;
  estado destino;
  list<Action> plan;
  node knownMap[200][200];
  int filaR = 100, colR = 100;

  bool firstTime = true;

  bool pathFinding(const estado &origen, const estado &destino, list<Action> &plan);
  void PintaPlan(list<Action> plan);
  bool isPath(unsigned char c);

  int addToKnownMap(Sensores sensores);
  int lookForPK(Sensores sensores);
  void nextStep();
  void valueToMap(int fila, int col, char c);

  bool reconstructPath(const map <estado, estado, estadocomp>& cameFrom,const estado& current);

};

#endif
