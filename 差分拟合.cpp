/*=====================================================================================
*                     Cha_fen_ni_he.c --
*	              Copyleft! 2017 William Yu
*          Some rights reserved：CC(creativecommons.org)BY-NC-SA
*                 Copyleft! 2017 William Yu
*      版权部分所有，遵循CC(creativecommons.org)BY-NC-SA协议授权方式使用
*
* Filename                : Cha_fen_ni_he.c
* Programmer(s)           : William Yu
* Description             : (OpenCV2.4.9+VS2013)差分拟合提取图片白线
* Reference               :
* Modification History	  : ver1.0, 2017.07.04, William Yu
*                           ver1.1, 2017.07.11, William Yu add notes
=====================================================================================*/

#include<opencv2\opencv.hpp>
#include<iostream>

using namespace std;
using namespace cv;

#define Picture  "red.jpg"
#define HEIGHT 240
#define WIDTH 320

int main()
{
	//--------------------------------------------------------------------
	//  
	//---------------------------------------------------------
	//[1]读入一张图片 
	Mat img = imread(Picture);
	//[2]在窗口中显示
	imshow(Picture, img);
	//[3]提取灰度图
	Mat gray;
	cvtColor(img, gray, CV_BGR2GRAY);
	imshow("gray", gray);
	//[4]3通道分离,看哪个图拿来处理效果最明显
	vector<Mat>channels;
	split(img, channels);
	Mat B = channels.at(0);
	Mat G = channels.at(1);
	Mat R = channels.at(2);
	//[5]再将单通道B恢复为3通道图片，方便后面用彩色线条做标注
	Mat dst;
	cvtColor(G, dst, CV_GRAY2BGR);  //3通道分离后觉得G通道更好，所以用G通道

	//--------------------------------------------------------------------
	//  标定列向的白线
	//---------------------------------------------------------
	vector<Point>vpoints;

	int left = 0, right = 0;
	int gray_diff1_max = 0, gray_diff1_min = 0;
	//差值，得出上升沿和下降沿，放入容器	
	for (int y = 0; y < HEIGHT; y++)
	{
		uchar *pg = G.ptr<uchar>(y);    //行指针访问像素
		for (int x = 5; x < WIDTH - 25; x++)  //x=1起始防止指针溢出
		{
			int gray_diff1 = pg[x] - pg[x - 1];
			if (gray_diff1>gray_diff1_max)// && pb[x]==255 && pb[x-1]==255)
			{
				gray_diff1_max = gray_diff1;
				left = x;
			}
			if (gray_diff1 < gray_diff1_min)//&& pb[x] == 255 && pb[x - 1] == 255)
			{
				gray_diff1_min = gray_diff1;
				right = x;
			}
		}//for...x
		if ((right - left)>20 && (right - left)<50
			&& gray_diff1_max>0 && -gray_diff1_min>0
			)
		{
			vpoints.push_back(Point((right + left) / 2, y));

			circle(dst, Point(left, y), 3, Scalar(255, 255, 0), -1);  //-1表示为实心圆形
			circle(dst, Point(right, y), 3, Scalar(255, 255, 0), -1);
		}
	}//for..y


	if (vpoints.size() > 60) //判断提取的点数是否足够多
	{
		Vec4f lines;
		//最小二乘法
		fitLine(vpoints, lines, CV_DIST_L1, 0, 0.01, 0.01); 
		//描绘图像，用k,b或者rho,theta参量
		float k = lines[1] / lines[0];
		float b = lines[3] - k*lines[2];
		line(dst, Point(-b / k, 0), Point((239 - b) / k, 239), Scalar(255, 0, 0), 3);
	}

	//--------------------------------------------------------------------
	//  标定行向的白线
	//---------------------------------------------------------
	//图像内存行连续，而不是列连续，所以行访问速度要快，列访问时将图像转置一下！
	vector<Point>hpoints;
	int down = 0, up = 0;
	int gray_diff2_max = 0, gray_diff2_min = 0;
	for (int x = 0; x < WIDTH - 25; x++)
	{
		for (int y = 1; y < HEIGHT; y++)
		{
			uchar *p1 = G.ptr<uchar>(y - 1);   //注意这里！！！！
			uchar *p2 = G.ptr<uchar>(y);       //对列的访问！！！

			int gray_diff2 = p2[x] - p1[x];
			if (gray_diff2>gray_diff2_max)// && pb[x]==255 && pb[x-1]==255)
			{
				gray_diff2_max = gray_diff2;
				up = y;
			}
			if (gray_diff2 < gray_diff2_min)//&& pb[x] == 255 && pb[x - 1] == 255)
			{
				gray_diff2_min = gray_diff2;
				down = y;
			}
		}
		if ((down - up)>20 && (down - up)<50
			&& gray_diff2_max>0 && -gray_diff2_min>0
			)
		{
			hpoints.push_back(Point(x, (up + down) / 2));

			circle(dst, Point(x, up), 3, Scalar(0, 255, 255), -1);
			circle(dst, Point(x, down), 3, Scalar(0, 255, 255), -1);
		}
	}

	if (hpoints.size() > 60)
	{
		Vec4f lines;
		fitLine(hpoints, lines, CV_DIST_L1, 0, 0.01, 0.01);
		//k,b或rho,theta
		float k = lines[1] / lines[0];
		float b = lines[3] - k*lines[2];
		line(dst, Point(0, b), Point(319, 319 * k + b), Scalar(0, 255, 0), 3);
	}

	//--------------------------------------------------------------------
	//  标定完毕，显示效果 
	//---------------------------------------------------------
	imshow("B", B);
	imshow("G", G);
	imshow("R", R);
	imshow("dst", dst);

	waitKey(0);
	return 0;
}
