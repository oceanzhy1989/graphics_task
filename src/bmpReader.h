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
 //BITMAPFILEHEADER   bitHead;    //位图的文件头
    //BITMAPINFOHEADER   bitInfoHead;//位图的信息头

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
	int read8Pix(int x,int y);  //读取8位图的像素值
	bool set8Pix(int x,int y, int color); //设置8位图的象素值
	RGB readPix(int x,int y);   //读取24位图的像素值
	bool setPix(int x,int y,RGB color); //读取24位图的像素值
	bool set8Rect(int x1,int y1,int x2,int y2,int color); //改变矩形范围的像素
	bool setRect(int x1,int y1,int x2,int y2,RGB color);//改变矩形范围的像素

private:
	 unsigned char *pBmpBuf;//读入图像数据的指针
	 floatColor *fBuf;

	 int bmpWidth;//图像的宽
	 int bmpHeight;//图像的高
	 RGBQUAD *pColorTable;//颜色表指针
	 int biBitCount;//图像类型，每像素位数 只能为8 或24
	 int lineByte;
};

#endif