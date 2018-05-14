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

bool operator== (const GameState& rhs, const GameState& lhs) {
  return rhs.getSeedsAt(J1, GRANERO) == lhs.getSeedsAt(J1, GRANERO) &&
    rhs.getSeedsAt(J1, P1) == lhs.getSeedsAt(J1, P1) &&
    rhs.getSeedsAt(J1, P2) == lhs.getSeedsAt(J1, P2) &&
    rhs.getSeedsAt(J1, P3) == lhs.getSeedsAt(J1, P3) &&
    rhs.getSeedsAt(J1, P4) == lhs.getSeedsAt(J1, P4) &&
    rhs.getSeedsAt(J1, P5) == lhs.getSeedsAt(J1, P5) &&
    rhs.getSeedsAt(J1, P6) == lhs.getSeedsAt(J1, P6) &&
    rhs.getSeedsAt(J2, GRANERO) == lhs.getSeedsAt(J2, GRANERO) &&
    rhs.getSeedsAt(J2, P1) == lhs.getSeedsAt(J2, P1) &&
    rhs.getSeedsAt(J2, P2) == lhs.getSeedsAt(J2, P2) &&
    rhs.getSeedsAt(J2, P3) == lhs.getSeedsAt(J2, P3) &&
    rhs.getSeedsAt(J2, P4) == lhs.getSeedsAt(J2, P4) &&
    rhs.getSeedsAt(J2, P5) == lhs.getSeedsAt(J2, P5) &&
    rhs.getSeedsAt(J2, P6) == lhs.getSeedsAt(J2, P6);
}

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
  bool is_root;

public:

  using action = Move;

  GameNode(GameState gs, Move _reach_this_node, bool _maximize, bool _is_root){
    game = gs;
    reach_this_node = _reach_this_node;
    maximize = _maximize;
    is_root = _is_root;
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

  GameState&
  get_game_state(){
    return game;
  }

  bool
  is_root_node(){
    return is_root;
  }
  list < pair<GameNode,action> >
  get_children(){

    list< pair<GameNode, action> > children;

    for(int id = 1; id <= 6; id++){

      GameState gs = game.simulateMove ( (Move) id);
      bool child_maximize = gs.getCurrentPlayer() == game.getCurrentPlayer() ? maximize : !maximize;
      GameNode child = GameNode (gs, (Move) id,  child_maximize, false);
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


  template <class node, class transposition_table>
  bound_and_action<node> alpha_beta_with_memory(node root, depth depth, bound alpha, bound beta, transposition_table& table){

    //for (auto it : table){
    //  cerr << it.second << endl;
    //}
    if(debug)
      cerr << "[AlphaBeta]: Entra algoritmo,  con profundidad " << depth << " y maximize " << root.is_max_node() <<  endl;
    if(debug)
      cerr << root;

    //cerr << root.is_root_node()<< endl;
    auto bound_in_hash = table.find(root.get_game_state());
    if(!root.is_root_node() && bound_in_hash != table.end()){
      if(debug)
        cerr << "Nodo encontrado en tabla " << bound_in_hash->second << endl;
      return {bound_in_hash->second, root.get_action()};
    }

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
        bound_and_action<node> possible_ret = alpha_beta_with_memory (child.first, depth - 1, alpha, beta, table);

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
        bound_and_action <node> possible_ret = alpha_beta_with_memory (child.first, depth - 1, alpha, beta, table);

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
    table[root.get_game_state()] = ret._bound;
    return ret;
  }

  template <class node, class transposition_table>
  bound_and_action<node> alpha_beta_with_memory2(node root, depth depth, bound alpha, bound beta, transposition_table& table){

    //for (auto it : table){
    //  cerr << it.second << endl;
    //}
    if(debug)
      cerr << "[AlphaBeta]: Entra algoritmo,  con profundidad " << depth << " y maximize " << root.is_max_node() <<  endl;
    if(debug)
      cerr << root;

    //cerr << root.is_root_node()<< endl;
    auto value_in_hash = table.find(root.get_game_state());
    if (debug)
      cerr << "Nodo buscado" << endl;
    if(!root.is_root_node() && value_in_hash != table.end()){
      if (debug)
        cerr << "Nodo encontrado" << endl;
      auto bound_in_hash = value_in_hash->second;
      if(debug)
        cerr << "Nodo encontrado en tabla " << bound_in_hash.lower << " " << bound_in_hash.upper << endl;

      if (bound_in_hash.lower >= beta)
        return {bound_in_hash.lower, root.get_action()};
      if (bound_in_hash.upper <= alpha)
        return {bound_in_hash.upper, root.get_action()};

      alpha = std::max(alpha,bound_in_hash.lower);
      beta = std::min(beta,bound_in_hash.upper);
    }

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
      bound a = alpha; //Save original alpha

      for (auto child : children){

        if ( beta <= ret._bound )
          break; // beta cut-off

        bound_and_action<node> possible_ret = alpha_beta_with_memory2 (child.first, depth - 1, a, beta, table);

        if (possible_ret._bound > ret._bound){
          ret._bound = possible_ret._bound;
          ret._action = child.first.get_action();
        }
        a = std::max(a, ret._bound);

      }

    } else { // if (min_node)
      ret._bound = INT_MAX;
      bound b = beta;

      for (auto child : children){

        if ( ret._bound <= alpha )
          break; // alpha cut-off

        bound_and_action <node> possible_ret = alpha_beta_with_memory2 (child.first, depth - 1, alpha, beta, table);

        if (possible_ret._bound < ret._bound) {
          ret._bound = possible_ret._bound;
          ret._action = child.first.get_action();
        }

        b = std::min(b, ret._bound);

      }
    }

    if (ret._bound <= alpha) table[root.get_game_state()].upper = ret._bound;

    if (ret._bound > alpha && ret._bound < beta) table[root.get_game_state()] = {ret._bound, ret._bound};

    if (ret._bound >= beta ) table[root.get_game_state()].lower = ret._bound;

    if (debug)
      cerr << "[AlphaBeta]: Devuelve Valor: " << ret._bound << " " << ret._action << endl;
    return ret;
  }


template <class node, class transposition_table>
bound_and_action <node> MTDF (node root, bound first, depth d, transposition_table& table){

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

    beta = (ret._bound == lower) ? ret._bound + 1 : ret._bound;

    ret = alpha_beta_with_memory2(root, d, beta - 1, beta, table);
    //ret = alpha_beta( root, d, beta - 1, beta);

    ret._bound < beta ? upper = ret._bound : lower = ret._bound;
    cont ++;
    if (debug)
      cerr << "[MTDF]: Aproximación actual " << ret._bound << " " << ret._action << endl;

  } while (lower < upper);

  table.clear();
  if (debug)
    cerr << "[MTDF]: Bound and Action: " << ret._bound << " " << ret._action << endl;
  return ret;

}

// int get_hash_value(const GameState& state){
//   ostringstream oss;
//   int c;
//   oss << (int)state.getSeedsAt(J1, GRANERO) << (int)state.getSeedsAt(J1, P1) << (int)state.getSeedsAt(J1, P2) << (int)state.getSeedsAt(J1, P3) << (int)state.getSeedsAt(J1, P4) << (int)state.getSeedsAt(J1, P5) << (int)state.getSeedsAt(J1, P6) << (int)state.getSeedsAt(J2, GRANERO) << (int)state.getSeedsAt(J2, P1) << (int)state.getSeedsAt(J2, P2) << (int)state.getSeedsAt(J2, P3) << (int)state.getSeedsAt(J2, P4) << (int)state.getSeedsAt(J2, P5) << (int)state.getSeedsAt(J2,P6);

//   istringstream iss(oss.str());
//   iss >> c;
//   cerr << c << endl;
//   return c;
// }


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
  bound b = im_first_player ? 7 : 8;
  GameNode node (state, M_NONE, im_first_player, true);

   // int firstguess = 0;
   //  for (int d = 0; d < 30; d++){
   //    firstguess = MTDF <GameNode> (node, firstguess, d, partition_table)._bound;
   //    cerr << firstguess << endl;
   // }

  bound_and_action <GameNode> b_and_m = MTDF (node, 8, 9, partition_table);

  movimiento = b_and_m._action;

  cerr << "Movimiento: " << b_and_m._bound << " " <<b_and_m._action << endl;

	return movimiento;
}
