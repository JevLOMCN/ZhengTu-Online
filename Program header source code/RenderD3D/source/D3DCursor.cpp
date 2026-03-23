#include "../../engine/include/engine.h"
#include "../include/RenderD3D.h"
#include "../include/D3DDevice.h"
#include "..\include\d3dcursor.h"

CD3DCursor::CD3DCursor(void)
: mCursorBitmap(NULL)
, mShow(false)
{
	extern CD3DCursor * g_pD3DCursor;
	g_pD3DCursor = this;
	mptHot.x = 0;
	mptHot.y = 0;
	mIndex = 0;
	mCurBmps = NULL;
}

CD3DCursor::~CD3DCursor(void)
{
}

void CD3DCursor::init()
{
	//HRESULT hr;
	//if(mCursorBitmap == NULL){
	//	//hr = GetD3DDevice()->GetDevice()->CreateTexture(32,32,1,0,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&mCursorBitmap);
	//	hr = GetD3DDevice()->GetDevice()->CreateImageSurface(32,32,D3DFMT_A8R8G8B8,&mCursorBitmap);
	//	if(FAILED(hr)){
	//		TRACE_DXERROR(hr);
	//	}
	//}else{
	//	hr = GetD3DDevice()->GetDevice()->SetCursorProperties(mptHot.x,mptHot.y,mCursorBitmap);
	//	if(FAILED(hr)){
	//		TRACE_DXERROR(hr);
	//		return;
	//	}
	//}

	if(GetD3DDevice()->IsSupportHardwareCursor()){
		if((size_t)mIndex < mCursorBitmap.size() && mCursorBitmap[mIndex])
			GetD3DDevice()->__SetCursorProperties(mptHot.x,mptHot.y,mCursorBitmap[mIndex]);
	}
}

void CD3DCursor::invalidateDevice()
{
}

void CD3DCursor::release()
{
	for(size_t i=0; i<mCursorBitmap.size(); ++i)
		SAFE_RELEASE(mCursorBitmap[i]);
	mCursorBitmap.resize(0);
}

bool CD3DCursor::loadCursor(IBitmaps* pBmps,POINT ptHot)
{
	HRESULT hr;
	if(mCurBmps == pBmps)
		return true;

	mIndex = -1;

	if(!GetD3DDevice()->IsSupportHardwareCursor())
		return false;

	int iCount = pBmps->GetBitmapCount();
	while(mCursorBitmap.size() > iCount){
		SAFE_RELEASE(mCursorBitmap.back());
		mCursorBitmap.resize(mCursorBitmap.size()-1);
	}

	mCursorBitmap.resize(iCount,0);

	for(int i=0; i<iCount; ++i){
		IBitmap* pBmp = pBmps->GetBitmapByIndex(i);
		if(pBmp->GetWidth() <= 32 && pBmp->GetHeight() <= 32){
			if(mCursorBitmap[i] == NULL)
				GetD3DDevice()->__CreateImageSurface(32,32,D3DFMT_A8R8G8B8,&mCursorBitmap[i]);

			if(mCursorBitmap[i]){
				D3DLOCKED_RECT lr;
				hr = mCursorBitmap[i]->LockRect(&lr,NULL,0);
				if(SUCCEEDED(hr)){
					stRectI rcClip = pBmp->GetClipRect();
					pBmp->GetBitmapData((BYTE*)lr.pBits + rcClip.top * lr.Pitch + rcClip.left * 4,lr.Pitch);

					for(size_t y=0; y < 32; ++y){
						DWORD * pBit = (DWORD*)lr.pBits;
						for(size_t x=0; x< 32; ++x){
							stPointI pt(x,y);
							if(rcClip.PtInRect(pt)){
								if(pBit[x] > 0x7f000000){
									pBit[x] |= 0xff000000;
								}else{
									pBit[x] = 0;
								}
							}else
								pBit[x] = 0;
						}
						lr.pBits = ((BYTE*)lr.pBits + lr.Pitch);
					}

					mCursorBitmap[i]->UnlockRect();
				}
			}
		}else
			return false;
	}

	mCurBmps = pBmps;

	setCursor(0);

	return true;
}

bool CD3DCursor::loadCursor(const char* filename,POINT ptHot)
{
	/*
	HRESULT hr;
	if(!mCursorBitmap) 
		return false;
	mptHot = ptHot;

	hr = D3DXLoadSurfaceFromFile(mCursorBitmap,NULL,NULL,filename,NULL,D3DX_DEFAULT,0,NULL);
	if(FAILED(hr)){
		TRACE_DXERROR(hr);
		return false;
	}

	D3DLOCKED_RECT lr;
	hr = mCursorBitmap->LockRect(&lr,NULL,0);
	if(FAILED(hr)){
		mCursorBitmap->Release();
		TRACE_DXERROR(hr);
		return false;
	}

	for(size_t i=0; i < 32; ++i){
		DWORD * pBit = (DWORD*)lr.pBits;
		for(size_t j=0; j< 32; ++j){
			if(pBit[j] > 0x7f000000){
				pBit[j] |= 0xff000000;
			}else{
				pBit[j] = 0;
			}
		}
		lr.pBits = ((BYTE*)lr.pBits + lr.Pitch);
	}

	hr = mCursorBitmap->UnlockRect();
	if(FAILED(hr)){
		TRACE_DXERROR(hr);
		return false;
	}

	hr = GetD3DDevice()->GetDevice()->SetCursorProperties(ptHot.x,ptHot.y,mCursorBitmap);
	if(FAILED(hr)){
		TRACE_DXERROR(hr);
		return false;
	}

	return true;
	//*/
	return false;
}

void CD3DCursor::setCursor(int index)
{
	if(index == mIndex)
		return;

	mIndex = index;

	if(GetD3DDevice()->IsSupportHardwareCursor()){
		if((size_t)mIndex < mCursorBitmap.size() && mCursorBitmap[mIndex])
			GetD3DDevice()->__SetCursorProperties(mptHot.x,mptHot.y,mCursorBitmap[mIndex]);
	}
}

void CD3DCursor::run()
{
	if(mCurBmps && mShow){
		const stPackAniHeader* pHdr = mCurBmps->GetAniInfo();
		int iFrame;
		if(pHdr->wdFrameCount <= 1) 
			iFrame = 0;
		else
			iFrame = (xtimeGetTime() / pHdr->wdAniSpeed) % pHdr->wdFrameCount;
		int index = mCurBmps->GetFrameIndex(iFrame);
		setCursor(index);
	}
}

void CD3DCursor::showCursor(bool bShow)
{
	mShow = bShow;
	BOOL bShowCursor = GetD3DDevice()->__ShowCursor(bShow);
}