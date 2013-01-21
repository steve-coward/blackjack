// Written by Steve Coward
// PSU CS 202 Fall 2009 Project 1
//
// card.cpp: implementation of the CCard class.
//		
//////////////////////////////////////////////////////////////////////

#include "DXUT.h"
#include "card.h"
#include <iostream>

// constructor
// initialize card to a reasonable value.
// the card is initially not visible to any player at the table.
CCard::CCard(void)
{
	setFrontTexture(NULL);
	setBackTexture(NULL);
	setRank(Ace);
	setSuit(Hearts);
	// for debug - set to true
	setFaceUp(false);
	setSpriteLength(0);
	setSpriteWidth(0);
	setLocation(0, 0);
	setFileName(L"");

	return;
}


// destructor
CCard::~CCard(void)
{
	// CCard does not release texture since it may be shared by other
	// other cards in this deck or other decks.

	return;
}


int CCard::translateCard(Rank rank, Suit suit) const
{
	int index;

	// Entry 0 refers to back face of the card
	// Ace of Hearts = 1, King of Spades = 52
	index = (suit - CCard::Hearts) * CCard::King + rank;
	return(index);
}

// translate the card value to a string.
// return the number of characters written to pRank, excluding
// the terminating '\0'.
int CCard::translateCard(char* pRank, char* pSuit) const
{
	int numChars;

	if (getFaceUp()) {
		// card is visible so print rank and suit
		numChars = sprintf(pSuit, "%c", (char)getSuit());
		pSuit[numChars] = '\0';

		numChars = getCharRank(pRank, 3);
		pRank[numChars] = '\0';
	}
	else {
		// card is not visible so display as ###
		strcpy(pSuit, "#");
		numChars = 1;
		pSuit[numChars] = '\0';

		strcpy(pRank, "##");
		numChars = 2;
		pRank[numChars] = '\0';
	}

	return(numChars);
}

// translate the numeric rank of a card to a meaningful symbol.
// return the number of characters written to pRank, excluding
// the terminating '\0'.
int CCard::getCharRank(char* pRank, int maxLength) const
{
	// assume no characters are written to pRank
	int numChars = 0;

	if ((getRank() >= Ace) && (getRank() <= King)) {
		// rank is in range of Rank enum, proceed with translation

		// most ranks require a single character
		numChars = 1;

		switch(getRank())
		{
		case Ace:
			strcpy(pRank, "A");
			break;
		case Ten:
			strcpy(pRank, "10");
			// this is only rank that requires 2 characters
			numChars = 2;
			break;
		case Jack:
			strcpy(pRank, "J");
			break;
		case Queen:
			strcpy(pRank, "Q");
			break;
		case King:
			strcpy(pRank, "K");
			break;
		default:
			// ranks 2 through 9
			numChars = sprintf(pRank, "%d", getRank());
			break;
		}
	}

	pRank[numChars] = '\0';

	return(numChars);
}

// This draw routine assumes that pSprite->Begin() called before and
// pSprite->End() is called after.
HRESULT CCard::draw(ID3DX10Sprite * pSprite, D3DX10_SPRITE* pSpriteArray) const
{
	HRESULT hResult = S_OK;
	D3DXMATRIXA16 matScaling;
	D3DXMATRIXA16 matTranslation;

	// Add one to width since right border of card is not displaying
	D3DXMatrixScaling( &matScaling, (float)getSpriteWidth()+1, (float)getSpriteLength(), 1.0f );
	// (0, 0) is lower left corner
	D3DXMatrixTranslation( &matTranslation, (float)getLocation().x, (float)getLocation().y, 0.1f);

	pSpriteArray->matWorld = matScaling * matTranslation;
	pSpriteArray->TexCoord.x = 0.0f;
	pSpriteArray->TexCoord.y = 0.0f;
	pSpriteArray->TexSize.x = 1.0f;
	pSpriteArray->TexSize.y = 1.0f;
	pSpriteArray->ColorModulate = D3DXCOLOR(1.0f,1.0f,1.0f,1.0f);

	if (getFaceUp()) {
		pSpriteArray->pTexture = getFrontTexture();
	}
	else {
		pSpriteArray->pTexture = getBackTexture();
	}
	pSpriteArray->TextureIndex = 0;

	return(hResult);
}
