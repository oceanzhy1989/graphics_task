#ifndef __MATRIX_H__
#define __MATRIX_H__

#pragma once

#include <math.h>


struct Vector3
{
	double a[3];
};

typedef struct Vector
{
	double a[4];

	inline Vector(double x=0,double y=0,double z=0,double c=1)
	{
		a[0]=x;a[1]=y;a[2]=z;
		a[3]=c;
	}

} Vector4;

typedef struct
{
	double a[3][3];
} Matrix3;

//typedef struct
//{
//	double a[4][4];
//} Matrix4;
//
//
//
//Matrix4 IdentityMat4();
enum Mat
{
	IDENTITY=1,
	ZERO,
	ROTATE_X_RAD,
	ROTATE_Y_RAD,
	ROTATE_Z_RAD,
	SCALE,
	SCALE_X,
	SCALE_Y,
	SCALE_Z,
	TRANSLATE,
	TRANSLATE_X,
	TRANSLATE_Y,
	TRANSLATE_Z,

};


class Matrix4
{
public:
	
	Matrix4(Mat type=IDENTITY, double value=0);
	Matrix4(Mat type, double value_x, double value_y, double value_z);
	Matrix4(const Matrix3 &x);
	//Matrix4(const Matrix4 &m);

	~Matrix4();

	void rotateX(double rad);//��������ϵ
	void rotateY(double rad);//��������ϵ
	void rotateZ(double rad);//��������ϵ
	void rotateAxis(const Vector4 &axis, double rad);//��������ϵ

	void pitch(double rad);//��������ϵ
	void yaw(double rad);//��������ϵ
	void roll(double rad);//��������ϵ

	void translate(double x, double y, double z);
	void translate(Vector4 v);
	void transform(Matrix4 mat);

	
	inline void set(int i, int j, double value)
	{
		a[i][j]=value;
	}

	Vector4 getAxis(int axis);//axis=0~2��Ӧ��ת��������������ᣬaxis=3��Ӧƽ������
	void setAxis(int axis, Vector4 v);

	//Matrix3 submat3();
	Matrix4 getRotatePart();
	Matrix4 T();

	Matrix4 operator +(const Matrix4 &x);
	Matrix4 operator -(const Matrix4 &x);
	Matrix4 operator *(const Matrix4 &x);

	Vector4 operator *(const Vector4 &v);
	//Matrix4 operator +(const Matrix4 &x);

private:

	double a[4][4];
};

const Matrix4 IdentityMatrix(IDENTITY);


inline Vector4 operator +(const Vector4 &x, const Vector4 &y)
{
	return Vector4(x.a[0]+y.a[0],x.a[1]+y.a[1],x.a[2]+y.a[2]);
}

inline Vector4 operator -(const Vector4 &x, const Vector4 &y)
{
	return Vector4(x.a[0]-y.a[0],x.a[1]-y.a[1],x.a[2]-y.a[2]);
}

inline Vector4 operator -(const Vector4 &x)
{
	return Vector4(-x.a[0],-x.a[1],-x.a[2]);
}

inline Vector4 operator *(double c, const Vector4 &x)
{
	return Vector4(c*x.a[0],c*x.a[1],c*x.a[2]);
}

inline void operator +=(Vector4 &x, const Vector4 &y)
{
	x.a[0]+=y.a[0];
	x.a[1]+=y.a[1];
	x.a[2]+=y.a[2];
}

inline void operator -=(Vector4 &x, const Vector4 &y)
{
	x.a[0]-=y.a[0];
	x.a[1]-=y.a[1];
	x.a[2]-=y.a[2];
}

inline void operator *=(Vector4 &x, double c)
{
	x.a[0]*=c;
	x.a[1]*=c;
	x.a[2]*=c;
}

inline double dot(const Vector4 &x, const Vector4 &y)
{
	return x.a[0]*y.a[0]+x.a[1]*y.a[1]+x.a[2]*y.a[2];
}

inline Vector4 cross(const Vector4 &x, const Vector4 &y)
{
	return Vector4( x.a[1]*y.a[2]-x.a[2]*y.a[1],
					x.a[2]*y.a[0]-x.a[0]*y.a[2],
					x.a[0]*y.a[1]-x.a[1]*y.a[0]);
}

double norm(const Vector4 &x);
Vector4 unitof(const Vector4 &x);

Vector4 rotate(const Vector4 &v, const Vector4 &axis, double rad);//���ؽ�����v����axis��������������������תrad���Ⱥ������
Vector4 findReflectVector(const Vector4 &v, const Vector4 &n);



//Vector4����Ԫ������
Vector4 conj(const Vector4 &x);
Vector4 inv(const Vector4 &x);
Vector4 operator *(const Vector4 &x, const Vector4 &y);

#endif