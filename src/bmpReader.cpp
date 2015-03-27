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
    FILE *fp=fopen(bmpName,"rb");//�����ƶ���ʽ��ָ����ͼ���ļ�

    if(fp==0) 
 {
  printf("Read Error!\n");
  return 0;
 }

    //����λͼ�ļ�ͷ�ṹBITMAPFILEHEADER

    fseek(fp, sizeof(BITMAPFILEHEADER),0);

    //����λͼ��Ϣͷ�ṹ��������ȡλͼ��Ϣͷ���ڴ棬����ڱ���head��

    BITMAPINFOHEADER head; 

    fread(&head, sizeof(BITMAPINFOHEADER), 1,fp); //��ȡͼ����ߡ�ÿ������ռλ������Ϣ

    bmpWidth = head.biWidth;

    bmpHeight = head.biHeight;

    biBitCount = head.biBitCount;//�������������ͼ��ÿ��������ռ���ֽ�����������4�ı�����

    lineByte=(bmpWidth * biBitCount/8+3)/4*4;//�Ҷ�ͼ������ɫ������ɫ�����Ϊ256

    if(biBitCount==8)
 {

        //������ɫ������Ҫ�Ŀռ䣬����ɫ����ڴ�

        pColorTable=new RGBQUAD[256];

        fread(pColorTable,sizeof(RGBQUAD),256,fp);

 }

    //����λͼ��������Ҫ�Ŀռ䣬��λͼ���ݽ��ڴ�

    pBmpBuf=new unsigned char[lineByte * bmpHeight];
	fBuf=new floatColor[lineByte * bmpHeight / 3];

    fread(pBmpBuf,1,lineByte * bmpHeight,fp);

    fclose(fp);//�ر��ļ�
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

    return 1;//��ȡ�ļ��ɹ�
}

bool MYBITMAP::saveBmp(char *bmpName, unsigned char *imgBuf, int width, int height,

             int biBitCount, RGBQUAD *pColorTable)

{

    //���λͼ����ָ��Ϊ0����û�����ݴ��룬��������

    if(!imgBuf)
 {
  printf("Write Error!\n");
        return 0;
 }

    //��ɫ���С�����ֽ�Ϊ��λ���Ҷ�ͼ����ɫ��Ϊ1024�ֽڣ���ɫͼ����ɫ���СΪ0

    int colorTablesize=0;

    if(biBitCount==8)

        colorTablesize=1024;

    //���洢ͼ������ÿ���ֽ���Ϊ4�ı���

    int lineByte=(width * biBitCount/8+3)/(4*4);

    //�Զ�����д�ķ�ʽ���ļ�

    FILE *fp=fopen(bmpName,"wb");

    if(fp==0) return 0;

    //����λͼ�ļ�ͷ�ṹ��������д�ļ�ͷ��Ϣ

    BITMAPFILEHEADER fileHead;

    fileHead.bfType = 0x4D42;//bmp����

    //bfSize��ͼ���ļ�4����ɲ���֮��

    fileHead.bfSize= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)

        + colorTablesize + lineByte*height;

    fileHead.bfReserved1 = 0;

    fileHead.bfReserved2 = 0;

    //bfOffBits��ͼ���ļ�ǰ3����������ռ�֮��

    fileHead.bfOffBits=54+colorTablesize;

    //д�ļ�ͷ���ļ�

    fwrite(&fileHead, sizeof(BITMAPFILEHEADER),1, fp);

    //����λͼ��Ϣͷ�ṹ��������д��Ϣͷ��Ϣ

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

    //дλͼ��Ϣͷ���ڴ�

    fwrite(&head, sizeof(BITMAPINFOHEADER),1, fp);

    //����Ҷ�ͼ������ɫ��д���ļ�

    if(biBitCount==8)

        fwrite(pColorTable, sizeof(RGBQUAD),256, fp);

    //дλͼ���ݽ��ļ�

    fwrite(imgBuf, height*lineByte, 1, fp);

    //�ر��ļ�

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

void MYBITMAP::writeTxt(char *txtName)  //��ͼƬ�������Ϣд��txt�ļ���
/*
��ʽ��

�����ȡ��λͼ��Ϣ���£�
λͼ��
λͼ�ߣ�
λͼλ����

Color Plate Number: (����8λͼ)
��ɫ����Ϣ��(����8λͼ)

����������Ϣ��
<x,y>:(R,G,B)
.....

*/

{
    //��ʾͼƬ�Ŀ�ȣ��߶ȣ�λ��
    //cout<<"width="<<bmpWidth<<" height="<<bmpHeight<<" biBitCount="<<biBitCount<<endl;
    
    //ѭ��������ͼ�������

    //ÿ���ֽ���

    int lineByte=(bmpWidth*biBitCount/8+3)/(4*4);

    //ѭ����������Բ�ɫͼ�񣬱���ÿ���ص���������

    int m=0,n=0;

 
     ofstream outfile(txtName,ios::in|ios::trunc);

  outfile<<"�����ȡ��λͼ��Ϣ����"<<endl;
  outfile<<"λͼ��:"<<bmpWidth<<endl;
  outfile<<"λͼ��:"<<bmpHeight<<endl;
  outfile<<"λͼλ��:"<<biBitCount<<endl;
  outfile<<"����������Ϣ:"<<endl<<"<x,y>(G,R,B)"<<endl;

    if(biBitCount==8) //���ڻҶ�ͼ��
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
    outfile<<"��ͼƬ�������ظ���Ϊ:"<<pix_count<<endl; 
 }

    //double xiang_su[2048];
 //ofstream outfile("xiang_su_zhi.txt",ios::in|ios::trunc);
 if(!outfile)
 {
  cout<<"open error!"<<endl;
   exit(1);  //return 1;
 }
    else if(biBitCount==24){//��ɫͼ��

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
    outfile<<"��ͼƬ�������ظ���Ϊ:"<<pix_count<<endl;
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