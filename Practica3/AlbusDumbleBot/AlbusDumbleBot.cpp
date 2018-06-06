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

GameNode::action GameNode::get_action()
{
    return reach_this_node;
}

inline bool GameNode::is_terminal()
{
    return game.isFinalState();
}

int  GameNode::heuristic_to_use = 1; // Default heuristic

inline bound GameNode::get_heuristic_value()
{
    Player max_player, min_player;

    if (heuristic_to_use == 1) {
        max_player = J1;
        min_player = J2;
    } else {
        max_player = J2;
        min_player = J1;
    }

    array<int, 6> pits;

    pits[0] = game.getSeedsAt(max_player, P1);
    pits[1] = game.getSeedsAt(max_player, P2);
    pits[2] = game.getSeedsAt(max_player, P3);
    pits[3] = game.getSeedsAt(max_player, P4);
    pits[4] = game.getSeedsAt(max_player, P5);
    pits[5] = game.getSeedsAt(max_player, P6);

    int h1 = pits[0];

    int h2 = 0;

    for (auto it : pits) {
        h2 += it;
    }

    int h3 = 0;

    for (auto it : pits)
        if (it != 0) {
            h3++;
        }

    int h4 = game.getScore(max_player);

    int h5 = pits[5] == 0 ? 1 : 0;

    int h6 = game.getScore(min_player);

    return 0.198649 * h1 + 0.190084 * h2 + 0.370793 * h3 + h4 + 0.418841 * h5 -
           0.565937 * h6;

}

inline GameState& GameNode::get_game_state()
{
    return game;
}

inline const GameState& GameNode::get_game_state_const() const
{
    return game;
}

inline bool GameNode::is_max_node()
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

bool GameNode::operator< (GameNode& lhsn)
{
    return get_heuristic_value() < lhsn.get_heuristic_value();
}

void GameNode::get_children(array<GameNode, 6>& children)
{

    //This is unrolled because it's compiled without any efficiency.

    GameState gs = game.simulateMove ( (Move) 1);
    bool child_maximize = gs.getCurrentPlayer() == game.getCurrentPlayer()
                          ?  is_a_max_node
                          :  !is_a_max_node;
    GameNode child (gs, (Move) 1,  child_maximize);
    children[0] = child;

    gs = game.simulateMove ( (Move) 2);
    child_maximize = gs.getCurrentPlayer() == game.getCurrentPlayer()
                     ? is_a_max_node
                     : !is_a_max_node;
    child = GameNode (gs, (Move) 2,  child_maximize);
    children[1] = child;

    gs = game.simulateMove ( (Move) 3);
    child_maximize = gs.getCurrentPlayer() == game.getCurrentPlayer()
                     ? is_a_max_node
                     : !is_a_max_node;
    child = GameNode (gs, (Move) 3,  child_maximize);
    children[2] = child;

    gs = game.simulateMove ( (Move) 4);
    child_maximize = gs.getCurrentPlayer() == game.getCurrentPlayer()
                     ? is_a_max_node
                     : !is_a_max_node;
    child = GameNode (gs, (Move) 4,  child_maximize);
    children[3] = child;

    gs = game.simulateMove ( (Move) 5);
    child_maximize = gs.getCurrentPlayer() == game.getCurrentPlayer()
                     ? is_a_max_node
                     : !is_a_max_node;
    child = GameNode (gs, (Move) 5,  child_maximize);
    children[4] = child;

    gs = game.simulateMove ( (Move) 6);
    child_maximize = gs.getCurrentPlayer() == game.getCurrentPlayer()
                     ? is_a_max_node
                     : !is_a_max_node;
    child = GameNode (gs, (Move) 6,  child_maximize);
    children[5] = child;

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

    if (value_in_hash != table.end()
            && value_in_hash->second._depth > depth) { // Transposition table lookup

        if ( value_in_hash->second.lower_bound >= beta )
            return { value_in_hash->second.lower_bound, value_in_hash->second._action };

        if ( value_in_hash->second.upper_bound <= alpha )
            return { value_in_hash->second.upper_bound, value_in_hash->second._action };

        if (value_in_hash->second.lower_bound > alpha) {
            alpha = value_in_hash->second.lower_bound;
        }

        if (value_in_hash->second.upper_bound < beta) {
            beta = value_in_hash->second.upper_bound;
        }

    }

    bound_and_action<node> ret;

    if ( depth == 0 || root.is_terminal() ) { // Leaf Node

        return { root.get_heuristic_value(), root.get_action() };

    } else {

        array <node, 6> children = {root, root, root, root, root, root};
        root.get_children(children);

        if (value_in_hash != table.end()) {
            for (auto it = children.begin(); it != children.end(); ++it) {
                if (it->get_action() == value_in_hash->second._action) {
                    iter_swap(children.begin(), it);
                    break;

                }
            }
        }

        if (root.is_max_node()) {

            ret._bound = INT_MIN;
            bound a = alpha; //Save original alpha

            for (auto child = children.begin(); child != children.end(); ++child) {

                bound_and_action<node> possible_ret = alpha_beta_with_memory(*child, depth - 1,
                                                      a, beta, table);

                if (possible_ret._bound > ret._bound ) {
                    ret._bound = possible_ret._bound;
                    ret._action = child->get_action();
                }

                if (a < ret._bound) {
                    a = ret._bound;
                }

                if ( beta <= ret._bound ) {
                    break;    // beta cut-off
                }

            }

        } else { // if root is a min node.

            ret._bound = INT_MAX;
            bound b = beta; //Save original beta


            for (auto child = children.begin(); child != children.end(); ++child) {


                bound_and_action <node> possible_ret = alpha_beta_with_memory(*child, depth - 1,
                                                       alpha, b, table);

                if (possible_ret._bound < ret._bound) {
                    ret._bound = possible_ret._bound;
                    ret._action = child->get_action();
                }

                if ( b > ret._bound) {
                    b = ret._bound;
                }

                if ( ret._bound <= alpha ) {
                    break;    // alpha cut-off
                }

            }

        }
    }


    //
    //  ----- Transposition table storing of bounds.

    hash_struct& hash_value = table[root];

    if (hash_value._depth < depth) {
        // Fail low result implies an upper bound.
        if (ret._bound <= alpha) {
            hash_value.upper_bound = ret._bound;
        }

        // Found an accurate minimax value - will not occur if called with zero window.
        // Fail high result implies a lower bound.
        if (ret._bound >= beta ) {
            hash_value.lower_bound = ret._bound;
        }

        hash_value._depth = depth;
        hash_value._action = ret._action;
    }

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

    static std::unordered_map <node, hash_struct, hash_game> transposition_table;

    if (d == 1) {
        transposition_table.clear();
    }

    bound_and_action <node> ret;
    ret._bound = first;

    bound upper = INT_MAX;
    bound lower = INT_MIN;
    bound beta;

    do {
        if (ret._bound == lower) {
            beta = ret._bound + 1;
        } else {
            beta = ret._bound;
        }

        ret = alpha_beta_with_memory (root, d, beta - 1, beta, transposition_table);

        if (ret._bound < beta) {
            upper = ret._bound;
        } else {
            lower = ret._bound;
        }

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
    bound firstguess = 8;

    if (!im_first_player) {
        GameNode::heuristic_to_use = 2 ;
    }

    GameNode node (state, M_NONE, true);

    bound_and_action <GameNode> b_and_m;

    // Iterative Deeping

    for (depth it = 1; it < 20 ; it++) {

        b_and_m = MTDF (node, firstguess, it);

        firstguess = b_and_m._bound;

    }

    return b_and_m._action;
}
