#include <stdlib.h>
#include <string.h>
#include "utils.h"

//vect& cross(vect x, vect y)
//{
//	double x1=x[0],x2=x[1],x3=x[2];
//	double y1=y[0],y2=y[1],y3=y[2];
//
//	vect temp;
//	temp[0]=x2*y3-x3*y2;
//	temp[1]=x3*y1-x1*y3;
//	temp[2]=x1*y2-x2*y1;
//	return temp;
//};
//
//GLvector& cross(GLvector x,GLvector y)
//{
//	GLvector tmp;
//	memcpy(tmp.a, cross(x.a,y.a),sizeof(vect));
//	return tmp;
//}
//
//double dot(vect x,vect y)
//{
//	double x1=x[0],x2=x[1],x3=x[2];
//	double y1=y[0],y2=y[1],y3=y[2];
//
//	return x1*y1+x2*y2+x3*y3;
//};
//
//double dot(GLvector x,GLvector y)
//{
//	return dot(x.a,y.a);
//	//return x.a[0]*y.a[0]+x.a[1]*y.a[1]+x.a[2]*y.a[2];
//}
//
//vect& unitof(vect v)
//{
//	vect temp;
//	double l=sqrt(dot(v,v));
//	for(int i=0;i<3;i++) temp[i]=v[i]/l;
//	return temp;
//}
//
//GLvector& unitof(GLvector v)
//{
//	GLvector tmp;
//	double l=sqrt(dot(v,v));
//	for(int i=0;i<3;i++) tmp.a[i]=v.a[i]/l;
//	return tmp;
//}
//
//vect& mul(const vect& v, double c)
//{
//	vect temp;
//	for(int i=0;i<3;i++) temp[i]=v[i]*c;
//	return temp;
//}
//vect& plus(const vect& v1, const vect& v2)
//{
//	vect temp;
//	for(int i=0;i<3;i++) temp[i]=v1[i]+v2[i];
//	return temp;
//}
//
//
//quat& operator *(const double A,const quat& X)
//{
//	quat temp;
//	temp.c=X.c*A;
//	for(int i=0;i<3;i++) temp.v[i]=A*X.v[i];
//	return temp;
//}
//
//quat& operator *(quat X,quat Y)
//{
//	
//	quat temp,q;
//	vect vtemp[3];
//
//	memcpy(vtemp[0],cross(X.v,Y.v),sizeof(vect));
//	
//	for(int i=0;i<3;i++) 
//	{
//		vtemp[1][i]=Y.c*X.v[i];
//		vtemp[2][i]=X.c*Y.v[i];
//	}
//	for(int i=0;i<3;i++) temp.v[i]=vtemp[0][i]+vtemp[1][i]+vtemp[2][i];
//	
//	temp.c=X.c*Y.c-dot(X.v,Y.v);
//	return temp;
//}
//
//quat& operator /(const quat& X,const double A)
//{
//	quat temp;
//	temp.c=X.c/A;
//	for(int i=0;i<3;i++) temp.v[i]=X.v[i]/A;
//
//	return temp;
//}
//
//quat& operator +(const quat& X,const quat& Y)
//{
//	quat temp;
//	temp.c=X.c+Y.c;
//	for(int i=0;i<3;i++) temp.v[i]=X.v[i]+Y.v[i];
//	
//	return temp;
//}
//
//quat& operator -(const quat& X,const quat& Y)
//{
//	quat temp;
//	temp.c=X.c-Y.c;
//	for(int i=0;i<3;i++) temp.v[i]=X.v[i]-Y.v[i];
//	return temp;
//}
//quat& conj(const quat& p)
//{
//	quat temp;
//	temp.c=p.c;
//	for(int i=0;i<3;i++) temp.v[i]=-p.v[i];
//	return temp;
//}
//double norm(quat p)
//{
//	return dot(p.v,p.v)+p.c*p.c;
//}
//
//quat& inv(quat p)
//{
//	double l=norm(p);
//	quat temp=conj(p);
//	temp.c/=l;
//	for(int i=0;i<3;i++) temp.v[i]/=l;
//	return temp;
//}
//
//vect& rotate(const vect v,int dx,int dy,int speed)
//{
//	double th=-(double)dx/(double)speed,w=-(double)dy/(double)speed;
//	vect n1={0,0,1};
//	vect n2;
//	quat q0,q,qr;
//
//	memcpy(q0.v,v,sizeof(vect));
//	q0.c=0;
//	
//	for(int i=0;i<3;i++) qr.v[i]=n1[i]*sin(th);
//	qr.c=cos(th);
//	
//	q=qr*q0*inv(qr);
//
//	memcpy(n2,cross(q.v,n1),sizeof(vect));
//	unitof(n2);
//	for(int i=0;i<3;i++) qr.v[i]=n2[i]*sin(w);
//	qr.c=cos(w);
//	q=qr*q*inv(qr);
//
//	return q.v;
//}
//
//GLvector& rotate(const GLvector v,int dx,int dy,int speed)
//{
//	GLvector tmp;
//
//	memcpy(tmp.a, rotate(v.a,dx,dy,speed), sizeof(vect));
//
//	return tmp;
//}
//
//vect& rotate(const vect v,double th,double w)
//{
//	vect n1={0,0,1};
//	vect n2;
//	quat q0,q,qr;
//
//	w/=-2;th/=-2;
//
//	memcpy(q0.v,v,sizeof(vect));
//	q0.c=0;
//	
//	for(int i=0;i<3;i++) qr.v[i]=n1[i]*sin(th);
//	qr.c=cos(th);
//	
//	q=qr*q0*inv(qr);
//
//	memcpy(n2,cross(q.v,n1),sizeof(vect));
//	unitof(n2);
//	for(int i=0;i<3;i++) qr.v[i]=n2[i]*sin(w);
//	qr.c=cos(w);
//	q=qr*q*inv(qr);
//
//
//
//	return q.v;
//}
//
//GLvector& rotate(const GLvector v,double th,double w)
//{
//	GLvector tmp;
//
//	memcpy(tmp.a, rotate(v.a,th,w), sizeof(vect));
//
//	return tmp;
//}
//
//GLvector& operator +(const GLvector X,const GLvector Y)
//{
//	GLvector temp;
//	for(int i=0;i<3;i++) temp.a[i]=X.a[i]+Y.a[i];
//	return temp;
//}
//
//GLvector& operator -(const GLvector X,const GLvector Y)
//{
//	GLvector temp;
//	for(int i=0;i<3;i++) temp.a[i]=X.a[i]-Y.a[i];
//	return temp;
//}
//GLvector& operator *(const double A,const GLvector X)
//{
//	GLvector temp;
//	for(int i=0;i<3;i++) temp.a[i]=X.a[i]*A;
//	return temp;
//}

