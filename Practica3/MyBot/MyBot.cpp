/*
 * MyBot.cpp
 *
 *  Created on: 07-05-2018
 *      Author: Ludvins
 */


#include "MyBot.h"
using namespace std;

bool debug = false;


// --------------------------------------------- GameState Equality Operator ----------------------------------



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

// --------------------------------------------------------------------------------------------------------
// -------------------------------------------- Node Class implementation ---------------------------------
// --------------------------------------------------------------------------------------------------------

// This class is needed for MTD-f and AlphaBeta with memory algorithms.

GameNode::GameNode(GameState gs, Move _reach_this_node, bool _maximize){
    game = gs;
    reach_this_node = _reach_this_node;
    maximize = _maximize;
}

bound
GameNode::get_heuristic_value(){
  return game.getScore(J1) - game.getScore(J2);
}

GameNode::action
GameNode::get_action(){
    return reach_this_node;
  }

  bool
  GameNode::is_terminal(){
    return game.isFinalState();
  }

  bool
  GameNode::is_max_node(){
    return maximize;
  }

  GameState&
  GameNode::get_game_state(){
    return game;
  }


list <GameNode>
GameNode::get_children(){

    list< GameNode > children;

    for(int id = 1; id <= 6; id++){

      GameState gs = game.simulateMove ( (Move) id);
      bool child_maximize = gs.getCurrentPlayer() == game.getCurrentPlayer() ? maximize : !maximize;
      GameNode child = GameNode (gs, (Move) id,  child_maximize);
      children.push_back( child );
    }

    return children;
  }

  ostream&
  operator<< (ostream& os, GameNode gn){
    os << "\tJ1: Granero="<< (int) gn.game.getSeedsAt(J1, GRANERO) << " C1=" << (int)gn.game.getSeedsAt(J1,P1) << " C2=" << (int)gn.game.getSeedsAt(J1,P2) << " C3=" << (int)gn.game.getSeedsAt(J1,P3) << " C4=" <<(int) gn.game.getSeedsAt(J1,P4) << " C5=" << (int) gn.game.getSeedsAt(J1,P5) << " C6=" <<(int) gn.game.getSeedsAt(J1,P6) << endl;
    os << "\tJ2: Granero="<< (int)gn.game.getSeedsAt(J2, GRANERO) << " C1=" << (int)gn.game.getSeedsAt(J2,P1) << " C2=" << (int)gn.game.getSeedsAt(J2,P2) << " C3=" << (int) gn.game.getSeedsAt(J2,P3) << " C4=" << (int)gn.game.getSeedsAt(J2,P4) << " C5=" << (int)gn.game.getSeedsAt(J2,P5) << " C6=" << (int)gn.game.getSeedsAt(J2,P6) << endl;
    return os;
  }

// -------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------- ALPHA BETA WITH MEMORY ---------------------------------------------
// -------------------------------------------------------------------------------------------------------------------

// This algorithm is used on a Node Class which encapsulates a game board. Several methos are needed in this Node Class.
//
// 1. get_heuristic_value()    Must return the heuristic value of that node (leaf node).
// 1. is_terminal()        Must return if the node is a leaf node.
// 3. get_action()         Must return the action that reached that node from its father.
// 4. get_children()       Must return a list of the child nodes of the current one.
// 5. is_max_node()        Must return if the node is a node to maximize o minimize (following a monimax algorithm).
// 6. get_game_state()     Must return the game the node is encapsulating.
//

// This algorithm algo needs a transposition table to be called over the game the node is encapsulating.
// So the key of the table is a game state and the return value must be a pair of lower and upper bounds.



  template <class node, class transposition_table>
  bound_and_action<node> alpha_beta_with_memory(node root, depth depth, bound alpha, bound beta, transposition_table& table){

    if (debug)
      for (auto it : table)
        cerr << it.second.lower << " " <<it.second.upper  << endl;
    if(debug)
      cerr << "[AlphaBeta]: Entra algoritmo,  con profundidad " << depth << " y maximize " << root.is_max_node() <<  endl;
    if(debug)
      cerr << root;

    auto value_in_hash = table.find(root.get_game_state());

    if (debug)
      cerr << "Nodo buscado" << endl;
    if(value_in_hash != table.end()){
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

        bound_and_action<node> possible_ret = alpha_beta_with_memory(child, depth - 1, a, beta, table);

        if (possible_ret._bound > ret._bound){
          ret._bound = possible_ret._bound;
          ret._action = child.get_action();
        }
        a = std::max(a, ret._bound);

      }

    } else { // if (min_node)
      ret._bound = INT_MAX;
      bound b = beta;

      for (auto child : children){

        if ( ret._bound <= alpha )
          break; // alpha cut-off

        bound_and_action <node> possible_ret = alpha_beta_with_memory(child, depth - 1, alpha, beta, table);

        if (possible_ret._bound < ret._bound) {
          ret._bound = possible_ret._bound;
          ret._action = child.get_action();
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

// ----------------------------------------------------------------------------------------------------------------
// ----------------------------------------------------- MTD-f ----------------------------------------------------
// ----------------------------------------------------------------------------------------------------------------

// This is the main algorithm of the program, it calls alpha_beta_with_memory several times so it gets the best
// minimax value.
// Memory is needed so the algorithm doesn't take too long (as it explores the same tree a few times on each call).
//

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

    ret = alpha_beta_with_memory(root, d, beta - 1, beta, table);

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

// ------------------------------------------------------------------------------------------------------------
// ----------------------------------------------- MyBot Methods ----------------------------------------------
// ------------------------------------------------------------------------------------------------------------


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
   //  for (int d = 0; d < 30; d++){
   //    firstguess = MTDF <GameNode> (node, firstguess, d, partition_table)._bound;
   //    cerr << firstguess << endl;
   // }

  bound_and_action <GameNode> b_and_m = MTDF (node, 8, 9, transposition_table);

  movimiento = b_and_m._action;

  cerr << "Movimiento: " << b_and_m._bound << " " <<b_and_m._action << endl;

	return movimiento;
}
