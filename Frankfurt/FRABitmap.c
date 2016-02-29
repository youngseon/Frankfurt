#include"FRABitamp.h"
#include<stdlib.h>
#include<stdio.h>

// result is allocated by this function.
int OpenBitmapFile(const char* filename, FRABitamp** result) {
	*result = (FRABitamp*)malloc(sizeof(FRABitamp));
	if (*result == NULL) {
		return FRABIT_MEM_ERROR;
	}
	(*result)->bits = NULL;

	if (filename == NULL || filename[0] == '\0') {
		return FRABIT_FILE_ERROR;
	}

	FILE* filePtr;
	filePtr = fopen(filename, "rb");

	if (filePtr == NULL) {
		fclose(filePtr);
		return FRABIT_FILE_ERROR;
	}

	FRABitmapFileHeader fileHeader;
	fread(&fileHeader, sizeof(fileHeader), 1, filePtr);

	//Check if file is bitmap or not.
	if (fileHeader.bfType != ('B' | ((int)'M' << 8))) {
		fclose(filePtr);
		return FRABIT_NOT_SUPPORTED_FORMAT;
	}

	FRABitampInfoHeader infoHeader;
	fread(&infoHeader, sizeof(infoHeader), 1, filePtr);

	//Check header size is equal to size of read data.
	const int frontOfDataPos = ftell(filePtr);
	if (frontOfDataPos != (int)(fileHeader.bfOffBits)) {
		fclose(filePtr);
		return FRABIT_NOT_SUPPORTED_FORMAT;
	}

	const int width = infoHeader.biWidth;
	const int height = infoHeader.biHeight;
	const int bitsPerPixel = infoHeader.biBitCount;
	const int bytesPerPixel = bitsPerPixel / 8;
	const int pitch = (width * bytesPerPixel + 3) & ~3;
	const int dataSize = pitch * height;
	(*result)->width = width;
	(*result)->height = height;
	(*result)->bytesPerPixel = bytesPerPixel;
	(*result)->bits = (char*)malloc(width * height * bytesPerPixel);

	if ((*result)->bits == NULL) {
		fclose(filePtr);
		return FRABIT_MEM_ERROR;
	}

	if (infoHeader.biCompression == 0
		&& bitsPerPixel == 24
		&& (int)infoHeader.biSizeImage == dataSize) {
		//Read 24bit and no compression bitmap
		const int destPitch = width * bytesPerPixel;
		const int gap = pitch - destPitch;
		char* curDest = (*result)->bits + height * destPitch;
		for (int y = height; y > 0; y--) {
			curDest -= destPitch;
			fread(curDest, destPitch, 1, filePtr);
			if (gap > 0) {
				fseek(filePtr, gap, SEEK_CUR);
			}
			for (int x = 0; x < width; x++) {
				char temp = curDest[x * 3];
				curDest[x * 3] = curDest[x * 3 + 2];
				curDest[x * 3 + 2] = temp;
			}
		}
	}
	else {
		fclose(filePtr);
		return FRABIT_NOT_SUPPORTED_FORMAT;
	}

	//Check if there no data remain.
	const int expectedEnd =
		fileHeader.bfOffBits + infoHeader.biSizeImage;
	if (expectedEnd == ftell(filePtr)) {
		fclose(filePtr);
		return FRABIT_SUCCESS;
	}
	else {
		fclose(filePtr);
		return FRABIT_NOT_SUPPORTED_FORMAT;
	}
}