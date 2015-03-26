#ifndef __CAMERA_H__
#define __CAMERA_H__

#pragma once

#include <math.h> 
#include "matrix.h"
#include <stddef.h>

const double pi=3.1415927;


class Camera
{
public:
	Camera(double Screen_z=1,double ScreenWidth=1,double ScreenHeight=1,double FocalLength=1);
	~Camera();

	void update();//根据CameraMatrix更新另外两个矩阵

	Vector4 getPosition();
	void setPosition(const Vector4 &pos);
	void lookAt(const Vector4 &viewDirection);

	//rotate针对世界坐标系
	void rotateByTarget(double rad, const Vector4 *targetPoint=NULL, const Vector4 *axis=NULL);

	//以下旋转针对镜头坐标系
	void pitch(double rad);//俯仰角，绕自身x轴旋转
	void yaw(double rad);//偏航角，绕自身y轴旋转
	void roll(double rad);//滚转角，绕自身z轴旋转

	void drawBack(double z);
	void goForward(double z);

	Vector4 getFacingDirection();


	Matrix4 *getModelViewMatrix();
	Matrix4 *getProjectionMatrix();

private:

	Matrix4 m_CameraMatrix;
	Matrix4 m_ModelViewMatrix;
	Matrix4 m_ProjectionMatrix;

	double screen_z;
	double focalLength;
	double screenWidth;
	double screenHeight;

	double scale;
};


#endif