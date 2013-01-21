// Written by Steve Coward
// PSU CS 202 Fall 2009 Project 1
//
// BlackJackPlayer.h: interface of the CBlackJackPlayer class.
//	Derived from CPlayer (player.h).  The CBlackJackPlayer class attempts to
//  encapsulate the functionality of a player as it
//  would occur in a real game of BlackJack. For this simple implementation
//  of the simple game of BlackJack, the only addition to the base class
//  is a set of cards (CBlackJackDeck m_Hand) treated as a BlackJack hand.
//  
//////////////////////////////////////////////////////////////////////

#if !defined(BLACKJACKPLAYER_H)
#define BLACKJACKPLAYER_H

#include "player.h"
#include "BlackJackDeck.h"
#include "BlackJackPlayer.h"

class CBlackJackPlayer : public CPlayer
{
public:
	// constructor
	CBlackJackPlayer(void);

	// destructor
	virtual ~CBlackJackPlayer(void);

	// inline access to player's hand - the set of cards this player currently holds
	CBlackJackDeck* getHand(void) { return(&m_Hand); }
	CBlackJackDeck* viewHand(void) { return(&m_Hand); }

private:
	CBlackJackDeck m_Hand;
};

#endif // !defined(BLACKJACKPLAYER_H)