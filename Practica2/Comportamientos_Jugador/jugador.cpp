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
bool case2 = true;

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

  if(case2){
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
  }

  int md = coldif + fildif ;

  return md + ori;
}

class comp : public std::binary_function<estado, estado, bool>{
  std::reference_wrapper < std::map <estado, int, estadocomp> > fScore;
public:
  comp(map <estado, int, estadocomp>& a): fScore(a){}

  int operator()(const estado& a, const estado& b) const {
    if (fScore.get()[a] < fScore.get()[b]) return true;
    if (fScore.get()[a] > fScore.get()[b]) return false;
    return estadocomp()(a,b);
  }
};

bool estadocomp::operator() (const estado& lhs, const estado& rhs) const{
  if (lhs.fila < rhs.fila) return true;
  if (lhs.fila > rhs.fila) return false;
  if (lhs.columna < rhs.columna) return true;
  if (lhs.columna > rhs.columna) return false;
  //if (lhs.orientacion < rhs.orientacion) return true;
  return false;
}

bool ComportamientoJugador::pathFinding(const estado &origen, const estado &destino, list<Action> &plan) {

  map < estado, int, estadocomp> gScore;
  map < estado, int, estadocomp> fScore;
  set < estado, estadocomp > closedSet;
  set < estado, comp > openSet((comp(fScore)));
  map < estado, estado, estadocomp> cameFrom;

  static int a = 0;
  bool Bounded = false;

  if(debug) cout << "[PathFinding]: Inicializa Estructuras" << endl;

  gScore[origen] = 0;

  fScore[origen] = estimateDistance(origen, destino);

  openSet.insert(origen);

  if (debug) cout << "[PathFinding]: Inserta valores iniciales" << endl;

  while (!openSet.empty()){
    estado current = *(openSet.begin());
    a++;
    if(debug) usleep(100000);
    if(debug) cout << "[PathFinding]: Iteracion bucle A* con fila " << current.fila << ", columna " << current.columna << " y orientacion " << current.orientacion << " y fScore " << fScore[current] << endl;

    if(current.fila == destino.fila && current.columna == destino.columna){
      cout <<  a << " Nodos explorados" << endl;
      return reconstructPath(cameFrom, current);
    }
    if (debug) cout << "[PathFinding]: No es el nodo buscado" << endl;
    openSet.erase(openSet.begin());
    if (debug) cout << "[PathFinding]: Nodo eliminado de nodos pendientes. " << endl;
    closedSet.insert(current);
    if (debug) cout << "[PathFinding]: Nodo insertado en nodos visitados "<< endl;

    // Lambda Function
    auto f = [&](estado neighbor, int dist){

      if (debug) cout << "[PathFinding; Lambda]: Entra función." << endl;
      if (!closedSet.count( neighbor )){ // Not in closedSet

        if (debug) cout << "[PathFinding; Lambda]: Vecino no explorado" << endl;
        if (!openSet.count( neighbor )){ // Not in openSet
            openSet.insert( neighbor );
            if (debug) cout << "[PathFinding; Lambda]: Vecino insertado en Lista de nodos pendientes" << endl;
        }

        if ( !gScore.insert(make_pair(neighbor, INT_MAX)).second )

        if (debug) cout << "[PathFinding; Lambda]: Inicializado valor de gScore a inf." << endl;

        int tentative_gScore = gScore[current] + dist;

        if (debug) cout << "[PathFinding; Lambda]: Calculado posible nuevo gScore." << endl;

        if (tentative_gScore <  gScore[neighbor]) {

          if (debug) cout << "[PathFinding; Lambda]: Valor calculado supone una mejora." << endl;

          cameFrom.erase( neighbor );
          cameFrom.insert( std::pair <estado, estado> (neighbor, current));
          if (debug) cout << "[PathFinding; Lambda]: Insertado nuevo camino en cameFrom." << endl;

          gScore.erase( neighbor );
          gScore.insert( make_pair ( neighbor, tentative_gScore ) );
          if (debug) cout << "[PathFinding; Lambda]: Actualizado valor en gScore: " << gScore[neighbor] << endl;

          openSet.erase( neighbor );
          fScore.erase( neighbor );
          fScore[neighbor] = estimateDistance(neighbor, destino) + (Bounded ? 0 : tentative_gScore);

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
      f(neighbor, 1);
    }

    if(isPath(mapaResultado [current.fila - (current.orientacion - 2)%2 ] [current.columna - (current.orientacion- 1)%2] )){ //Right
      //estado neighbor {current.fila, current.columna, (current.orientacion+1)%4};
      estado neighbor {current.fila - (current.orientacion -2)%2, current.columna - (current.orientacion-1)%2, (current.orientacion+1)%4};

      if (debug) cout << "[PathFinding]: Nodo Vecino 2: " << neighbor.fila << " " << neighbor.columna << " " << neighbor.orientacion << endl;
      f(neighbor, 2);
    }

    if(isPath(mapaResultado [current.fila + (current.orientacion - 2)%2 ] [current.columna + (current.orientacion- 1)%2] )){ //Left
      //estado neighbor {current.fila, current.columna, (current.orientacion+3)%4};
      estado neighbor {current.fila + (current.orientacion - 2)%2, current.columna + (current.orientacion - 1)%2, (current.orientacion+3)%4};

      if (debug) cout << "[PathFinding]: Nodo Vecino 3: " << neighbor.fila << " " << neighbor.columna << " " << neighbor.orientacion << endl;
      f(neighbor, 2);
    }

    if (isPath(mapaResultado [origen.fila - (origen.orientacion - 1 )%2][ origen.columna + (origen.orientacion - 2)%2])){
      estado neighbor = {origen.fila - (origen.orientacion - 1)%2, origen.columna + (origen.orientacion - 2)%2, 2};

      if (debug) cout << "[PathFinding]: Nodo vecino 4. " << neighbor.fila << " " << neighbor.columna << " " << neighbor.orientacion << endl;
      f(neighbor, 3);
    }


  }
}

bool ComportamientoJugador::reconstructPath(const map<estado, estado, estadocomp>& cameFrom, const estado& current ){

  estado b = current;

  if (debug) cout << "[PathFinding; Lambda; reconstructPath]: Entra funcion con estado: " << b.fila << " " << b.columna << " " << b.orientacion <<endl;

  for(auto a = cameFrom.find(b) ;a != cameFrom.end(); a=cameFrom.find(a->second)){
    if (debug) cout << "[PathFinding; Lambda; reconstructPath]: Estado anterior: " << a->second.fila << " " << a->second.columna << " " << a->second.orientacion <<endl;

    plan.push_back(actFORWARD);

    if ( (a->second.orientacion + 1)%4 == b.orientacion )
      plan.push_back(actTURN_R);
    else if ( (a->second.orientacion + 3)%4 == b.orientacion )
      plan.push_back(actTURN_L);
    else if ( (a->second.orientacion + 2)%4 == b.orientacion ){
      plan.push_back(actTURN_L);
      plan.push_back(actTURN_L);
    }

    b = a->second;
  }
  return true;


}

bool ComportamientoJugador::isPath(unsigned char c){
  return c == 'T' || c == 'S' || c == 'K';
}

int ComportamientoJugador::lookForPK(Sensores sensores){
  for (int i = 0; i < 10 ; i++){
    if (sensores.terreno[i] == 'K') return i;
  }
  return -1;
}

void ComportamientoJugador::valueToMap(int fila, int col, char c){

  cout << "[valueToMap]: Entra a función: fila " << fila << " columna " << col << " char " << c << endl;
  if(isPath(c)){
    cout << "[valueToMap]; Es un camino. " << endl;
    knownMap[fila][col].cont = INT_MAX;
  }

  knownMap[fila][col].c = c;
}

int ComportamientoJugador::addToKnownMap(Sensores sensores){

  //estado neighbor {current.fila + (current.orientacion-1)%2, current.columna - (current.orientacion - 2)%2, current.orientacion};
  cout << "[addToKnownMap]: Posicion actual: " << filaR << " " << colR << " " << brujula << endl;
  knownMap[filaR][colR].c = sensores.terreno[0];

  cout << sensores.terreno[9] << " " << sensores.terreno[10] << " " << sensores.terreno[11] << " " << sensores.terreno[12] << " " << sensores.terreno[13] << " " << sensores.terreno[14]  << " " << sensores.terreno[15] << endl;
  cout << "  " << sensores.terreno[4] << " " << sensores.terreno[5] << " " << sensores.terreno[6] << " " << sensores.terreno[7] << " " << sensores.terreno[8] << endl;
  cout << "    "  << sensores.terreno[1] << " " << sensores.terreno[2] << " " << sensores.terreno[3] << endl;
  cout << "      " << sensores.terreno[0] << endl;

  switch (brujula) {
  case 0:

    valueToMap(filaR-1, colR-1, sensores.terreno[1]);
    valueToMap(filaR-1, colR, sensores.terreno[2]);
    valueToMap(filaR-1, colR+1, sensores.terreno[3]);
    valueToMap(filaR-2, colR-2, sensores.terreno[4]);
    valueToMap(filaR-2, colR-1, sensores.terreno[5]);
    valueToMap(filaR-2, colR,sensores.terreno[6]);
    valueToMap(filaR-2, colR+1,sensores.terreno[7]);
    valueToMap(filaR-2, colR+2,sensores.terreno[8]);
    valueToMap(filaR-3, colR-3, sensores.terreno[9]);
    valueToMap(filaR-3, colR-2, sensores.terreno[10]);
    valueToMap(filaR-3, colR-1, sensores.terreno[11]);
    valueToMap(filaR-3, colR, sensores.terreno[12]);
    valueToMap(filaR-3, colR+1, sensores.terreno[13]);
    valueToMap(filaR-3, colR+2, sensores.terreno[14]);
    valueToMap(filaR-3, colR+3, sensores.terreno[15]);
    break;
  case 1:
    valueToMap(filaR-1, colR+1, sensores.terreno[1]);
    valueToMap(filaR, colR+1, sensores.terreno[2]);
    valueToMap(filaR+1, colR+1, sensores.terreno[3]);
    valueToMap(filaR-2, colR+2, sensores.terreno[4]);
    valueToMap(filaR-1, colR+2, sensores.terreno[5]);
    valueToMap(filaR, colR+2,sensores.terreno[6]);
    valueToMap(filaR+1, colR+2,sensores.terreno[7]);
    valueToMap(filaR+2, colR+2,sensores.terreno[8]);
    valueToMap(filaR-3, colR+3, sensores.terreno[9]);
    valueToMap(filaR-2, colR+3, sensores.terreno[10]);
    valueToMap(filaR-1, colR+3, sensores.terreno[11]);
    valueToMap(filaR, colR+3, sensores.terreno[12]);
    valueToMap(filaR+1, colR+3, sensores.terreno[13]);
    valueToMap(filaR+2, colR+3, sensores.terreno[14]);
    valueToMap(filaR+3, colR+3, sensores.terreno[15]);
    break;
  case 2:
    valueToMap(filaR+1, colR+1, sensores.terreno[1]);
    valueToMap(filaR+1, colR, sensores.terreno[2]);
    valueToMap(filaR+1, colR-1, sensores.terreno[3]);
    valueToMap(filaR+2, colR+2, sensores.terreno[4]);
    valueToMap(filaR+2, colR+1, sensores.terreno[5]);
    valueToMap(filaR+2, colR,sensores.terreno[6]);
    valueToMap(filaR+2, colR-1,sensores.terreno[7]);
    valueToMap(filaR+2, colR-2,sensores.terreno[8]);
    valueToMap(filaR+3, colR+3, sensores.terreno[9]);
    valueToMap(filaR+3, colR+2, sensores.terreno[10]);
    valueToMap(filaR+3, colR+1, sensores.terreno[11]);
    valueToMap(filaR+3, colR, sensores.terreno[12]);
    valueToMap(filaR+3, colR-1, sensores.terreno[13]);
    valueToMap(filaR+3, colR-2, sensores.terreno[14]);
    valueToMap(filaR+3, colR-3, sensores.terreno[15]);
    break;

  case 3:
    valueToMap(filaR+1, colR-1, sensores.terreno[1]);
    valueToMap(filaR, colR-1, sensores.terreno[2]);
    valueToMap(filaR-1, colR-1, sensores.terreno[3]);
    valueToMap(filaR+2, colR-2, sensores.terreno[4]);
    valueToMap(filaR+1, colR-2, sensores.terreno[5]);
    valueToMap(filaR, colR-2,sensores.terreno[6]);
    valueToMap(filaR-1, colR-2,sensores.terreno[7]);
    valueToMap(filaR-2, colR-2,sensores.terreno[8]);
    valueToMap(filaR+3, colR-3, sensores.terreno[9]);
    valueToMap(filaR+2, colR-3, sensores.terreno[10]);
    valueToMap(filaR+1, colR-3, sensores.terreno[11]);
    valueToMap(filaR, colR-3, sensores.terreno[12]);
    valueToMap(filaR-1, colR-3, sensores.terreno[13]);
    valueToMap(filaR-2, colR-3, sensores.terreno[14]);
    valueToMap(filaR-3, colR-3, sensores.terreno[15]);
    break;

    }

  cout << "[addToKnownMap]: Añadidos valores a la vista." << endl;

  return lookForPK(sensores);

}

void ComportamientoJugador::nextStep(){

  node forward = knownMap[filaR + (brujula-1)%2][colR - (brujula-2)%2];
  node right = knownMap[filaR - (brujula-2)%2][colR - (brujula - 1)%2];
  node left = knownMap[filaR + (brujula-2)%2][colR + (brujula - 1)%2];

  if(isPath(forward.c)){
    cout << "[nextStep]: Hacia delante." << endl;
    filaR = filaR + (brujula-1)%2;
    colR = colR - (brujula-2)%2;
    plan.push_back(actFORWARD);
  }
  else if (right.cont == min (right.cont, left.cont) && isPath(right.c)){
    cout << "[nextStep]: Hacia derecha." << endl;
    filaR = filaR - (brujula-2)%2;
    colR = colR - (brujula-1)%2;
    brujula = (brujula + 1)%4;
    plan.push_back(actFORWARD);
    plan.push_back(actTURN_R);
  }
  else if (isPath(left.c) && left.cont == min(left.cont, right.cont)){
    cout << "[nextStep]: Hacia izquierda. " << endl;
    filaR = filaR + (brujula-2)%2;
    colR = colR + (brujula - 1)%2;
    brujula = (brujula - 1)%4;
    plan.push_back(actFORWARD);
    plan.push_back(actTURN_L);

  }
}

void ComportamientoJugador::goToPK(Sensores sensores, int k){

  cout << "[goToPK]: K en posición: " << k << endl;
  switch(k){
  case 1:
    if (isPath(sensores.terreno[2])){
      plan.push_back(actFORWARD);
      plan.push_back(actTURN_L);
      plan.push_back(actFORWARD);
    }
    else plan.push_back(actTURN_L);
    break;
  case 3:
    if (isPath(sensores.terreno[2])){
      plan.push_back(actFORWARD);
      plan.push_back(actTURN_R);
      plan.push_back(actFORWARD);
    }
    else plan.push_back(actTURN_R);
    break;
  case 6:
    goToPK(sensores, 2);
    break;
  case 2:
    if(isPath(sensores.terreno[2])) plan.push_back(actFORWARD);
    break;

  case 4:
  case 5:
  case 9:
  case 10:
  case 11:
    plan.push_back(actTURN_L);
    goToPK(sensores, 6);
    break;

  default:
    plan.push_back(actTURN_R);
    goToPK(sensores, 6);
    break;
  }

}
Action ComportamientoJugador::think(Sensores sensores) {

  int nivel = 3;

  if(nivel == 3 ){
    cout << "Nivel 3 plan vacio. " << endl;
    int k = addToKnownMap(sensores);
    usleep(100000);
    if(k == -1){
      cout << "No hay k" << endl;
      nextStep();

    }else{
      cout << "Hay k" << endl;
      usleep(10000000);
      goToPK(sensores, k);
    }

    Action ret = plan.back();
    plan.pop_back();
    return ret;

  }

  if(nivel == 1){
  static int it = 0;

  if(firstTime){
     fil = sensores.mensajeF;
     col = sensores.mensajeC;

     destino = {sensores.destinoF, sensores.destinoC, 0};
     cout << "Fila Inicial: " << fil << "\nColumna Inicial: "<< col  << "\nBrujula: "<< brujula << endl;
     cout << "Destino " << sensores.destinoF << " " << sensores.destinoC << endl;
     cout << "Entra funcion PathFinding" << endl;
     estado a = {fil, col, brujula};
     pathFinding(a, destino, plan);

     cout << "Algoritmo terminado" << endl;


     firstTime = false;
  }

  Action ret = plan.back();

  if (plan.empty() == true){
    cout << "Número de pasos: " << it << endl;
  ret = actIDLE;
  }
  else
    if (ret == actFORWARD && sensores.superficie[2] == 'A'){
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

  it++;
  return ret;
  }
  
}

int ComportamientoJugador::interact(Action accion, int valor){
  return false;
}
