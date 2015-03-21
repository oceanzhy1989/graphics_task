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

	Vector(double x=0,double y=0,double z=0,double c=1)
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

	void rotateX(double rad);
	void rotateY(double rad);
	void rotateZ(double rad);
	void rotateAxis(const Vector4 &axis, double rad);

	void translate(double x, double y, double z);
	void translate(Vector4 v);
	void transform(Matrix4 mat);

	Vector4 getAxis(int axis);//axis=0~2对应旋转矩阵的三根坐标轴，axis=3对应平移向量

	Matrix3 submat3();

	Matrix4 operator +(const Matrix4 &x);
	Matrix4 operator -(const Matrix4 &x);
	Matrix4 operator *(const Matrix4 &x);

	Vector4 operator *(const Vector4 &v);
	//Matrix4 operator +(const Matrix4 &x);

private:

	double a[4][4];
};


Vector4 operator +(const Vector4 &x, const Vector4 &y);
Vector4 operator -(const Vector4 &x, const Vector4 &y);
Vector4 operator -(const Vector4 &x);
Vector4 operator *(double c, const Vector4 &x);
void operator +=(Vector4 &x, const Vector4 &y);

double dot(const Vector4 &x, const Vector4 &y);
Vector4 cross(const Vector4 &x, const Vector4 &y);

double norm(const Vector4 &x);
Vector4 unitof(const Vector4 &x);

Vector4 rotate(const Vector4 &v, const Vector4 &axis, double rad);//返回将向量v沿着axis轴沿着左手螺旋方向旋转rad弧度后的向量



//Vector4的四元数操作
Vector4 conj(const Vector4 &x);
Vector4 inv(const Vector4 &x);
Vector4 operator *(const Vector4 &x, const Vector4 &y);

#endif