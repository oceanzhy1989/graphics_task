#include "utils.h"


Material newMaterial(double Ka, double Kd, double Ks, double T, double n, double a, double *alpha)
{
	Material tmp;

	tmp.Ka=Ka;	
	tmp.Ks=(1-Kd)*Ks/(Ks+T+a);
	tmp.Kd=Kd*Ks/(Ks+T+a);
	tmp.T=T/(Ks+T+a);
	tmp.n=n;
	tmp.roughn=10/pow(Kd,1);
	if(alpha)
		memcpy(&(tmp.alpha),alpha,3*sizeof(double));
	else
	{
		for(int i=0;i<3;i++)
			tmp.alpha[i]=0;
	}

	return tmp;
}

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

bool cutLine(const Vector &v1, const Vector &v2, int x, double &targetY)
{
	if(x<v1.a[0] || x>v2.a[0])
		return false;

	targetY=v1.a[1]+(v2.a[1]-v1.a[1])*(x-v1.a[0])/(v2.a[0]-v1.a[0]);
	return true;
}
