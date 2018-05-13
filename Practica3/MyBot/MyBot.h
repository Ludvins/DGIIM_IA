/*
 * MyBot.h
 *
 *  Created on: 07-05-2018
 *      Author: Ludvins
 */

#include "Bot.h"

#ifndef MANUPCBOT_H_
#define MANUPCBOT_H_

class MyBot:Bot {
public:
	MyBot();
	~MyBot();


	void initialize();
	string getName();
	Move nextMove(const vector<Move> &adversary, const GameState &state);
};

#endif /* MANUPCBOT_H_ */
