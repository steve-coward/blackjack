// Written by Steve Coward
// PSU CS 202 Fall 2009 Project 1
//
// deck.cpp: implementation of the CDeck class.
//		
//////////////////////////////////////////////////////////////////////

#include <iostream> 
#include <stdlib.h>		// srand & rand
#include <time.h>		// system time
#include "DXUT.h"
#include "deck.h"

// constructor
CDeck::CDeck(void)
{
	m_pD3D10Device = NULL;

	int i;
	for (i=0;i<NUM_CARD_VIEWS;i++) {
		m_cardGraphics[i].pShaderResource = NULL;
		m_cardGraphics[i].lpFileName[0] = '\0';
		m_cardGraphics[i].lpCardDesc[0] = '\0';
		m_cardGraphics[i].ID = 0;
	}

	resetDeck(false);

#if defined(_DEBUG)
	////////////////////////////////
	// Testing
	// Bang on the list swap functions
	////////////////////////////////
	{
		bool bTestResult = true;
		//bTestResult = m_deck.testDriveSwap();
		if (!bTestResult) {
			std::cout << "ERROR in testDriveSwap()!!\n" << std::flush;
		}
	}
#endif // _DEBUG

	return;
}

CDeck::CDeck(ID3D10Device* pD3D10Device, bool bFull)
{
	int i;

	setDevice(pD3D10Device);

	for (i=0;i<NUM_CARD_VIEWS;i++) {
		m_cardGraphics[i].pShaderResource = NULL;
		m_cardGraphics[i].lpFileName[0] = '\0';
		m_cardGraphics[i].lpCardDesc[0] = '\0';
		m_cardGraphics[i].ID = 0;
	}

	resetDeck(bFull);

#if defined(_DEBUG)
	////////////////////////////////
	// Testing
	// Bang on the list swap functions
	////////////////////////////////
	{
		bool bTestResult = true;
		//bTestResult = m_deck.testDriveSwap();
		if (!bTestResult) {
			std::cout << "ERROR in testDriveSwap()!!\n" << std::flush;
		}
	}
#endif // _DEBUG

	InitSprite(getDevice(), bFull);

	return;
}

// destructor
CDeck::~CDeck(void)
{
	int i;
	// Release graphic texture for each card
	for (i=0;i<NUM_CARD_VIEWS;i++) {
		if (m_cardGraphics[i].pShaderResource != NULL) {
			m_cardGraphics[i].pShaderResource->Release();
			m_cardGraphics[i].pShaderResource = NULL;
		}
	}

	EmptyHand();

	return;
}

// assumes that deck is full of cards
// if bFull, create a full deck, otherwise create an empty deck
// empty decks get cards from other non empty decks
HRESULT CDeck::InitSprite(ID3D10Device* pD3D10Device, bool bFull)
{
	HRESULT hr = S_OK;
	D3DX10_IMAGE_INFO InfoFromFile;
	D3DX10_IMAGE_LOAD_INFO LoadImageInfo;
	const_deckIterator deckitr;
	int i;
	DWORD nDirectoryLength = 255;
	TCHAR lpCurrentDir[255];

	if (bFull) {
		// index 0 refers to back side of card
		LPTSTR lpCardName[NUM_CARD_VIEWS] = { L"b1fv",
			L"h1", L"h2", L"h3", L"h4", L"h5", L"h6", L"h7", L"h8", L"h9", L"h10", L"hj", L"hq", L"hk",
			L"d1", L"d2", L"d3", L"d4", L"d5", L"d6", L"d7", L"d8", L"d9", L"d10", L"dj", L"dq", L"dk",
			L"c1", L"c2", L"c3", L"c4", L"c5", L"c6", L"c7", L"c8", L"c9", L"c10", L"cj", L"cq", L"ck",
			L"s1", L"s2", L"s3", L"s4", L"s5", L"s6", L"s7", L"s8", L"s9", L"s10", L"sj", L"sq", L"sk",
		};

		// set up file path
		GetCurrentDirectory(nDirectoryLength, lpCurrentDir);
		lstrcat(lpCurrentDir, (LPCTSTR)L"\\Graphics\\");

		deckitr = getDeckIterator();
		for (i=0;i<NUM_CARD_VIEWS;i++) {
			if (m_cardGraphics[i].pShaderResource == NULL) {
				m_cardGraphics[i].ID = i;
				m_cardGraphics[i].lpCardDesc[0] = '\0';
				lstrcpy((LPWSTR)m_cardGraphics[i].lpFileName, lpCurrentDir);
				lstrcat((LPWSTR)m_cardGraphics[i].lpFileName, lpCardName[i]);
				lstrcat((LPWSTR)m_cardGraphics[i].lpFileName, L".png");
				hr = D3DX10GetImageInfoFromFile((LPWSTR)m_cardGraphics[i].lpFileName, NULL, &InfoFromFile, NULL);
				if (FAILED(hr)) {
					return -1;
				}

				LoadImageInfo.Width = InfoFromFile.Width;
				LoadImageInfo.Height = InfoFromFile.Height;
				LoadImageInfo.Depth = InfoFromFile.Depth;
				LoadImageInfo.FirstMipLevel = 1;
				LoadImageInfo.MipLevels = InfoFromFile.MipLevels;
				LoadImageInfo.Usage = D3D10_USAGE_DEFAULT;
				LoadImageInfo.BindFlags = D3D10_BIND_SHADER_RESOURCE;
				LoadImageInfo.CpuAccessFlags = 0;
				LoadImageInfo.MiscFlags = 0;
				LoadImageInfo.Format = InfoFromFile.Format;
				LoadImageInfo.Filter = D3DX10_FILTER_NONE;
				LoadImageInfo.MipFilter = D3DX10_FILTER_NONE;
				LoadImageInfo.pSrcInfo = &InfoFromFile;

				hr = D3DX10CreateShaderResourceViewFromFile(pD3D10Device, (LPWSTR)m_cardGraphics[i].lpFileName, &LoadImageInfo, NULL, &(m_cardGraphics[i].pShaderResource), NULL);
				if (FAILED(hr)) {
					return hr;
				}

				// now assign textures to each card.
				// assume that deck has not been shuffled
				// and is in same order as lpCardName array.
				if ((deckitr != NULL ) && (i >= 1)) {
					POINT pt;
					pt.x = 100;
					pt.y = 100;
					deckitr->data->setFrontTexture(m_cardGraphics[i].pShaderResource);
					deckitr->data->setBackTexture(m_cardGraphics[0].pShaderResource);
					deckitr->data->setSpriteWidth(InfoFromFile.Width);
					deckitr->data->setSpriteLength(InfoFromFile.Height);
					deckitr->data->setFileName(m_cardGraphics[i].lpFileName);
					deckitr->data->setLocation(pt);
					deckitr = deckitr->next;
				}
			}
			else {
				if ((deckitr != NULL ) && (i >= 1)) {
					deckitr->data->setFrontTexture(m_cardGraphics[i].pShaderResource);
					deckitr->data->setBackTexture(m_cardGraphics[0].pShaderResource);
					deckitr = deckitr->next;
				}
			}
		}
	}
	
	return S_OK;
}

// return the number of cards in the deck
int CDeck::CountDeck(void)
{
	return(m_deck.size());
}
	
// returns top card and removes it from the deck
CCard* CDeck::DealCard(void)
{
	return(m_deck.pop_front());
}


// add card to the bottom of the deck
void CDeck::AddCard(CCard* thiscard)
{
	m_deck.push_back(thiscard);
	return;
}

// take out a card at num position
CCard* CDeck::DealCard(int num)
{
	return(m_deck.erase(num));
}

// show a card at num position
CCard* CDeck::GetCard(int num) const
{
	int i = 0;
	const_deckIterator deckitr;

	deckitr = getDeckIterator();

	while ((deckitr != NULL) && (i != num)) {
		deckitr = deckitr->next;
		i++;
	}

	if (deckitr != NULL) {
		return(deckitr->data);
	}
	else {
		return(NULL);
	}
}

// Turn all cards either face up or face down depending on bShow
void CDeck::ShowDeck(bool bShow)
{
	const_deckIterator deckitr;

	deckitr = getDeckIterator();

	while (deckitr != NULL) {
		deckitr->data->setFaceUp(bShow);
		deckitr = deckitr->next;
	}

	return;
}

// finds a num card and sets its value to thiscard
void CDeck::SetCard(CCard thiscard, int num)
{
	int i = 0;
	const_deckIterator deckitr;

	deckitr = getDeckIterator();

	while ((deckitr != NULL) && (i != num)) {
		deckitr = deckitr->next;
		i++;
	}

	if (deckitr != NULL) {
		deckitr->data->setCard(thiscard.getRank(), thiscard.getSuit());
	}

	return;
}

// shuffles deck
// Each position in the deck will have the opportunity to be
// randomly swapped with at least one other random location
// in the deck.
void CDeck::shuffle(void)
{
	int i;
	int x;

#if defined(_DEBUG)
	// non randomly set seed for random number generator
	//srand(1);
	srand((unsigned int)time(NULL));
#else
	// use current time to randomly set seed for random number generator
	srand((unsigned int)time(NULL));
#endif // _DEBUG

	for (i=0; i<m_deck.size(); i++) {
		x = rand() % m_deck.size();
		m_deck.swapPos(i, x);
	}

	return;
}

// resets deck
void CDeck::resetDeck(bool bFull)
{
	CCard* pCard;
	POINT pt;

	pt.x = 100;
	pt.y = 100;
	EmptyHand();

	if (bFull) {
		// initialize the 52 card deck
		for(CCard::Suit suit = CCard::Hearts; suit <= CCard::Spades; suit++)
		{
			for(CCard::Rank rank = CCard::Ace; rank <= CCard::King; rank++)
			{
				pCard = new CCard();
				pCard->setCard(rank, suit);
				pCard->setFaceUp(false);
				pCard->setFileName(L"");
				pCard->setLocation(pt);
				pCard->setSpriteLength(0);
				pCard->setSpriteWidth(0);
				pCard->setFrontTexture(NULL);
				pCard->setBackTexture(NULL);

				AddCard(pCard);
			}
		}
	}

	if (m_pD3D10Device != NULL) {
		InitSprite(m_pD3D10Device, bFull);
	}

	return;
}

// deletes all cards in deck.
// resets top of deck pointer.
// All memory is properly deallocated.
void CDeck::EmptyHand(void)
{
	while (!m_deck.empty()) {
		m_deck.erase(0);
	}

	return;
}


// display the cards in the deck on the console
// for console app only
void CDeck::displayDeck(void) const
{
	const_deckIterator deckitr;
	char chCardRank[3];
	char chCardSuit[2];

	deckitr = getDeckIterator();

	while (deckitr != NULL) {
		deckitr->data->translateCard(chCardRank, chCardSuit);
		std::cout << chCardRank << chCardSuit << " ";
		deckitr = deckitr->next;
	}

	std::cout << "\n" << std::flush;

	return;
}


// computes and returns the 'value' of the deck
// This should be overloaded based on card game being implemented.
int CDeck::getDeckValue(void) const
{
	return(0);
}

// ISSUE: draw procedure should be const
// ISSUE:
// This draw routine assumes that pSprite->Begin() called before and
// pSprite->End() is called after.
HRESULT CDeck::draw(ID3DX10Sprite * pSprite, D3DX10_SPRITE* pSpriteArray, int x, int y, int xOffset, int yOffset) const
{
	HRESULT hResult = S_OK;
	int i = 0;
	int xCard, yCard;

	// draw each of the player's cards
	xCard = x;
	yCard = y;
	const_deckIterator deckitr;

	deckitr = getDeckIterator();

	if (deckitr != NULL) {
		while (deckitr != NULL) {
			deckitr->data->setLocation(xCard, yCard);
			hResult = deckitr->data->draw(pSprite, pSpriteArray+i);
			xCard += xOffset;
			yCard -= yOffset;
			i++;
			deckitr = deckitr->next;
		}
	} 

	// draw the set of sprites to the back buffer
	if (i>0) {
		hResult = pSprite->DrawSpritesImmediate(pSpriteArray, i, 0, 0);
	}
	
	return(i);
}
