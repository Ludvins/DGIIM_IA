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

void AnularMatriz(vector<vector<unsigned char> > &m){
	for (int i=0; i<m[0].size(); i++){
		for (int j=0; j<m.size(); j++){
			m[i][j]=0;
		}
	}
}

void ComportamientoJugador::VisualizaPlan(const estado &st, const list<Action> &plan){
  AnularMatriz(mapaConPlan);
	estado cst = st;

	auto it = plan.begin();
	while (it!=plan.end()){
		if (*it == actFORWARD){
			switch (cst.orientacion) {
				case 0: cst.fila--; break;
				case 1: cst.columna++; break;
				case 2: cst.fila++; break;
				case 3: cst.columna--; break;
			}
			mapaConPlan[cst.fila][cst.columna]=1;
		}
		else if (*it == actTURN_R){
			cst.orientacion = (cst.orientacion+1)%4;
		}
		else {
			cst.orientacion = (cst.orientacion+3)%4;
		}
		it++;
	}
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

    if(debug) cout << "Comparando: "<< a << "  " << b << "   " << fScore.get().find(a)->second << " " << fScore.get().find(b)->second << endl;
    if (fScore.get().find(b)-> second == 0)  return true;

    if (fScore.get()[a] < fScore.get()[b]) return true;
    if (fScore.get()[a] > fScore.get()[b]) return false;
    return estadocomp()(a,b);

  }
};

bool estadocomp::operator() (const estado& lhs, const estado& rhs) const{
  if (lhs.fila < rhs.fila) return true;
  if (lhs.fila > rhs.fila) return false;
  if (lhs.columna < rhs.columna) return true;
  //if (lhs.columna > rhs.columna) return false;
  //if (lhs.orientacion < rhs.orientacion) return true;
  return false;
}

bool isPath(unsigned char c){
  return c == 'T' || c == 'S' || c == 'K';
}

bool reconstructPath(const map<estado, estado, estadocomp>& cameFrom, const estado& current, list<Action>& plan ){

  estado b = current;

  if (debug) cout << "[PathFinding; Lambda; reconstructPath]: Entra funcion con estado: " << b.fila << " " << b.columna << " " << b.orientacion <<endl;

  for(auto a = cameFrom.find(b) ; a != cameFrom.end(); a=cameFrom.find(a->second)){
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

template <typename T>
bool a_star_algorithm(const estado& origen, const estado& destino, list<Action>& plan, T& mapaR){

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

    if(current == destino){
      cout <<  a << " Nodos explorados" << endl;
      return reconstructPath(cameFrom, current, plan);
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
        /*if (!openSet.count( neighbor )){ // Not in openSet
            openSet.insert( neighbor );
            if (debug) cout << "[PathFinding; Lambda]: Vecino insertado en Lista de nodos pendientes" << endl;
            }*/

        if ( gScore.insert(make_pair(neighbor, INT_MAX)).second )
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

          if(debug) for(auto it : openSet){
            cout << it.fila  << " " << it.columna << " " << it.orientacion << "  " << fScore.find(it)->second << endl;
          }
          if (debug)cout << endl;
        }
      }
    }; //END

    int rowForward = current.fila + (current.orientacion-1)%2; int colForward = current.columna - (current.orientacion-2)%2;
    int rowRight = current.fila - (current.orientacion - 2)%2; int colRight = current.columna - (current.orientacion - 1)%2;
    int rowLeft = current.fila + (current.orientacion - 2)%2; int colLeft = current.columna + (current.orientacion - 1)%2;
    int rowBack = current.fila - (current.orientacion - 1)%2; int colBack = current.columna + (current.orientacion - 2)%2;

    if(isPath(mapaR[ rowForward ] [ colForward ])){ //Forward
      estado neighbor { rowForward, colForward, current.orientacion};
      if (debug) cout << "[PathFinding]: Nodo Vecino 1: " << neighbor.fila << " " << neighbor.columna << " " << neighbor.orientacion << endl;
      f(neighbor, 1);
    }

    if(isPath(mapaR[ rowRight ] [ colRight ] )){ //Right
      //estado neighbor {current.fila, current.columna, (current.orientacion+1)%4};
      estado neighbor {rowRight, colRight, (current.orientacion+1)%4};
      if (debug) cout << "[PathFinding]: Nodo Vecino 2: " << neighbor.fila << " " << neighbor.columna << " " << neighbor.orientacion << endl;
      f(neighbor, 2);
    }

    if(isPath(mapaR[ rowLeft ] [ colLeft ] )){ //Left
      //estado neighbor {current.fila, current.columna, (current.orientacion+3)%4};
      estado neighbor {rowLeft, colLeft, (current.orientacion+3)%4};
      if (debug) cout << "[PathFinding]: Nodo Vecino 3: " << neighbor.fila << " " << neighbor.columna << " " << neighbor.orientacion << endl;
      f(neighbor, 2);
    }

    if (isPath(mapaR[ rowBack ][ colBack ])){ //Backwards
      estado neighbor = {rowBack, colBack, (current.orientacion+2)%4};
      if (debug) cout << "[PathFinding]: Nodo vecino 4. " << neighbor.fila << " " << neighbor.columna << " " << neighbor.orientacion << endl;
      f(neighbor, 3);
    }

  }
  return false;
}

bool ComportamientoJugador::pathFinding(const estado &origen, const estado &destino, list<Action> &plan) {
  return a_star_algorithm <> (origen, destino, plan, mapaResultado);

}

void ComportamientoJugador::valueToMap(int fila, int col, char c, estado& ret){

  //cout << "[valueToMap]: Entra a función: fila " << fila << " columna " << col << " char " << c << endl;
  if(!isPath(c)){
    //cout << "[valueToMap]: No es un camino. " << endl;
    knownMapI[fila][col] = INT_MAX;
  }

  if (c == 'K')  ret = {fila, col, 0};
  knownMapC[fila][col] = c;
}

estado ComportamientoJugador::addToKnownMap(Sensores sensores){

  estado ret = {-1, -1, 0};
  //estado neighbor {current.fila + (current.orientacion-1)%2, current.columna - (current.orientacion - 2)%2, current.orientacion};
  cout << "[addToKnownMap]: Posicion actual: " << filaR << " " << colR << " " << brujula << endl;
  knownMapC[filaR][colR] = sensores.terreno[0];

  cout << sensores.terreno[9] << " " << sensores.terreno[10] << " " << sensores.terreno[11] << " " << sensores.terreno[12] << " " << sensores.terreno[13] << " " << sensores.terreno[14]  << " " << sensores.terreno[15] << endl;
  cout << "  " << sensores.terreno[4] << " " << sensores.terreno[5] << " " << sensores.terreno[6] << " " << sensores.terreno[7] << " " << sensores.terreno[8] << endl;
  cout << "    "  << sensores.terreno[1] << " " << sensores.terreno[2] << " " << sensores.terreno[3] << endl;
  cout << "      " << sensores.terreno[0] << endl;


  int m[15][2] = {{-1,-1},{-1,0},{-1,1},{-2,-2},{-2,-1},{-2,0},{-2,1},{-2,2},{-3,-3},{-3,-2},{-3,-1},{-3,0},{-3,1},{-3,2},{-1,3}};

  switch (brujula) {
  case 0:
    for( int i = 0; i < 15; i++)
      valueToMap(filaR+m[i][0], colR+m[i][1], sensores.terreno[i+1], ret);
    break;
  case 1:
    for( int i = 0; i < 15; i++)
      valueToMap(filaR+m[i][1], colR-m[i][0], sensores.terreno[i+1], ret);
    break;
  case 2:
    for( int i = 0; i < 15; i++)
      valueToMap(filaR-m[i][0], colR-m[i][1], sensores.terreno[i+1], ret);
    break;

  case 3:
    for( int i = 0; i < 15; i++)
      valueToMap(filaR-m[i][0], colR+m[i][1], sensores.terreno[i+1], ret);
    break;

    }

  cout << "[addToKnownMap]: Añadidos valores a la vista." << endl;

  return ret;

}

void ComportamientoJugador::nextStep(){


  estado forward = {filaR + (brujula-1)%2, colR - (brujula-2)%2, brujula};
  cout << "[nextStep]: Estado de delante: " << forward <<  " " << knownMapI[forward.fila][forward.columna] << endl;;
  estado right = { filaR - (brujula-2)%2, colR - (brujula - 1)%2, (brujula+1)%4};
  cout << "[nextStep]: Estado de la derecha: " << right << " " << knownMapI[right.fila][right.columna] << endl;;
  estado left = {filaR + (brujula-2)%2, colR + (brujula - 1)%2, (brujula+3)%4};
  cout << "[nextStep]: Estado de la izquierda: " << left << " " << knownMapI[left.fila][left.columna] << endl;

  if(isPath(knownMapC[forward.fila][forward.columna])){
    cout << "[nextStep]: Hacia delante." << endl;
    plan.push_back(actFORWARD);
  }
  else if (knownMapI[right.fila][right.columna] == min (knownMapI[right.fila][right.columna], knownMapI[left.fila][left.columna]) && isPath(knownMapC[right.fila][right.columna])){
    cout << "[nextStep]: Hacia derecha." << endl;
    plan.push_back(actFORWARD);
    plan.push_back(actTURN_R);
  }
  else if (isPath(knownMapC[left.fila][left.columna]) && knownMapI[left.fila][left.columna] == min(knownMapI[left.fila][left.columna], knownMapI[right.fila][right.columna])){
    cout << "[nextStep]: Hacia izquierda. " << endl;
    plan.push_back(actFORWARD);
    plan.push_back(actTURN_L);

  }
}

void ComportamientoJugador::goToPK( estado k){

  estado origen = {filaR, colR, brujula};

  if(a_star_algorithm <> (origen, k, plan, knownMapC)){
    cout << "[goToPK]: Encontrado camino con A* " << endl;
  }

}

// Updates row (r) and column (c) using an Action (a) and orientation (b).
void updatePosition(Action a, int& r, int& c, int& b){

  switch(a){
  case actFORWARD:
    r += (b - 1)%2;
    c -= (b - 2)%2;
    break;
  case actTURN_L:
    b = (b + 3)%4;
    break;
  case actTURN_R:
    b = (b + 1)%4;
    break;
  default:
    break;
  }

}


Action ComportamientoJugador::think(Sensores sensores) {

  static bool firstTime = true;
  static int it = 0;
  static int level = 3;

  if(firstTime){

    fil = sensores.mensajeF;
    col = sensores.mensajeC;

    if(fil != -1){

      level = 1;
      destino = {sensores.destinoF, sensores.destinoC, 0};
      cout << "Fila Inicial: " << fil << "\nColumna Inicial: "<< col  << "\nBrujula: "<< brujula << endl;
      cout << "Destino " << sensores.destinoF << " " << sensores.destinoC << endl;
      cout << "Entra funcion PathFinding" << endl;
      estado a = {fil, col, brujula};
      pathFinding(a, destino, plan);

      cout << "Algoritmo terminado" << endl;
      PintaPlan(plan);

    }

    firstTime = false;
  }

  if ( level == 3 ){
    cout << "Posición relativa actual: " << filaR << " " << colR << " " << brujula << endl;

    if(plan.empty()){
      cout << "Nivel 3 plan vacio. " << endl;
      estado k = addToKnownMap(sensores);
      usleep(100000);
      if(k.fila == -1){
        cout << "No hay k" << endl;
        nextStep();
      }else{
        cout << "Hay k" << endl;
        usleep(10000000);
        goToPK( k );
      }
    }
  }


  Action ret;

  if (plan.empty() == true){
    cout << "Número de pasos: " << it << endl;
    ret = actIDLE;
  }
  else{
    ret = plan.back();
    if (ret == actFORWARD && sensores.superficie[2] == 'a'){
      cout << "Aldeano esquivado." << endl;;
      return actIDLE;
    }
    else plan.pop_back();
  }

  if(level == 3) updatePosition (ret, filaR, colR, brujula);
  else updatePosition (ret, fil, colR, brujula);

  if (debug)
    cout << "Posicion Actual: \n\tFila: " << fil << "\n\tColumna: " << col << "\n\tOrientacion: " << brujula << endl;

  it++;
  return ret;

}


int ComportamientoJugador::interact(Action accion, int valor){
  return false;
}
