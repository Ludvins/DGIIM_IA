#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "../include/comportamientos/comportamiento.hpp"

#include <queue>
#include <list>
#include <iostream>
#include <map>
#include <unistd.h>
#include <limits.h>

struct pairIntchar{
  int i = INT_MAX;
  char c = '?';

  operator char&() { return c; }
  operator int&() { return i; }

  void operator= (char _c){ c = _c; }
  bool operator== (char _c) {return c == _c ;}

  void operator++ (){
    i++;
  }

};

struct mapOfChar {
  map < int , map <int, pairIntchar> > m;
  int cont = 0;

public:
  void insert (int x, int y, char c){
    m[x][y] = c;
  }

  void count( int f, int c){
    m[f][c].i = cont;
    cout << cont << endl;
    cont++;
  }

  friend ostream& operator<< (ostream& os, mapOfChar& m){
    for(auto it: m.m){
      int row = it.first;
      for (auto it2 : it.second){
        int col = it2.first;
        cout << "Fila: " << row << " columna: " << col << " " << static_cast<char>(it2.second) << endl;
      }
    }
    return os;
  }


};


struct estado {
  int fila;
  int columna;
  int orientacion;


  bool operator==(estado b){
    return fila == b.fila && columna == b.columna;
  }

  friend ostream& operator<<(ostream& c, estado& a){
    c << a.fila << " " << a.columna << " " << a.orientacion;
    return c;
  }

  friend ostream& operator<<(ostream& c, const estado& a){
    c << a.fila << " " << a.columna << " " << a.orientacion;
    return c;
  }
};

struct nodeestado {
  estado a;
  estado *father;
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
      fil = col = -1;
      brujula = 0; // 0: Norte, 1:Este, 2:Sur, 3:Oeste
      destino.fila = -1;
      destino.columna = -1;
      destino.orientacion = -1;
    }
    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR) {
      // Inicializar Variables de Estado
      fil = col = -1;
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
  void VisualizaPlan(const estado &st, const list<Action> &plan);
private:
  // Declarar Variables de Estado
  int fil, col, brujula;
  estado destino= {-1, -1, -1};
  list<Action> plan;

  bool pathFinding(const estado &origen, const estado &destino, list<Action> &plan);
  void PintaPlan(list<Action> plan);
  //bool isPath(unsigned char c);

  void copyInMap(Sensores sensores);
  template <class T> void saveVisibleMap(Sensores s, T& m);
  //int lookForPK(Sensores sensores);
  void valueToMap(int fila, int col, char c, estado& ret);
  void goToPK( estado k);
  bool isDestination( estado a);
  //bool reconstructPath(const map <estado, estado, estadocomp>& cameFrom,const estado& current);


  void RandomBehaviour(mapOfChar& m);
  bool inicializeCoordenates(Sensores s, bool a);
  bool objetiveHaschanged(estado objetive, Sensores s);
  bool canSeeK(Sensores s, estado& k, mapOfChar& m);
  bool thereIsPathForK(estado k, mapOfChar& m);
  estado interpretVision ( int x, mapOfChar& m);
  void copyKnownMap(mapOfChar& m, Sensores s);
  void heuristicBehaviour (Sensores s, bool reset);
  estado closestKnown(Sensores s, estado a);
};

#endif
