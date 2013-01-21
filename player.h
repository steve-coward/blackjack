//////////////////////////////////////////////////////////////////////
// Written by Steve Coward
// PSU CS 202 Fall 2009 Project 1
//
// player.h: interface of the CPlayer class.
//	The CPlayer class attempts to
//  encapsulate the functionality of one player in a real game of BlackJack.
//  Player state is very simple as it consists only of a name, and the
//	win/loss/tie history of the player.
//  The dealer is considered to be a player, but does not actually own
//	the deck from which cards are dealt.
//  
//  Contains the following functions:
//		Only simple get/set functions on the 4 member variables.
//
//////////////////////////////////////////////////////////////////////

#if !defined(PLAYER_H)
#define PLAYER_H

#include <iostream> 

class CPlayer
{
public:
	enum PlayerConstants {
		MaxNameChars = 12,
	};

	CPlayer(void);
	virtual ~CPlayer(void);

private:
	char m_name[MaxNameChars];
public:
	void setName(char* name) { strcpy(m_name, name); m_name[MaxNameChars-1] = '\0';}
	const char* getName(void) const { return(m_name); }

	

private:
	int m_wins;
public:
	void setWins(int wins) { m_wins = wins; }
	void incWins(void) { m_wins++; }
	int getWins(void) const { return(m_wins); }

private:
	int m_losses;
public:
	void setLosses(int losses) { m_losses = losses; }
	void incLosses(void) { m_losses++; }
	int getLosses(void) const { return(m_losses); }

private:
	int m_ties;
public:
	void setTies(int ties) { m_ties = ties; }
	void incTies(void) { m_ties++; }
	int getTies(void) const { return(m_ties); }
};

#endif // !defined(PLAYER_H)