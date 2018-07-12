/*=====================================================================================
*                     Cha_fen_ni_he.c --
*	              Copyleft! 2017 William Yu
*          Some rights reserved��CC(creativecommons.org)BY-NC-SA
*                 Copyleft! 2017 William Yu
*      ��Ȩ�������У���ѭCC(creativecommons.org)BY-NC-SAЭ����Ȩ��ʽʹ��
*
* Filename                : Cha_fen_ni_he.c
* Programmer(s)           : William Yu
* Description             : (OpenCV2.4.9+VS2013)��������ȡͼƬ����
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
	//[1]����һ��ͼƬ 
	Mat img = imread(Picture);
	//[2]�ڴ�������ʾ
	imshow(Picture, img);
	//[3]��ȡ�Ҷ�ͼ
	Mat gray;
	cvtColor(img, gray, CV_BGR2GRAY);
	imshow("gray", gray);
	//[4]3ͨ������,���ĸ�ͼ��������Ч��������
	vector<Mat>channels;
	split(img, channels);
	Mat B = channels.at(0);
	Mat G = channels.at(1);
	Mat R = channels.at(2);
	//[5]�ٽ���ͨ��B�ָ�Ϊ3ͨ��ͼƬ����������ò�ɫ��������ע
	Mat dst;
	cvtColor(G, dst, CV_GRAY2BGR);  //3ͨ����������Gͨ�����ã�������Gͨ��

	//--------------------------------------------------------------------
	//  �궨����İ���
	//---------------------------------------------------------
	vector<Point>vpoints;

	int left = 0, right = 0;
	int gray_diff1_max = 0, gray_diff1_min = 0;
	//��ֵ���ó������غ��½��أ���������	
	for (int y = 0; y < HEIGHT; y++)
	{
		uchar *pg = G.ptr<uchar>(y);    //��ָ���������
		for (int x = 5; x < WIDTH - 25; x++)  //x=1��ʼ��ָֹ�����
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

			circle(dst, Point(left, y), 3, Scalar(255, 255, 0), -1);  //-1��ʾΪʵ��Բ��
			circle(dst, Point(right, y), 3, Scalar(255, 255, 0), -1);
		}
	}//for..y


	if (vpoints.size() > 60) //�ж���ȡ�ĵ����Ƿ��㹻��
	{
		Vec4f lines;
		//��С���˷�
		fitLine(vpoints, lines, CV_DIST_L1, 0, 0.01, 0.01); 
		//���ͼ����k,b����rho,theta����
		float k = lines[1] / lines[0];
		float b = lines[3] - k*lines[2];
		line(dst, Point(-b / k, 0), Point((239 - b) / k, 239), Scalar(255, 0, 0), 3);
	}

	//--------------------------------------------------------------------
	//  �궨����İ���
	//---------------------------------------------------------
	//ͼ���ڴ����������������������������з����ٶ�Ҫ�죬�з���ʱ��ͼ��ת��һ�£�
	vector<Point>hpoints;
	int down = 0, up = 0;
	int gray_diff2_max = 0, gray_diff2_min = 0;
	for (int x = 0; x < WIDTH - 25; x++)
	{
		for (int y = 1; y < HEIGHT; y++)
		{
			uchar *p1 = G.ptr<uchar>(y - 1);   //ע�����������
			uchar *p2 = G.ptr<uchar>(y);       //���еķ��ʣ�����

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
		//k,b��rho,theta
		float k = lines[1] / lines[0];
		float b = lines[3] - k*lines[2];
		line(dst, Point(0, b), Point(319, 319 * k + b), Scalar(0, 255, 0), 3);
	}

	//--------------------------------------------------------------------
	//  �궨��ϣ���ʾЧ�� 
	//---------------------------------------------------------
	imshow("B", B);
	imshow("G", G);
	imshow("R", R);
	imshow("dst", dst);

	waitKey(0);
	return 0;
}
