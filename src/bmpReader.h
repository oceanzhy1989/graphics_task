#ifndef __BMP_READER_H__
#define __BMP_READER_H__

#include<math.h>
#include <iomanip> 
#include <stdlib.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

using namespace std;

struct RGB
{
 int RED;
 int GREEN;
 int BLUE;
};

struct floatColor
{
	double color[3];
};

class MYBITMAP
{
public:
 //BITMAPFILEHEADER   bitHead;    //λͼ���ļ�ͷ
    //BITMAPINFOHEADER   bitInfoHead;//λͼ����Ϣͷ

	MYBITMAP();
	int readBmp(const char *bmpName);
	bool saveBmp(char *bmpName, unsigned char *imgBuf, int width, int height, 
					int biBitCount, RGBQUAD *pColorTable);

	inline floatColor readPixel(double u, double v)
	{
		int index=(int)(u*bmpWidth)+(int)(v*bmpHeight)*bmpWidth;
		
		return fBuf[index];
	}
	floatColor readPixel1(double u, double v);


	void writeTxt(char *txtName);
	int read8Pix(int x,int y);  //��ȡ8λͼ������ֵ
	bool set8Pix(int x,int y, int color); //����8λͼ������ֵ
	RGB readPix(int x,int y);   //��ȡ24λͼ������ֵ
	bool setPix(int x,int y,RGB color); //��ȡ24λͼ������ֵ
	bool set8Rect(int x1,int y1,int x2,int y2,int color); //�ı���η�Χ������
	bool setRect(int x1,int y1,int x2,int y2,RGB color);//�ı���η�Χ������

private:
	 unsigned char *pBmpBuf;//����ͼ�����ݵ�ָ��
	 floatColor *fBuf;

	 int bmpWidth;//ͼ��Ŀ�
	 int bmpHeight;//ͼ��ĸ�
	 RGBQUAD *pColorTable;//��ɫ��ָ��
	 int biBitCount;//ͼ�����ͣ�ÿ����λ�� ֻ��Ϊ8 ��24
	 int lineByte;
};

#endif