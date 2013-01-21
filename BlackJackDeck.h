// Written by Steve Coward
// PSU CS 202 Fall 2009 Project 1
//
// BlackJackDeck.h: interface of the CBlackJackDeck class.
//	Derived from CDeck (deck.h).  The CBlackJackDeck class attempts to
//  encapsulate the functionality of a collection of cards as it
//  would occur in a real game of BlackJack.  This class is not
//  intended to model just the one deck of cards used to play the game,
//  but will also be used to implement the dealer's hand and the
//  player's hand.
//  
//////////////////////////////////////////////////////////////////////

#if !defined(BLACKJACKDECK_H)
#define BLACKJACKDECK_H

#include "deck.h"

class CBlackJackDeck : public CDeck
{
public:
	// constructor
	CBlackJackDeck(void);
	CBlackJackDeck(ID3D10Device* pD3D10Device, bool bFull);

	// destructor
	virtual ~CBlackJackDeck(void) {}

	// Prints to std::cout a space separated list of cards in the deck.
	// This is the definition of the virtual function in the CDeck class.
	void displayDeck(void) const; // display the cards in the deck

	// Returns the value of the cards in the deck as defined by the game of BlackJack.
	// This is the definition of the virtual function in the CDeck class.
	int getDeckValue(void) const; // computes and returns the 'value' of the deck
};

#endif // !defined(BLACKJACKDECK_H)