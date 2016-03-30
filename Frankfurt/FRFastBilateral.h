#pragma once
#include"FRABitamp.h"

int FRAFastBilateral(FRARawImage* input, FRARawImage* base, int sigmaColor, int sigmaSpace);
int FRAFastJointBilateralGreenChannel(FRARawImage* input, int sigmaColor, int sigmaSpace);
