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
	Vector4 pos;
	Vector4 n;

	double color[3];
	Material mat;
} Vertex;

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



inline int round(double x)
{
	return int(x+0.5);
}

void storeLine(const Vertex *v1, const Vertex *v2);




Vertex operator + (const Vertex &v1, const Vertex &v2);
Vertex operator * (double c, const Vertex &v2);


#endif