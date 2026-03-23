#pragma once

#include "ExitWindows.h"

void HackError();


//防调试宏
inline void __tryint()
{
	_asm
	{
		cmp eax,eax;
		je	MyInt;
		__emit	0xff;
MyInt:
		nop;
		int	3;
	}
}

#if defined	_DEBUG || defined _AUTOPLAY || defined RELEASE_TEST
#undef USE_DEFENCE
#else
#define USE_DEFENCE
#endif

#define		FORCE_SHUT_DOWN		{CExitWindows ew;ew.ForceShutDown();}

#if defined USE_DEFENCE
#define		DEBUG_INT_AT_FUNCTION_BEGIN()						__try{__tryint();HackError();}__except(GetExceptionCode() == 0x80000003){
#define		DEBUG_INT_AT_FUNCTION_BEGIN1(ERROR)					__try{__tryint();ERROR; }__except(GetExceptionCode() == 0x80000003){
#define		DEBUG_INT_AT_FUNCTION_BEGIN2(_a,ERROR)				__try{ {_asm cmp eax,eax _asm je _a	_asm __emit 0xff _asm _a: _asm nop _asm int 3 } ERROR}__except(GetExceptionCode() == 0x80000003){
#define		DEBUG_INT_AT_FUNCTION_BEGIN_FORCE_SHUT_DOWN()		DEBUG_INT_AT_FUNCTION_BEGIN1(FORCE_SHUT_DOWN)
#define		DEBUG_INT_AT_FUNCTION_BEGIN_FORCE_SHUT_DOWN1(a)		DEBUG_INT_AT_FUNCTION_BEGIN2(a,FORCE_SHUT_DOWN)
#define		DEBUG_INT_AT_FUNCTION_END					}
#else
#define		DEBUG_INT_AT_FUNCTION_BEGIN()					__noop;
#define		DEBUG_INT_AT_FUNCTION_BEGIN1(ERROR)				__noop;
#define		DEBUG_INT_AT_FUNCTION_BEGIN2(_a,ERROR)			__noop;
#define		DEBUG_INT_AT_FUNCTION_BEGIN_FORCE_SHUT_DOWN		__noop;
#define		DEBUG_INT_AT_FUNCTION_BEGIN_FORCE_SHUT_DOWN1(a)	__noop;
#define		DEBUG_INT_AT_FUNCTION_END						__noop;
#endif

extern unsigned char	g_Code[4];
#define	GET_FUNCTION_CODE		((g_Code[0]&g_Code[3]|g_Code[2]&g_Code[1])%8)			//稍微改动了一下算子的计算方式
#define SET_FUNCTION_CODE		{srand(xtimeGetTime());memset(g_Code,rand(),4);}		//随机修改算子

class	CSetFunctionCode
{
public:
	CSetFunctionCode(){}
	~CSetFunctionCode(){SET_FUNCTION_CODE;}	//在析构时才修改g_Code的值,避免函数退出时不修改g_Code
};
//函数开头模块
#define FUNCTION_ERROR						HackError();return;
#define FUNCTION_ERROR_R(rt)				HackError();return rt;
#define FUNCTION_DEFINE_BEGIN(index)		if(GET_FUNCTION_CODE != index){FUNCTION_ERROR;}CSetFunctionCode __code;
#define FUNCTION_DEFINE_BEGIN_R(index,rt)	if(GET_FUNCTION_CODE != index){FUNCTION_ERROR_R(rt);}CSetFunctionCode __code;

//测试用的函数主体代码
#define	DEBUG_FUNCTION						TRACE("call:");TRACE(__FUNCTION__);TRACE("\n");


/**************************************  公共宏定义 ***************************************/
//定义函数指针数组
#define FUNCTION_ARRAY(type,name)			type lst##name[8]={name##0,name##1,name##2,name##3,name##4,name##5,name##6,name##7};
#define EXTERN_FUNCTION_ARRAY(type,name)		extern type lst##name[8];
/**************************************  公共宏定义 ***************************************/

/**************************************  全局函数相关宏定义 ***************************************/
//函数申明模块
#define FUNCTION_DECLARE(name,index)		void name##index ();

//函数定义模块
#define FUNCTION_DEFINE(name,index,code)	void name##index () {FUNCTION_DEFINE_BEGIN(index);code;}

//同时申明8个函数
#define FUNCTION_DECLARE_8(name)			FUNCTION_DECLARE(name,0)		FUNCTION_DECLARE(name,1)		FUNCTION_DECLARE(name,2)	FUNCTION_DECLARE(name,3)			FUNCTION_DECLARE(name,4)		FUNCTION_DECLARE(name,5)		FUNCTION_DECLARE(name,6)		FUNCTION_DECLARE(name,7)

//同时定义8个函数
#define FUNCTION_DEFINE_8(name,code)		FUNCTION_DEFINE(name,0,code)	FUNCTION_DEFINE(name,1,code)	FUNCTION_DEFINE(name,2,code)	FUNCTION_DEFINE(name,3,code)	FUNCTION_DEFINE(name,4,code)	FUNCTION_DEFINE(name,5,code)	FUNCTION_DEFINE(name,6,code)	FUNCTION_DEFINE(name,7,code)

//调用函数
#define	FUNCTION_CALL(name)					lst##name[GET_FUNCTION_CODE]();

///带1个参数的宏
#define FUNCTION_DECLARE1(n,i,T)			void n##i (T);
#define FUNCTION_DECLARE1_8(n,T)			FUNCTION_DECLARE1(n,0,T)		FUNCTION_DECLARE1(n,1,T)		FUNCTION_DECLARE1(n,2,T)		FUNCTION_DECLARE1(n,3,T)			FUNCTION_DECLARE1(n,4,T)		FUNCTION_DECLARE1(n,5,T)		FUNCTION_DECLARE1(n,6,T)		FUNCTION_DECLARE1(n,7,T)
#define FUNCTION_DEFINE1(n,i,T,t,code)		void n##i (T t) {FUNCTION_DEFINE_BEGIN(i);code;}
#define FUNCTION_DEFINE1_8(n,T,t,code)		FUNCTION_DEFINE1(n,0,T,t,code)	FUNCTION_DEFINE1(n,1,T,t,code)	FUNCTION_DEFINE1(n,2,T,t,code)	FUNCTION_DEFINE1(n,3,T,t,code)		FUNCTION_DEFINE1(n,4,T,t,code)	FUNCTION_DEFINE1(n,5,T,t,code)	FUNCTION_DEFINE1(n,6,T,t,code)	FUNCTION_DEFINE1(n,7,T,t,code)
#define	FUNCTION_CALL1(n,t)					lst##n[GET_FUNCTION_CODE](t);

///带2个参数的宏
#define FUNCTION_DECLARE2(n,i,T1,T2)			void n##i (T1,T2);
#define FUNCTION_DECLARE2_8(n,T1,T2)			FUNCTION_DECLARE2(n,0,T1,T2)		FUNCTION_DECLARE2(n,1,T1,T2)		FUNCTION_DECLARE2(n,2,T1,T2)		FUNCTION_DECLARE2(n,3,T1,T2)			FUNCTION_DECLARE2(n,4,T1,T2)		FUNCTION_DECLARE2(n,5,T1,T2)		FUNCTION_DECLARE2(n,6,T1,T2)		FUNCTION_DECLARE2(n,7,T1,T2)
#define FUNCTION_DEFINE2(n,i,T1,t1,T2,t2,code)	void n##i (T1 t1,T2 t2) {FUNCTION_DEFINE_BEGIN(i);code;}
#define FUNCTION_DEFINE2_8(n,T1,t1,T2,t2,code)	FUNCTION_DEFINE2(n,0,T1,t1,T2,t2,code)	FUNCTION_DEFINE2(n,1,T1,t1,T2,t2,code)	FUNCTION_DEFINE2(n,2,T1,t1,T2,t2,code)	FUNCTION_DEFINE2(n,3,T1,t1,T2,t2,code)		FUNCTION_DEFINE2(n,4,T1,t1,T2,t2,code)	FUNCTION_DEFINE2(n,5,T1,t1,T2,t2,code)	FUNCTION_DEFINE2(n,6,T1,t1,T2,t2,code)	FUNCTION_DEFINE2(n,7,T1,t1,T2,t2,code)
#define	FUNCTION_CALL2(n,t1,t2)					lst##n[GET_FUNCTION_CODE](t1,t2);


///带1个参数的宏return
#define FUNCTION_DECLARE1_R(function_name,function_index,function_param1,return_type)			return_type function_name##function_index(function_param1);
#define FUNCTION_DECLARE1_R_8(function_name,function_param1,return_type)			FUNCTION_DECLARE1_R(function_name,0,function_param1,return_type)		FUNCTION_DECLARE1_R(function_name,1,function_param1,return_type)		FUNCTION_DECLARE1_R(function_name,2,function_param1,return_type)		FUNCTION_DECLARE1_R(function_name,3,function_param1,return_type)			FUNCTION_DECLARE1_R(function_name,4,function_param1,return_type)		FUNCTION_DECLARE1_R(function_name,5,function_param1,return_type)		FUNCTION_DECLARE1_R(function_name,6,function_param1,return_type)		FUNCTION_DECLARE1_R(function_name,7,function_param1,return_type)
#define FUNCTION_DEFINE1_R(funtion_name,function_index,param1_type,param1_value,return_type,return_value_error,code)		return_type funtion_name##function_index (param1_type param1_value) {FUNCTION_DEFINE_BEGIN_R(function_index,return_value_error);code;}
#define FUNCTION_DEFINE1_R_8(n,T,t,rt,dr,code)		FUNCTION_DEFINE1_R(n,0,T,t,rt,dr,code)	FUNCTION_DEFINE1_R(n,1,T,t,rt,dr,code)	FUNCTION_DEFINE1_R(n,2,T,t,rt,dr,code)	FUNCTION_DEFINE1_R(n,3,T,t,rt,dr,code)		FUNCTION_DEFINE1_R(n,4,T,t,rt,dr,code)	FUNCTION_DEFINE1_R(n,5,T,t,rt,dr,code)	FUNCTION_DEFINE1_R(n,6,T,t,rt,dr,code)	FUNCTION_DEFINE1_R(n,7,T,t,rt,dr,code)
#define	FUNCTION_CALL1_R(n,t,rt)			lst##n[GET_FUNCTION_CODE](t);

//带2个参数有返回值的宏
#define FUNCTION_DECLARE2_R(n,i,T1,T2,rT)					rT n##i(T1,T2);
#define FUNCTION_DECLARE2_R_8(n,T1,T2,rT)					FUNCTION_DECLARE2_R(n,0,T1,T2,rT)	\
															FUNCTION_DECLARE2_R(n,1,T1,T2,rT)	\
															FUNCTION_DECLARE2_R(n,2,T1,T2,rT)	\
															FUNCTION_DECLARE2_R(n,3,T1,T2,rT)	\
															FUNCTION_DECLARE2_R(n,4,T1,T2,rT)	\
															FUNCTION_DECLARE2_R(n,5,T1,T2,rT)	\
															FUNCTION_DECLARE2_R(n,6,T1,T2,rT)	\
															FUNCTION_DECLARE2_R(n,7,T1,T2,rT)	

#define FUNCTION_DEFINE2_R(n,i,T1,t1,T2,t2,code,rT,rt)		rT n##i(T1 t1,T2 t2) {FUNCTION_DEFINE_BEGIN_R(i,rt);code;return rt;}
#define FUNCTION_DEFINE2_R_8(n,T1,t1,T2,t2,code,rT,rt)		FUNCTION_DEFINE2_R(n,0,T1,t1,T2,t2,code,rT,rt)	\
															FUNCTION_DEFINE2_R(n,1,T1,t1,T2,t2,code,rT,rt)	\
															FUNCTION_DEFINE2_R(n,2,T1,t1,T2,t2,code,rT,rt)	\
															FUNCTION_DEFINE2_R(n,3,T1,t1,T2,t2,code,rT,rt)	\
															FUNCTION_DEFINE2_R(n,4,T1,t1,T2,t2,code,rT,rt)	\
															FUNCTION_DEFINE2_R(n,5,T1,t1,T2,t2,code,rT,rt)	\
															FUNCTION_DEFINE2_R(n,6,T1,t1,T2,t2,code,rT,rt)	\
															FUNCTION_DEFINE2_R(n,7,T1,t1,T2,t2,code,rT,rt)	

#define FUNCTION_CALL2_R(n,t1,t2)							lst##n[GET_FUNCTION_CODE](t1,t2);
/**************************************  全局函数相关宏定义 ***************************************/

/**************************************  成员函数相关宏定义 ***************************************/
//成员函数定义模块
#define M_FUNCTION_DEFINE(C,name,i,code)	void C::name##i () {FUNCTION_DEFINE_BEGIN(i);code;}

//同时申明8个成员函数
#define M_FUNCTION_DECLARE_8				FUNCTION_DECLARE_8

//同时定义8个成员函数
#define M_FUNCTION_DEFINE_8(C,name,code)	M_FUNCTION_DEFINE(C,name,0,code)	M_FUNCTION_DEFINE(C,name,1,code)	M_FUNCTION_DEFINE(C,name,2,code)	M_FUNCTION_DEFINE(C,name,3,code)	M_FUNCTION_DEFINE(C,name,4,code)	M_FUNCTION_DEFINE(C,name,5,code)	M_FUNCTION_DEFINE(C,name,6,code)	M_FUNCTION_DEFINE(C,name,7,code)

//内联成员函数定义
#define M_INLINE_FUNCTION(name,i,code)		void name##i () {FUNCTION_DEFINE_BEGIN(i);code;}

//同时定义8个内联函数
#define M_INLINE_FUN_DEFINE_8(name,code)	FUNCTION_DEFINE_8(name,code)

//申明私有的成员函数指针数组
#define M_FUNCTION_ARRAY_DECLARE(type,name)	type lst##name[8]

//初始化成员函数指针数组(在构造函数内调用)
#define M_FUNCTION_ARRAY_DEFINE(type,name)	{lst##name[0]=name##0;lst##name[1]=name##1;lst##name[2]=name##2;lst##name[3]=name##3;lst##name[4]=name##4;lst##name[5]=name##5;lst##name[6]=name##6;lst##name[7]=name##7;}

//成员函数调用函数定义
#define M_FUNCTION_CALL_DEFINE(name)		void Call##name (int i) {(this->*lst##name[i])();}

//成员函数内部调用
#define M_FUNCTION_CALL(name)				Call##name(GET_FUNCTION_CODE);	

//成员函数对象方式调用
#define M_FUNCTION_CALL_O(o,name)			o.##M_FUNCTION_CALL(name)

//成员函数指针方式调用
#define M_FUNCTION_CALL_P(p,name)			p->##M_FUNCTION_CALL(name)


///有1个参数的宏
#define M_FUNCTION_DECLARE1(name,i,T)		void name##i(T);
#define M_FUNCTION_DECLARE1_8(name,T)		M_FUNCTION_DECLARE1(name,0,T)		M_FUNCTION_DECLARE1(name,1,T)		M_FUNCTION_DECLARE1(name,2,T)	M_FUNCTION_DECLARE1(name,3,T)			M_FUNCTION_DECLARE1(name,4,T)		M_FUNCTION_DECLARE1(name,5,T)		M_FUNCTION_DECLARE1(name,6,T)		M_FUNCTION_DECLARE1(name,7,T)
#define M_FUNCTION_DEFINE1(C,n,i,T,t,code)	void C::n##i (T t) {FUNCTION_DEFINE_BEGIN(i);code;}
#define M_FUNCTION_DEFINE1_8(C,n,T,t,code)	M_FUNCTION_DEFINE1(C,n,0,T,t,code)	M_FUNCTION_DEFINE1(C,n,1,T,t,code)	M_FUNCTION_DEFINE1(C,n,2,T,t,code)	M_FUNCTION_DEFINE1(C,n,3,T,t,code)	M_FUNCTION_DEFINE1(C,n,4,T,t,code)	M_FUNCTION_DEFINE1(C,n,5,T,t,code)	M_FUNCTION_DEFINE1(C,n,6,T,t,code)	M_FUNCTION_DEFINE1(C,n,7,T,t,code)
#define M_FUNCTION_CALL_DEFINE1(name,T)		void Call##name (int i,T __t) {(this->*lst##name[i])(__t);}
#define M_FUNCTION_CALL1(name,t)			Call##name(GET_FUNCTION_CODE,t);
#define M_FUNCTION_CALL1_O(o,name,t)		o.##M_FUNCTION_CALL1(name,t)
#define M_FUNCTION_CALL1_P(p,name,t)		p->##M_FUNCTION_CALL1(name,t)

///有2个参数的宏
#define M_FUNCTION_DECLARE2(name,i,T1,T2)			void name##i(T1,T2);
#define M_FUNCTION_DECLARE2_8(name,T1,T2)			M_FUNCTION_DECLARE2(name,0,T1,T2)		M_FUNCTION_DECLARE2(name,1,T1,T2)		M_FUNCTION_DECLARE2(name,2,T1,T2)	M_FUNCTION_DECLARE2(name,3,T1,T2)			M_FUNCTION_DECLARE2(name,4,T1,T2)		M_FUNCTION_DECLARE2(name,5,T1,T2)		M_FUNCTION_DECLARE2(name,6,T1,T2)		M_FUNCTION_DECLARE2(name,7,T1,T2)
#define M_FUNCTION_DEFINE2(C,n,i,T1,t1,T2,t2,code)	void C::n##i (T1 t1,T2 t2) {FUNCTION_DEFINE_BEGIN(i);code;}
#define M_FUNCTION_DEFINE2_8(C,n,T1,t1,T2,t2,code)	M_FUNCTION_DEFINE2(C,n,0,T1,t1,T2,t2,code)	M_FUNCTION_DEFINE2(C,n,1,T1,t1,T2,t2,code)	M_FUNCTION_DEFINE2(C,n,2,T1,t1,T2,t2,code)	M_FUNCTION_DEFINE2(C,n,3,T1,t1,T2,t2,code)	M_FUNCTION_DEFINE2(C,n,4,T1,t1,T2,t2,code)	M_FUNCTION_DEFINE2(C,n,5,T1,t1,T2,t2,code)	M_FUNCTION_DEFINE2(C,n,6,T1,t1,T2,t2,code)	M_FUNCTION_DEFINE2(C,n,7,T1,t1,T2,t2,code)
#define M_FUNCTION_CALL_DEFINE2(name,T1,T2)			void Call##name (int i,T1 __t1,T2 __t2) {(this->*lst##name[i])(__t1,__t2);}
#define M_FUNCTION_CALL2(name,t1,t2)				Call##name(GET_FUNCTION_CODE,t1,t2);
#define M_FUNCTION_CALL2_O(o,name,t1,t2)			o.##M_FUNCTION_CALL2(name,t1,t2)
#define M_FUNCTION_CALL2_P(p,name,t1,t2)			p->##M_FUNCTION_CALL2(name,t1,t2)

///有1个参数的有返回值的宏
#define M_FUNCTION_DECLARE1_R(name,i,T,rt)		rt name##i(T);
#define M_FUNCTION_DECLARE1_R_8(name,T,rt)		M_FUNCTION_DECLARE1_R(name,0,T,rt)		M_FUNCTION_DECLARE1_R(name,1,T,rt)		M_FUNCTION_DECLARE1_R(name,2,T,rt)	M_FUNCTION_DECLARE1_R(name,3,T,rt)			M_FUNCTION_DECLARE1_R(name,4,T,rt)		M_FUNCTION_DECLARE1_R(name,5,T,rt)		M_FUNCTION_DECLARE1_R(name,6,T,rt)		M_FUNCTION_DECLARE1_R(name,7,T,rt)
#define M_FUNCTION_DEFINE1_R(C,n,i,T,t,RT,rt,c)	RT C::n##i (T t) {FUNCTION_DEFINE_BEGIN_R(i,rt);c;}
#define M_FUNCTION_DEFINE1_R_8(C,n,T,t,RT,rt,c)	M_FUNCTION_DEFINE1_R(C,n,0,T,t,RT,rt,c)	M_FUNCTION_DEFINE1_R(C,n,1,T,t,RT,rt,c)	M_FUNCTION_DEFINE1_R(C,n,2,T,t,RT,rt,c)	M_FUNCTION_DEFINE1_R(C,n,3,T,t,RT,rt,c)	M_FUNCTION_DEFINE1_R(C,n,4,T,t,RT,rt,c)	M_FUNCTION_DEFINE1_R(C,n,5,T,t,RT,rt,c)	M_FUNCTION_DEFINE1_R(C,n,6,T,t,RT,rt,c)	M_FUNCTION_DEFINE1_R(C,n,7,T,t,RT,rt,c)
#define M_FUNCTION_CALL_DEFINE1_R(name,T,rt)	rt Call##name (int i,T __t) {return (this->*lst##name[i])(__t);}

///函数指针初始化段
#define M_FUNCTION_ARRAY_INIT_BEGIN			void	init__functionarray(){
#define M_FUNCTION_ARRAY_INIT_END			}

#if defined USE_DEFENCE
#define M_FUNCTION_ARRAY_INIT				init__functionarray();
#else
#define M_FUNCTION_ARRAY_INIT				__noop;
#endif
/**************************************  成员函数相关宏定义 ***************************************/