#include "FRABitamp.h"
#include<stdlib.h>
#include<stdio.h>

int main() {
	FRARawImage* input;
	int ret = OpenBitmapFile("test.bmp", &input);
	printf("%d\n", ret);
	SaveBitmapFile("test_out.bmp", &input);
	return 0;
}