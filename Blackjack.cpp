//--------------------------------------------------------------------------------------
// File: Blackjack.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "DXUT.h"
#include "DXUTgui.h"
#include "DXUTmisc.h"
#include "DXUTCamera.h"
#include "DXUTSettingsDlg.h"
#include "SDKmisc.h"
#include "SDKmesh.h"
#include "resource.h"
#include <string>

#include "BlackJackDeck.h"
#include "BlackJackPlayer.h"

//#define DEBUG_VS   // Uncomment this line to debug D3D9 vertex shaders 
//#define DEBUG_PS   // Uncomment this line to debug D3D9 pixel shaders 
#define TIMERID_DEAL_PAUSE				1990

#define DEFAULT_WIN_WIDTH 640
#define DEFAULT_WIN_HEIGHT 480

#define BLACKJACK 21 // player busts on over 21
#define DEALER_HIT 16 // dealer must hit on 16 or below

struct graphicRsrc {
	int ID;
	TCHAR lpFileName[255];
	TCHAR lpCardDesc[25];
	ID3D10ShaderResourceView* pShaderResource;
};

enum GameState {
	StateWelcome, // wait for player to sit down at table
	StateGetName, // player enters a playing name
	StateNewGame, // first four cards are dealt automatically
	StatePlayerTurn, // player accepting cards
	StateDealerTurn, // dealer accepting cards
	StateGameDealerWins, // game state is static
	StateGameDealerLoses, // game state is static
	StateGameDealerTies, // game state is static
	StateGameOver,
};

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
CModelViewerCamera          g_Camera;               // A model viewing camera
CDXUTDialogResourceManager  g_DialogResourceManager; // manager for shared resources of dialogs
CD3DSettingsDlg             g_SettingsDlg;          // Device settings dialog
CDXUTTextHelper*            g_pTxtHelper = NULL;
CDXUTDialog                 g_HUD;                  // dialog for standard controls
CDXUTDialog                 g_SampleUI;             // dialog for sample specific controls

// Direct3D 9 resources
ID3DXFont*                  g_pFont9 = NULL;
ID3DXSprite*                g_pSprite9 = NULL;

ID3DX10Font*                g_pFont10 = NULL;
ID3DX10Sprite*              g_pSprite10 = NULL;

GameState m_gameState;std::string g_strGameStatus;
UINT g_timerID;
graphicRsrc m_tableTopGraphics;

CDeck* m_pMasterDeck;
CBlackJackPlayer m_playerOne;
CBlackJackPlayer m_dealer;

//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           2
#define IDC_CHANGEDEVICE        3


//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
                          void* pUserContext );
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );
void CALLBACK OnMouse( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta, int xPos, int yPos, void* pUserContext );
void CALLBACK OnTimer(HWND hwnd, UINT uMsg, UINT_PTR timerID, DWORD dwTime);
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext );
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext );

bool CALLBACK IsD3D9DeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat,
                                      bool bWindowed, void* pUserContext );
HRESULT CALLBACK OnD3D9CreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                                     void* pUserContext );
HRESULT CALLBACK OnD3D9ResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                                    void* pUserContext );
void CALLBACK OnD3D9FrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
void CALLBACK OnD3D9LostDevice( void* pUserContext );
void CALLBACK OnD3D9DestroyDevice( void* pUserContext );

bool CALLBACK IsD3D10DeviceAcceptable( UINT Adapter, UINT Output, D3D10_DRIVER_TYPE DeviceType,
                                       DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext );
HRESULT CALLBACK OnD3D10CreateDevice( ID3D10Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
                                      void* pUserContext );
HRESULT CALLBACK OnD3D10ResizedSwapChain( ID3D10Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                          const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext );
void CALLBACK OnD3D10FrameRender( ID3D10Device* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
bool CALLBACK OnDeviceRemoved( void* pUserContext );
void CALLBACK OnD3D10ReleasingSwapChain( void* pUserContext );
void CALLBACK OnD3D10DestroyDevice( void* pUserContext );

int InitApp();
int InitBlackJack(ID3D10Device* pD3D10Device);
void RenderText();
HRESULT ReleaseApp();

#if defined(_DEBUG)
char* TranslateGameState(const GameState gameState);
#endif // _DEBUG


//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    // DXUT will create and use the best device (either D3D9 or D3D10) 
    // that is available on the system depending on which D3D callbacks are set below

    // Set DXUT callbacks
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackKeyboard( OnKeyboard );
	DXUTSetCallbackMouse( OnMouse, false );
    DXUTSetCallbackFrameMove( OnFrameMove );
    DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );

    DXUTSetCallbackD3D9DeviceAcceptable( IsD3D9DeviceAcceptable );
    DXUTSetCallbackD3D9DeviceCreated( OnD3D9CreateDevice );
    DXUTSetCallbackD3D9DeviceReset( OnD3D9ResetDevice );
    DXUTSetCallbackD3D9DeviceLost( OnD3D9LostDevice );
    DXUTSetCallbackD3D9DeviceDestroyed( OnD3D9DestroyDevice );
    DXUTSetCallbackD3D9FrameRender( OnD3D9FrameRender );

	// Set the D3D10 DXUT callbacks. Remove these sets if the app doesn't need to support D3D10
    DXUTSetCallbackD3D10DeviceAcceptable( IsD3D10DeviceAcceptable );
    DXUTSetCallbackD3D10DeviceCreated( OnD3D10CreateDevice );
    DXUTSetCallbackD3D10SwapChainResized( OnD3D10ResizedSwapChain );
    DXUTSetCallbackD3D10FrameRender( OnD3D10FrameRender );
    DXUTSetCallbackD3D10SwapChainReleasing( OnD3D10ReleasingSwapChain );
    DXUTSetCallbackD3D10DeviceDestroyed( OnD3D10DestroyDevice );
	
	InitApp();
    DXUTInit( true, true, NULL ); // Parse the command line, show msgboxes on error, no extra command line params
    DXUTSetCursorSettings( true, true );
    DXUTCreateWindow( L"Blackjack" );
    DXUTCreateDevice( true, 640, 480 );

	int initResult = InitBlackJack(DXUTGetD3D10Device());
	if (initResult != 0) {
		// timer was not created
        return E_OUTOFMEMORY;
	}
	
	DXUTMainLoop(); // Enter into the DXUT render loop

	ReleaseApp();
	
	return DXUTGetExitCode();
}

HRESULT ReleaseApp()
{
	HRESULT hr;

	KillTimer( DXUTGetHWND(), g_timerID );

    hr = D3DX10UnsetAllDeviceObjects( DXUTGetD3D10Device() );
	SAFE_RELEASE( g_pSprite9 );
    SAFE_RELEASE( g_pSprite10 );
	SAFE_RELEASE( g_pFont10 );
    SAFE_DELETE( g_pTxtHelper );
    SAFE_RELEASE( g_pFont9 );
	if (m_pMasterDeck != NULL) {
		delete m_pMasterDeck;
		m_pMasterDeck = NULL;
	}

	return(hr);
}

HRESULT InitSprite(ID3D10Device* pD3D10Device)
{
	HRESULT hr = S_OK;
	D3DX10_IMAGE_INFO InfoFromFile;
	D3DX10_IMAGE_LOAD_INFO LoadImageInfo;
	
	// set of file path
	DWORD nDirectoryLength = 255;
	TCHAR lpCurrentDir[255];
	GetCurrentDirectory(nDirectoryLength, lpCurrentDir);
	lstrcat(lpCurrentDir, (LPCTSTR)L"\\Graphics\\");
	
	// table top image
	m_tableTopGraphics.ID = 0;
	m_tableTopGraphics.lpCardDesc[0] = '\0';
	lstrcpy(m_tableTopGraphics.lpFileName, (LPCTSTR)lpCurrentDir);
	lstrcat(m_tableTopGraphics.lpFileName, (LPCTSTR)L"table top.jpg");
	hr = D3DX10GetImageInfoFromFile(m_tableTopGraphics.lpFileName, NULL, &InfoFromFile, NULL);
	if (FAILED(hr)){return -1;}

	LoadImageInfo.Width = InfoFromFile.Width;
	LoadImageInfo.Height = InfoFromFile.Height;
	LoadImageInfo.Depth = InfoFromFile.Depth;
	LoadImageInfo.FirstMipLevel = 1;
	LoadImageInfo.MipLevels = InfoFromFile.MipLevels;
	LoadImageInfo.Usage = D3D10_USAGE_DEFAULT;
	LoadImageInfo.BindFlags = D3D10_BIND_SHADER_RESOURCE ;
	LoadImageInfo.CpuAccessFlags = 0;
	LoadImageInfo.MiscFlags = 0;
	LoadImageInfo.Format = InfoFromFile.Format;
	LoadImageInfo.Filter = D3DX10_FILTER_NONE;
	LoadImageInfo.MipFilter = D3DX10_FILTER_NONE;
	LoadImageInfo.pSrcInfo = &InfoFromFile;

	hr = D3DX10CreateShaderResourceViewFromFile(pD3D10Device, m_tableTopGraphics.lpFileName, &LoadImageInfo, NULL, &(m_tableTopGraphics.pShaderResource), NULL);
	if( FAILED(hr) ) {return hr;}

	return S_OK;
}

int InitBlackJack(ID3D10Device* pD3D10Device)
{
	m_gameState = StateWelcome;

	InitSprite(pD3D10Device);

	m_playerOne.getHand()->setDevice(pD3D10Device);
	m_dealer.getHand()->setDevice(pD3D10Device);
	
	m_pMasterDeck = new CDeck(pD3D10Device, true);
	m_pMasterDeck->InitSprite(pD3D10Device, true);

	HWND hwnd = DXUTGetHWND();
	g_timerID = SetTimer(hwnd, TIMERID_DEAL_PAUSE, 1000, OnTimer );
	if (g_timerID == 0) {
		// timer was not created
        return E_OUTOFMEMORY;
	}
	
	return(0);
}


//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
int InitApp()
{
    g_SettingsDlg.Init( &g_DialogResourceManager );
    g_HUD.Init( &g_DialogResourceManager );
    g_SampleUI.Init( &g_DialogResourceManager );

    g_HUD.SetCallback( OnGUIEvent ); int iY = 10;
    g_HUD.AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 35, iY, 125, 22 );
    g_HUD.AddButton( IDC_TOGGLEREF, L"Toggle REF (F3)", 35, iY += 24, 125, 22, VK_F3 );
    g_HUD.AddButton( IDC_CHANGEDEVICE, L"Change device (F2)", 35, iY += 24, 125, 22, VK_F2 );

    g_SampleUI.SetCallback( OnGUIEvent ); iY = 10;// Start the timer to delay dealing of dealer's cards

	return(0);
}


//--------------------------------------------------------------------------------------
// Render the help and statistics text. This function uses the ID3DXFont interface for 
// efficient text rendering.
//--------------------------------------------------------------------------------------
void RenderText()
{
	char buffer[50];
	WCHAR wbuffer[50];

	g_pTxtHelper->Begin();

	g_pTxtHelper->SetInsertionPos( 5, 5 );
    g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
    g_pTxtHelper->DrawTextLine( DXUTGetFrameStats( DXUTIsVsyncEnabled() ) );
    g_pTxtHelper->DrawTextLine( DXUTGetDeviceStats() );
	
	switch (m_gameState) {
		case StateWelcome:
			{
				//g_pTxtHelper->SetInsertionPos( 5, 5 );
				g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
				g_pTxtHelper->DrawTextLine( L"Welcome!" );

				g_pTxtHelper->SetInsertionPos( 5, 460 );
				g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ) );
				g_pTxtHelper->DrawTextLine( L"Click to start a new game." );
				break;
			}
		case StateGetName:
			{
				g_pTxtHelper->SetInsertionPos( 200, 200 );
				g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ) );
				g_pTxtHelper->DrawTextLine( L"Please enter your name:" );

				g_pTxtHelper->SetInsertionPos( 220, 220 );
				g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ) );
				sprintf_s(buffer, 50, "%s", m_playerOne.getName());
				MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, buffer, -1, wbuffer, 50);
				g_pTxtHelper->DrawTextLine(wbuffer);

				g_pTxtHelper->SetInsertionPos( 5, 460 );
				g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ) );
				g_pTxtHelper->DrawTextLine( L"Enter your name followed by CR." );
				break;
			}
		
		case StatePlayerTurn:
			{
				g_pTxtHelper->SetInsertionPos( 5, 460 );
				g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ) );
				g_pTxtHelper->DrawTextLine( L"Left click to hit, Right click to stand." );
				break;
			}
		case StateGameDealerLoses:
			{
				g_pTxtHelper->SetInsertionPos( 5, 440 );
				g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ) );
				g_strGameStatus = m_playerOne.getName();
				g_strGameStatus += " wins";
				sprintf_s(buffer, 50, "%s wins.", m_playerOne.getName());
				MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, buffer, -1, wbuffer, 50);
				g_pTxtHelper->DrawTextLine(wbuffer);

				g_pTxtHelper->SetInsertionPos( 5, 460 );
				g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ) );
				g_pTxtHelper->DrawTextLine( L"Click to start a new game." );
				break;
			}
		case StateGameDealerWins:
			{
				g_pTxtHelper->SetInsertionPos( 5, 440 );
				g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ) );
				g_strGameStatus = m_playerOne.getName();
				g_strGameStatus += " loses";
				sprintf_s(buffer, 50, "%s loses.", m_playerOne.getName());
				MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, buffer, -1, wbuffer, 50);
				g_pTxtHelper->DrawTextLine(wbuffer);

				g_pTxtHelper->SetInsertionPos( 5, 460 );
				g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ) );
				g_pTxtHelper->DrawTextLine( L"Click to start a new game." );
				break;
			}

		case StateNewGame:
			{
				g_pTxtHelper->SetInsertionPos( 5, 460 );
				g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ) );
				g_pTxtHelper->DrawTextLine( L"Dealing ... please wait." );
				break;
			}

		case StateGameOver:
			{
				g_pTxtHelper->SetInsertionPos( 5, 440 );
				g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ) );
				sprintf_s(buffer, 50, "%s loses.", m_playerOne.getName());
				MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, g_strGameStatus.c_str(), -1, wbuffer, 50);
				g_pTxtHelper->DrawTextLine(wbuffer);

				g_pTxtHelper->SetInsertionPos( 5, 460 );
				g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ) );
				g_pTxtHelper->DrawTextLine( L"Click to start a new game." );
				break;
			}

		case StateGameDealerTies:
			{
				g_pTxtHelper->SetInsertionPos( 5, 440 );
				g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ) );
				g_strGameStatus = m_playerOne.getName();
				g_strGameStatus += " ties";
				sprintf_s(buffer, 50, "%s ties.", m_playerOne.getName());
				MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, buffer, -1, wbuffer, 50);
				g_pTxtHelper->DrawTextLine(wbuffer);

				g_pTxtHelper->SetInsertionPos( 5, 460 );
				g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ) );
				g_pTxtHelper->DrawTextLine( L"Click to start a new game." );
				break;
			}
	}

#if defined(_DEBUG)
	g_pTxtHelper->SetInsertionPos( 460, 460 );
	g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ) );
	sprintf_s(buffer, 50, "Game State is %s.", TranslateGameState(m_gameState));
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, buffer, -1, wbuffer, 50);
	g_pTxtHelper->DrawTextLine(wbuffer);
#endif // _DEBUG

	g_pTxtHelper->End();
}


//--------------------------------------------------------------------------------------
// Rejects any D3D9 devices that aren't acceptable to the app by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D9DeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat,
                                      D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    // Skip backbuffer formats that don't support alpha blending
    IDirect3D9* pD3D = DXUTGetD3D9Object();
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                                         AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
                                         D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
        return false;

    // No fallback defined by this app, so reject any device that 
    // doesn't support at least ps2.0
    if( pCaps->PixelShaderVersion < D3DPS_VERSION( 2, 0 ) )
        return false;

    return true;
}


//--------------------------------------------------------------------------------------
// Called right before creating a D3D9 or D3D10 device, allowing the app to modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
    if( pDeviceSettings->ver == DXUT_D3D9_DEVICE )
    {
        IDirect3D9* pD3D = DXUTGetD3D9Object();
        D3DCAPS9 Caps;
        pD3D->GetDeviceCaps( pDeviceSettings->d3d9.AdapterOrdinal, pDeviceSettings->d3d9.DeviceType, &Caps );

        // If device doesn't support HW T&L or doesn't support 1.1 vertex shaders in HW 
        // then switch to SWVP.
        if( ( Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) == 0 ||
            Caps.VertexShaderVersion < D3DVS_VERSION( 1, 1 ) )
        {
            pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
        }

        // Debugging vertex shaders requires either REF or software vertex processing 
        // and debugging pixel shaders requires REF.  
#ifdef DEBUG_VS
        if( pDeviceSettings->d3d9.DeviceType != D3DDEVTYPE_REF )
        {
            pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
            pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_PUREDEVICE;
            pDeviceSettings->d3d9.BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
        }
#endif
#ifdef DEBUG_PS
        pDeviceSettings->d3d9.DeviceType = D3DDEVTYPE_REF;
#endif
    }

    // For the first device created if its a REF device, optionally display a warning dialog box
    static bool s_bFirstTime = true;
    if( s_bFirstTime )
    {
        s_bFirstTime = false;
        if( ( DXUT_D3D9_DEVICE == pDeviceSettings->ver && pDeviceSettings->d3d9.DeviceType == D3DDEVTYPE_REF ) ||
            ( DXUT_D3D10_DEVICE == pDeviceSettings->ver &&
              pDeviceSettings->d3d10.DriverType == D3D10_DRIVER_TYPE_REFERENCE ) )
            DXUTDisplaySwitchingToREFWarning( pDeviceSettings->ver );
    }

    return true;
}


//--------------------------------------------------------------------------------------
// Create any D3D9 resources that will live through a device reset (D3DPOOL_MANAGED)
// and aren't tied to the back buffer size
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D9CreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                                     void* pUserContext )
{
    HRESULT hr;

    V_RETURN( g_DialogResourceManager.OnD3D9CreateDevice( pd3dDevice ) );
    V_RETURN( g_SettingsDlg.OnD3D9CreateDevice( pd3dDevice ) );

    V_RETURN( D3DXCreateFont( pd3dDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
                              OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                              L"Arial", &g_pFont9 ) );

#ifdef DEBUG_VS
        dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
    #endif
#ifdef DEBUG_PS
        dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
    #endif

    // Setup the camera's view parameters
    D3DXVECTOR3 vecEye( 0.0f, 0.0f, -5.0f );
    D3DXVECTOR3 vecAt ( 0.0f, 0.0f, -0.0f );
    g_Camera.SetViewParams( &vecEye, &vecAt );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3D9 resources that won't live through a device reset (D3DPOOL_DEFAULT) 
// or that are tied to the back buffer size 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D9ResetDevice( IDirect3DDevice9* pd3dDevice,
                                    const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;

    V_RETURN( g_DialogResourceManager.OnD3D9ResetDevice() );
    V_RETURN( g_SettingsDlg.OnD3D9ResetDevice() );

    if( g_pFont9 ) V_RETURN( g_pFont9->OnResetDevice() );

    V_RETURN( D3DXCreateSprite( pd3dDevice, &g_pSprite9 ) );
    g_pTxtHelper = new CDXUTTextHelper( g_pFont9, g_pSprite9, NULL, NULL, 15 );

    // Setup the camera's projection parameters
    float fAspectRatio = pBackBufferSurfaceDesc->Width / ( FLOAT )pBackBufferSurfaceDesc->Height;
    g_Camera.SetProjParams( D3DX_PI / 4, fAspectRatio, 0.1f, 1000.0f );
    g_Camera.SetWindow( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height );

    g_HUD.SetLocation( pBackBufferSurfaceDesc->Width - 170, 0 );
    g_HUD.SetSize( 170, 170 );
    g_SampleUI.SetLocation( pBackBufferSurfaceDesc->Width - 170, pBackBufferSurfaceDesc->Height - 350 );
    g_SampleUI.SetSize( 170, 300 );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
    // Update the camera's position based on user input 
    g_Camera.FrameMove( fElapsedTime );
}


void CALLBACK OnD3D9FrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
}
//--------------------------------------------------------------------------------------
// Render the scene using the D3D9 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D10FrameRender( ID3D10Device* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
	HRESULT hr;
    D3DXMATRIXA16 mWorld;
    D3DXMATRIXA16 mView;
    D3DXMATRIXA16 mProj;
    D3DXMATRIXA16 mWorldViewProjection;

	// Clear the render target and the zbuffer 
	float ClearColor[4] = { 0.176f, 0.196f, 0.667f, 0.0f };
    pd3dDevice->ClearRenderTargetView( DXUTGetD3D10RenderTargetView(), ClearColor );
    pd3dDevice->ClearDepthStencilView( DXUTGetD3D10DepthStencilView(), D3D10_CLEAR_DEPTH, 1.0, 0 );
	
	D3DX10_SPRITE cardSprites[NUM_CARD_VIEWS];
	D3DX10_SPRITE tableTopSprite;

	D3DXMATRIXA16 matScaling;
	D3DXMATRIXA16 matTranslation;
	D3DXMATRIXA16 matProjection;

	int xOffset;
	int yOffset;

	D3DXMatrixOrthoOffCenterLH(&matProjection, 0.0f, DEFAULT_WIN_WIDTH, 0.0f, DEFAULT_WIN_HEIGHT, 0.1f, 10);

	hr = g_pSprite10->Begin( D3DX10_SPRITE_SORT_TEXTURE );
	hr = g_pSprite10->SetProjectionTransform(&matProjection);

	D3DXMatrixScaling( &matScaling, DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT, 1.0f );

	// (0, 0) is lower left corner
	D3DXMatrixTranslation( &matTranslation, DEFAULT_WIN_WIDTH/2, DEFAULT_WIN_HEIGHT/2, 0.1f);

	// Always draw the table top
	tableTopSprite.matWorld = matScaling * matTranslation;
	tableTopSprite.TexCoord.x = 0.0f;
	tableTopSprite.TexCoord.y = 0.0f;
	tableTopSprite.TexSize.x = 1.0f;
	tableTopSprite.TexSize.y = 1.0f;
	tableTopSprite.ColorModulate = D3DXCOLOR(1.0f,1.0f,1.0f,1.0f);

	tableTopSprite.pTexture = m_tableTopGraphics.pShaderResource;
	tableTopSprite.TextureIndex = 0;
	
	hr = g_pSprite10->DrawSpritesImmediate(&tableTopSprite, 1, 0, 0);
	hr = g_pSprite10->Flush();
	
	// Draw state specific sprites
	switch (m_gameState) {
		case StateGetName:
			{
				break;
			}

		case StateWelcome:
			{
				// Draw the deck of cards in fan-like fashion.
				// At this point the cards are nicely ordered.
				xOffset = 8;
				yOffset = 4;
				m_pMasterDeck->ShowDeck(true);
				//numCarsDrawn = m_pMasterDeck->draw(pSprite, cardSprites, 50, 380, xOffset, yOffset);
				hr = m_pMasterDeck->draw(g_pSprite10, cardSprites, 50, 380, xOffset, yOffset);
				//hr = pSprite->DrawSpritesBuffered(cardSprites, numCarsDrawn);
				m_pMasterDeck->ShowDeck(false);
				break;
			}

	case StateNewGame:
	case StatePlayerTurn:
	case StateDealerTurn:
	case StateGameDealerLoses:
	case StateGameDealerWins:
	case StateGameDealerTies:
	case StateGameOver:
		{
			// draw each player's cards
			xOffset = 18;
			yOffset = 7;

			// ISSUE: const issue
			// ISSUE:
			CBlackJackDeck* p;
			p = m_playerOne.viewHand();
			hr = p->draw(g_pSprite10, cardSprites, 280, 175, xOffset, yOffset);
			//hr = m_playerOne.viewHand()->draw(pSprite, cardSprites, 280, 175, xOffset, yOffset);

			p = m_dealer.viewHand();
			hr = p->draw(g_pSprite10, cardSprites, 245, 320, xOffset, yOffset);
			//hr = m_dealer.viewHand()->draw(pSprite, cardSprites, 245, 320, xOffset, yOffset);

			break;
		}
	}

	//hResult = pSprite->Flush();
	hr = g_pSprite10->End();
	
	RenderText();
}


//--------------------------------------------------------------------------------------
// Handle messages to the application
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
                          void* pUserContext )
{
    // Pass messages to dialog resource manager calls so GUI state is updated correctly
    *pbNoFurtherProcessing = g_DialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    // Pass messages to settings dialog if its active
    if( g_SettingsDlg.IsActive() )
    {
        g_SettingsDlg.MsgProc( hWnd, uMsg, wParam, lParam );
        return 0;
    }

    // Give the dialogs a chance to handle the message first
    *pbNoFurtherProcessing = g_HUD.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;
    *pbNoFurtherProcessing = g_SampleUI.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    // Pass all remaining windows messages to camera so it can respond to user input
    g_Camera.HandleMessages( hWnd, uMsg, wParam, lParam );

    return 0;
}


//--------------------------------------------------------------------------------------
// Handle key presses
//--------------------------------------------------------------------------------------
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
	if (!bKeyDown) {
		switch (m_gameState) {
		case StateGetName:
		{
			// Player names are limited to alphabetic characters of length CBlackJackPlayer::MaxNameChars-1.
			// If no name is entered player gets default name of 'Player'.
			// To edit the name, backspace and delete are both supported.
			// Entry ends upon CR.
			char strName[CBlackJackPlayer::MaxNameChars];
			int len;

			// CBlackJackPlayer::MaxNameChars includes terminating '\0'
			if (nChar == VK_RETURN) {
				m_gameState = StateNewGame;
				if (strlen(m_playerOne.getName()) == 0)
				{
					m_playerOne.setName("Player");
				}
			}
			if ((nChar == VK_BACK) || (nChar == VK_DELETE)) {
				if ((len = strlen(m_playerOne.getName())) > 0) {
					strcpy(strName, m_playerOne.getName());
					strName[len-1] = '\0';
					m_playerOne.setName(strName);
				}
			}
			else if ((len = strlen(m_playerOne.getName())) < CBlackJackPlayer::MaxNameChars-1) {
				if (isalpha(nChar)) {
					char caseKey;

					// virtual keys are always capitalized
					if (GetAsyncKeyState(VK_SHIFT)) {
						// Shift key is pressed, do nothing
						caseKey = nChar;
					}
					else {
						// Shift key is not pressed, translate to lower case
						caseKey = nChar - 'A' + 'a';
					}
					strcpy(strName, m_playerOne.getName());
					strName[CBlackJackPlayer::MaxNameChars-1] = '\0';
					strName[len] = caseKey;
					strName[len+1] = '\0';
					m_playerOne.setName(strName);
					// -2 because we just added one character
					if (len >= CBlackJackPlayer::MaxNameChars-2) {
						m_gameState = StateNewGame;
					}
				}
			}
			else {
				strName[CBlackJackPlayer::MaxNameChars-1] = '\0';
				m_gameState = StateNewGame;
			}

			break;
		}
		}
	}

	return;
}
void CALLBACK OnMouse( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta, int xPos, int yPos, void* pUserContext )
{
	switch (m_gameState) {
		case StateWelcome:
			{
				// Move to get player's name state
				m_gameState = StateGetName;
				m_pMasterDeck->shuffle();
				break;
			}

		case StateGameDealerLoses:
		case StateGameDealerTies:
		case StateGameDealerWins:
			{
				m_gameState = StateGameOver;
				break;
			}
		case StateGameOver:
			{
				CCard* pCard;
				// Move to new game state
				// Initialize game state by clearing all player's hands,
				// resetting deck and shuffling deck.
				m_gameState = StateNewGame;
				
				while (m_playerOne.getHand()->CountDeck() > 0) {
					pCard = m_playerOne.getHand()->DealCard();
					assert(pCard != NULL);
					pCard->setFaceUp(false);
					m_pMasterDeck->AddCard(pCard);
				}
				while (m_dealer.getHand()->CountDeck() > 0) {
					pCard = m_dealer.getHand()->DealCard();
					assert(pCard != NULL);
					pCard->setFaceUp(false);
					m_pMasterDeck->AddCard(pCard);
				}
				m_pMasterDeck->shuffle();
				m_playerOne.getHand()->EmptyHand();
				m_dealer.getHand()->EmptyHand();
				break;
			}

		case StatePlayerTurn:
			{
				if (bLeftButtonDown) {
					// deal a card to player
					// subsequent player cards dealt face up
					CCard* pTopCard;
					pTopCard = m_pMasterDeck->DealCard();
					assert(pTopCard != NULL);
					pTopCard->setFaceUp(true);
					m_playerOne.getHand()->AddCard(pTopCard);

					if (m_playerOne.getHand()->getDeckValue() > BLACKJACK) {
						m_gameState = StateGameDealerWins;
						m_dealer.getHand()->ShowDeck(true);
						m_playerOne.incLosses();
						m_dealer.incWins();
					}
				}
				else if (bRightButtonDown) {
					// player's turn is over so now it is dealer's turn
					m_gameState = StateDealerTurn;
					m_dealer.getHand()->ShowDeck(true);
				}
				break;
			}
	}

	return;
}
void CALLBACK OnTimer(HWND hwnd, UINT uMsg, UINT_PTR timerID, DWORD dwTime)
{
	switch (timerID) {
		case TIMERID_DEAL_PAUSE:
			{
				switch (m_gameState) {
					case StateDealerTurn:
						{
							// deal a card to player
							// subsequent player cards dealt face up
							if (m_dealer.getHand()->getDeckValue() <= DEALER_HIT) {
								CCard* pTopCard;
								pTopCard = m_pMasterDeck->DealCard();
								assert(pTopCard != NULL);
								pTopCard->setFaceUp(true);
								m_dealer.getHand()->AddCard(pTopCard);
							}

							if (m_dealer.getHand()->getDeckValue() > DEALER_HIT) {
								if (m_playerOne.getHand()->getDeckValue() > BLACKJACK) {
									m_gameState = StateGameDealerWins;
									m_dealer.getHand()->ShowDeck(true);
									m_playerOne.incLosses();
									m_dealer.incWins();
								}
								else if (m_dealer.getHand()->getDeckValue() > BLACKJACK) {
									m_gameState = StateGameDealerLoses;
									m_dealer.getHand()->ShowDeck(true);
									m_playerOne.incWins();
									m_dealer.incLosses();
								}
								else if (m_dealer.getHand()->getDeckValue() > m_playerOne.getHand()->getDeckValue()) {
									m_gameState = StateGameDealerWins;
									m_dealer.getHand()->ShowDeck(true);
									m_playerOne.incLosses();
									m_dealer.incWins();
								}
								else if (m_dealer.getHand()->getDeckValue() == m_playerOne.getHand()->getDeckValue()) {
									m_gameState = StateGameDealerTies;
									m_dealer.getHand()->ShowDeck(true);
									m_playerOne.incTies();
									m_dealer.incTies();
								}
								else {
									m_gameState = StateGameDealerLoses;
									m_dealer.getHand()->ShowDeck(true);
									m_playerOne.incWins();
									m_dealer.incLosses();
								}
							}

							break;
						}
					case StatePlayerTurn:
						{
							break;
						}
					case StateGameOver:
						{
							break;
						}
					case StateGameDealerLoses:
					case StateGameDealerWins:
					case StateGameDealerTies:
						{
							m_gameState = StateGameOver;
							break;
						}
					case StateNewGame:
						{
							int numDealt;

							// count cards dealt
							numDealt = m_dealer.getHand()->CountDeck() + m_playerOne.getHand()->CountDeck();

							if (numDealt < 4) {
								// then deal a card to appropriate player if necessary
								CCard* pTopCard;
								if (m_playerOne.getHand()->CountDeck() == 0) {
									pTopCard = m_pMasterDeck->DealCard();
									assert(pTopCard != NULL);
									pTopCard->setFaceUp(true);
									m_playerOne.getHand()->AddCard(pTopCard);
								}
								else if (m_dealer.getHand()->CountDeck() == 0) {
									pTopCard = m_pMasterDeck->DealCard();
									assert(pTopCard != NULL);
									pTopCard->setFaceUp(false);
									m_dealer.getHand()->AddCard(pTopCard);
								}
								else if (m_playerOne.getHand()->CountDeck() == 1) {
									pTopCard = m_pMasterDeck->DealCard();
									assert(pTopCard != NULL);
									pTopCard->setFaceUp(true);
									m_playerOne.getHand()->AddCard(pTopCard);
								}
								else {
									pTopCard = m_pMasterDeck->DealCard();
									assert(pTopCard != NULL);
									pTopCard->setFaceUp(true);
									m_dealer.getHand()->AddCard(pTopCard);

									if (m_dealer.getHand()->getDeckValue() == BLACKJACK) {
										m_gameState = StateGameDealerWins;
										m_dealer.getHand()->ShowDeck(true);
										m_playerOne.incLosses();
										m_dealer.incWins();
									}
									else if (m_playerOne.getHand()->getDeckValue() == BLACKJACK) {
										m_gameState = StateGameDealerLoses;
										m_dealer.getHand()->ShowDeck(true);
										m_playerOne.incWins();
										m_dealer.incLosses();
									}
								}
							}
							else {
								// else switch game state
								m_gameState = StatePlayerTurn;
							}

							break;
						}
				}

				break;
			}
	}
}

//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
    switch( nControlID )
    {
        case IDC_TOGGLEFULLSCREEN:
            DXUTToggleFullScreen(); break;
        case IDC_TOGGLEREF:
            DXUTToggleREF(); break;
        case IDC_CHANGEDEVICE:
            g_SettingsDlg.SetActive( !g_SettingsDlg.IsActive() ); break;
    }
}


//--------------------------------------------------------------------------------------
// Release D3D9 resources created in the OnD3D9ResetDevice callback 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9LostDevice( void* pUserContext )
{
    g_DialogResourceManager.OnD3D9LostDevice();
    g_SettingsDlg.OnD3D9LostDevice();
    if( g_pFont9 ) g_pFont9->OnLostDevice();
    SAFE_RELEASE( g_pSprite9 );
    SAFE_RELEASE( g_pSprite10 );
	SAFE_RELEASE( g_pFont10 );
    SAFE_DELETE( g_pTxtHelper );
}


//--------------------------------------------------------------------------------------
// Release D3D9 resources created in the OnD3D9CreateDevice callback 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9DestroyDevice( void* pUserContext )
{
    g_DialogResourceManager.OnD3D9DestroyDevice();
    g_SettingsDlg.OnD3D9DestroyDevice();
    SAFE_RELEASE( g_pFont9 );
	if (m_pMasterDeck != NULL) {
		delete m_pMasterDeck;
		m_pMasterDeck = NULL;
	}
	if (m_tableTopGraphics.pShaderResource != NULL) {
		m_tableTopGraphics.pShaderResource->Release();
	}
}


//--------------------------------------------------------------------------------------
// Reject any D3D10 devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D10DeviceAcceptable( UINT Adapter, UINT Output, D3D10_DRIVER_TYPE DeviceType,
                                       DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    return true;
}




//--------------------------------------------------------------------------------------
// Create any D3D10 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D10CreateDevice( ID3D10Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
                                      void* pUserContext )
{
	HRESULT hr = S_OK;

	hr = D3DX10CreateSprite(pd3dDevice, 1, &g_pSprite10);
    if( FAILED( hr ) ) {
		//DXUTDisplayErrorMessage( hr );
        return hr;
	}

	hr = D3DX10CreateFont( pd3dDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial", &g_pFont10 );
	g_pTxtHelper = new CDXUTTextHelper( NULL, NULL, g_pFont10, g_pSprite10, 15 );
	if (g_pTxtHelper == NULL) {
		return E_OUTOFMEMORY;
	}


    return hr;
}


//--------------------------------------------------------------------------------------
// Create any D3D10 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D10ResizedSwapChain( ID3D10Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                          const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    return S_OK;
}
//--------------------------------------------------------------------------------------
// Release D3D10 resources created in OnD3D10ResizedSwapChain 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D10ReleasingSwapChain( void* pUserContext )
{
}


//--------------------------------------------------------------------------------------
// Release D3D10 resources created in OnD3D10CreateDevice 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D10DestroyDevice( void* pUserContext )
{
	g_DialogResourceManager.OnD3D10DestroyDevice();
    g_SettingsDlg.OnD3D10DestroyDevice();
    SAFE_RELEASE( g_pFont10 );
	if (m_pMasterDeck != NULL) {
		delete m_pMasterDeck;
		m_pMasterDeck = NULL;
	}
	if (m_tableTopGraphics.pShaderResource != NULL) {
		m_tableTopGraphics.pShaderResource->Release();
	}
}

#if defined(_DEBUG)
char* TranslateGameState(const GameState gameState)
{
	switch (gameState) {
		case StateWelcome: return("welcome");
		case StateGetName: return("get name");
		case StateGameOver: return("game over");
		case StateNewGame: return("new game");
		case StatePlayerTurn: return("player's turn");
		case StateDealerTurn: return("dealer's turn");
		case StateGameDealerWins: return("dealer wins");
		case StateGameDealerLoses: return("dealer loses");
		case StateGameDealerTies: return("dealer ties");
	}

	return("unknown state");
}
#endif // _DEBUG
