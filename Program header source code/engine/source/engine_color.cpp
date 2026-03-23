#include "../include/engine.h"
#include "../Include/engine_color.h"

/********************************************************************
	创建时间:	2004/12/22   14:58
	文件名: 	engine_color.cpp
	文件类型:	cpp
	作者:		$$$03公司
	
	用途:	
*********************************************************************/

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param color1 : 描述
 * \param color2 : 描述
 * \param color3 : 描述
 * \return 返回值的描述
 */
static unsigned __int64 s_mask0 = 0x00000000000000ff;
static unsigned __int64 s_mask1 = 0x000000000000ff00;
static unsigned __int64 s_mask2 = 0x0000000000ff0000;
static unsigned __int64 s_mask3 = 0x00000000ff000000;
// out = color1 + color3 * (color2 - color1)
DWORD _ColorLerp(DWORD color1,DWORD color2,DWORD color3)
{
	_asm
	{
		movd mm0,color2;	// 00 00 00 00 a3 a2 a1 a0
		movd mm1,color1;	// 00 00 00 00 b3 b2 b1 b0
		pxor mm2,mm2;
		movd mm3,color3

		punpcklbw mm0,mm2;//00a3 00a2 00a1 00a0  color1
		punpcklbw mm1,mm2;//00b3 00b2 00b1 00b0  color2
		punpcklbw mm3,mm2;//00b3 00b2 00b1 00b0  color3

		//psubusw mm0,mm1; // color2 - color1
		psubw mm0,mm1; // color2 - color1
		pmullw  mm0,mm3; // color3 * ( color2 - color1 )
		psrlw mm0,8;	// mm0>>8  00c3 00c2 00c1 00c1
		paddsw mm0,mm1; // color1 + color3 * (color2 - color1)

		movq mm3,mm0;
		movq mm4,mm0;
		movq mm5,mm0;
		psrlq mm3,8;
		psrlq mm4,16;
		psrlq mm5,24;
		pand mm3,s_mask1;
		pand mm4,s_mask2;
		pand mm5,s_mask3;
		pand mm0,s_mask0;
		por mm3,mm4;
		por mm0,mm5;
		por mm0,mm3;
		movd eax,mm0;
		emms;
	}
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param color1 : 描述
 * \param color2 : 描述
 * \return 返回值的描述
 */
DWORD _ColorBlend(DWORD color1,DWORD color2)
{
	_asm
	{
		/*
		mov eax,color1;
		mov ebx,color2;
		cmp eax,0
		jne _cal0;
		jmp _out;

		_cal0:	cmp eax,0xffffffff;
		jne _cal1;
		mov eax,ebx;
		jmp _out;

		_cal1:	cmp ebx,0;
		jne _cal2;
		xor eax,eax;
		jmp _out;

		_cal2:	cmp ebx,0xffffffff;
		jne _cal3;
		jmp _out;
		//*/

_cal3:	movd mm0,color1;	// 00 00 00 00 a3 a2 a1 a0
		pxor mm2,mm2;
		movd mm1,color2;	// 00 00 00 00 b3 b2 b1 b0
		punpcklbw mm0,mm2;//00a3 00a2 00a1 00a0
		punpcklbw mm1,mm2;//00b3 00b2 00b1 00b0

		pmullw mm0 ,mm1;// mm0 = mm0 * mm1
		psrlw mm0,8;	// mm0>>8	
		movq mm3,mm0;
		movq mm4,mm0;
		movq mm5,mm0;
		psrlq mm3,8;
		psrlq mm4,16;
		psrlq mm5,24;
		pand mm3,s_mask1;
		pand mm4,s_mask2;
		pand mm5,s_mask3;
		pand mm0,s_mask0;
		por mm3,mm4;
		por mm0,mm5;
		por mm0,mm3;
		movd eax,mm0;
		emms;
_out:
		;
	}

	/*
	UINT a,r,g,b;

	if(color1 == 0) return 0;
	if(color2 == 0) return 0;
	if(color1 == 0xffffffff) return color2;
	if(color2 == 0xffffffff) return color1;

	a = (COLOR_GET_ALPHA(color1) * COLOR_GET_ALPHA(color2)) >> 8;

	r = (COLOR_GET_R(color1) * COLOR_GET_R(color2)) >> 8;

	g = (COLOR_GET_G(color1) * COLOR_GET_G(color2)) >> 8;

	b = (COLOR_GET_B(color1) * COLOR_GET_B(color2)) >> 8;

	return COLOR_ARGB(a,r,g,b);
	//*/
}