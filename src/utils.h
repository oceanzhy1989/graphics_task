#ifndef __UTILS_H__
#define __UTILS_H__

#include "matrix.h"
#include "graphicsAPI.h"

typedef struct
{
	
	double Ka;
	double Kd;
	double Ks;  //������Ks+͸����T+������a = 1 , �����������Kd��ռKs���еı���
	double T;
	double n;
	double alpha[3];
	double roughn;
} Material;

typedef struct
{
	Vector4 pos;
	Vector4 n;
	int triangle_count;

	double I[3];//����

	double color[3];//rgba
	Material mat;
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

typedef struct
{
	double a[2];
} Point2D;

typedef struct makeBuffer
{
	double I[3];
	double alpha[3];
	double depth;

	makeBuffer(){}

	makeBuffer(const Vertex &v)
	{
		memcpy(I,v.I,3*sizeof(double));
		memcpy(alpha,v.mat.alpha,3*sizeof(double));
		depth=v.pos.a[2];
	}
} IllumWithDepth;

inline int round(double x)
{
	return int(x+0.5);
}




Material newMaterial(double Ka, double Kd, double Ks, double T=0, double n=1.001, double a=0, double *alpha=NULL);

Vertex operator + (const Vertex &v1, const Vertex &v2);
Vertex operator * (double c, const Vertex &v2);

IllumWithDepth operator + (const IllumWithDepth &v1, const IllumWithDepth &v2);
IllumWithDepth operator * (double c, const IllumWithDepth &v2);



#endif