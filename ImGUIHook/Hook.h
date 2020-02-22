//  ____    _____  __     __  _____   _        ___    ____    _____   ____            ____   __   __         __        __  _  _     _   _    ___    _       _____ 
// |  _ \  | ____| \ \   / / | ____| | |      / _ \  |  _ \  | ____| |  _ \          | __ )  \ \ / /         \ \      / / | || |   | | | |  / _ \  | |     |_   _|
// | | | | |  _|    \ \ / /  |  _|   | |     | | | | | |_) | |  _|   | | | |         |  _ \   \ V /           \ \ /\ / /  | || |_  | |_| | | (_) | | |       | |  
// | |_| | | |___    \ V /   | |___  | |___  | |_| | |  __/  | |___  | |_| |         | |_) |   | |             \ V  V /   |__   _| |  _  |  \__, | | |___    | |  
// |____/  |_____|    \_/    |_____| |_____|  \___/  |_|     |_____| |____/          |____/    |_|              \_/\_/       |_|   |_| |_|    /_/  |_____|   |_|  
                                                                                                                                                                                                                                                                                                  

typedef HRESULT(APIENTRY *SetStreamSource)(IDirect3DDevice9*, UINT, IDirect3DVertexBuffer9*, UINT, UINT);
SetStreamSource SetStreamSource_orig = 0;

typedef HRESULT(APIENTRY *DrawIndexedPrimitive)(IDirect3DDevice9*, D3DPRIMITIVETYPE, INT, UINT, UINT, UINT, UINT);
DrawIndexedPrimitive DrawIndexedPrimitive_orig = 0;

typedef HRESULT(APIENTRY* Present) (IDirect3DDevice9*, const RECT *, const RECT *, HWND, const RGNDATA *);
Present Present_orig = 0;

typedef HRESULT(APIENTRY* EndScene) (IDirect3DDevice9*);
EndScene EndScene_orig = 0;

typedef HRESULT(APIENTRY *Reset)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
Reset Reset_orig = 0;

bool Menu = true;
HMODULE Hand;
UINT Stride;
HWND TargetWnd = NULL;
WNDPROC oWndProc = NULL;

#pragma warning (disable: 4099) 
#if defined _M_X64
#pragma comment(lib, "detours.X64/detours.lib")
#elif defined _M_IX86
#pragma comment(lib, "detours.X86/detours.lib")
#endif


