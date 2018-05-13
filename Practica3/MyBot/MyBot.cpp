/*
 * MyBot.cpp
 *
 *  Created on: 07-05-2018
 *      Author: Ludvins
 */

#include <string>
#include <cstdlib>
#include <iostream>
#include <limits.h>
#include <cmath>
#include <list>

#include "MyBot.h"
using namespace std;

bool debug = false;

MyBot::MyBot() {
	// Inicializar las variables necesarias para ejecutar la partida

}

MyBot::~MyBot() {
	// Liberar los recursos reservados (memoria, ficheros, etc.)
}

void MyBot::initialize() {
	// Inicializar el bot antes de jugar una partida
}

string MyBot::getName() {
	return "MyBot"; // Sustituir por el nombre del bot
}

using depth = int;
using bound = int;


class GameNode{

  GameState game;
  Move reach_this_node;
  bool maximize;

public:

  using action = Move;

  GameNode(GameState gs, Move _reach_this_node, bool _maximize){
    game = gs;
    reach_this_node = _reach_this_node;
    maximize = _maximize;
  }

  bound
  get_heuristic_value(){
    return game.getScore(J1) - game.getScore(J2);
  }

  action
  get_action(){
    return reach_this_node;
  }

  bool
  is_terminal(){
    return game.isFinalState();
  }

  bool
  is_max_node(){
    return maximize;
  }

  list < pair<GameNode,action> >
  get_children(){

    list< pair<GameNode, action> > children;

    for(int id = 1; id <= 6; id++){

      GameState gs = game.simulateMove ( (Move) id);
      bool child_maximize = gs.getCurrentPlayer() == game.getCurrentPlayer() ? maximize : !maximize;
      GameNode child = GameNode (gs, (Move) id,  child_maximize);
      children.push_back( make_pair(child, (Move) id ) );
    }

    return children;
  }

  friend ostream&
  operator<< (ostream& os, GameNode gn){
    os << "\tJ1: Granero="<< (int) gn.game.getSeedsAt(J1, GRANERO) << " C1=" << (int)gn.game.getSeedsAt(J1,P1) << " C2=" << (int)gn.game.getSeedsAt(J1,P2) << " C3=" << (int)gn.game.getSeedsAt(J1,P3) << " C4=" <<(int) gn.game.getSeedsAt(J1,P4) << " C5=" << (int) gn.game.getSeedsAt(J1,P5) << " C6=" <<(int) gn.game.getSeedsAt(J1,P6) << endl;
    os << "\tJ2: Granero="<< (int)gn.game.getSeedsAt(J2, GRANERO) << " C1=" << (int)gn.game.getSeedsAt(J2,P1) << " C2=" << (int)gn.game.getSeedsAt(J2,P2) << " C3=" << (int) gn.game.getSeedsAt(J2,P3) << " C4=" << (int)gn.game.getSeedsAt(J2,P4) << " C5=" << (int)gn.game.getSeedsAt(J2,P5) << " C6=" << (int)gn.game.getSeedsAt(J2,P6) << endl;
    return os;
  }

};



template <class node>
struct bound_and_action {

  bound _bound;
  typename node::action _action;

  bool operator< (bound_and_action lhs) const {
    return _bound < lhs._bound;
  }

};


template <class node>
bound_and_action<node> alpha_beta(node root, depth depth, bound alpha, bound beta){

  if(debug)
    cerr << "[AlphaBeta]: Entra algoritmo,  con profundidad " << depth << " y maximize " << root.is_max_node() <<  endl;
  if(debug)
    cerr << root;

  if ( depth == 0 || root.is_terminal() )
    {
      bound_and_action<node> ret = {root.get_heuristic_value(), root.get_action()};
      if (debug)
        cerr << "[AlphaBeta]: Nodo Hoja, devuelve valor: " << ret._bound << " " << ret._action << endl;
      return ret;
    }

  auto children = root.get_children();

  bound_and_action<node> ret;

  if (root.is_max_node()){
    ret._bound = INT_MIN;

    for (auto child : children){
      bound_and_action<node> possible_ret = alpha_beta (child.first, depth - 1, alpha, beta);

      if (possible_ret._bound > ret._bound){
        ret._bound = possible_ret._bound;
        ret._action = child.first.get_action();
      }

      alpha = std::max(alpha, ret._bound);
      if ( beta <= alpha )
        break; // beta cut-off
    }

  } else { // if (min_node)
    ret._bound = INT_MAX;

    for (auto child : children){
      bound_and_action <node> possible_ret = alpha_beta (child.first, depth - 1, alpha, beta);

      if (possible_ret._bound < ret._bound) {
        ret._bound = possible_ret._bound;
        ret._action = child.first.get_action();
      }

      beta = std::min(beta, ret._bound);
      if ( beta <= alpha )
        break; // alpha cut-off
    }
  }

  if (debug)
    cerr << "[AlphaBeta]: Devuelve Valor: " << ret._bound << " " << ret._action << endl;

  return ret;
}


template <class node>
bound_and_action <node> MTDF (node root, bound first, depth d){

  if (debug)
    cerr << "[MTDF]: Entra MTDF." << endl;
  bound_and_action <node> ret;
  ret._bound = first;

  bound upper = INT_MAX;
  bound lower = INT_MIN;
  bound beta;
  int cont = 0;
  do {

    if (debug)
      cerr << "[MTDF]: Iteracion número: " << cont << endl;

    // if ( ret.bound == lower )
    //   beta = ret.bound + 1;
    // else
    //   beta = ret.bound;
    beta = (ret._bound == lower) ? ret._bound + 1 : ret._bound;

    ret = alpha_beta <GameNode> ( root, d, beta - 1, beta);

    // if ( ret.bound < beta )
    //   upper = ret.bound;
    // else
    //   lower = ret.bound;
    ret._bound < beta ? upper = ret._bound : lower = ret._bound;
    cont ++;
    if (debug)
      cerr << "[MTDF]: Aproximación actual " << ret._bound << " " << ret._action << endl;
  } while (lower < upper);

  if (debug)
    cerr << "[MTDF]: Bound and Action: " << ret._bound << " " << ret._action << endl;
  return ret;

}




Move MyBot::nextMove(const vector<Move> &adversary, const GameState &state) {


	Player turno= this->getPlayer();
	long timeout= this->getTimeOut();

	Move movimiento= M_NONE;

	// Implementar aquí la selección de la acción a realizar

	// OJO: Recordatorio. NO USAR cin NI cout.
	// Para salidas por consola (debug) utilizar cerr. Ejemplo:
	// cerr<< "Lo que quiero mostrar"<<endl;


	// OJO: Recordatorio. El nombre del bot y de la clase deben coincidir.
	// En caso contrario, el bot no podrá participar en la competición.
	// Se deberá sustituir el nombre MiBot como nombre de la clase por otro
	// seleccionado por el alumno. Se deberá actualizar también el nombre
	// devuelto por el método getName() acordemente.


  bool im_first_player = turno == J1;

  GameNode node (state, M_NONE, im_first_player);

  // int firstguess = 0;
  // for (int d = 0; d < 30; d++){
  //   firstguess = MTDF <GameNode> (node, firstguess, d)._bound;
  //   cerr << firstguess << endl;
  // }

  movimiento = MTDF <GameNode> (node, 9, 10)._action;


	return movimiento;
}
