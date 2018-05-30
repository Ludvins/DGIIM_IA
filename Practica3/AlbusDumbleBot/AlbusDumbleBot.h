
/*
 * AlbusDumbleBot.h
 *
 *  Created on: 07-05-2018
 *      Author: Ludvins
 */

#include "Bot.h"
#include "unordered_map"
#include <iostream>
#include <sstream>
#include <string>
#include <limits.h>
#include <cstdlib>
#include <cmath>
#include <list>
#include <chrono>
#include <fstream>
#include <algorithm>

#ifndef ALBUSDUMBLEBOT_H_
#define ALBUSDUMBLEBOT_H_

int main()                                                                        __attribute__((optimize("-O2")));

using depth = int;
using bound = double;

class GameNode
{
    GameState game;
    Move reach_this_node;
    bool is_a_max_node;

  public:
    using action = Move;
    static int heuristic_to_use;

    GameNode (GameState gs, Move _reach_this_node, bool _maximize) : game(gs),
        reach_this_node(_reach_this_node), is_a_max_node(_maximize) {};

    bound get_heuristic_value()                                                   __attribute__((optimize("-O2")));

    inline action get_action()                                                    __attribute__((optimize("-O2")));
    inline bool is_terminal()                                                     __attribute__((optimize("-O2")));
    inline bool is_max_node()                                                     __attribute__((optimize("-O2")));
    inline int get_stones()
    {
        return 48 - game.getScore(J1) - game.getScore(J2);
    }
    bool is_better_than(GameNode& other)                                          __attribute__((optimize("-O2")));
    inline GameState& get_game_state()                                            __attribute__((optimize("-O2")));
    inline const GameState& get_game_state_const() const                          __attribute__((optimize("-O2")));
    bool operator== (const GameNode& lhsn) const                                  __attribute__((optimize("-O2")));
    bool operator< ( GameNode& lhsn)                                              __attribute__((optimize("-O2")));
    void get_children(array<GameNode, 6>& children)                               __attribute__((optimize("-O2")));
    friend ostream& operator<< (ostream& os, GameNode gn);

};

template <class node>
struct bound_and_action {

    bound _bound;
    typename node::action _action;

    bool operator< (bound_and_action lhs) const                                    __attribute__((optimize("-O2")))
    {
        return _bound < lhs._bound;
    }
};


class hash_game
{
  public:
    size_t operator() (const GameNode& node) const                                __attribute__((optimize("-O2")));
};

struct hash_struct {

    bound lower_bound = INT_MIN;
    bound upper_bound = INT_MAX;
    depth _depth = INT_MIN;
    GameNode::action _action;
};

class AlbusDumbleBot: Bot
{

  public:
    AlbusDumbleBot();
    ~AlbusDumbleBot();


    void initialize();
    string getName();
    Move nextMove(const vector<Move>& adversary,
                  const GameState& state)                                       __attribute__((optimize("-O2")));
};

template <class node, class transposition_table>
bound_and_action<node> alpha_beta_with_memory(node& root, depth depth,
        bound alpha, bound beta,
        transposition_table& table)                                             __attribute__((optimize("-O2")));

template <class node>
bound_and_action <node> MTDF (node& root, bound first,
                              depth d)                                          __attribute__((optimize("-O2")));


#endif /* ALBUSDUMBLEBOT_H_ */
