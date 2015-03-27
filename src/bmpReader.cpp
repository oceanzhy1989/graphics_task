#include "bmpReader.h"

MYBITMAP::MYBITMAP()
{
 pBmpBuf=NULL;
 fBuf=NULL;
 bmpWidth=0;
 bmpHeight=0;
 pColorTable=NULL;
 biBitCount=0;
}

int MYBITMAP:: readBmp(const char *bmpName) 
{
    FILE *fp=fopen(bmpName,"rb");//二进制读方式打开指定的图像文件

    if(fp==0) 
 {
  printf("Read Error!\n");
  return 0;
 }

    //跳过位图文件头结构BITMAPFILEHEADER

    fseek(fp, sizeof(BITMAPFILEHEADER),0);

    //定义位图信息头结构变量，读取位图信息头进内存，存放在变量head中

    BITMAPINFOHEADER head; 

    fread(&head, sizeof(BITMAPINFOHEADER), 1,fp); //获取图像宽、高、每像素所占位数等信息

    bmpWidth = head.biWidth;

    bmpHeight = head.biHeight;

    biBitCount = head.biBitCount;//定义变量，计算图像每行像素所占的字节数（必须是4的倍数）

    lineByte=(bmpWidth * biBitCount/8+3)/4*4;//灰度图像有颜色表，且颜色表表项为256

    if(biBitCount==8)
 {

        //申请颜色表所需要的空间，读颜色表进内存

        pColorTable=new RGBQUAD[256];

        fread(pColorTable,sizeof(RGBQUAD),256,fp);

 }

    //申请位图数据所需要的空间，读位图数据进内存

    pBmpBuf=new unsigned char[lineByte * bmpHeight];
	fBuf=new floatColor[lineByte * bmpHeight / 3];

    fread(pBmpBuf,1,lineByte * bmpHeight,fp);

    fclose(fp);//关闭文件
 printf("Read success!\n");

	for(int i=0;i<bmpWidth;i++)
	{
		for(int j=0;j<bmpHeight;j++)
		{
			int index=i+bmpWidth*j;
			RGB rgb=readPix(i,j);
			fBuf[index].color[0]=rgb.RED/256.0;
			fBuf[index].color[1]=rgb.GREEN/256.0;
			fBuf[index].color[2]=rgb.BLUE/256.0;
		}
	}

    return 1;//读取文件成功
}

bool MYBITMAP::saveBmp(char *bmpName, unsigned char *imgBuf, int width, int height,

             int biBitCount, RGBQUAD *pColorTable)

{

    //如果位图数据指针为0，则没有数据传入，函数返回

    if(!imgBuf)
 {
  printf("Write Error!\n");
        return 0;
 }

    //颜色表大小，以字节为单位，灰度图像颜色表为1024字节，彩色图像颜色表大小为0

    int colorTablesize=0;

    if(biBitCount==8)

        colorTablesize=1024;

    //待存储图像数据每行字节数为4的倍数

    int lineByte=(width * biBitCount/8+3)/(4*4);

    //以二进制写的方式打开文件

    FILE *fp=fopen(bmpName,"wb");

    if(fp==0) return 0;

    //申请位图文件头结构变量，填写文件头信息

    BITMAPFILEHEADER fileHead;

    fileHead.bfType = 0x4D42;//bmp类型

    //bfSize是图像文件4个组成部分之和

    fileHead.bfSize= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)

        + colorTablesize + lineByte*height;

    fileHead.bfReserved1 = 0;

    fileHead.bfReserved2 = 0;

    //bfOffBits是图像文件前3个部分所需空间之和

    fileHead.bfOffBits=54+colorTablesize;

    //写文件头进文件

    fwrite(&fileHead, sizeof(BITMAPFILEHEADER),1, fp);

    //申请位图信息头结构变量，填写信息头信息

    BITMAPINFOHEADER head;

    head.biBitCount=biBitCount;

    head.biClrImportant=0;

    head.biClrUsed=0;

    head.biCompression=0;

    head.biHeight=height;

    head.biPlanes=1;

    head.biSize=40;

    head.biSizeImage=lineByte*height;

    head.biWidth=width;

    head.biXPelsPerMeter=0;

    head.biYPelsPerMeter=0;

    //写位图信息头进内存

    fwrite(&head, sizeof(BITMAPINFOHEADER),1, fp);

    //如果灰度图像，有颜色表，写入文件

    if(biBitCount==8)

        fwrite(pColorTable, sizeof(RGBQUAD),256, fp);

    //写位图数据进文件

    fwrite(imgBuf, height*lineByte, 1, fp);

    //关闭文件

    fclose(fp);
 
    return 1;

}

floatColor MYBITMAP::readPixel1(double u, double v)
{
	//int index=u*bmpWidth+v*bmpHeight*bmpWidth;
	RGB mycolor=readPix(u*bmpWidth,v*bmpHeight);

	floatColor tmp;
	tmp.color[0]=mycolor.RED/256.0;
	tmp.color[1]=mycolor.GREEN/256.0;
	tmp.color[2]=mycolor.BLUE/256.0;

	return tmp;
	//return fBuf[index];
}

void MYBITMAP::writeTxt(char *txtName)  //将图片的相关信息写入txt文件中
/*
格式：

程序读取的位图信息如下：
位图宽：
位图高：
位图位数：

Color Plate Number: (仅限8位图)
颜色板信息：(仅限8位图)

像素数据信息：
<x,y>:(R,G,B)
.....

*/

{
    //显示图片的宽度，高度，位数
    //cout<<"width="<<bmpWidth<<" height="<<bmpHeight<<" biBitCount="<<biBitCount<<endl;
    
    //循环变量，图像的坐标

    //每行字节数

    int lineByte=(bmpWidth*biBitCount/8+3)/(4*4);

    //循环变量，针对彩色图像，遍历每像素的三个分量

    int m=0,n=0;

 
     ofstream outfile(txtName,ios::in|ios::trunc);

  outfile<<"程序读取的位图信息如下"<<endl;
  outfile<<"位图宽:"<<bmpWidth<<endl;
  outfile<<"位图高:"<<bmpHeight<<endl;
  outfile<<"位图位数:"<<biBitCount<<endl;
  outfile<<"像素数据信息:"<<endl<<"<x,y>(G,R,B)"<<endl;

    if(biBitCount==8) //对于灰度图像
 {  
 int pix_count=0;
 for(int i=0;i<bmpHeight;i++)
   for(int j=0;j<bmpWidth;j++)
   { 
    outfile<<"<"<<i<<","<<j<<">:";
    outfile<<"("<<(int)(*(pBmpBuf+i*lineByte+j))<<")";
    outfile<<endl;
    pix_count++;
   }
    outfile<<"该图片的总像素个数为:"<<pix_count<<endl; 
 }

    //double xiang_su[2048];
 //ofstream outfile("xiang_su_zhi.txt",ios::in|ios::trunc);
 if(!outfile)
 {
  cout<<"open error!"<<endl;
   exit(1);  //return 1;
 }
    else if(biBitCount==24){//彩色图像

      int pix_count=0;
   for(int i=0;i<bmpHeight;i++)
   for(int j=0;j<bmpWidth;j++)
   { 
    outfile<<"<"<<i<<","<<j<<">:";
    RGB rgb;
    rgb.BLUE=*(pBmpBuf+i*lineByte+j*3+2);
    rgb.GREEN=*(pBmpBuf+i*lineByte+j*3+1);
    rgb.RED=*(pBmpBuf+i*lineByte+j*3);
    outfile<<"("<<rgb.BLUE<<","<<rgb.GREEN<<","<<rgb.RED<<")";
    outfile<<endl;
    pix_count++;
   }
    outfile<<"该图片的总像素个数为:"<<pix_count<<endl;
    }
}

int MYBITMAP::read8Pix(int x,int y)
{
 int color;
 int lineByte=(bmpWidth*biBitCount/8+3)/4*4;
 color=*(pBmpBuf+x*lineByte+y);
 return color;
}

bool MYBITMAP::set8Pix(int x,int y,int color)
{
 int lineByte=(bmpWidth*biBitCount/8+3)/4*4;
 *(pBmpBuf+x*lineByte+y)=color;
 return 1;
}

RGB MYBITMAP::readPix(int x,int y)
{
	RGB rgb;
 
    rgb.RED=*(pBmpBuf+y*lineByte+x*3+2);
	rgb.GREEN=*(pBmpBuf+y*lineByte+x*3+1);
    rgb.BLUE=*(pBmpBuf+y*lineByte+x*3);

	return rgb;
}

bool MYBITMAP::setPix(int x,int y,RGB rgb)
{

 int lineByte=(bmpWidth*biBitCount/8+3)/4*4;
 *(pBmpBuf+x*lineByte+y*3+2)=rgb.BLUE;
 *(pBmpBuf+x*lineByte+y*3+1)=rgb.GREEN;
 *(pBmpBuf+x*lineByte+y*3)=rgb.RED;
 return 1;
}

bool MYBITMAP::setRect(int x1,int y1,int x2,int y2,RGB rgb)
{
 int lineByte=(bmpWidth*biBitCount/8+3)/4*4;
 int m,n;
 for(m=x1;m<x2;m++)
  for(n=y1;n<y2;n++)
  {
   *(pBmpBuf+m*lineByte+n*3+2)=rgb.BLUE;
   *(pBmpBuf+m*lineByte+n*3+1)=rgb.GREEN;
   *(pBmpBuf+m*lineByte+n*3)=rgb.RED;
  }
 return 1;
}

bool MYBITMAP::set8Rect(int x1,int y1,int x2,int y2,int color)
{
 int lineByte=(bmpWidth*biBitCount/8+3)/4*4;
 int m,n;
 for(m=x1;m<x2;m++)
  for(n=y1;n<y2;n++)
   *(pBmpBuf+m*lineByte+n)=color;
 return 1;

}