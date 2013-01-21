// Written by Steve Coward
// PSU CS 202 Fall 2009 Project 1
//
// BlackJackDeck.cpp: implementation of the CBlackJackDeck class.
//		
//////////////////////////////////////////////////////////////////////

#include <iostream> 
#include "DXUT.h"
#include "BlackJackDeck.h"

CBlackJackDeck::CBlackJackDeck()
{
	setDevice(NULL);
}

CBlackJackDeck::CBlackJackDeck(ID3D10Device* pD3D10Device, bool bFull)
{
	setDevice(pD3D10Device);
 	CDeck::CDeck(pD3D10Device, bFull);
}

//displays deck to console
void CBlackJackDeck::displayDeck(void) const
{
	const_deckIterator deckitr;
	char chCardRank[3];
	char chCardSuit[2];

	deckitr = getDeckIterator();

	while (deckitr != NULL) {
		deckitr->data->translateCard(chCardRank, chCardSuit);
		std::cout << chCardRank << chCardSuit << " ";
		deckitr = deckitr->next;
		//deckitr++;
	}

	std::cout << "\n" << std::flush;
}

int CBlackJackDeck::getDeckValue(void) const
{
	int number = 0;
	int aces = 0;

	const_deckIterator deckitr;

	deckitr = getDeckIterator();

	while (deckitr != NULL) {
		if (deckitr->data->getRank() == 1) {
			aces++;
			number += 11;
		}
		else if (deckitr->data->getRank() > 10) {
			number += 10;
		}
		else {
			number += deckitr->data->getRank();
		}

		deckitr = deckitr->next;
	}

	while (aces > 0) {
		if (number > 21) {
			number -= 10;
			aces--;
		}
		else {
			break;
		}
	}

	return(number);
}