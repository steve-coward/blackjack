//////////////////////////////////////////////////////////////////////
// Written by Steve Coward
// PSU CS 202 Fall 2009 Project 1
//
// card.h: interface of the CCard class.
//	The CCard class attempts to encapsulate the functionality of a
//  generic playing card.
//  
//  Contains the following functions:
//	
//public:
//	void setCard(Rank rank, Suit suit);
//		initialize the card to rank and suit
//
//	int translateCard(char* pRank, char* pSuit) const;
//		translate the card value to a string.
//		return the number of characters written to pRank, excluding
//		the terminating '\0'. Useful only for console applications.
//
//	int translateCard(Rank rank, Suit suit) const;
//		Return the index of this card into the graphics texture array.
//
//	HRESULT draw(ID3DX10Sprite * pSprite, D3DX10_SPRITE* pSpriteArray) const
//		This routine assumes that pSprite->Begin() called before and
//		pSprite->End() is called after. This routine fills one entry in the
//		pSpriteArray array.
//
//	Rank getRank(void) const;
//	void setRank(Rank rank);
//	Suit getSuit(void) const;
//	void setSuit(Suit suit);
//	bool getFaceUp(void) const;
//	void setFaceUp(bool bFaceUp);
//		These are simple member access functions.
//
//	const TCHAR* getFileName(void) const;
//	void setFileName(TCHAR* lpFileName);
//		The full path name to the front graphics file for this card.
//
//	ID3D10ShaderResourceView* getFrontTexture(void) const;
//	void setFrontTexture(ID3D10ShaderResourceView* pFrontTexture);
//		A pointer to the texture resource for the front of this card.
//
//	ID3D10ShaderResourceView* getBackTexture(void) const;
//	void setBackTexture(ID3D10ShaderResourceView* pBackTexture);
//		A pointer to the texture resource for the back of this card.
//
//	const POINT getLocation(void) const;
//	void setLocation(POINT & ptLocation);
//	void setLocation(int x, int y);
//		Screen location where the card is to be drawn.
//
//	int getSpriteWidth(void) const;
//	void setSpriteWidth(int spriteWidth);
//		Width of sprite in pixels.
//
//	int getSpriteLength(void) const;
//	void setSpriteLength(int spriteLength);
//		Length (Y direction) of sprite in pixels.
//	
//	inline CCard::Suit operator++( CCard::Suit &refSuit, int )
//		helper function to facilitate iterating through Suit enum values.
//
//	inline CCard::Rank operator++( CCard::Rank &refRank, int )
//		helper function to facilitate iterating through Rank enum values.
//
//private:
//	int getCharRank(char* pRank, int maxLength) const;
//		Write rank string (e.g. A, K, 10) to pRank, and 
//		return number of characters written.
//		Useful only for console applications.
//
//////////////////////////////////////////////////////////////////////

#if !defined(CARD_H)
#define CARD_H

// Direct3D10 includes
#include <dxgi.h>
#include <d3d10_1.h>
#include <d3d10.h>
#include <d3dx10.h>

class CCard
{
public:
	enum Suit { 
		// start at three because ascii symbol for hearts is 3
		Hearts = 3,
		Diamonds = 4, 
		Clubs = 5, 
		Spades = 6
	};
	enum Rank {
		Ace = 1,
		Two = 2,
		Three = 3,
		Four = 4,
		Five = 5,
		Six = 6,
		Seven = 7,
		Eight = 8,
		Nine = 9,
		Ten = 10,
		Jack = 11,
		Queen = 12,
		King = 13
	};

	// constructor
	CCard(void);

	// destructor
	virtual ~CCard(void);
	
	// member functions
	void setCard(Rank rank, Suit suit) { setRank(rank); setSuit(suit); }	// set values of card
	int translateCard(char* pRank, char* pSuit) const;
	int translateCard(Rank rank, Suit suit) const;
	HRESULT draw(ID3DX10Sprite * pSprite, D3DX10_SPRITE* pSpriteArray) const;

private:
	Rank m_rank;
	int getCharRank(char* pRank, int maxLength) const;
public:
	Rank getRank(void) const { return(m_rank); }
	void setRank(Rank rank) { m_rank = rank; }

private:
	Suit m_suit;
public:
	Suit getSuit(void) const { return(m_suit); }
	void setSuit(Suit suit) { m_suit = suit; }

private:
	bool m_bFaceUp;
public:
	bool getFaceUp(void) const { return(m_bFaceUp); }
	void setFaceUp(bool bFaceUp) { m_bFaceUp = bFaceUp; }

// full path to file containing graphics image for card
private:
	TCHAR m_lpFileName[255];
public:
	const TCHAR* getFileName(void) const { return(m_lpFileName); }
	void setFileName(TCHAR* lpFileName) { lstrcpy(m_lpFileName, lpFileName); m_lpFileName[254] = '\0'; }

private:
	ID3D10ShaderResourceView* m_pFrontTexture;
public:
	ID3D10ShaderResourceView* getFrontTexture(void) const { return(m_pFrontTexture); }
	void setFrontTexture(ID3D10ShaderResourceView* pFrontTexture) { m_pFrontTexture = pFrontTexture; }

private:
	ID3D10ShaderResourceView* m_pBackTexture;
public:
	ID3D10ShaderResourceView* getBackTexture(void) const { return(m_pBackTexture); }
	void setBackTexture(ID3D10ShaderResourceView* pBackTexture) { m_pBackTexture = pBackTexture; }

private:
	POINT m_ptLocation;
public:
	const POINT getLocation(void) const { return(m_ptLocation); }
	void setLocation(POINT & ptLocation) { m_ptLocation = ptLocation; }
	void setLocation(int x, int y) { m_ptLocation.x = x; m_ptLocation.y = y; }

private:
	int m_spriteWidth;
public:
	int getSpriteWidth(void) const { return(m_spriteWidth); }
	void setSpriteWidth(int spriteWidth) { m_spriteWidth = spriteWidth; }

private:
	int m_spriteLength;
public:
	int getSpriteLength(void) const { return(m_spriteLength); }
	void setSpriteLength(int spriteLength) { m_spriteLength = spriteLength; }
};

inline CCard::Suit operator++( CCard::Suit &refSuit, int )
{
	return refSuit = (CCard::Suit)(refSuit+1);
}

inline CCard::Rank operator++( CCard::Rank &refRank, int )
{
	return refRank = (CCard::Rank)(refRank+1);
}

#endif // !defined(CARD_H)