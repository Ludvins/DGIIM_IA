#include "./jugador.hpp"
#include "../include/motorlib/util.h"

#include <iostream>
#include <cmath>
#include <map>
#include <limits.h>
#include <algorithm>
#include <set>
#include <memory>

int debug = false;

void ComportamientoJugador::PintaPlan(list<Action> plan) {
	auto it = plan.begin();
	while (it!=plan.end()){
		if (*it == actFORWARD){
			cout << "A ";
		}
		else if (*it == actTURN_R){
			cout << "D ";
		}
		else if (*it == actTURN_L){
			cout << "I ";
		}
		else {
			cout << "- ";
		}
		it++;
	}
	cout << endl;
}

int estimateDistance(const estado &n1, const estado &n2){

  if ( n1.fila == n2.fila && n1.columna == n2.columna) return 0;



  int coldif = abs(n2.columna - n1.columna);
  int fildif = abs(n2.fila - n1.fila);

  int ori = 2;

  switch(n1.orientacion){
  case 0:
    if (n1.columna == n2.columna){
      if (n2.fila < n1.fila) ori = 0;
      else ori = 2;
    }
    else if (n2.fila > n1.fila) ori = 2;
    else ori = 1;
    break;
  case 1:
    if (n1.fila == n2.fila){
      if (n2.columna > n1.columna) ori = 0;
      else ori = 2;
    }
    else if (n2.columna < n1.columna) ori = 2;
    else ori = 1;
    break;
  case 2:
    if (n1.columna == n2.columna){
      if (n2.fila > n1.fila) ori = 0;
      else ori = 2;
    }
    else if (n2.fila < n1.fila) ori = 2;
    else ori = 1;
    break;
  case 3:
    if (n1.fila == n2.fila){
      if (n2.columna < n1.columna) ori = 0;
      else ori = 2;
    }
    else if (n2.columna > n1.columna) ori = 2;
    else ori = 1;
    break;
  }


  int md = coldif + fildif ;

  return md + ori;
}

class comp : public std::binary_function<estado, estado, bool>{
  std::reference_wrapper < std::map <estado, int, estadocomp> > fScore;
public:
  comp(map <estado, int, estadocomp>& a): fScore(a){}

  int operator()(const estado& a, const estado& b){
    return fScore.get()[a] < fScore.get()[b];
  }
};

bool estadocomp::operator() (const estado& lhs, const estado& rhs) const{
  if (lhs.fila < rhs.fila) return true;
  if (lhs.fila > rhs.fila) return false;
  if (lhs.columna < rhs.columna) return true;
  if (lhs.columna > rhs.columna) return false;
  if (lhs.orientacion < rhs.orientacion) return true;
  return false;
}

bool ComportamientoJugador::pathFinding(const estado &origen, const estado &destino, list<Action> &plan) {

  map < estado, int, estadocomp> gScore;
  map < estado, int, estadocomp> fScore;

  set < estado, estadocomp > closedSet;

  multiset < estado, comp > openSet((comp(fScore)));

  map < estado, estado, estadocomp> cameFrom;

  if(debug) cout << "[PathFinding]: Inicializa Estructuras" << endl;

  openSet.insert(origen);

  gScore[origen] = 0;

  fScore[origen] = estimateDistance(origen, destino);

  if (debug) cout << "[PathFinding]: Inserta valores iniciales" << endl;

  while (!openSet.empty()){
    estado current = *(openSet.begin());
    if(debug) usleep(100000);
    if(debug) cout << "[PathFinding]: Iteracion bucle A* con fila " << current.fila << ", columna " << current.columna << " y orientacion " << current.orientacion << " y fScore " << fScore[current] << endl;

    if(current.fila == destino.fila && current.columna == destino.columna) return reconstructPath(cameFrom, current);

    if (debug) cout << "[PathFinding]: No es el nodo buscado" << endl;
    openSet.erase(openSet.begin());
    if (debug) cout << "[PathFinding]: Nodo eliminado de nodos pendientes. " << endl;
    closedSet.insert(current);
    if (debug) cout << "[PathFinding]: Nodo insertado en nodos visitados "<< endl;

    // Lambda Function
    auto f = [&](estado neighbor){

      if (debug) cout << "[PathFinding; Lambda]: Entra función." << endl;
      if (!closedSet.count(neighbor)){ // Not in closedSet

        if (debug) cout << "[PathFinding; Lambda]: Vecino no explorado" << endl;
        if (openSet.find( neighbor) == openSet.end()){ // Not in openSet
            openSet.insert( neighbor );
            if (debug) cout << "[PathFinding; Lambda]: Vecino insertado en Lista de nodos pendientes" << endl;
        }

        gScore.insert(make_pair(neighbor, INT_MAX));

        if (debug) cout << "[PathFinding; Lambda]: Inicializado valor de gScore a inf." << endl;

        int tentative_gScore = gScore[current] + 1; //Cambiar esta distancia

        if (debug) cout << "[PathFinding; Lambda]: Calculado posible nuevo gScore." << endl;

        if (tentative_gScore <  gScore[neighbor]) {

          if (debug) cout << "[PathFinding; Lambda]: Valor calculado supone una mejora." << endl;

          cameFrom.insert( std::pair<estado,estado> (neighbor, current));
          if (debug) cout << "[PathFinding; Lambda]: Insertado nuevo camino en cameFrom." << endl;

          gScore.erase(neighbor);
          gScore.insert(make_pair(neighbor,tentative_gScore));
          if (debug) cout << "[PathFinding; Lambda]: Actualizado valor en gScore: " << gScore[neighbor] << endl;

          openSet.erase(neighbor);
          fScore.erase(neighbor);
          fScore[neighbor] = gScore[neighbor] + estimateDistance(neighbor, destino); //same
          if (debug) cout << "[PathFinding; Lambda]: Actualizado valor en fScore: " << fScore[neighbor] << endl;

          openSet.insert(neighbor);
          if (debug) cout << "[PathFinding; Lambda]: Actualizado posicion en openSet." << endl << "[PathFinding; Lambda]: Estado de osenSet: ";
          if (debug) for(auto it : openSet){
            cout << it.fila  << " " << it.columna << " " << it.orientacion << "  ";
          }
          if (debug)cout << endl;
        }
      }
    }; //END

    if(isPath(mapaResultado[ current.fila + (current.orientacion-1) %2 ] [ current.columna - (current.orientacion - 2)%2])){ //Forward
      estado neighbor {current.fila + (current.orientacion-1)%2, current.columna - (current.orientacion - 2)%2, current.orientacion};
      if (debug) cout << "[PathFinding]: Nodo Vecino 1: " << neighbor.fila << " " << neighbor.columna << " " << neighbor.orientacion << endl;
      f(neighbor);
    }

    if(isPath(mapaResultado [current.fila - (current.orientacion - 2)%2 ] [current.columna - (current.orientacion- 1)%2] )){ //Right
      estado neighbor {current.fila, current.columna, (current.orientacion+1)%4};

      if (debug) cout << "[PathFinding]: Nodo Vecino 2: " << neighbor.fila << " " << neighbor.columna << " " << neighbor.orientacion << endl;
      f(neighbor);
    }

    if(isPath(mapaResultado [current.fila + (current.orientacion - 2)%2 ] [current.columna + (current.orientacion- 1)%2] )){ //Left
      estado neighbor {current.fila, current.columna, (current.orientacion+3)%4};
      if (debug) cout << "[PathFinding]: Nodo Vecino 3: " << neighbor.fila << " " << neighbor.columna << " " << neighbor.orientacion << endl;
      f(neighbor);
    }
  }
}

bool ComportamientoJugador::reconstructPath(const map<estado, estado, estadocomp>& cameFrom, const estado& current ){

  estado b = current;

  if (debug) cout << "[PathFinding; Lambda; reconstructPath]: Entra funcion con estado: " << b.fila << " " << b.columna << " " << b.orientacion <<endl;

  for(auto a = cameFrom.find(b) ;a != cameFrom.end(); a=cameFrom.find(a->second)){
    if (debug) cout << "[PathFinding; Lambda; reconstructPath]: Estado anterior: " << a->second.fila << " " << a->second.columna << " " << a->second.orientacion <<endl;

    if(a->second.orientacion == b.orientacion) plan.push_back(actFORWARD);
    else
      if ( (a->second.orientacion + 1)%4 ==b.orientacion ) plan.push_back(actTURN_R);
      else if ( (a->second.orientacion + 3)%4 == b.orientacion ) plan.push_back(actTURN_L);

    b = a->second;
  }
  return true;


}

bool ComportamientoJugador::isPath(unsigned char c){
  return c == 'T' || c == 'S' || c == 'K';
}

Action ComportamientoJugador::think(Sensores sensores) {


  if(hola){


    fil = sensores.mensajeF;
    col = sensores.mensajeC;
    destino = {sensores.destinoF, sensores.destinoC, 0};
    cout << "Fila Inicial:" << fil << "\nColumna Inicial: "<< col  << "\nBrujula: "<< brujula << endl;



    cout << "Entra funcion PathFinding" << endl; 
    estado a = {fil, col, brujula};
    pathFinding(a, destino, plan);

    cout << "Algoritmo terminado" << endl;
    hola = false;
  }
  Action ret = plan.back();

  if (plan.empty() == true) ret = actIDLE;
  else
    if (ret == actFORWARD && !isPath(mapaResultado[fil + (brujula-1)%2][col - (brujula-2)%2])){
      cout << 1 << endl;;
      return actIDLE;
    }
    else plan.pop_back();

  switch(ret){
  case actFORWARD:

    fil += (brujula-1)%2;
    col -= (brujula -2)%2;
    break;
  case actTURN_L:
    brujula = (brujula+3)%4;
    break;
  case actTURN_R:
    brujula = (brujula +1)%4;
    break;
  }

  if (debug)
    cout << "Posicion Actual: \n\tFila: " << fil << "\n\tColumna: " << col << "\n\tOrientacion: " << brujula << endl;


  return ret;



}

int ComportamientoJugador::interact(Action accion, int valor){
  return false;
}
