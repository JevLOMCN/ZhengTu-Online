#ifndef _D3DINTERFACE_H
#define _D3DINTERFACE_H

#define USE_D3D9

#ifdef USE_D3D9
#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9math.h>
#include <dxerr9.h>

#define IDS IDirect3DSurface9
#define IDT IDirect3DTexture9
#define IDD IDirect3DDevice9
#define ID3D IDirect3D9
#define IDVB IDirect3DVertexBuffer9
#define IDIB IDirect3DIndexBuffer9
#else

#define IDS IDirect3DSurface8
#define IDT IDirect3DTexture8
#define IDD IDirect3DDevice8
#define ID3D IDirect3D8

#endif

#endif