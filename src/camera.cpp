#include <stdlib.h>
#include <string.h>
#include "camera.h"

const Vector4 nz(0,0,1);

Camera::Camera(double Screen_z, double ScreenWidth, double ScreenHeight, double FocalLength) : m_ProjectionMatrix(ZERO)
{
	this->screen_z=Screen_z;
	this->tmp_screen_z=Screen_z;
	this->screenWidth=ScreenWidth;
	this->screenHeight=ScreenHeight;
	this->focalLength=FocalLength;

	scale=1;
	focalLength=FocalLength/scale;


	update();
}

Camera::~Camera()
{
	//if(m_ModelViewMatrix)
	//	delete m_ModelViewMatrix;
	//if(m_ProjectionMatrix)
	//	delete m_ProjectionMatrix;
}

void Camera::setTmpZ0(double z)
{
	tmp_screen_z=z+focalLength;
}

void Camera::update()
{
	Vector4 minusT=-getPosition();
	m_ModelViewMatrix=m_CameraMatrix.getRotatePart().T()*Matrix4(TRANSLATE,minusT.a[0],minusT.a[1],minusT.a[2]);

	z0=screen_z-focalLength;
	double tmpz0=tmp_screen_z-focalLength;

	m_ProjectionMatrix.set(0,0,focalLength);
	m_ProjectionMatrix.set(1,1,focalLength);
	m_ProjectionMatrix.set(2,2,tmp_screen_z);
	m_ProjectionMatrix.set(3,2,1);
	m_ProjectionMatrix.set(2,3,-tmpz0*tmp_screen_z);
	m_ProjectionMatrix.set(3,3,-tmpz0);


	m_ProjectionMatrix=Matrix4(SCALE,scale)*m_ProjectionMatrix;

	//*m_ProjectionMatrix=(*m_ProjectionMatrix)*(*m_ModelViewMatrix);//总的投影矩阵

}

Vector4 Camera::getPosition()
{
	return m_CameraMatrix.getAxis(3);
}

void Camera::setPosition(const Vector4 &pos)
{
	m_CameraMatrix.setAxis(3,pos);
	update();
}

void Camera::lookAt(const Vector4 &viewDirection)
{
	//nz=viewDirection
	Vector4 sz_nz=unitof(viewDirection);
	Vector4 sz_nx=unitof(cross(nz,viewDirection));
	Vector4 sz_ny=cross(sz_nz,sz_nx);

	m_CameraMatrix.setAxis(0,sz_nx);
	m_CameraMatrix.setAxis(1,sz_ny);
	m_CameraMatrix.setAxis(2,sz_nz);

	update();
}

Vector4 Camera::getFacingDirection()
{
	return m_CameraMatrix.getAxis(2);
}

Matrix4 *Camera::getModelViewMatrix()
{
	return &(this->m_ModelViewMatrix);
}

Matrix4 *Camera::getProjectionMatrix()
{
	return &(this->m_ProjectionMatrix);
}

void Camera::pitch(double rad)
{
	m_CameraMatrix.pitch(rad);
	update();
}

void Camera::yaw(double rad)
{
	m_CameraMatrix.yaw(rad);
	update();
}

void Camera::roll(double rad)
{
	m_CameraMatrix.roll(rad);
	update();
}

void Camera::drawBack(double z)
{
	this->screen_z-=z;
	tmp_screen_z=screen_z;
	update();
}

void Camera::goForward(double z)
{
	this->screen_z+=z;
	tmp_screen_z=screen_z;
	update();
}

void Camera::rotateByTarget(double rad, const Vector4 *targetPoint, const Vector4 *axis)
{
	Vector4 pos=m_CameraMatrix.getAxis(3);


	const Vector4 *sz_targetpoint=targetPoint? targetPoint:&pos;
	const Vector4 *sz_axis=axis? axis:&nz;

	Vector4 tmpPos=pos-*sz_targetpoint;
	//Matrix4 M=m_CameraMatrix.getRotatePart();

	tmpPos=rotate(tmpPos,*sz_axis,rad);
	m_CameraMatrix.rotateAxis(*sz_axis,rad);

	pos=tmpPos+*sz_targetpoint;

	m_CameraMatrix.setAxis(3,pos);

	update();

}