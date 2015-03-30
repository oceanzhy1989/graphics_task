#ifndef __UTILS_H__
#define __UTILS_H__

#include "matrix.h"
#include "graphicsAPI.h"

typedef struct
{
	
	double Ka;
	double Kd;
	double Ks;  //反射率Ks+透射率T+吸收率a = 1 , 其中漫反射光Kd是占Ks光中的比例
	double T;
	double n;
	double alpha[3];
	double roughn;
} Material;

typedef struct
{
	double u;
	double v;
} TextureCoord;

typedef struct
{
	Vector4 pos;
	Vector4 n;
	int triangle_count;

	double I[3];//光照

	double color[3];//rgba
	Material mat;
	int tex_id;
	TextureCoord tex_coord;

	unsigned char rcode;//区域码
} Vertex;

typedef struct
{
	double I[3];

	Vector4 direction;
} LightSource;

typedef int VertexID;
typedef struct
{
	Vector4 n;
	VertexID vert[3];
} Triangle;



typedef struct makeBuffer
{
	double I[3];
	double alpha[3];
	double depth;

	int tex_id;
	double tex_u;
	double tex_v;


	makeBuffer(){}

	inline makeBuffer(const Vertex &v, double Depth, double coe)
	{
		for(int i=0;i<3;i++)
		{
			I[i]=v.I[i]*coe;
			alpha[i]=v.mat.alpha[i]*coe;
		}
		tex_id=v.tex_id;
		tex_u=v.tex_coord.u*coe;
		tex_v=v.tex_coord.v*coe;
		depth=Depth*coe;
	}
} IllumWithDepth;

inline int round(double x)
{
	return int(x+0.5);
}

inline unsigned char sgn(const double &x)
{
	return x>0?1:0;
}




Material newMaterial(double Ka, double Kd, double Ks, double T=0, double n=1.001, double a=0, double *alpha=NULL);

Vertex operator + (const Vertex &v1, const Vertex &v2);
Vertex operator * (double c, const Vertex &v2);

bool cutLine(const Vector &v1, const Vector &v2, int x, double &targetY);
bool lineinsblock(const Vector &startv, const Vector &endv, double bound[], double &tNear, double &tFar);

IllumWithDepth operator + (const IllumWithDepth &v1, const IllumWithDepth &v2);
IllumWithDepth operator * (double c, const IllumWithDepth &v2);
inline void operator *= ( IllumWithDepth &v, double c)
{
	v.depth*=c;
	v.tex_u*=c;
	v.tex_v*=c;
	for(int i=0;i<3;i++)
	{
		v.alpha[i]*=c;
		v.I[i]*=c;
	}
}

void operator *= (Vertex &v,double c);



#endif