//  ____    _____  __     __  _____   _        ___    ____    _____   ____            ____   __   __         __        __  _  _     _   _    ___    _       _____ 
// |  _ \  | ____| \ \   / / | ____| | |      / _ \  |  _ \  | ____| |  _ \          | __ )  \ \ / /         \ \      / / | || |   | | | |  / _ \  | |     |_   _|
// | | | | |  _|    \ \ / /  |  _|   | |     | | | | | |_) | |  _|   | | | |         |  _ \   \ V /           \ \ /\ / /  | || |_  | |_| | | (_) | | |       | |  
// | |_| | | |___    \ V /   | |___  | |___  | |_| | |  __/  | |___  | |_| |         | |_) |   | |             \ V  V /   |__   _| |  _  |  \__, | | |___    | |  
// |____/  |_____|    \_/    |_____| |_____|  \___/  |_|     |_____| |____/          |____/    |_|              \_/\_/       |_|   |_| |_|    /_/  |_____|   |_|  
                                                                                                                                                                                                                                                                                                  
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <iostream>
#include <fstream>
#include "Detours.h"
#include "Hook.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_impl_win32.h"
#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "d3d9.lib")
using namespace std;

void InputHandler()
{
	for (int i = 0; i < 5; i++) ImGui::GetIO().MouseDown[i] = false;
	int button = -1;
	if (GetAsyncKeyState(VK_LBUTTON)) button = 0;
	if (button != -1) ImGui::GetIO().MouseDown[button] = true;
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (Menu && ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}

	return CallWindowProc(oWndProc, hWnd, msg, wParam, lParam);
}

HRESULT APIENTRY Present_hook(IDirect3DDevice9* pDevice, const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion)
{
	if (GetAsyncKeyState(VK_INSERT) & 1) Menu = !Menu;
	static bool init = true;
	if (init)
	{
		init = false;
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		D3DDEVICE_CREATION_PARAMETERS d3dcp;
		pDevice->GetCreationParameters(&d3dcp);
		TargetWnd = d3dcp.hFocusWindow;
		if (TargetWnd != NULL)
		{
			oWndProc = (WNDPROC)SetWindowLongPtr(TargetWnd, GWLP_WNDPROC, (LONG_PTR)WndProc);
			ImGui_ImplWin32_Init(TargetWnd);
			ImGui_ImplDX9_Init(pDevice);
			ImGui::GetIO().ImeWindowHandle = TargetWnd;
		}

	}
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	if (Menu == true)
	{
		InputHandler();
		ImGui::SetNextWindowSize(ImVec2(620.0f, 430.0f));
		ImGui::Begin("#Title", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);
		ImGui::End();
	}
	ImGui::GetIO().MouseDrawCursor = Menu;
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	return Present_orig(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

HRESULT APIENTRY EndScene_hook(IDirect3DDevice9* pDevice)
{
	if (pDevice == nullptr) return EndScene_orig(pDevice);
	//Hack:
	return EndScene_orig(pDevice);
}

HRESULT APIENTRY Reset_hook(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS *pPresentationParameters)
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	HRESULT ResetReturn = Reset_orig(pDevice, pPresentationParameters);
	ImGui_ImplDX9_CreateDeviceObjects();
	return ResetReturn;
}

HRESULT APIENTRY DrawIndexedPrimitive_hook(IDirect3DDevice9* pDevice, D3DPRIMITIVETYPE PrimType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT PrimitiveCount)
{
	//visual hack:
	return DrawIndexedPrimitive_orig(pDevice, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, PrimitiveCount);
}


HRESULT APIENTRY SetStreamSource_hook(LPDIRECT3DDEVICE9 pDevice, UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT sStride)
{
	if (StreamNumber == 0)
		Stride = sStride;

	return SetStreamSource_orig(pDevice, StreamNumber, pStreamData, OffsetInBytes, sStride);
}

DWORD WINAPI Thread(LPVOID)
{
	while (!GetModuleHandleA("d3d9.dll")) {
		Sleep(200);
	}
	IDirect3D9* d3d = Direct3DCreate9(D3D_SDK_VERSION);
	IDirect3DDevice9* d3ddev = NULL;
	HWND tmpWnd = CreateWindowA("BUTTON", "DX", WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 300, 300, NULL, NULL, Hand, NULL);
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = tmpWnd;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	HRESULT result = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, tmpWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3ddev);
    #if defined _M_X64
	DWORD64* dVtable = (DWORD64*)d3ddev;
	dVtable = (DWORD64*)dVtable[0];
    #elif defined _M_IX86
	DWORD* dVtable = (DWORD*)d3ddev;
	dVtable = (DWORD*)dVtable[0];
    #endif
	SetStreamSource_orig = (SetStreamSource)dVtable[100];
	DrawIndexedPrimitive_orig = (DrawIndexedPrimitive)dVtable[82];
	Present_orig = (Present)dVtable[17];
	EndScene_orig = (EndScene)dVtable[42];
	Reset_orig = (Reset)dVtable[16];
	Sleep(2000);
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(LPVOID&)SetStreamSource_orig, (PBYTE)SetStreamSource_hook);
	DetourAttach(&(LPVOID&)DrawIndexedPrimitive_orig, (PBYTE)DrawIndexedPrimitive_hook);
	DetourAttach(&(LPVOID&)Present_orig, (PBYTE)Present_hook);
	DetourAttach(&(LPVOID&)EndScene_orig, (PBYTE)EndScene_hook);
	DetourAttach(&(LPVOID&)Reset_orig, (PBYTE)Reset_hook);
	DetourTransactionCommit();
	d3ddev->Release();
	d3d->Release();
	DestroyWindow(tmpWnd);
	return 1;
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpvReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH) {
		CreateThread(0, 0, Thread, 0, 0, 0);
	}
	return TRUE;
}