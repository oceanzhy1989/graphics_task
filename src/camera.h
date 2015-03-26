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

	void update();//����CameraMatrix����������������

	Vector4 getPosition();
	void setPosition(const Vector4 &pos);
	void lookAt(const Vector4 &viewDirection);

	//rotate�����������ϵ
	void rotateByTarget(double rad, const Vector4 *targetPoint=NULL, const Vector4 *axis=NULL);

	//������ת��Ծ�ͷ����ϵ
	void pitch(double rad);//�����ǣ�������x����ת
	void yaw(double rad);//ƫ���ǣ�������y����ת
	void roll(double rad);//��ת�ǣ�������z����ת

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