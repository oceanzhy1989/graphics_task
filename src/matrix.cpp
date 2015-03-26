#include "matrix.h"
#include <string.h>

Matrix4::Matrix4(Mat type,double value)
{
	memset(a,0,16*sizeof(double));
	a[3][3]=1;

	switch(type)
	{
		case IDENTITY:
		{
			for(int i=0;i<3;i++)
			{
				a[i][i]=1.0;
			}
		}
		break;

		case ZERO:
		{
		}
		break;

		case ROTATE_X_RAD:
		{
			a[0][0]=1;
			a[1][1]=cos(value);a[1][2]=-sin(value);
			a[2][1]=sin(value);a[2][2]=cos(value);
		}
		break;
		
		case ROTATE_Y_RAD:
		{
			a[1][1]=1;
			a[0][0]=cos(value);a[0][2]=-sin(value);
			a[2][0]=sin(value);a[2][2]=cos(value);
		}
		break;

		case ROTATE_Z_RAD:
		{
			a[2][2]=1;
			a[0][0]=cos(value);a[0][1]=-sin(value);
			a[1][0]=sin(value);a[1][1]=cos(value);
		}
		break;

		case SCALE:
		{
			for(int i=0;i<3;i++)
			{
				a[i][i]=value;
			}
		}
		break;

		case TRANSLATE_X:
		{
			for(int i=0;i<3;i++)
			{
				a[i][i]=1;
			}
			a[0][3]=value;
		}
		break;
		case TRANSLATE_Y:
		{
			for(int i=0;i<3;i++)
			{
				a[i][i]=1;
			}
			a[1][3]=value;
		}
		break;
		case TRANSLATE_Z:
		{
			for(int i=0;i<3;i++)
			{
				a[i][i]=1;
			}
			a[2][3]=value;
		}
		break;
	default:
		break;
	}
}

Matrix4::Matrix4(Mat type, double value_x, double value_y, double value_z)
{
	memset(a,0,16*sizeof(double));

	switch(type)
	{
	case TRANSLATE:
		{
			for(int i=0;i<4;i++)
			{
				a[i][i]=1;
			}
			a[0][3]=value_x;
			a[1][3]=value_y;
			a[2][3]=value_z;
		}
		break;
	default:
		break;
	}

}

Matrix4::Matrix4(const Matrix3 &x)
{
	memset(a,0,16*sizeof(double));
	for(int i=0;i<3;i++)
	{
		memcpy(a[i],x.a[i],3*sizeof(double));
	}
	a[3][3]=1;
}


Matrix4::~Matrix4()
{
}


void Matrix4::rotateX(double rad)
{
	Matrix4 tmp( Matrix4(ROTATE_X_RAD,rad)*(*this) );

	memcpy(this,&tmp,sizeof(Matrix4));
}
void Matrix4::rotateY(double rad)
{
	Matrix4 tmp( Matrix4(ROTATE_Y_RAD,rad)*(*this) );

	memcpy(this,&tmp,sizeof(Matrix4));
}
void Matrix4::rotateZ(double rad)
{
	Matrix4 tmp( Matrix4(ROTATE_Z_RAD,rad)*(*this) );

	memcpy(this,&tmp,sizeof(Matrix4));
}

void Matrix4::pitch(double rad)
{
	Matrix4 tmp( (*this)*Matrix4(ROTATE_X_RAD,rad) );

	memcpy(this,&tmp,sizeof(Matrix4));
}
void Matrix4::yaw(double rad)
{
	Matrix4 tmp( (*this)*Matrix4(ROTATE_Y_RAD,rad) );

	memcpy(this,&tmp,sizeof(Matrix4));
}
void Matrix4::roll(double rad)
{
	Matrix4 tmp( (*this)*Matrix4(ROTATE_Z_RAD,rad) );

	memcpy(this,&tmp,sizeof(Matrix4));
}

void Matrix4::rotateAxis(const Vector4 &axis, double rad)
{
	Vector4 newAxis[3];

	for(int i=0;i<3;i++)
		newAxis[i]=rotate(this->getAxis(i),axis,rad);

	for(int i=0;i<3;i++)
		for(int j=0;j<3;j++)
		{
			a[i][j]=newAxis[j].a[i];
		}

}

void Matrix4::translate(double x, double y, double z)
{
	Matrix4 tmp( Matrix4(TRANSLATE,x,y,z)*(*this) );

	memcpy(this,&tmp,sizeof(Matrix4));
}

void Matrix4::translate(Vector4 v)
{
	Matrix4 tmp( Matrix4(TRANSLATE,v.a[0],v.a[1],v.a[2])*(*this) );

	memcpy(this,&tmp,sizeof(Matrix4));
}



Vector4 Matrix4::getAxis(int axis)
{
	return Vector4(a[0][axis],a[1][axis],a[2][axis]);
}

void Matrix4::setAxis(int axis, Vector4 v)
{
	for(int i=0;i<4;i++)
	{
		a[i][axis]=v.a[i];
	}
}

Matrix4 Matrix4::getRotatePart()
{
	Matrix4 tmp(*this);

	for(int i=0;i<3;i++)
		tmp.a[i][3]=0;

	return tmp;
}

Matrix4 Matrix4::T()
{
	Matrix4 tmp;

	for(int i=0;i<3;i++)
	{
		for(int j=0;j<3;j++)
		{
			tmp.a[i][j]=a[j][i];
		}
	}

	return tmp;
}

Matrix4 Matrix4::operator +(const Matrix4 &x)
{
	Matrix4 tmp;

	for(int i=0;i<4;i++)
	{
		for(int j=0;j<4;j++)
		{
			tmp.a[i][j]=a[i][j]+x.a[i][j];
		}
	}

	return tmp;
}

Matrix4 Matrix4::operator -(const Matrix4 &x)
{
	Matrix4 tmp;

	for(int i=0;i<4;i++)
	{
		for(int j=0;j<4;j++)
		{
			tmp.a[i][j]=a[i][j]-x.a[i][j];
		}
	}

	return tmp;
}

Matrix4 Matrix4::operator *(const Matrix4 &x)
{
	Matrix4 tmp;

	for(int i=0;i<3;i++)
	{
		for(int j=0;j<4;j++)
		{
			tmp.a[i][j]=0;
			for(int k=0;k<4;k++)
			{
				tmp.a[i][j]+=a[i][k]*x.a[k][j];
			}
		}
	}

	return tmp;
}

Vector4 operator +(const Vector4 &x, const Vector4 &y)
{
	return Vector4(x.a[0]+y.a[0],x.a[1]+y.a[1],x.a[2]+y.a[2]);
}
Vector4 operator -(const Vector4 &x, const Vector4 &y)
{
	return Vector4(x.a[0]-y.a[0],x.a[1]-y.a[1],x.a[2]-y.a[2]);
}
Vector4 operator -(const Vector4 &x)
{
	return Vector4(-x.a[0],-x.a[1],-x.a[2]);
}
void operator +=(Vector4 &x, const Vector4 &y)
{
	x.a[0]+=y.a[0];
	x.a[1]+=y.a[1];
	x.a[2]+=y.a[2];
}

void operator *=(Vector4 &x, double c)
{
	x.a[0]*=c;
	x.a[1]*=c;
	x.a[2]*=c;
}

void operator -=(Vector4 &x, const Vector4 &y)
{
	x.a[0]-=y.a[0];
	x.a[1]-=y.a[1];
	x.a[2]-=y.a[2];
}

Vector4 operator *(double c, const Vector4 &x)
{
	return Vector4(c*x.a[0],c*x.a[1],c*x.a[2]);
}

double dot(const Vector4 &x, const Vector4 &y)
{
	return x.a[0]*y.a[0]+x.a[1]*y.a[1]+x.a[2]*y.a[2];
}

Vector4 cross(const Vector4 &x, const Vector4 &y)
{
	return Vector4( x.a[1]*y.a[2]-x.a[2]*y.a[1],
					x.a[2]*y.a[0]-x.a[0]*y.a[2],
					x.a[0]*y.a[1]-x.a[1]*y.a[0]);
}

double norm(const Vector4 &x)
{
	return sqrt(dot(x,x));
}

Vector4 unitof(const Vector4 &x)
{
	double c=1/norm(x);

	return Vector4(c*x.a[0],c*x.a[1],c*x.a[2]);
}

Vector4 Matrix4::operator *(const Vector4 &v)
{
	Vector4 tmp;

	for(int i=0;i<4;i++)
	{
		tmp.a[i]=0;
		for(int k=0;k<4;k++)
		{
			tmp.a[i]+=a[i][k]*v.a[k];
		}
		
	}

	double c=1.0/tmp.a[3];
	//if(fabs(c)>0.2)
	//	c=0.2;
	for(int i=0;i<4;i++)
	{
		tmp.a[i]*=c;
	}
	tmp.a[3]=1;

	return tmp;
}



Vector4 conj(const Vector4 &x)
{
	return Vector4(-x.a[0],-x.a[1],-x.a[2],x.a[3]);
}
Vector4 inv(const Vector4 &x)
{
	double l2=1/(dot(x,x)+x.a[3]*x.a[3]);
	Vector4 tmp=conj(x);
	
	return Vector4(tmp.a[0]*l2,tmp.a[1]*l2,tmp.a[2]*l2,tmp.a[3]*l2);
}

Vector4 operator *(const Vector4 &x, const Vector4 &y)
{
	Vector4 vtmp[3];

	vtmp[0]=cross(x,y);

	for(int i=0;i<3;i++) 
	{
		vtmp[1].a[i]=y.a[3]*x.a[i];
		vtmp[2].a[i]=x.a[3]*y.a[i];
	}

	Vector4 tmp=vtmp[0]+vtmp[1]+vtmp[2];
	tmp.a[3]=x.a[3]*y.a[3]-dot(x,y);

	return tmp;

}



Vector4 rotate(const Vector4 &v, const Vector4 &axis, double rad)
{
	Vector4 sz_axis=unitof(axis);
	Vector4 q0(v.a[0],v.a[1],v.a[2],0);
	Vector4 qr=sin(0.5*rad)*sz_axis;
	qr.a[3]=cos(0.5*rad);

	Vector4 q=qr*q0*inv(qr);
	q.a[3]=1;
	
	return q;
}

