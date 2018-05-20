
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

#ifndef ALBUSDUMBLEBOT_H_
#define ALBUSDUMBLEBOT_H_


using depth = int;
using bound = double;

class GameNode
{
    GameState game;
    Move reach_this_node;
    bool is_a_max_node;
    bool is_a_root_node;

  public:
    using action = Move;
    static int heuristic_to_use;

    GameNode (GameState gs, Move _reach_this_node, bool _maximize, bool _is_root_node = false);
    bound get_heuristic_value();
    action get_action();
    bool is_terminal();
  bool is_root_node();
    bool is_max_node();
    GameState& get_game_state() ;
    const GameState& get_game_state_const() const;

    bool operator== (const GameNode& lhsn) const;
    void get_children(list<GameNode>& children);
    friend ostream& operator<< (ostream& os, GameNode gn);

};

template <class node>
struct bound_and_action {

    bound _bound;
    typename node::action _action;

    bool operator< (bound_and_action lhs) const
    {
        return _bound < lhs._bound;
    }
};


class hash_game
{
  public:
    size_t operator() (const GameNode& node) const;
};

struct lower_and_upper_bound {
    bound lower = INT_MIN;
    bound upper = INT_MAX;
};

class AlbusDumbleBot: Bot
{

  public:
    AlbusDumbleBot();
    ~AlbusDumbleBot();


    void initialize();
    string getName();
    Move nextMove(const vector<Move>& adversary, const GameState& state);
};

#endif /* ALBUSDUMBLEBOT_H_ */
