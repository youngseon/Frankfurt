#include "FRAImageProcedure.h"
#define _CPU_ONLY

//Header files should be located in exact ifdef area.
#ifdef _CPU_ONLY

#endif

#ifdef _CPU_PARRELL

#endif

#ifdef _GPU_PARRELL

#endif

int FRAInvertImage(FRARawImage* input) {
	//Invert image with no parrellization.
#ifdef _CPU_ONLY
	int maxBitsNum = input->height * input->width * 3;
	for (int x = 0; x < maxBitsNum; x++) {
		input->bits[x] = 255 - input->bits[x];
	}
#endif

	//나영선
#ifdef _CPU_PARRELL

#endif

	//김강산
#ifdef _GPU_PARRELL

#endif
}