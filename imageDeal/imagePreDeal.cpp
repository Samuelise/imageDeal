/*
 * @filename:imagePreDeal.c
 * @version: 1.0
 * @Description: 图像预处理,二值化的过程
 * @Author: wloves
 * @Date: 2020 2020年5月3日 下午8:14:38
 * @LastEditors: wloves
 * @LastEditTime: 2020-07-31 16:29:33
 */
#include "imagePreDeal.h"
uint8 SampleRow[ROW] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66, 68, 70, 72, 74, 76, 78, 80, 82, 84, 86, 88 };
#if 0
// uint8 real_pre_width[MT9V03X_H] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
// 								   3, 5, 7, 9, 11, 13, 15, 17, 19, 21,
// 								   23, 25, 27, 29, 31, 33, 35, 37, 39, 40,
// 								   42, 44, 46, 48, 51, 53, 55, 57, 59, 61,
// 								   63, 65, 67, 69, 71, 74, 76, 78, 80, 82,
// 								   84, 86, 88, 90, 92, 94, 96, 98, 100, 102,
// 								   104, 106, 108, 111, 113, 115, 117, 119, 122, 124,
// 								   126, 128, 130, 132, 134, 136, 138, 140, 142, 144,
// 								   146, 149, 151, 153, 155, 157, 159, 161, 163, 165,
// 								   167, 169, 171, 172, 173, 174, 175, 176, 177, 178,
// 								   179, 179, 180, 180, 188, 188, 188, 188, 188, 188,
// 								   188, 188, 188, 188, 188, 188, 188, 188, 188, 188};

// uint8 SampleRow[ROW] = {
// 	7, 9, 11, 13, 15, 17, 19, 21, 23, 25,
// 	27, 29, 31, 33, 35, 37, 40, 42, 44, 46,
// 	48, 50, 52, 54, 56, 58, 60, 62, 64, 66,
// 	68, 70, 72, 74, 76, 78, 80, 82, 84, 86,
// 	87, 88, 89, 90, 91, 92, 93, 94, 95, 96,
// 	97, 98, 99, 100, 103, 106, 109, 112, 115, 118};

// uint8 SampleRow[ROW] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
// 						10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
// 						20, 22, 24, 26, 28, 30, 32, 34, 36, 38,
// 						40, 42, 44, 46, 48, 50, 52, 54, 56, 58,
// 						60, 63, 66, 69, 72, 75, 78, 81, 84, 87,
// 						90, 93, 96, 99, 102, 105, 108, 111, 114, 118};
#endif
uint8 imageGray[ROW][COLUMN];
uint8 imageBin[ROW][COLUMN];
#define GrayScale 256
#define pixelSum (COLUMN * ROW)

uint8 threshold;
/**
 * @name: imageBinary
 * @details: 灰度图像二值化函数：中值滤波->大津法求阈值->二值化
 * @param {灰度数组（全局变量）}
 * @return: 二值化数组
 */
void imageBinary(void)
{
	//sauvola(0.7,50);
	//中值滤波
	//medianFilter();
	//大津法求阈值

	uint8 threshold = _otsuThreshold();
	//uint8 threshold1 = otsu2(20, 0)+threshold>>1;
	//uint8 threshold2 = otsu2(40, 20) + threshold >> 1;
	//uint8 threshold3 = otsu2(60, 40) + threshold >> 1;
	//二值化处理
	uint8* src = &(imageGray[0][0]);
	uint8* dst = &(imageBin[0][0]);
#if 0
	for (uint16 i = 0; i < 20 * COLUMN; i++)
		dst[i] = src[i] > threshold1;
	for (uint16 i = 20*COLUMN; i < 40 * COLUMN; i++)
		dst[i] = src[i] > threshold2;
	for (uint16 i = 40*COLUMN; i < 60 * COLUMN; i++)
		dst[i] = src[i] > threshold3;
#else
	for (uint16 i = 0; i < ROW*COLUMN; i++)
		dst[i] = src[i] > threshold;
#endif
}
//************************************
// 函数名称: sauvola
// 函数说明: 局部均值二值化
// 参    数:
//           const int k                            [in]        threshold = mean*(1 + k*((std / 128) - 1))
//           const int windowSize                   [in]        处理区域宽高
// 返 回 值: void
//************************************

void sauvola(const int k, const int windowSize) 
{

	int whalf = windowSize >> 1;
	int16 i, j;
	// create the integral image
	unsigned long* integralImg = (unsigned long*)malloc(COLUMN * COLUMN * sizeof(unsigned long*));
	unsigned long* integralImgSqrt = (unsigned long*)malloc(COLUMN * COLUMN * sizeof(unsigned long*));
	int sum = 0;
	int sqrtsum = 0;
	int index;
	uint8* grayPtr = &imageGray[0][0];
	uint8* binPtr = &imageBin[0][0];
	//收集数据 integralImg像素和积分图 integralImgSqrt像素平方和积分图
	for (i = 0; i < COLUMN; i++) {
		// reset this column sum
		sum = 0;
		sqrtsum = 0;
		for (j = 0; j < COLUMN; j++)
		{
			index = i * COLUMN + j;
			sum += grayPtr[index];
			sqrtsum += grayPtr[index] * grayPtr[index];
			if (i == 0) {
				integralImg[index] = sum;
				integralImgSqrt[index] = sqrtsum;
			}
			else {
				integralImg[index] = integralImg[(i - 1) * COLUMN + j] + sum;
				integralImgSqrt[index] = integralImgSqrt[(i - 1) * COLUMN + j] + sqrtsum;
			}
		}
	}
	//Calculate the mean and standard deviation using the integral image
	int xmin, ymin, xmax, ymax;
	double mean, std, threshold;
	double diagsum, idiagsum, diff, sqdiagsum, sqidiagsum, sqdiff, area;
	for (i = 0; i < COLUMN; i++) {
		for (j = 0; j < COLUMN; j++) {
			xmin = max(0, i - whalf);
			ymin = max(0, j - whalf);
			xmax = min(COLUMN - 1, i + whalf);
			ymax = min(COLUMN - 1, j + whalf);
			area = (xmax - xmin + 1) * (ymax - ymin + 1);
			if (area <= 0) {
				binPtr[i * COLUMN + j] = 255;
				continue;
			}
			if (xmin == 0 && ymin == 0) {
				diff = integralImg[ymax * COLUMN + xmax];
				sqdiff = integralImgSqrt[ymax * COLUMN + xmax];
			}
			else if (xmin > 0 && ymin == 0) {
				diff = integralImg[ymax * COLUMN + xmax] - integralImg[ymax * COLUMN + xmin - 1];
				sqdiff = integralImgSqrt[ymax * COLUMN + xmax] - integralImgSqrt[ymax * COLUMN + xmin - 1];
			}
			else if (xmin == 0 && ymin > 0) {
				diff = integralImg[ymax * COLUMN + xmax] - integralImg[(ymin - 1) * COLUMN + xmax];
				sqdiff = integralImgSqrt[ymax * COLUMN + xmax] - integralImgSqrt[(ymin - 1) * COLUMN + xmax];;
			}
			else {
				diagsum = integralImg[ymax * COLUMN + xmax] + integralImg[(ymin - 1) * COLUMN + xmin - 1];
				idiagsum = integralImg[(ymin - 1) * COLUMN + xmax] + integralImg[ymax * COLUMN + xmin - 1];
				diff = diagsum - idiagsum;
				sqdiagsum = integralImgSqrt[ymax * COLUMN + xmax] + integralImgSqrt[(ymin - 1) * COLUMN + xmin - 1];
				sqidiagsum = integralImgSqrt[(ymin - 1) * COLUMN + xmax] + integralImgSqrt[ymax * COLUMN + xmin - 1];
				sqdiff = sqdiagsum - sqidiagsum;
			}
			mean = diff / area;
			std = sqrt((sqdiff - diff * diff / area) / (area - 1));
			threshold = mean * (1 + k * ((std / 128) - 1));
			if (grayPtr[j * COLUMN + i] < threshold)
				binPtr[j * COLUMN + i] = 0;
			else
				binPtr[j * COLUMN + i] = 255;
		}

	}
	free(integralImg);
	free(integralImgSqrt);
}
void imageSample(void)
{
	uint8 i, j;
	uint8 line;
	for (i = 0; i < ROW; i++)
	{
		line = SampleRow[i];
		for (j = 3; j < COLUMN; j++)
		{
			imageGray[i][j] = mt9v30x_image[line][j];
		}
	}
}
/***************中值滤波函数*******************/
/**
 * @name: medianFilter
 * @details: 综合上、左、右、当前坐标共四个点求中值滤波,效果比较好,速度未测算
 * @param {灰度数组（全局变量）}
 * @return: 滤波后的灰度数组（全局变量）
 */
void medianFilter(void)
{
	//for (int i = 1; i < ROW; i++)
	//for (int j = 1; j < COLUMN - 1; j++)
	//image[i][j] = (image[i][j] + image[i - 1][j] + image[i][j - 1] + image[i][j + 1]) >> 2;
}
/***************大津法求阈值函数*******************/
/**
 * @name: otsuThreshold
 * @details: 大津法求二值化阈值（最大类间方差法）；速度较快
 * @param {灰度数组（全局变量）}
 * @return: 二值化阈值
 */
uint8 otsu2(uint8 down,uint8 top)
{
	uint16 pixelSum_part = (down - top) * COLUMN;
	uint16 histogram[GrayScale] = { 0 };
	uint32 iMulHistogram[GrayScale] = { 0 };
	uint8 i;
	uint8 threshold = 0;
	uint32 sum = 0;
	//统计灰度级中每个像素在整幅图像中的个数
	unsigned char* ptr = &imageGray[top][0];
	for (uint16 i = 0; i < (down-top) * COLUMN; i++)
		histogram[ptr[i]]++; //将像素值作为计数数组的下标
	//获取总的像素值
	for (i = 0; i < GrayScale - 1; i++)
	{
		iMulHistogram[i] = i * histogram[i];
		sum += iMulHistogram[i];
	}
	uint16 MinValue, MaxValue;
	for (MinValue = 0; MinValue < GrayScale - 1 && histogram[MinValue] == 0; MinValue++)
		; //获取最小灰度的值
	for (MaxValue = GrayScale - 1; MaxValue > MinValue && histogram[MaxValue] == 0; MaxValue--)
		; //获取最大灰度的值
	//遍历灰度级[0,255]
	if (MaxValue == MinValue)
		return (uint8)MaxValue; // 图像中只有一个颜色
	if (MinValue + 1 == MaxValue)
		return (uint8)MinValue; // 图像中只有二个颜色
	//back：背景像素点；front：前景像素点
	//w：比例；u：平均值；
	uint16 w_back = 0, w_front = 0;
	uint32 u_back_sum = 0, u_front_sum = 0;
	float u_back = 0, u_front = 0;
	float deltaTmp = 0, deltaMax = 0;
	for (i = MinValue; i < MaxValue; i++) // i作为阈值
	{
		//更新前景、后景像素点比例
		w_back += histogram[i];
		w_front = pixelSum_part - w_back;
		//更新前景、后景像素总灰度值
		u_back_sum += iMulHistogram[i];
		u_front_sum = sum - u_back_sum;
		//更新前景、后景像素平均灰度值
		u_back = u_back_sum / (float)w_back;
		u_front = u_front_sum / (float)w_front;
		//计算类间方差
		deltaTmp = w_back * w_front * (u_back - u_front) * (u_back - u_front) / (float)pixelSum_part;
		//求最大类间方差
		if (deltaTmp > deltaMax)
		{
			deltaMax = deltaTmp;
			threshold = (uint8)i;
		}
	}
	return threshold;
}
uint8 _otsuThreshold(void)
{
	uint16 histogram[GrayScale] = { 0 };
	uint32 iMulHistogram[GrayScale] = { 0 };
	uint8 i;
	uint8 threshold = 0;
	uint32 sum = 0;
	//统计灰度级中每个像素在整幅图像中的个数
	unsigned char* ptr = &imageGray[0][0];
	for (uint16 i = 0; i < ROW * COLUMN; i++)
		histogram[ptr[i]]++; //将像素值作为计数数组的下标
	//获取总的像素值
	for (i = 0; i < GrayScale - 1; i++)
	{
		iMulHistogram[i] = i * histogram[i];
		sum += iMulHistogram[i];
	}
	uint16 MinValue, MaxValue;
	for (MinValue = 0; MinValue < GrayScale - 1 && histogram[MinValue] == 0; MinValue++)
		; //获取最小灰度的值
	for (MaxValue = GrayScale - 1; MaxValue > MinValue && histogram[MaxValue] == 0; MaxValue--)
		; //获取最大灰度的值
	//遍历灰度级[0,255]
	if (MaxValue == MinValue)
		return (uint8)MaxValue; // 图像中只有一个颜色
	if (MinValue + 1 == MaxValue)
		return (uint8)MinValue; // 图像中只有二个颜色
	//back：背景像素点；front：前景像素点
	//w：比例；u：平均值；
	uint16 w_back = 0, w_front = 0;
	uint32 u_back_sum = 0, u_front_sum = 0;
	float u_back = 0, u_front = 0;
	float deltaTmp = 0, deltaMax = 0;
	for (i = MinValue; i < MaxValue; i++) // i作为阈值
	{
		//更新前景、后景像素点比例
		w_back += histogram[i];
		w_front = pixelSum - w_back;
		//更新前景、后景像素总灰度值
		u_back_sum += iMulHistogram[i];
		u_front_sum = sum - u_back_sum;
		//更新前景、后景像素平均灰度值
		u_back = u_back_sum / (float)w_back;
		u_front = u_front_sum / (float)w_front;
		//计算类间方差
		deltaTmp = w_back * w_front * (u_back - u_front) * (u_back - u_front) / (float)pixelSum;
		//求最大类间方差
		if (deltaTmp > deltaMax)
		{
			deltaMax = deltaTmp;
			threshold = (uint8)i;
		}
	}
	return threshold;
}
