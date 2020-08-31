/*
 * @filename:imageDeal.c
 * @version: 1.0
 * @Description: 图像处理、搜线、巡线
 * @Author: wloves
 * @Date: 2020 2020年5月3日 下午8:14:50
 * @LastEditors: wloves
 * @LastEditTime: 2020-08-23 20:34:22
 */
#include "imageDeal.h"

#define DEFAULT_MID (IMAGE_W / 2)
#define DEFAULT_LEFT 2
#define DEFAULT_RIGHT COLUMN - 3
 /**
  * @name: 丢线判断宏定义
  * @details: 通过简单的等于与不等于判断
  * @param {uint8 行号}
  * @return: 是否丢线
  */
#define isLoseL(i) (LL[i] == DEFAULT_LEFT)
#define isLoseR(i) (RL[i] == DEFAULT_RIGHT)
#define notLoseL(i) (LL[i] != DEFAULT_LEFT)
#define notLoseR(i) (RL[i] != DEFAULT_RIGHT)
#define isRound (imgInfo.flag_round_stage > 0 && imgInfo.flag_round_stage < 6)
  /**
   * @name: limitRl
   * @details: 边线限幅内联函数，保证补线时边线不越界
   * @param {uint8 边线列号}
   * @return: 限幅后的列号
   */
inline uint8 limitRL(int16 input)
{
	if (input < 0)
		return 0;
	else if (input < COLUMN)
		return (uint8)input;
	else
		return COLUMN;
}
uint8 RL[ROW], ML[ROW], LL[ROW];
uint8 RLB[ROW], MLB[ROW], LLB[ROW];
int avg_mid;
uint8 whitePoint[ROW];
uint8 width[ROW];
struct imageInformation imgInfo;
uint8 half_width[ROW] = { 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 19, 20, 21, 22, 23, 24, 25, 26, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 42, 44, 46, 48, 50, 52, 54, 56, 58, 61, 63, 65, 67, 69, 71, 73, 75, 77, 79, 81, 83, 85, 87, 89, 90, 91, 91, 91, 91 };
/**
 * @name: imageDeal
 * @details: 图像处理函数；当前版本仅适用于直道、小弯道、环岛
 * @param {二值化数组}
 * @return: differ
 */
void imageDeal(void)
{
	//搜线
	uint8 i;
	for (i = 0; i < ROW; i++)
	{
		imageBin[i][2] = 1;
		imageBin[i][1] = 0;
		imageBin[i][0] = 0;
		imageBin[i][COLUMN - 3] = 1;
		imageBin[i][COLUMN - 2] = 0;
		imageBin[i][COLUMN - 1] = 0;
	}
	//getLen();
	searchLine();
	imgInfo.len = getCorrectLenOfStraight(ML);
	getCliffs();
	imgInfo.straight_sure = isStraight();
	memcpy(RLB, RL, ROW);
	memcpy(LLB, LL, ROW);
	memcpy(MLB, ML, ROW);
	imgInfo.roadType = ROAD_straight;
	imgInfo.lastMid = (ML[ROW - 4] + ML[ROW - 3] + ML[ROW - 2] + ML[ROW - 1]) >> 2;
	//弯道处理
	imgInfo.k = leastSquares();
	turnDeal();
	//环岛处理
	roundDealCliff();
	//得到最终differ
	//crossDeal();
	getDiffer();

}
/**
 * @name: getLen
 * @details: 判断最长直道长度
 * @param {void}
 * @return: 最长直道长度及对应列号存入imgInfo数组
 */
void getLen(void)
{
	int8 len;
	uint8 j;
	imgInfo.len = 0;
	for (j = 4; j < COLUMN; j += 10)
	{
		for (len = ROW - 2; len >= 0 && imageBin[len][j]; len -= 2)
			;
		len = ROW - len;
		if (imgInfo.len < len)
		{
			imgInfo.len = len;
			imgInfo.len_col = j;
		}
	}
	/**
 * @name: imgInfoInit
 * @details: 初始化imgInfo结构体
 * @param {type}
 * @return:
 */
}
void imgInfoInit(void)
{
	imgInfo.bottom = ROW - 1;
	imgInfo.lastMid = DEFAULT_MID;
	imgInfo.roadType = ROAD_straight;
	imgInfo.top = 0;
	imgInfo.flag_round_stage = 0;
	imgInfo.flag_differ = 0;
}
/**
 * @name: getDiffer
 * @details: 获取当前偏差值
 * @param {void}
 * @return: differ
 */
void getDiffer(void)
{
	//修改以下部分，并配合合适的pid参数，以达到最好效果
	int8 i;
	int16 sum = 0;
	int8 lineNum = 1;
	for (i = imgInfo.top; i < imgInfo.bottom; i++)
	{
		MLB[i] = (LLB[i] + RLB[i]) >> 1;
	}
#if 0
	//权重设置：近处权重大，远处权重小
	for (i = imgInfo.top; i < ROW / 2; i++)
	{
		sum += MLB[i];
	}
	for (; i < imgInfo.bottom; i++)
	{
		sum += MLB[i] * 2;
	}
	if (ROW / 2 - imgInfo.top > 0)
		lineNum += ROW / 2 - imgInfo.top;
	if (2 * imgInfo.bottom - ROW > 0)
		lineNum += 2 * imgInfo.bottom - ROW;
	sum /= lineNum;
#elif 0
	//权重设置：近处权重小，远处权重大
	for (i = imgInfo.top; i < imgInfo.bottom; i++)
	{
		sum += MLB[i];
	}
	lineNum = imgInfo.bottom - imgInfo.top;
	sum /= lineNum;
#else
	//取底下十行
	if (imgInfo.flag_round_stage == 0 || imgInfo.flag_round_stage == 6)
	{
		uint8 start, end;
		uint8 len = imgInfo.bottom - imgInfo.top;
		start = imgInfo.bottom;
		end = start - len / 2;
		if (start - end < 15)
		{
			end = start - 15;
		}
		if (end < imgInfo.top)
			end = imgInfo.top;
		lineNum = 0;
		for (i = start; i > end; i--)
		{
			sum += MLB[i];
			lineNum++;
		}
		sum /= lineNum;
	}
	else
	{
		for (i = imgInfo.top; i < imgInfo.bottom; i++)
		{
			sum += MLB[i];
		}
		lineNum = imgInfo.bottom - imgInfo.top;
		sum /= lineNum;
	}

#endif
#if 0
	//对转向偏差进行二次函数处理
	if (sum < 15)
		sum = sum / 2;
	else
		sum = sum * myabs(sum) / 30;
	if (imgInfo.len > 30)
	{
		if (sum > 0)
			sum = sum * sum / 48;
		else
			sum = -sum * sum / 48;
	}

	if (sum > 95)
		sum = 95;
	if (sum < -95)
		sum = -95;
#endif
	sum -= DEFAULT_MID;
	debugFlag.differOrigin = sum;

	if (imgInfo.flag_differ != 0)
	{

		differ = imgInfo.flag_differ;
		imgInfo.flag_differ = 0;
	}
	else
		differ = sum;
}
/**
 * @name: getCliffs
 * @details: 获取前后左右四个拐点
 * @param {void}
 * @return: 结果存入Lcliffs和Rcliffs结构体
 */
void getCliffs(void)
{
	int8 i;
	imgInfo.Lcliffs.downExist = 0;
	imgInfo.Lcliffs.topExist = 0;
	imgInfo.Rcliffs.downExist = 0;
	imgInfo.Rcliffs.topExist = 0;
	uint8 top = imgInfo.top;
	uint8 bottom = ROW - 6;
	top = (top - 5 >= 0) ? (top - 5) : top;
	for (i = bottom; i >= top; i--)
	{
		if (LL[i] >= LL[i + 2] && LL[i] >= LL[i - 2] * 2 + 10 && LL[i - 2] <= 20 && LL[i - 4] <= 20)
			if (imageBin[i - 1][LL[i]] && imageBin[i][LL[i] + 1])
				if (LL[i] > 2 && LL[i] < COLUMN - 3 && i > 2 && i < ROW - 2)
				{
					imgInfo.Lcliffs.downx = LL[i];
					imgInfo.Lcliffs.downy = i;
					imgInfo.Lcliffs.downExist = 1;
					break;
				}
	}
	for (i = bottom; i >= top; i--)
	{
		if (LL[i] > LL[i + 2] * 2 + 3 && myabs(LL[i] - LL[i - 1]) < 5 && LL[i + 3] < 10 && LL[i] > 30)
			if (imageBin[i + 1][LL[i]] && imageBin[i][LL[i] + 1])
				if (LL[i] > 2 && LL[i] < COLUMN - 3 && i > 2 && i < ROW - 2)
				{
					imgInfo.Lcliffs.topx = LL[i];
					imgInfo.Lcliffs.topy = i;
					imgInfo.Lcliffs.topExist = 1;
					break;
				}
	}
	for (i = bottom; i >= top; i--)
	{
		if (RL[i] <= RL[i + 2] && RL[i] <= DEFAULT_RIGHT - (DEFAULT_RIGHT - RL[i - 2]) * 2 - 10 && RL[i - 2] >= DEFAULT_RIGHT - 20 && RL[i - 4] >= DEFAULT_RIGHT - 20)
			if (imageBin[i - 1][RL[i]] && imageBin[i][RL[i] - 1])
				if (RL[i] > 2 && RL[i] < COLUMN - 3 && i > 2 && i < ROW - 2)
				{
					imgInfo.Rcliffs.downx = RL[i];
					imgInfo.Rcliffs.downy = i;
					imgInfo.Rcliffs.downExist = 1;
					break;
				}
	}
	for (i = bottom; i >= top; i--)
	{
		if (RL[i] < DEFAULT_RIGHT - (DEFAULT_RIGHT - RL[i + 2]) * 2 - 3 && myabs(RL[i] - RL[i - 1]) < 5 && RL[i + 3] > DEFAULT_RIGHT - 10 && RL[i] < DEFAULT_RIGHT - 30)
			if (imageBin[i + 1][RL[i]] && imageBin[i][RL[i] - 1])
				if (RL[i] > 2 && RL[i] < COLUMN - 3 && i > 2 && i < ROW - 2)
				{
					imgInfo.Rcliffs.topx = RL[i];
					imgInfo.Rcliffs.topy = i;
					imgInfo.Rcliffs.topExist = 1;
					break;
				}
	}
}
/**
 * @name: crossDeal
 * @details: 十字处理函数，待完善
 * @param {type}
 * @return:
 */
void crossDeal(void)
{
	// static uint8 cross_count=0;
	static uint8 cross_flag = 0;
	if (isRound)
		return;
	if ((imgInfo.Rcliffs.topExist || imgInfo.Rcliffs.downExist) && (imgInfo.Lcliffs.topExist || imgInfo.Lcliffs.downExist))
		if ((imgInfo.Rcliffs.topExist + imgInfo.Rcliffs.downExist) + (imgInfo.Lcliffs.topExist + imgInfo.Lcliffs.downExist) >= 3)
			cross_flag++;
	if (2 == cross_flag)
	{
		if (!imgInfo.Rcliffs.downExist && !imgInfo.Lcliffs.downExist && imgInfo.Rcliffs.topExist && imgInfo.Lcliffs.topExist)
		{
			cross_flag = 0;
		}
		imgInfo.roadType = ROAD_cross;
		imgInfo.bottom = ROW - 1;
		imgInfo.top = ROW / 2;
	}
	// if (cross_flag)
	// {
	// 	imgInfo.roadType = ROAD_cross;
	// 	searchLineCross();
	// }
	// // cross_count=imgInfo.Rcliffs.topExist+imgInfo.Rcliffs.downExist+imgInfo.Lcliffs.downExist+imgInfo.Lcliffs.topExist;
	// if ((imgInfo.Rcliffs.topExist || imgInfo.Rcliffs.downExist) && (imgInfo.Lcliffs.topExist || imgInfo.Lcliffs.downExist))
	// 	cross_flag = 1;
	// else
	// 	cross_flag = 0;
	// if (cross_flag)
	// {
	// }
	// for (i = imgInfo.bottom; i >= imgInfo.top && isLoseR(i); i--)
	// 	;
	// for (; i >= imgInfo.top && notLoseR(i); i--)
	// 	;
	// Rlose.downx = RL[i];
	// Rlose.downy = i;
	// for (; i >= imgInfo.top && isLoseR(i); i--)
	// 	;
	// Rlose.topx = RL[i];
	// Rlose.topy = i;
	// for (i = imgInfo.bottom; i >= imgInfo.top && isLoseL(i); i--)
	// 	;
	// for (; i >= imgInfo.top && notLoseL(i); i--)
	// 	;
	// Llose.downx = LL[i];
	// Llose.downy = i;
	// for (; i >= imgInfo.top && isLoseL(i); i--)
	// 	;
	// Llose.topx = LL[i];
	// Llose.topy = i;
	// Llose.len=Llose.downy - Llose.topy;
	// Rlose.len=Rlose.downy - Rlose.topy ;
	// if ( Llose.len> ROW / 6 && Rlose.len> ROW / 6)
	// {
	// 	imgInfo.roadType=ROAD_big_turn;
	// 	imgInfo.top=(Rlose.downy+Llose.downy)>>1;
	// 	// Llose.midy = Llose.topy + Llose.downy;
	// 	// Rlose.midy = Rlose.topy + Rlose.downy;
	// 	// if (Llose.midy < Rlose.midy)
	// 	// {
	// 	// 	for (i = Llose.downy; i > Llose.topy; i--)
	// 	// 		if (RL[i] - half_width[i] * 2 > 0)
	// 	// 			LLB[i] = RL[i] - half_width[i] * 2;
	// 	// }
	// 	// else
	// 	// {
	// 	// 	for (i = Rlose.downy; i > Rlose.topy; i--)
	// 	// 		if (LL[i] + half_width[i] * 2 < 188)
	// 	// 			RLB[i] = LL[i] + half_width[i] * 2;
	// 	// }
	// }
}
/**
 * @name: turnDeal
 * @details: 弯道处理函数
 * @param {imgInfo.k}
 * @return: 道路类型
 */
void turnDeal(void)
{
	//根据最小二乘法拟合得到的斜率判断是否为弯道
	int16 i;
	if (imgInfo.len < 40)
		imgInfo.roadType = ROAD_trun;
	else if (imgInfo.len < 30)
		imgInfo.roadType = ROAD_big_turn;
	if (imgInfo.k > 1)
		imgInfo.roadType = ROAD_turn_left;
	else if (imgInfo.k < -1)
		imgInfo.roadType = ROAD_turn_right;
	else if (imgInfo.k > 1.6)
		imgInfo.roadType = ROAD_big_turn_left;
	else if (imgInfo.k < -1.6)
		imgInfo.roadType = ROAD_big_turn_right;
	else
		imgInfo.roadType = ROAD_straight;
	if (imgInfo.roadType == ROAD_turn_left)
	{
		for (i = imgInfo.bottom; i > imgInfo.top; i--)
		{
			if (isLoseL(i) && RL[i] - 2 * half_width[i] > 0)
				LLB[i] = RL[i] - 2 * half_width[i];
		}
	}
	else if (imgInfo.roadType == ROAD_turn_right)
	{
		for (i = imgInfo.bottom; i > imgInfo.top; i--)
		{
			if (isLoseR(i) && LL[i] + 2 * half_width[i] < 188)
				RLB[i] = LL[i] + 2 * half_width[i];
		}
	}
	// int8 i;
	// int8 start;
	// uint8 section = imgInfo.top / (ROW / 8);
	// uint8 left_top = 0;
	// uint8 right_top = 0;
	// uint8 judgeHold = (imgInfo.bottom - imgInfo.top) / 5;
	// start = imgInfo.top - 4;
	// start = (start >= 1) ? start : 1;
	// for (i = start; i < imgInfo.bottom; i++)
	// {
	// 	if (LL[i] != DEFAULT_LEFT && LL[i - 1] != DEFAULT_LEFT)
	// 	{
	// 		left_top = i;
	// 		break;
	// 	}
	// }
	// for (i = start; i < imgInfo.bottom; i++)
	// {
	// 	if (RL[i] != DEFAULT_RIGHT && RL[i - 1] != DEFAULT_RIGHT)
	// 	{
	// 		right_top = i;
	// 		break;
	// 	}
	// }
	// uint16 rightMin = RL[ROW - 1] + RL[ROW - 2] + RL[ROW - 3];
	// uint8 rightIndex = 0;
	// uint16 leftMax = LL[ROW - 1] + LL[ROW - 2] + LL[ROW - 3];
	// uint8 leftIndex = 0;
	// for (i = ROW - 4; i >= 2; i -= 3)
	// {
	// 	if (RL[i] + RL[i - 1] + RL[i - 2] < rightMin)
	// 	{
	// 		rightMin = RL[i] + RL[i - 1] + RL[i - 2];
	// 		rightIndex = i - 1;
	// 	}
	// }
	// for (i = ROW - 4; i >= 2; i -= 3)
	// {
	// 	if (LL[i] + LL[i - 1] + LL[i - 2] > leftMax)
	// 	{
	// 		leftMax = LL[i] + LL[i - 1] + LL[i - 2];
	// 		leftIndex = i - 1;
	// 	}
	// }
	// switch (section)
	// {
	// case 0:
	// 	imgInfo.roadType = ROAD_straight;
	// 	break;
	// case 1:
	// case 2:
	// case 3:
	// case 4:
	// case 5:
	// case 6:
	// 	if (LL[imgInfo.top + 1] > COLUMN * 3 / 4 || right_top > left_top + judgeHold || leftMax > COLUMN * 3 / 4||imgInfo.k<-1)
	// 		imgInfo.roadType = ROAD_turn_right;
	// 	if (RL[imgInfo.top + 1] < COLUMN / 4 || left_top > right_top + judgeHold || rightMin < COLUMN / 4||imgInfo.k>1)
	// 		imgInfo.roadType = ROAD_turn_left;
	// 	break;
	// case 7:
	// 	if (((ML[ROW * 80 / 100] + ML[ROW * 85 / 100] + ML[ROW * 90 / 100] + ML[ROW * 95 / 100]) >> 2) > 0)
	// 		imgInfo.roadType = ROAD_big_turn_right;
	// 	else
	// 		imgInfo.roadType = ROAD_big_turn_left;
	// 	break;
	// default:
	// 	break;
	// }
	// if (imgInfo.len < 30)
	// 	imgInfo.roadType = ROAD_trun;
}
#define storeN (uint8)(1 + COLUMN / 8)
/**
 * @name: getCorrectMin
 * @details: 暂未使用
 * @param {type}
 * @return:
 */
uint8 getCorrectMin(uint8* line, uint8* y)
{
	uint8 i;
	uint8 store[storeN] = { 0 };
	for (i = ROW; i > 0; i--)
	{
		store[line[i] >> 3] = i - 1;
	}
	for (i = 0; i < storeN - 1; i++)
	{
		if (store[i] && store[i + 1])
			break;
	}
	*y = i;
	if (i == storeN - 1)
		return 255;
	else
		return i * 8 + 4;
}
/**
 * @name: roundDealCliff
 * @details: 环岛处理函数
 * @param {边线}
 * @return: 环岛补线
 */
void roundDealCliff(void)
{
	// if(imgInfo.straight_sure)
	// {
	// 	imgInfo.flag_round_stage=0;
	// 	return;
	// }
	int8 i;
	uint8 k;
	uint8 nowStage = imgInfo.flag_round_stage;
	static uint8 count = 0;
	//寻找入口处下面的拐点
	uint8 sum = 0;
	uint8 minR = COLUMN;
	uint8 minIndexR = 0;
	imgInfo.flag_round_stage = 2;
	//阶段处理部分（决定在阶段内该如何行动
	switch (imgInfo.flag_round_stage)
	{
	case 0:
		//入环前
		//imgInfo.roadType = ROAD_straight;
		break;
	case 1:
		//准备入环阶段
		imgInfo.roadType = ROAD_straight;
		for (i = 0; i < ROW; i++)
		{
			if (isLoseL(i))
				LLB[i] = limitRL(RL[i] - 2 * half_width[i]);
		}

		break;
	case 2:
		//入环阶段
		imgInfo.roadType = ROAD_straight;
		if (!imgInfo.Lcliffs.topExist)
		{
			imgInfo.Lcliffs.topx = limitRL(RL[15] - half_width[15] * 2);
			imgInfo.Lcliffs.topy = 10;
		}
		k = ((imgInfo.Lcliffs.topx + RL[ROW * 80 / 100]) >> 1);
		addLineWithSlope(RLB + imgInfo.Lcliffs.topy, 48 - imgInfo.Lcliffs.topy + 1);
		imgInfo.top = imgInfo.Lcliffs.topy;
		if (imgInfo.top > ROW - 10)
			imgInfo.top = ROW - 10;
		imgInfo.bottom = ROW - 1;

		break;
	case 3:
		//环内阶段
		//按照右边跑
		imgInfo.roadType = ROAD_straight;
		for (i = ROW - 3; i >= 2; i--)
		{
			if (isLoseL(i))
			{
				if (RL[i] >= half_width[i] * 2)
					LLB[i] = limitRL(RL[i] - half_width[i] * 2) + 45;
				else
					LLB[i] = 45;
			}
			if (i < ROW - 10 && RL[i] < RL[i - 2] && RL[i] < RL[i + 2] && RL[i] < RL[i - 1] && notLoseR(i + 2))
			{
				imgInfo.top = i;
				break;
			}
		}
		break;
	case 4:
		imgInfo.roadType = ROAD_straight;
		break;
	case 5:
		//防止再次入环阶段
		//根据左拐点进行补线，防止再次入环
		imgInfo.roadType = ROAD_straight;
		for (i = 10; i < ROW - 10; i++)
		{
			if (LL[i] < 30)
			{
				LLB[i] = limitRL(RL[i] - half_width[i] * 2 + 10);
				sum++;
			}
		}
		break;
	default:
		break;
	}
	static uint8 timer = 0;
	//阶段变更部分（决定何时进入下一个阶段)
	switch (imgInfo.flag_round_stage)
	{
	case 0:
		//如果找到入口处拐点，进入下一阶段
		if (imgInfo.Lcliffs.downExist && imgInfo.Lcliffs.downy >= 0.2 * ROW && !imgInfo.Rcliffs.downExist && !imgInfo.Rcliffs.topExist)
		{
			imgInfo.flag_round_stage++;
			carCtrl.distance = 0;
		}
		break;
	case 1:
		//如果找到入口处上拐点，进入下一阶段
		if (imgInfo.Lcliffs.topExist && !imgInfo.Rcliffs.topExist && notLoseR(30))
		{
			for (i = imgInfo.Lcliffs.topy + 10; i < ROW; i += 3)
			{
				if (LL[i] > 4)
					break;
			}
			if (i >= ROW - 5)
				imgInfo.flag_round_stage++;
		}
		if (carCtrl.distance > 20000 && !imgInfo.Lcliffs.topExist && !imgInfo.Lcliffs.downExist)
		{
			imgInfo.flag_round_stage = 0;
		}
		break;
	case 2:
		//根据拐点进行补线入环
		for (i = ROW - 2; i > 1 && RL[i - 1] <= RL[i]; i--)
			;
		if (RL[i + 1] < 80 && RL[i] < 80 && imgInfo.len < 40)
			imgInfo.flag_round_stage++;
		break;
	case 3:
		//按照右边线跑
		if (imgInfo.top > ROW * 2 / 3)
		{
			i = imgInfo.top;
			if (i < ROW - 10 && RL[i] < RL[i - 2] && RL[i] < RL[i + 2] && RL[i] < RL[i - 1] && notLoseR(i + 2))
				imgInfo.flag_round_stage++;
		}

		// for (i = ROW - 1; i >= 0 && LL[i] < COLUMN / 2; i -= 10)
		// 	;
		// if (i <= -1)
		// 	if (imgInfo.len > ROW * 0.7 && notLoseR(10) && notLoseR(20) && notLoseR(30) && RL[50] > RL[30] && RL[30] > RL[10])
		// 	{
		// 		imgInfo.flag_round_stage++;
		// 	}
		break;
	case 4:
		imgInfo.flag_differ = -45;
		for (i = ROW - 6; i > ROW * 0.1; i--)
			if (isLoseR(i))
				break;
		timer++;
		if (timer > 7)
		{
			imgInfo.flag_round_stage++;
			timer = 0;
		}
		break;
	case 5:
		//根据左拐点进行补线，防止再次入环
		if (imgInfo.len > ROW * 0.9)
			imgInfo.flag_round_stage++;
		break;
	default:
		break;
	}

	if (carCtrl.distance > 14000 && imgInfo.flag_round_stage != 6)
	{
		//imgInfo.flag_round_stage = 6;
	}
	if (nowStage != imgInfo.flag_round_stage)
		carCtrl.distance = 0;
}

/***************搜线函数*******************/
/**
 * @name: searchLine
 * @details: 普通搜线方法；搜线原则：从中线往左右两边搜线，若搜到改行中点为黑点，则重新搜线
 * @param {二值化数组}
 * @return: RL、LL、ML三个数组，分别为右、左、中
 */
void searchLine(void)
{
	int8 i; //i必须为int
	uint8 j;
	uint8 flag = 0;
	imgInfo.roadType = ROAD_straight;
	imgInfo.bottom = ROW - 1;
	imgInfo.top = 0;
	pid_set(ROAD_straight);
	uint8 start;
	//先搜最上面一行的左右边线
	//以下6行为哨兵元素，作用：使图像的左右边界始终符合退出循环条件，如此即可减少对于循环是否完毕的判断。
	for (j = imgInfo.lastMid; !imageBin[ROW - 1][j] || imageBin[ROW - 1][j - 1] || imageBin[ROW - 1][j - 2]; j--)
		;
	LL[ROW - 1] = j;
	for (j = imgInfo.lastMid; !imageBin[ROW - 1][j] || imageBin[ROW - 1][j + 1] || imageBin[ROW - 1][j + 2]; j++)
		;
	RL[ROW - 1] = j;
	ML[ROW - 1] = (RL[ROW - 1] + LL[ROW - 1]) >> 1;
	/*****************搜左右边线***********************/
	imgInfo.bottom = ROW - 1;
	uint8 sum;

	for (i = imgInfo.bottom - 1; i >= 0; i--)
	{
		start = imgInfo.len_col;
		if (start < LL[i + 1] + 5 || start > RL[i + 1] - 5)
		{
			start = ML[i + 1];
		}
		for (j = start; !imageBin[i][j] || imageBin[i][j - 1] || imageBin[i][j - 2]; j--)
			;
		LL[i] = j;
		for (j = start; !imageBin[i][j] || imageBin[i][j + 1] || imageBin[i][j + 2]; j++)
			;
		RL[i] = j;
		ML[i] = (RL[i] + LL[i]) >> 1;
		// if (!imageBin[i][ML[i]])
		// {
		// 	for (j = RL[i + 1]; !imageBin[i][j] || imageBin[i][j - 1] || imageBin[i][j - 2]; j--)
		// 		;
		// 	LL[i] = j;
		// 	ML[i] = (RL[i] + LL[i]) >> 1;
		// }
		for (sum = 0, j = LL[i]; j < RL[i]; j++)
		{
			sum += imageBin[i][j];
		}
		whitePoint[i] = sum;
		width[i] = RL[i] - LL[i];
		if (!flag && i < ROW - 10)
			if ((i < ROW * 0.5 && width[i] - whitePoint[i] > 4) || (width[i] < 4) || (whitePoint[i] < 4))
			{
				imgInfo.top = i;
				flag = 1;
			}
	}
}
/**
 * @name: searchLineCross
 * @details: 十字搜线方法：搜线原则：从右边线往左搜线，并从左边线往右搜线
 * @param {二值化数组}
 * @return: RL、LL、ML三个数组，分别为右、左、中
 */
void searchLineCross(void)
{
	int8 i; //i必须为int
	uint8 j;
	uint8 flag = 0;
	//先搜最上面一行的左右边线
	//以下6行为哨兵元素，作用：使图像的左右边界始终符合退出循环条件，如此即可减少对于循环是否完毕的判断。
	for (j = DEFAULT_RIGHT; !imageBin[ROW - 1][j] || imageBin[ROW - 1][j - 1] || imageBin[ROW - 1][j - 2]; j--)
		;
	LLB[ROW - 1] = j;
	for (j = DEFAULT_LEFT; !imageBin[ROW - 1][j] || imageBin[ROW - 1][j + 1] || imageBin[ROW - 1][j + 2]; j++)
		;
	RLB[ROW - 1] = j;
	MLB[ROW - 1] = (RLB[ROW - 1] + LLB[ROW - 1]) >> 1;
	/*****************搜左右边线***********************/
	for (i = ROW - 2; i >= 0; i--)
	{
		for (j = RLB[i + 1]; !imageBin[i][j] || imageBin[i][j - 1] || imageBin[i][j - 2]; j--)
			;
		LLB[i] = j;
		for (j = LLB[i + 1]; !imageBin[i][j] || imageBin[i][j + 1] || imageBin[i][j + 2]; j++)
			;
		RLB[i] = j;
		if (i < ROW - 10)
		{
			if (RLB[i] > 1 + RLB[i + 1])
				RLB[i] = RLB[i + 1] - half_width[i + 1] + half_width[i];
			if (LLB[i] < LLB[i + 1] - 1)
				LLB[i] = LLB[i + 1] + half_width[i + 1] - half_width[i];
		}

		MLB[i] = (RLB[i] + LLB[i]) >> 1;
		width[i] = RLB[i] - LLB[i];
	}
}
void setTop(uint8 roadType, uint8 i)
{
	imgInfo.roadType = roadType;
	imgInfo.top = i;
	pid_set(roadType);
}
/**
 * @name: getEffectiveSection
 * @details: 粗略获取左右引导线、中线的有效区间；
 * @param {二值化数组}
 * @return: 结果存入imgInfo结构体
 */
void getEffectiveSection(void)
{
	uint8 i;
	for (i = 0; i < ROW - 1; i++)
		if (LL[i] - 2 || RL[i] - (COLUMN - 3))
			break;
	imgInfo.bottom = i;
	for (i = ROW - 1; i > 0; i--)
		if (LL[i] - 2 || RL[i] - (COLUMN - 3))
			break;
	for (; i > imgInfo.bottom; i--)
		if (RL[i] - LL[i] > 2)
			break;
	imgInfo.top = i;
}
/***************最小二乘法求斜率函数*******************/
/**
 * @name: leastSquares
 * @details: 最小二乘法求取中线斜率
 * @param {二值化数组}
 * @return: 斜率b
 */
float leastSquares(void)
{
	float k, b;
	int16 sum1 = 0;
	uint16 sum2 = 0;
	int16 xavg = 0;
	int16 yavg = 0;
	int8 i;
	xavg = (imgInfo.bottom + imgInfo.top) / 2;
	for (i = imgInfo.bottom; i > imgInfo.top; i--)
		yavg += ML[i];
	yavg /= (imgInfo.bottom - imgInfo.top + 1);
	for (i = imgInfo.bottom; i >= imgInfo.top; i--)
	{
		sum1 += (i - xavg) * (ML[i] - yavg);
		sum2 += (i - xavg) * (i - xavg);
	}
	if (sum2 != 0)
		k = sum1 / (float)sum2;
	else
		k = 255;
	return k;
}

uint8 isStraight(void)
{
	int8 i;
	uint8 notLose = 0;
	for (i = ROW - 5; i >= 5; i--)
	{
		if (notLoseL(i) && notLoseR(i))
			notLose++;
		if (RL[i] - RL[i + 1] > 5 && RL[i - 1] - RL[i + 2] > 5)
			return 0;
		if (LL[i + 1] - LL[i] > 5 && LL[i + 2] - LL[i - 1] > 5)
			return 0;
	}
	if (notLose > 45)
	{
		return 1;
	}
	return 0;
}
uint8 addLineWithSlope(uint8* line, uint8 len)
{
	if (len <=1)
		return 0;
	for (uint8 i = 0; i < len; i++)
		line[i] = line[0] + i * (line[len - 1] - line[0]) / (len - 1);
	return 0;
}
uint8 getCorrectLenOfStraight(uint8* line)
{
	//uint16 sum=0;
	//uint8 avgMid;
	//int8 i;
	//for (i = ROW - 1; i >= 0; i--)
	//{
	//	sum += line[i];
	//	avgMid = sum / (ROW - i);
	//	if(line[i]-avgMid)
	//}
	//return imgInfo.len;
	return 0;
}