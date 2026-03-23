#ifndef _MATH_EX_H
#define _MATH_EX_H

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class stVector2{
public:
	int x,y;

	stVector2(int ix,int iy) : x(ix) , y(iy){}
	stVector2(){}
	stVector2(const stVector2 & v) : x(v.x) , y(v.y) {}

	stVector2 & operator = (const stVector2 & v)
	{
		x = v.x;
		y = v.y;
		return *this;
	}

	bool operator == (const stVector2 & v) const
	{
		return (x == v.x && y == v.y);
	}

	stVector2 operator + (const stVector2 & v) const
	{
		return stVector2(x + v.x,y + v.y);
	}

	stVector2 operator - (const stVector2 & v) const
	{
		return stVector2(x - v.x,y - v.y);
	}

	// 平行四边形的带符号的面积
	//若 P × Q > 0 , 则P在Q的顺时针方向。
	//若 P × Q < 0 , 则P在Q的逆时针方向。
	//若 P × Q = 0 , 则P与Q共线，但可能同向也可能反向。
	int Cross(const stVector2 & v) const
	{
		return x * v.y - v.x * y;
	}
};

struct stLine{
	float a,b,c;

	void SetLine(stPointI pt1,stPointI pt2)
	{
		int dx,dy;
		dx = pt1.x - pt2.x;
		dy = pt1.y - pt2.y;
		a = (float)dy;
		b = (float)-dx;
		c = (float)(pt1.x * pt2.y - pt2.x * pt1.y);
		float x1 = (float)(pt1.x * a + pt1.y * b + c);
		float x2 = (float)(pt2.x * a + pt2.y * b + c);
		Assert(fabs(x1) < 0.0001f && fabs(x2) < 0.0001f);
	}

	stLine(stPointI pt1,stPointI pt2)
	{
		SetLine(pt1,pt2);
	}

	stLine(const stLine & line)
		: a(line.a) , b(line.b) , c(line.c)
	{}

	stLine(POINT pt,float fAngle)
	{
		POINT pt2;
		float tg;
		if(fabs(fAngle - PI/2) < 0.001 || fabs(fAngle - 270 * PI/180) < 0.001)
		{
			pt2.x = pt.x;
			pt2.y = pt.y + 64;
		}
		else
		{
			tg = tanf(fAngle);
			pt2.x = pt.x + 64; 
			pt2.y = pt.y + (long)(64 * tg);
		}
		SetLine(pt,pt2);
	}

	stLine() {}

	stLine & operator = (const stLine & line)
	{
		a = line.a;
		b = line.b;
		c = line.c;
	}

	bool IsAtLine(POINT pt) const
	{
		return abs(a * pt.x + b * pt.y + c) < 0.001;
	}

	bool Intersection( const stLine & line,POINT & pt ) const
	{
		float v1 = b * line.a - line.b * a;
		if(fabs(v1) < 0.0001) return false;

		if(fabs(a) < 0.0001)
		{
			pt.y = (long)((-c)/b);
			pt.x = (long)((-line.c - line.b * pt.y)/line.a);
			return true;
		}
		if(fabs(line.a) < 0.0001)
		{
			pt.y = (long)((-line.c)/line.b);
			pt.x = (long)((-c - b * pt.y)/a);
			return true;
		}
		if(fabs(b) < 0.0001)
		{
			pt.x = (long)((-c)/a);
			pt.y = (long)((-line.c - line.a * pt.x)/line.b);
			return true;
		}
		if(fabs(line.b) < 0.0001)
		{
			pt.x = (long)((-line.c)/line.a);
			pt.y = (long)((-c - a * pt.x)/b);
			return true;
		}


		pt.y = (long)((line.c * a - c * line.a)/v1);
		pt.x = (long)((-c - b * pt.y) / a);

		float x1 = pt.x * a + pt.y * b + c;
		float x2 = pt.x * line.a + pt.y * line.b + line.c;
		Assert(fabs(x1) < 0.0001 && fabs(x2) < 0.0001);

		return true;
	}


	bool Intersection( const RECT * prect,POINT& pt1,POINT& pt2 ) const
	{
		stPointI pt0[4][2] = {	{ stPointI(prect->left,prect->top),stPointI(prect->right ,prect->top) },
		{ stPointI(prect->right,prect->top),stPointI(prect->right ,prect->bottom) },
		{ stPointI(prect->right,prect->bottom ),stPointI(prect->left,prect->bottom ) },
		{ stPointI(prect->left,prect->bottom ),stPointI(prect->left,prect->top) },
		};

		stLine line[4] = {	stLine( stPointI(prect->left,prect->top),stPointI(prect->right ,prect->top) ),
			stLine( stPointI(prect->right,prect->top),stPointI(prect->right ,prect->bottom ) ),
			stLine( stPointI(prect->right,prect->bottom ),stPointI(prect->left,prect->bottom ) ),
			stLine( stPointI(prect->left,prect->bottom ),stPointI(prect->left,prect->top) ),
		};

		POINT pt[2];
		int ix = -1;

		for(int i=0;i<4;++i)
		{
			stPointI pt3;
			if( Intersection( line[i],pt3))
			{
				if( ( (pt0[i][0].x == pt0[i][1].x) || (pt3.x >= getMin( pt0[i][0].x,pt0[i][1].x ) && pt3.x <= getMax( pt0[i][0].x,pt0[i][1].x ) ) ) 
				&& ( (pt0[i][0].y == pt0[i][1].y) || (pt3.y >= getMin( pt0[i][0].y,pt0[i][1].y ) && pt3.y <= getMax( pt0[i][0].y,pt0[i][1].y ) ) ) 
				&& (ix == -1 || pt[0].x != pt3.x || pt[0].y != pt3.y ) )
				{
					pt[++ix]  = pt3;
					if( ix == 1 ) break;
				}
			}
		}

		if(ix < 1) return false;

		pt1 = pt[0];
		pt2 = pt[1];

		return true;
	}
};


#endif