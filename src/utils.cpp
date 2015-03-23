#include "utils.h"



Vertex operator + (const Vertex &v1, const Vertex &v2)
{
	Vertex tmp;

	tmp.pos=v1.pos+v2.pos;
	tmp.n=v1.n+v2.n;

	tmp.mat.T=v1.mat.T+v2.mat.T;
	tmp.mat.Ka=v1.mat.Ka+v2.mat.Ka;
	tmp.mat.Kd=v1.mat.Kd+v2.mat.Kd;
	tmp.mat.Ks=v1.mat.Ks+v2.mat.Ks;
	tmp.mat.n=v1.mat.n+v2.mat.n;
	tmp.mat.roughn=v1.mat.roughn+v2.mat.roughn;

	for(int i=0;i<3;i++)
	{
		tmp.mat.alpha[i]=v1.mat.alpha[i]+v2.mat.alpha[i];
		tmp.color[i]=v1.color[i]+v2.color[i];
	}

	return tmp;
}
Vertex operator * (double c, const Vertex &v2)
{
	Vertex tmp;

	tmp.pos=c*v2.pos;
	tmp.n=c*v2.n;

	tmp.mat.T=c*v2.mat.T;
	tmp.mat.Ka=c*v2.mat.Ka;
	tmp.mat.Kd=c*v2.mat.Kd;
	tmp.mat.Ks=c*v2.mat.Ks;
	tmp.mat.n=c*v2.mat.n;
	tmp.mat.roughn=c*v2.mat.roughn;

	for(int i=0;i<3;i++)
	{
		tmp.mat.alpha[i]=c*v2.mat.alpha[i];
		tmp.color[i]=c*v2.color[i];
	}

	return tmp;
}

IllumWithDepth operator + (const IllumWithDepth &v1, const IllumWithDepth &v2)
{
	IllumWithDepth tmp;

	tmp.depth=v1.depth+v2.depth;
	for(int i=0;i<3;i++)
	{
		tmp.alpha[i]=v1.alpha[i]+v2.alpha[i];
		tmp.I[i]=v1.I[i]+v2.I[i];
	}

	return tmp;
}
IllumWithDepth operator * (double c, const IllumWithDepth &v2)
{
	IllumWithDepth tmp;

	tmp.depth=c*v2.depth;
	for(int i=0;i<3;i++)
	{
		tmp.alpha[i]=c*v2.alpha[i];
		tmp.I[i]=c*v2.I[i];
	}

	return tmp;
}
