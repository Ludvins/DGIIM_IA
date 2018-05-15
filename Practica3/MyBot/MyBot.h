/*
 * MyBot.h
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

#ifndef MANUPCBOT_H_
#define MANUPCBOT_H_


using depth = int;
using bound = int;


class GameNode {
  GameState game;
  Move reach_this_node;
  bool maximize;
  bool is_root;

 public:
  using action = Move;

  GameNode (GameState gs, Move _reach_this_node, bool _maximize);
  bound get_heuristic_value();
  action get_action();
  bool is_terminal();
  bool is_max_node();
  GameState& get_game_state();
  list <GameNode> get_children();
  friend ostream& operator<< (ostream& os, GameNode gn);

};

template <class node>
struct bound_and_action {

  bound _bound;
  typename node::action _action;

  bool operator< (bound_and_action lhs) const {
    return _bound < lhs._bound;
  }
};


class hash_game_state {
 public:
  size_t operator() (const GameState& state) const {
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

    std::hash<std::string> hash_fn;

    return hash_fn(s);
  }
};

struct lower_and_upper_bound{
  int lower = INT_MIN;
  int upper = INT_MAX;
};

class MyBot:Bot {

  std::unordered_map <GameState, lower_and_upper_bound, hash_game_state> transposition_table;

 public:
	MyBot();
	~MyBot();


	void initialize();
	string getName();
	Move nextMove(const vector<Move> &adversary, const GameState &state);
};

#endif /* MANUPCBOT_H_ */
