#pragma once

#pragma pack(push,1)
typedef struct {
	unsigned short bfType;
	unsigned int   bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned int   bfOffBits;
} FRABitmapFileHeader;

typedef struct {
	unsigned int    biSize;
	int             biWidth;
	int             biHeight;
	unsigned short  biPlanes;
	unsigned short  biBitCount;
	unsigned int    biCompression;
	unsigned int    biSizeImage;
	int             biXPelsPerMeter;
	int             biYPelsPerMeter;
	unsigned int    biClrUsed;
	unsigned int    biClrImportant;
} FRABitampInfoHeader;
#pragma pack(pop)

typedef enum {
	FRABIT_MEM_ERROR = -1,
	FRABIT_SUCCESS = 0,
	FRABIT_FILE_ERROR,
	FRABIT_NOT_SUPPORTED_FORMAT
} FRABitmapError;

typedef struct {
	int width;
	int height;
	int bytesPerPixel;
	char* bits;
} FRABitamp;

int OpenBitmapFile(const char* filename, FRABitamp** result);