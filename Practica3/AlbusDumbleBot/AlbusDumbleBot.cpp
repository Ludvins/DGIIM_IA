/*
 * AlbusDumbleBot.cpp
 *
 *  Created on: 07-05-2018
 *      Author: Ludvins
 */


#include "AlbusDumbleBot.h"
using namespace std;



// --------------------------------------------------------------------------------------------------------
// -------------------------------------------- Node Class implementation ---------------------------------
// --------------------------------------------------------------------------------------------------------

// This class is needed for MTD-f and AlphaBeta with memory algorithms.

GameNode::GameNode(GameState gs, Move _reach_this_node, bool _maximize, bool _is_root_node )
{
    game = gs;
    reach_this_node = _reach_this_node;
    is_a_max_node = _maximize;
    is_a_root_node = _is_root_node;
}

GameNode::action GameNode::get_action()
{
    return reach_this_node;
}

bool GameNode::is_terminal()
{
    return game.isFinalState();
}

bool GameNode::is_root_node(){
  return is_a_root_node;
}

bool GameNode::is_better_than(GameNode& other){

  if (!other.is_a_max_node && is_a_max_node)
    return true;
  if (!is_a_max_node && other.is_a_max_node)
    return false;

  // int sumaa = game.getSeedsAt(J1, P1) + game.getSeedsAt(J1, P2) + game.getSeedsAt(J1, P3) + game.getSeedsAt(J1, P4) + game.getSeedsAt(J1, P5) + game.getSeedsAt(J1, P6) + game.getSeedsAt(J2, P1) + game.getSeedsAt(J2, P2) + game.getSeedsAt(J2, P3) + game.getSeedsAt(J2, P4) + game.getSeedsAt(J2, P5) + game.getSeedsAt(J2, P6);

  // GameState game2 = other.get_game_state();
  // int sumab = game2.getSeedsAt(J1, P1) + game2.getSeedsAt(J1, P2) + game2.getSeedsAt(J1, P3) + game2.getSeedsAt(J1, P4) + game2.getSeedsAt(J1, P5) + game2.getSeedsAt(J1, P6) + game2.getSeedsAt(J2, P1) + game2.getSeedsAt(J2, P2) + game2.getSeedsAt(J2, P3) + game2.getSeedsAt(J2, P4) + game2.getSeedsAt(J2, P5) + game2.getSeedsAt(J2, P6);

  //if (sumaa < sumab) return true;


  return false;

}

int  GameNode::heuristic_to_use = 1; // Default heuristic

bound GameNode::get_heuristic_value()
{

    switch (this->heuristic_to_use) {

    case 2:
      return game.getScore(J2) - game.getScore(J1);
      break;

    case 3:
      return 500/(49-game.getScore(J1)) - 500/(49-game.getScore(J2));
      break;

    case 4:
      return 1000/(49-game.getScore(J2)) - 1000/(49-game.getScore(J1));
      break;

    default:
      return game.getScore(J1) - game.getScore(J2);
      break;

    }

}

GameState& GameNode::get_game_state()
{
    return game;
}

const GameState& GameNode::get_game_state_const() const
{
    return game;
}

bool GameNode::is_max_node()
{
    return is_a_max_node;
}

bool GameNode::operator== (const GameNode& lhsn) const
{
    const GameState rhs = get_game_state_const();
    const GameState lhs = lhsn.get_game_state_const();
    return
        rhs.getSeedsAt(J1, GRANERO) == lhs.getSeedsAt(J1, GRANERO) &&
        rhs.getSeedsAt(J2, GRANERO) == lhs.getSeedsAt(J2, GRANERO) &&
        rhs.getSeedsAt(J1, P1) == lhs.getSeedsAt(J1, P1) &&
        rhs.getSeedsAt(J1, P2) == lhs.getSeedsAt(J1, P2) &&
        rhs.getSeedsAt(J1, P3) == lhs.getSeedsAt(J1, P3) &&
        rhs.getSeedsAt(J1, P4) == lhs.getSeedsAt(J1, P4) &&
        rhs.getSeedsAt(J1, P5) == lhs.getSeedsAt(J1, P5) &&
        rhs.getSeedsAt(J1, P6) == lhs.getSeedsAt(J1, P6) &&
        rhs.getSeedsAt(J2, P1) == lhs.getSeedsAt(J2, P1) &&
        rhs.getSeedsAt(J2, P2) == lhs.getSeedsAt(J2, P2) &&
        rhs.getSeedsAt(J2, P3) == lhs.getSeedsAt(J2, P3) &&
        rhs.getSeedsAt(J2, P4) == lhs.getSeedsAt(J2, P4) &&
        rhs.getSeedsAt(J2, P5) == lhs.getSeedsAt(J2, P5) &&
        rhs.getSeedsAt(J2, P6) == lhs.getSeedsAt(J2, P6) &&
        rhs.getCurrentPlayer() == lhs.getCurrentPlayer();
}

void GameNode::get_children(list<GameNode>& children)
{

    for (int id = 1; id <= 6; id++) {

        GameState gs = game.simulateMove ( (Move) id);
        bool child_maximize = gs.getCurrentPlayer() == game.getCurrentPlayer() ?
                              is_a_max_node : !is_a_max_node;
        GameNode child = GameNode (gs, (Move) id,  child_maximize);
        children.push_back( child );
    }

}

ostream& operator<< (ostream& os, GameNode gn)
{
    os << "\tJ1: Granero=" << (int) gn.game.getSeedsAt(J1, GRANERO) <<
       " C1=" << (int) gn.game.getSeedsAt(J1, P1) <<
       " C2=" << (int) gn.game.getSeedsAt(J1, P2) <<
       " C3=" << (int) gn.game.getSeedsAt(J1, P3) <<
       " C4=" << (int) gn.game.getSeedsAt(J1, P4) <<
       " C5=" << (int) gn.game.getSeedsAt(J1, P5) <<
       " C6=" << (int) gn.game.getSeedsAt(J1, P6) << endl;

    os << "\tJ2: Granero=" << (int) gn.game.getSeedsAt(J2, GRANERO) <<
       " C1=" << (int) gn.game.getSeedsAt(J2, P1) <<
       " C2=" << (int) gn.game.getSeedsAt(J2, P2) <<
       " C3=" << (int) gn.game.getSeedsAt(J2, P3) <<
       " C4=" << (int) gn.game.getSeedsAt(J2, P4) <<
       " C5=" << (int) gn.game.getSeedsAt(J2, P5) <<
       " C6=" << (int) gn.game.getSeedsAt(J2, P6) << endl;
    return os;
}

// ------------------------------------------------------------------------------------------------------------------
// -----------------------------------------  HASHING GAME NODE -----------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------

size_t hash_game::operator() (const GameNode& node) const
{
    const GameState state = node.get_game_state_const();
    string s;
    s.push_back((int) state.getSeedsAt(J1, GRANERO));
    s.push_back((int) state.getSeedsAt(J1, P1));
    s.push_back((int) state.getSeedsAt(J1, P2));
    s.push_back((int) state.getSeedsAt(J1, P3));
    s.push_back((int) state.getSeedsAt(J1, P4));
    s.push_back((int) state.getSeedsAt(J1, P5));
    s.push_back((int) state.getSeedsAt(J1, P6));
    s.push_back((int) state.getSeedsAt(J2, GRANERO));
    s.push_back((int) state.getSeedsAt(J2, P1));
    s.push_back((int) state.getSeedsAt(J2, P2));
    s.push_back((int) state.getSeedsAt(J2, P3));
    s.push_back((int) state.getSeedsAt(J2, P4));
    s.push_back((int) state.getSeedsAt(J2, P5));
    s.push_back((int) state.getSeedsAt(J2, P6));
    s.push_back((int) state.getCurrentPlayer());

    std::hash<std::string> hash_fn;

    return hash_fn(s);

}

// -------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------- ALPHA BETA WITH MEMORY ---------------------------------------------
// -------------------------------------------------------------------------------------------------------------------

// This algorithm is used on a Node Class which encapsulates a game board. Several methos are needed in this Node Class.
//
// 1. is_terminal()           Must return if the node is a leaf node.
// 2. get_heuristic_value()   Must return the heuristic value of the node (leaf node).
// 3. get_action()            Must return the action that reached that node from its father.
// 4. get_children()          Must take a list of nodes and push on it the child nodes of the current one.
// 5. is_max_node()           Must return if the node is a node to maximize o minimize (following a minimax algorithm).
// 6. get_game_state()        Must return the game the node is encapsulating.
//

// This algorithm algo needs a transposition table to be called over the game the node is encapsulating.
// So the key of the table is a game state and the return value must be a pair of lower and upper bounds.
//



template <class node, class transposition_table>
bound_and_action<node> alpha_beta_with_memory(node& root, depth depth,
        bound alpha, bound beta, transposition_table& table)
{

  auto value_in_hash = table.find(root);

    if (value_in_hash != table.end() && value_in_hash->second.d >= depth) { // Transposition table lookup

        auto bound_in_hash = value_in_hash->second;

        if (bound_in_hash.lower >= beta)
            return {bound_in_hash.lower, root.get_action()};

        if (bound_in_hash.upper <= alpha)
            return {bound_in_hash.upper, root.get_action()};

        alpha = std::max(alpha, bound_in_hash.lower);

        beta = std::min(beta, bound_in_hash.upper);
    }

    bound_and_action<node> ret;

    if ( depth == 0 || root.is_terminal() ) { // Leaf Node

        ret = { root.get_heuristic_value(), root.get_action()};

    } else {

        list<node> children;
        root.get_children(children);

        GameNode bestchild = *(children.begin());
        if (root.is_max_node()) {

            ret._bound = INT_MIN;
            bound a = alpha; //Save original alpha

            for (auto child : children) {

                if ( beta <= ret._bound ) {
                    break;    // beta cut-off
                }

                bound_and_action<node> possible_ret = alpha_beta_with_memory(child, depth - 1,
                                                      a, beta, table);

                if (possible_ret._bound > ret._bound || (possible_ret._bound == ret._bound && child.is_better_than(bestchild)) ) {
                    ret._bound = possible_ret._bound;
                    ret._action = child.get_action();
                    bestchild = child;
                }

                a = std::max(a, ret._bound);

            }

        } else { // if root is a min node.

            ret._bound = INT_MAX;
            bound b = beta; //Save original beta


            for (auto child : children) {

                if ( ret._bound <= alpha ) {
                    break;    // alpha cut-off
                }

                bound_and_action <node> possible_ret = alpha_beta_with_memory(child, depth - 1,
                                                       alpha, b, table);

                if (possible_ret._bound < ret._bound || (possible_ret._bound == ret._bound && child.is_better_than(bestchild))) {
                    ret._bound = possible_ret._bound;
                    ret._action = child.get_action();
                }

                b = std::min(b, ret._bound);

            }
        }
    }

    //
    //  ----- Transposition table storing of bounds.
    // Fail low result implies an upper bound.
    bounds_and_depth& ref = table[root];

    if (ret._bound <= alpha) {
      ref.upper = ret._bound;
    }

    // Found an accurate minimax value - this will not ocurr if called with zero window.
    if (ret._bound > alpha && ret._bound < beta) {
        ref.upper = ref.lower=  ret._bound;
    }

    // Fail high result implies a lower bound.
    if (ret._bound >= beta ) {
        ref.lower = ret._bound;
    }

    ref.d = depth;

    return ret;
}

// ----------------------------------------------------------------------------------------------------------------
// ----------------------------------------------------- MTD-f ----------------------------------------------------
// ----------------------------------------------------------------------------------------------------------------

// This is the main algorithm of the program, it calls alpha_beta_with_memory several times so it gets the best
// minimax value.
// Memory is needed so the algorithm doesn't take too long (as it explores the same tree a few times on each call).
//

template <class node>
bound_and_action <node> MTDF (node& root, bound first, depth d)
{

    std::unordered_map <node, bounds_and_depth, hash_game> transposition_table;
    bound_and_action <node> ret;
    ret._bound = first;

    bound upper = INT_MAX;
    bound lower = INT_MIN;
    bound beta;

    do {

        beta = (ret._bound == lower)
               ? ret._bound + 1
               : ret._bound;

        ret = alpha_beta_with_memory (root, d, beta - 1, beta, transposition_table);

        transposition_table.erase(root);
        //root = GameNode(root.get_game_state(), ret._action, true, true);

        ret._bound < beta
               ? upper = ret._bound
               : lower = ret._bound;

    } while (lower < upper);


    return ret;

}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------ ALBUSDUMBLEBOT METHODS ------------------------------------------
// ------------------------------------------------------------------------------------------------------------


AlbusDumbleBot::AlbusDumbleBot()
{
    // Inicializar las variables necesarias para ejecutar la partida
}

AlbusDumbleBot::~AlbusDumbleBot()
{
    // Liberar los recursos reservados (memoria, ficheros, etc.)
}

void AlbusDumbleBot::initialize()
{
    // Inicializar el bot antes de jugar una partida
}

string AlbusDumbleBot::getName()
{
    return "AlbusDumbleBot"; // Sustituir por el nombre del bot
}


Move AlbusDumbleBot::nextMove(const vector<Move>& adversary,
                              const GameState& state)
{

    Player current_player = this->getPlayer();
    long timeout = this->getTimeOut();

    bool im_first_player = current_player == J1;
    static bound firstguess = 8;
    static depth d = im_first_player
                     ? 15
                     : 14;

    if (!im_first_player) {
        GameNode::heuristic_to_use = 2 ;
    }


    static int cont = 0;
    cont++;
    cerr << cont << endl;
    if (cont == 1 && im_first_player)
      return (Move) 1;

    // if(im_first_player)
    //   GameNode::heuristic_to_use = 3;
    // else
    //   GameNode::heuristic_to_use = 4;


    chrono::high_resolution_clock::time_point begin =
        chrono::high_resolution_clock::now();

    Move next_move = M_NONE;

    GameNode node (state, M_NONE, true, true);

    bound_and_action <GameNode> b_and_m;

    chrono::high_resolution_clock::time_point end =
        chrono::high_resolution_clock::now();

    chrono::duration<double> time_span = chrono::duration_cast <chrono::duration<double>> (end - begin);



    // Iterative Deeping

    for (depth it = 1; time_span.count() < 1 && it < 50; it++){
      //cerr << "Profundidad: "<< it << " con tiempo: " << time_span.count() << endl;
      b_and_m = MTDF (node, firstguess, it);
      end = chrono::high_resolution_clock::now();
      time_span = chrono::duration_cast <chrono::duration<double>> (end - begin);
      //cerr << b_and_m._action << " " << time_span.count() << endl;
    }

    // FirstGuess

    // b_and_m._bound = 0;
    // for (int d = 0; d < 30; d++){
    //    b_and_m = MTDF <GameNode> (node, b_and_m._bound, d);
    //    cerr << b_and_m._bound << " " << b_and_m._action << endl;
    // }

    //b_and_m = MTDF (node, firstguess, d);

    end = chrono::high_resolution_clock::now();
    time_span = chrono::duration_cast<chrono::duration<double>>( end - begin);

    cerr << "Tiempo del movimiento: " << time_span.count() << endl;

    next_move = b_and_m._action;
    //firstguess = b_and_m._bound;

    return next_move;
}
