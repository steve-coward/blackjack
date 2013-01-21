// Written by Steve Coward
// PSU CS 202 Fall 2009 Project 1
//
// player.cpp: implementation of the CPlayer class.
//		
//////////////////////////////////////////////////////////////////////

#include <iostream> 
#include "DXUT.h"
#include "player.h"

// constructor
CPlayer::CPlayer(void)
{
	m_name[0] = '\0';
	setWins(0);
	setLosses(0);
	setTies(0);
}

// destructor
CPlayer::~CPlayer(void)
{
}