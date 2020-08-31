#include <iostream>
#include<string>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv.hpp>
#include"mainInclude.h"
#define CVUI_IMPLEMENTATION
#include"cvui-2.7.0/cvui.h"
using namespace std;
using namespace cv;
enum type { color, gray, bin };
void mat2array(Mat img);
Mat array2mat(uint8* arr, uint8 h, uint8 w);
void matShow(Mat img, enum type imgType, string str);
Mat merge(Mat imgBin);
uint8 mt9v30x_image[120][188];
int16 differ;
int main()
{
	cvui::init("button");
	Mat frame = Mat(300, 600, CV_8UC3);
	int imgNum = 47;
	imgInfoInit();
	while (waitKey(30) != 27)
	{

		char str[20];
		sprintf_s(str, "D:\\photo\\%d.bmp", imgNum);
		Mat imgGray1 = imread(str, IMREAD_GRAYSCALE);
		uint8 h, w;
		matShow(imgGray1, gray, "gray origin");
		mat2array(imgGray1);

		imageSample();
		Mat imgGray2 = array2mat(&imageGray[0][0], _ROW, _COLUMN);
		matShow(imgGray2, gray, "gray sample");

		imageBinary();
		Mat imgBin = array2mat(&imageBin[0][0], _ROW, _COLUMN);
		matShow(imgBin, bin, "bin");

		imageDeal();
		Mat	imgSearch = merge(imgBin);
		matShow(imgSearch, color, "search");

		moveWindow("button", 1150, 0);
		moveWindow("gray origin", 0, 0);
		moveWindow("gray sample", 565, 0);
		moveWindow("bin", 1130, 361);
		moveWindow("search", 565, 361);

		frame = cv::Scalar(49, 52, 49);
		//*****************************************************************************************************
		cvui::printf(frame, 0, 150, 1, 0xff0000, "imgNum: %d ", imgNum);
		cvui::printf(frame, 0, 200, 1, 0xff0000, "differ: %d ", differ);
		cvui::printf(frame, 0, 250, 1, 0xff0000, "k: %f ", imgInfo.k);
		cvui::printf(frame, 200, 150, 1, 0xff0000, "len: %d ", imgInfo.len);
		if (cvui::button(frame, 50, 50, 100, 100, "previous"))
		{
			while (true)
			{
				imgNum--;
				sprintf_s(str, "D:\\photo\\%d.bmp", imgNum);
				string name = str;
				struct stat buffer;
				if ((stat(name.c_str(), &buffer) == 0))
					break;
				if (imgNum < 0)
					exit(0);
			}
		}
		if (cvui::button(frame, 200, 50, 100, 100, "next"))
		{
			while (true)
			{
				imgNum++;
				sprintf_s(str, "D:\\photo\\%d.bmp", imgNum);
				string name = str;
				struct stat buffer;
				if ((stat(name.c_str(), &buffer) == 0))
					break;
				if (imgNum > 1000)
					exit(0);
			}
		}
		if (cvui::button(frame, 400, 50, 100, 100, "end"))
		{
			exit(0);
		}
		cvui::update();
		cv::imshow("button", frame);
	}

}
const Vec3b RED = Vec3b(0, 0, 255);
const Vec3b GREEN = Vec3b(0, 255, 0);
const Vec3b BLUE = Vec3b(255, 0, 0);
const Vec3b YELLOW = Vec3b(0, 255, 255);
const Vec3b CYAN = Vec3b(255, 255, 0);
const Vec3b DARKYELLOW = Vec3b(0, 128, 128);
Mat merge(Mat imgBin)
{
	Mat imgGray;
	imgBin.copyTo(imgGray);
	imgGray = imgGray * 255;
	Mat imgColor;
	cvtColor(imgGray, imgColor, COLOR_GRAY2BGR);
	//imgGray.convertTo(imgColor, CV_GRAY2BGR);
	for (int i = 0; i < _ROW; i++)
	{
		imgColor.at<Vec3b>(i, RLB[i]) = RED;
		imgColor.at<Vec3b>(i, LLB[i]) = RED;
		imgColor.at<Vec3b>(i, MLB[i]) = GREEN;
		if (_ROW - i <= imgInfo.len)
			imgColor.at<Vec3b>(i, imgInfo.len_col)=DARKYELLOW;

	}
	for (int j = 0; j < _COLUMN; j++)
	{
		imgColor.at<Vec3b>(imgInfo.bottom, j) = YELLOW;
		imgColor.at<Vec3b>(imgInfo.top, j) = YELLOW;
	}
	if (imgInfo.Lcliffs.downExist)
		for (int i = -2; i <= 2; i++)
		{
			imgColor.at<Vec3b>(imgInfo.Lcliffs.downy, imgInfo.Lcliffs.downx + i) = CYAN;
			imgColor.at<Vec3b>(imgInfo.Lcliffs.downy + i, imgInfo.Lcliffs.downx) = CYAN;
		}
	if (imgInfo.Lcliffs.topExist)
		for (int i = -2; i <= 2; i++)
		{
			imgColor.at<Vec3b>(imgInfo.Lcliffs.topy, imgInfo.Lcliffs.topx + i) = RED;
			imgColor.at<Vec3b>(imgInfo.Lcliffs.topy + i, imgInfo.Lcliffs.topx) = RED;
		}
	if (imgInfo.Rcliffs.downExist)
		for (int i = -2; i <= 2; i++)
		{
			imgColor.at<Vec3b>(imgInfo.Rcliffs.downy, imgInfo.Rcliffs.downx + i) = CYAN;
			imgColor.at<Vec3b>(imgInfo.Rcliffs.downy + i, imgInfo.Rcliffs.downx) = CYAN;
		}
	if (imgInfo.Rcliffs.topExist)
		for (int i = -2; i <= 2; i++)
		{
			imgColor.at<Vec3b>(imgInfo.Rcliffs.topy, imgInfo.Rcliffs.topx + i) = CYAN;
			imgColor.at<Vec3b>(imgInfo.Rcliffs.topy + i, imgInfo.Rcliffs.topx) = CYAN;
		}
	return imgColor;
}
Mat array2mat(uint8* arr, uint8 h, uint8 w)
{
	return Mat(h, w, CV_8UC1, (uint8*)arr);
}
void mat2array(Mat img)
{
	int h = img.size().height;
	int w = img.size().width;
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			mt9v30x_image[i][j] = img.at<uint8>(i, j);
		}
	}
}
void matShow(Mat img, enum type imgType, string str)
{
	int h = img.size().height;
	int w = img.size().width;
	namedWindow(str);
	Size dsize = Size(w * 3, h * 6);
	Mat bigImg = Mat(dsize, img.type());
	cv::resize(img, bigImg, dsize, 0, 0, 0);
	switch (imgType)
	{
	case color:
		break;
	case gray:
		break;
	case bin:
		bigImg = bigImg * 255;
		break;
	default:
		break;
	}
	imshow(str, bigImg);
}
int  myabs(int dat)
{
	if (dat >= 0)  return dat;
	else        return -dat;
}