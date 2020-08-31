#pragma once
/*
 * @Description: 图像预处理，二值化的过程
 * @version:
 * @Author: wloves
 * @Date: 2020-05-03 20:14:25
 * @LastEditors: wloves
 * @LastEditTime: 2020-07-23 19:28:59
 */
#ifndef CODE_IMAGEPREDEAL_H_
#define CODE_IMAGEPREDEAL_H_
#include"mainInclude.h"
#include<algorithm>
using namespace std;
uint8 otsu2(uint8 down, uint8 top);
uint8 _otsuThreshold(void);
void imageBinary(void);
void sauvola(const int k, const int windowSize);
void medianFilter(void);
void imageSample(void);
#endif /* CODE_IMAGEPREDEAL_H_ */
