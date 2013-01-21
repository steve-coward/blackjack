// Written by Steve Coward
// PSU CS 202 Fall 2009 Project 1
//
// BlackJackPlayer.cpp: implementation of the CBlackJackPlayer class.
//		
//////////////////////////////////////////////////////////////////////

#include "DXUT.h"
#include "BlackJackPlayer.h" 

// constructor
// Ensure that when a player first sits at the BlackJack table he holds no cards.
// 
CBlackJackPlayer::CBlackJackPlayer(void)
{
	getHand()->EmptyHand();
}

// destructor
// Ensure that when a player leaves the BlackJack table he holds no cards.
// 
CBlackJackPlayer::~CBlackJackPlayer(void)
{
	getHand()->EmptyHand();
}