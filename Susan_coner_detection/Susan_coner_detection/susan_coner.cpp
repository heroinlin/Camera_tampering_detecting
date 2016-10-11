#include <opencv2/opencv.hpp>    
#include "highgui.h"    
#include <math.h>    
typedef  unsigned long uint32;
typedef  unsigned int  uint16;
typedef  unsigned char uint8;
#define THRESHOLD  20  
#define RADIUS  3  
IplImage *src_gray1, *src_gray2, *src_gray3;
IplImage* src_img, *dst_img;
CvMat  *NumR0, *Rr0;
int MaxNumR0 = 0;

void AllocateImage(IplImage* I)   //给图像分配大小      
{
	CvSize sz = cvGetSize(I);

	dst_img = cvCreateImage(sz, IPL_DEPTH_8U, 1);
	cvSetZero(dst_img);
	src_gray1 = cvCreateImage(sz, IPL_DEPTH_8U, 1);    //原图的三个通道      
	src_gray2 = cvCreateImage(sz, IPL_DEPTH_8U, 1);
	src_gray3 = cvCreateImage(sz, IPL_DEPTH_8U, 1);
	NumR0 = cvCreateMat(sz.height, sz.width, CV_16SC1);
	Rr0 = cvCreateMat(sz.height, sz.width, CV_16SC1);
	cvSetZero(NumR0);
	cvSetZero(Rr0);

}

void DeallocateImage()
{
	cvReleaseImage(&src_img);
	cvReleaseImage(&dst_img);
	cvReleaseImage(&src_gray1);
	cvReleaseImage(&src_gray2);
	cvReleaseImage(&src_gray3);
	cvReleaseMat(&NumR0);

}

void SUSAN_check(IplImage* I, IplImage* dst, int r)                    //SUSAN角点检测函数  
{
	int i, j, x, y;
	int num, MaxNum = 0, g, CValue;
	int R0Value, temp;

	for (i = r; i < (I->height - r); i++)               //计算n(r0),模板内图像USAN的像元数量  
	{
		for (j = r; j<(I->width - r); j++)
		{

			R0Value = cvGetReal2D(I, i, j);
			num = 0;
			for (y = i - r; (y - i <= r) && (y - i >= -r); y++)
			{
				for (x = j - r; (x - j <= r) && (x - j >= -r); x++)
				{
					if (((x - j)*(x - j) + (y - i)*(y - i)) <= r*r && (x != j || y != i))                //中心点圆形邻域内  
					{
						temp = cvGetReal2D(I, y, x);
						if (abs(R0Value - temp) <= THRESHOLD)
							num++;
					}
				}
			}
			cvSetReal2D(NumR0, i, j, num);
			if (num>MaxNum)  MaxNum = num;
		}
	}
	g = 3 * MaxNum / 4;
	for (i = r; i < (I->height - r); i++)               //计算R(r0),USAN特征图像  
	{
		for (j = r; j < (I->width - r); j++)
		{
			temp = cvGetReal2D(NumR0, i, j);
			if (temp < g)
				cvSetReal2D(Rr0, i, j, (g - temp));
		}
	}

	for (i = r; i < (I->height - r); i++)             //非极大抑制  
	{
		for (j = r; j < (I->width - r); j++)
		{
			CValue = cvGetReal2D(Rr0, i, j);       //USAN特征图像中心点值  
			num = 0;
			if (CValue != 0)
			{
				for (y = i - r; (y - i <= r) && (y - i >= -r); y++)         //查看周围半径r圆形邻域是否为极大值  
				{
					for (x = j - r; (x - j <= r) && (x - j >= -r); x++)
					{
						if (((x - j)*(x - j) + (y - i)*(y - i)) <= r*r && (x != j || y != i))                //中心点圆形邻域内  
						{
							temp = cvGetReal2D(Rr0, y, x);
							if (j == 27 && i == 129)
							{
								MaxNum = MaxNum;
							}
							if (CValue > temp)
								num++;
						}
					}
				}
				if (num == MaxNum)
				{
					cvCircle(src_img, cvPoint(j, i), 3, cvScalar(0, 0, 255), 1, 8, 0);
					//cvSet2D(src_img,i,j,cvScalar(0,0,255));  
					*(dst->imageData + i*dst->widthStep + j) = 255;
				}
			}

		}
	}

}

int main(int argc, char* argv[])
{
	//src_img  = cvLoadImage("角点.bmp");  
	//src_img  = cvLoadImage("5.2.09.pgm");  
	src_img = cvLoadImage("../videos/0000001.jpg");
	AllocateImage(src_img);
	cvSplit(src_img, src_gray1, src_gray2, src_gray3, 0);




	cvNamedWindow("my picture", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("my dst", CV_WINDOW_AUTOSIZE);

	cvShowImage("my picture", src_img);
	SUSAN_check(src_gray1, dst_img, RADIUS);
	cvShowImage("my dst", src_img);

	cvWaitKey(0);
	DeallocateImage();
	cvDestroyWindow("my picture");
	cvDestroyWindow("my dst");
	return 0;
}