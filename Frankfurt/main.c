#include "FRABitamp.h"
#include<stdlib.h>

int main() {
	FRARawImage* input;
	OpenBitmapFile("test.bmp", &input);
	SaveBitmapFile("test_out.bmp", &input);
	return 0;
}