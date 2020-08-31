/*
 * @Description: ͼ�������ߡ�Ѳ��
 * @version:
 * @Author: wloves
 * @Date: 2020-05-03 20:14:59
 * @LastEditors: wloves
 * @LastEditTime: 2020-08-02 20:22:58
 */
#ifndef CODE_IMAGEDEAL_H_
#define CODE_IMAGEDEAL_H_
#include "mainInclude.h"

struct cliffs
{
	uint8 topExist;
	uint8 topx;
	uint8 topy;
	uint8 downExist;
	uint8 downx;
	uint8 downy;
};
struct imageInformation
{
	//ͼ�������Ϣ
	uint8 bottom;
	uint8 top;
	uint8 roadType;
	uint8 lastMid;
	uint8 flag_round_stage;//0:��ͨ״̬,1:��һ�����ǰ��2���ڶ������ǰ��3�����������ǰ��4������
	uint8 len_col;
	uint8 len;
	int16 flag_differ;
	uint8 straight_sure;
	float k;
	float b;
	struct cliffs Rcliffs;
	struct cliffs Lcliffs;
};

extern struct imageInformation imgInfo;
void imageDeal(void);
float leastSquares(void);
void setTop(uint8 roadType, uint8 i);
void getEffectiveSection(void);
void searchLine(void);
void imgInfoInit(void);
void roundDealCliff(void);
void crossDeal(void);
void turnDeal(void);
void getDiffer(void);
void getLen(void);
void searchLineCross(void);
void getCliffs(void);
uint8 getCorrectMin(uint8* line, uint8* y);
uint8 isStraight(void);
uint8 addLineWithSlope(uint8* line, uint8 len);
uint8 getCorrectLenOfStraight(uint8* line);
#endif /* CODE_IMAGEDEAL_H_ */
