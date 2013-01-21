//////////////////////////////////////////////////////////////////////
// Written by Steve Coward
// PSU CS 202 Fall 2009 Project 1
//
// deck.h: interface of the CDeck class.
//	The CDeck class attempts to encapsulate the functionality of a
//  generic collection of cards.  It is intended that specific game
//  implementations will use this class as a base class for any decks
//  of cards that the game requires.
//  
//  Contains the following functions:
//	
//public:
//	HRESULT draw(ID3DX10Sprite * pSprite, D3DX10_SPRITE* pSpriteArray,
//	  int x, int y, int xOffset, int yOffset)
//		This routine assumes that pSprite->Begin() called before and
//		pSprite->End() is called after. This routine fills up to 52 entries in the
//		pSpriteArray array.  The first card is drawn at (x, y).  Subsequent locations
//		are offset by (xOffset, yOffset).
//
//	void shuffle(void);
//		randomly shuffle the cards in the deck
//
//	virtual void displayDeck(void) const;
//		display the cards in the deck
//
//	virtual int getDeckValue(void) const;
//		computes and returns the 'value' of the deck
//
//	void resetDeck(bool bFull);
//		reorders the deck so cards are sequential and grouped by suit
//
//	CCard* DealCard(void);
//		removes and returns the card on the top of the deck
//
//	CCard* DealCard(int num);
//		removes and returns the num th card from the top of the deck
//
//	void AddCard(CCard* thiscard);
//		adds a card to the bottom of the deck
//
//	void EmptyHand(void);
//		throws away all cards in the deck
//
//	void ShowDeck(bool bShow);
//		Turns all cards in the deck either face up or face down,
//		depending on value of bShow
//
//	const_deckIterator getDeckIterator(void) const
//		returns an iterator that can be used to iterate sequentially
//		through each card in the deck, starting from the top of the deck
//		ISSUE: iterator value can not be incremented properly since the
//		ISSUE: operator ++ has not been overloaded.
//		ISSUE: Must iterate via deckitr = deckitr->next;
//
//private:
//	CCard* GetCard(int num) const;
//		returns a ptr to the num th card from the top of the deck.
//		The card is not removed from the deck
//
//	void SetCard(CCard thiscard, int num);
//		sets the num th card from the top of the deck to have same
//		suit and rank as thiscard.
//
//	virtual void CBlackJackDeck::displayDeck() const
//		Prints to std::cout a space separated list of cards in the deck.
//		This is the definition of the virtual function in the CDeck class.
//
//	virtual int CBlackJackDeck::getDeckValue() const
//		Returns the value of the cards in the deck as defined by the game of BlackJack.
//		This is the definition of the virtual function in the CDeck class.
//		
//////////////////////////////////////////////////////////////////////

#if !defined(DECK_H)
#define DECK_H

#include "linkedList.h"
#include "card.h"

// 52 cards plus one view of the back of the card
#define NUM_CARD_VIEWS 53

struct graphicRsrcX {
	int ID;
	TCHAR lpFileName[255];
	TCHAR lpCardDesc[25];
	ID3D10ShaderResourceView* pShaderResource;
};

typedef tnode<CCard*>* deckIterator;
typedef const tnode<CCard*>* const_deckIterator;

class CDeck
{
public:
	CDeck(); // default constructor
	CDeck(ID3D10Device* pD3D10Device, bool bFull); // constructor (load deck with values)
	virtual ~CDeck(void);
	void shuffle(void); // randomly shuffle the cards in the deck
	virtual void displayDeck(void) const;  // display the cards in the deck
	virtual int getDeckValue(void) const; // computes and returns the 'value' of the deck
	void resetDeck(bool bFull);
	CCard* DealCard(void);
	CCard* DealCard(int num);
	void AddCard(CCard* thiscard);
	void EmptyHand(void);
	void ShowDeck(bool bShow);
	int CountDeck(void);
	const_deckIterator getDeckIterator(void) const { return(m_deck.getFirstNode()); }
	HRESULT InitSprite(ID3D10Device* pD3D10Device, bool bFull);
	HRESULT draw(ID3DX10Sprite * pSprite, D3DX10_SPRITE* pSpriteArray, int x, int y, int xOffset, int yOffset) const;

private:
	CCard* GetCard(int num) const;
	void SetCard(CCard thiscard, int num);
	graphicRsrcX m_cardGraphics[NUM_CARD_VIEWS];

	CLinkedList<class CCard *> m_deck;

private:
	ID3D10Device* m_pD3D10Device;
public:
	ID3D10Device* getDevice(void) { return(m_pD3D10Device); }
	void setDevice(ID3D10Device* pD3D10Device) { m_pD3D10Device = pD3D10Device; }

private:
	POINT m_ptLocation;
public:
	const POINT getLocation(void) const { return(m_ptLocation); }
	//void setLocation(POINT & ptLocation) { m_ptLocation = ptLocation; }
	void setLocation(int x, int y) { m_ptLocation.x = x; m_ptLocation.y = y; }
};

#endif // !defined(DECK_H)