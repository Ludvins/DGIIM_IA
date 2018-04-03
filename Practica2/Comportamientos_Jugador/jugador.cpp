#include "./jugador.hpp"
#include "../include/motorlib/util.h"

#include <iostream>
#include <cmath>
#include <map>
#include <limits.h>
#include <algorithm>
#include <set>

void print(node* n){
  if(n ==nullptr) cout << "Es nulo\n";
  else
    cout << "Fila:" << n->i << " Columna:" << n->j << " Brujula: "<< n->b << endl; 
}


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

int ComportamientoJugador::estimateDistance(const estado &n1, const estado &n2){
  return ( (abs(n2.fila - n1.fila)) + (abs(n2.columna - n1.columna)) );
}


struct classcomp {
  bool operator() (const pair<estado,int>& lhs, const pair<estado,int>& rhs) const
  {return lhs.second < rhs.second;}
};

bool ComportamientoJugador::pathFinding(const estado &origen, const estado &destino, list<Action> &plan) {

  set <pair<estado,int>, classcomp> closedSet;
  set <pair<estado,int>, classcomp> openSet;

  openSet.insert( pair <estado, int> (origen, nodeDistance(origen, destino)) );

  map<estado,int> gScore;
  map<estado,int> fScore;
  map <estado,estado> cameFrom;
  gScore.insert(std::pair <estado,int> (origen, 0));

  fScore.insert( std::pair <estado,int> (origen, nodeDistance(origen, destino)));
 

  while (!openSet.empty()){

    estado current = openSet.back();

    if(current.fila == destino.fila && current.columna == destino.columna) return reconstructPath(cameFrom, current);

    closedSet.push_back(current);

    for(int i = -1; i < 3; i++){

      estado neighbor {current.fila + (i%2), current.columna + ((i-1)%2), 0};

      if (find(closedSet.begin(), closedSet.end(), neighbor) != closedSet.end()) //In closed set
        continue;

      if (find(openSet.begin(), openSet.end(), neighbor) == openSet.end()) // Not in openSet
        openSet.push_back(neighbor);

      gScore.insert( std::pair <estado, int> (neighbor, INT_MAX));

      int tentative_gScore = gScore[current] + nodeDistance(neighbor, destino);
      if (tentative_gScore >= gScore[neighbor]) continue;

      cameFrom.insert( std::pair<estado,estado> (neighbor, current));
      gScore[neighbor] = tentative_gScore;
      fScore[neighbor] = gScore[neighbor] + nodeDistance(neighbor, destino);
~
    }







  }

}

bool ComportamientoJugador::isPath(unsigned char c){
  return c == 'T' || c == 'S';
}


void ComportamientoJugador::addSurroundings(node* n){
  if( isPath(mapaResultado[n->i][n->j - 1])){
    node* a = new node {n->i, n->j - 1, 1, n, nullptr, nullptr};
    n->hijo = a;
  }

  if( isPath(mapaResultado[n->i - 1][n->j])){
    node* b = new node {n->i - 1, n->j, 2 ,n, nullptr, nullptr};
    if(n->hijo == nullptr)
      n->hijo = b;
    else{
      auto it = n->hijo;
      for(it; it->next != nullptr; it = it->next);
      it->next = b;
    }
  }

  if( isPath(mapaResultado[n->i + 1][n->j])){
    node* c = new node {n->i + 1, n->j, 0,  n, nullptr, nullptr};
    if(n->hijo == nullptr)
      n->hijo = c;
    else{
      auto it = n->hijo;
      for(it; it->next != nullptr; it = it->next);
      it->next = c;
    }

  }

  if( isPath(mapaResultado[n->i][n->j + 1])){
    node* d = new node {n->i, n->j + 1, 3, n, nullptr, nullptr};
    if(n->hijo == nullptr)
      n->hijo = d;
    else{
      auto it = n->hijo;
      for(it; it->next != nullptr; it = it->next);
      it->next = d;
    }

  }
}

node* ComportamientoJugador::create(node* n, int i , int j){
  cout << i << j;

  queue<node*> unvisited_nodes;
  unvisited_nodes.push(n);

  while(true){

    node* subtree = unvisited_nodes.front();
    print(subtree);
    unvisited_nodes.pop();

    if(subtree->i == i && subtree->j == j){
      cout << "a" << endl;
      return subtree;
    }

    addSurroundings(subtree);

    for(auto it = subtree->hijo; it != nullptr; it=it->next){

      //Si ya he visitado el nodo nada
      //Si no
      unvisited_nodes.push(it);
    }
  }

}

/*Action ComportamientoJugador::decideAccion(node* n){

  if()


  }*/


Action ComportamientoJugador::think(Sensores sensores) {

    static node* m;

    static queue<Action> q;

    fil = sensores.mensajeF;
    col = sensores.mensajeC;

    cout << fil << col << brujula << endl;

  if(hola){

    destino.fila = sensores.destinoF;
    destino.columna = sensores.destinoC;

    node* n = new node {destino.fila, destino.columna, 0,  nullptr, nullptr, nullptr};

    m = create(n,fil,col);
    cout << "b" << endl;;
    hola = false;
    }

  if (m->i != destino.fila || m->j != destino.columna){
    print(m);
    while(brujula != m->b){
      cout << "c" << m->b << brujula << endl;
      brujula = (brujula +1)%4;
      q.push(actTURN_R);
    }
    q.push(actFORWARD);

    m=m->padre;

  }

  Action ret = q.front();

  if (ret == actFORWARD){
    cout << "A ";
  }
  else if (ret == actTURN_R){
    cout << "D ";
  }
  else if (ret == actTURN_L){
    cout << "I ";
  }
  else {
    cout << "- ";
  }

  q.pop();
  return ret;
}

int ComportamientoJugador::interact(Action accion, int valor){
  return false;
}
