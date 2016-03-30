#include "FRFastBilateral.h"
#include<stdio.h>
#include<stdlib.h>

typedef enum
{
	R = 0,
	G = 1,
	B = 2
}RGB;

typedef struct
{
	size_t bits[3];

}RGB_pixel;

typedef struct
{
	double weight;
	RGB_pixel result;

}mixed_vector;

//clamp implementation
size_t Clamp(size_t min, size_t max, size_t x)
{
	 return
          ( x < (size_t)(min) ) ? (size_t)(min) :
        ( x < (size_t)(max) ) ? (size_t)(x) :
        (size_t)(max);
}

//trilinear interpolation implementation

mixed_vector trilinear_interpolation( mixed_vector*** data, double x, double y, double z, size_t height, size_t width, size_t depth)
{
	mixed_vector output;

    const size_t y_index  = Clamp(0, height-1, (size_t)(y));
    const size_t yy_index = Clamp(0, height-1, y_index+1);
    const size_t x_index  = Clamp(0, width-1, (size_t)(x));
    const size_t xx_index = Clamp(0, width-1, x_index+1);
    const size_t z_index  = Clamp(0, depth-1, (size_t)(z));
    const size_t zz_index = Clamp(0, depth-1, z_index+1);
    const double y_alpha = y - y_index;
    const double x_alpha = x - x_index;
    const double z_alpha = z - z_index;

	//printf(" x : %zd, y : %zd, z : %zd, xx : %zd, yy : %zd, x_alpha : %f, y_alpha : %f\n", x_index, y_index, z_index, xx_index, yy_index, x_alpha, y_alpha);
	//printf(" weight : %f", data[x_index][y_index][z_index].weight);
	//printf(" wow %f", (1.0-y_alpha) * (1.0-x_alpha) * (1.0-z_alpha) * data[x_index][y_index][z_index].weight);
	output.weight =  
		(1.0-y_alpha) * (1.0-x_alpha) * (1.0-z_alpha) * data[x_index][y_index][z_index].weight +
		(1.0-y_alpha) * x_alpha       * (1.0-z_alpha) * data[xx_index][y_index][z_index].weight +
        y_alpha       * (1.0-x_alpha) * (1.0-z_alpha) * data[x_index][yy_index][z_index].weight +
        y_alpha       * x_alpha       * (1.0-z_alpha) * data[xx_index][yy_index][z_index].weight +
        (1.0-y_alpha) * (1.0-x_alpha) * z_alpha       * data[x_index][y_index][zz_index].weight +
        (1.0-y_alpha) * x_alpha       * z_alpha       * data[xx_index][y_index][zz_index].weight +
        y_alpha       * (1.0-x_alpha) * z_alpha       * data[x_index][yy_index][zz_index].weight +
        y_alpha       * x_alpha       * z_alpha       * data[xx_index][yy_index][zz_index].weight;

    for( int c = 0; c < 3; c ++ )
	{
		output.result.bits[c] = 
        (1.0-y_alpha) * (1.0-x_alpha) * (1.0-z_alpha) * data[x_index][y_index][z_index].result.bits[c] +
        (1.0-y_alpha) * x_alpha       * (1.0-z_alpha) * data[xx_index][y_index][z_index].result.bits[c] +
        y_alpha       * (1.0-x_alpha) * (1.0-z_alpha) * data[x_index][yy_index][z_index].result.bits[c] +
        y_alpha       * x_alpha       * (1.0-z_alpha) * data[xx_index][yy_index][z_index].result.bits[c] +
        (1.0-y_alpha) * (1.0-x_alpha) * z_alpha       * data[x_index][y_index][zz_index].result.bits[c] +
        (1.0-y_alpha) * x_alpha       * z_alpha       * data[xx_index][y_index][zz_index].result.bits[c] +
        y_alpha       * (1.0-x_alpha) * z_alpha       * data[x_index][yy_index][zz_index].result.bits[c] +
        y_alpha       * x_alpha       * z_alpha       * data[xx_index][yy_index][zz_index].result.bits[c];
	}

	return output;
}

int FRAPosToIndex_RGB(int x, int y, int width, int height)
{
	int row = ( width * 3 ) * y;
	int col = ( x * 3 );
	return row + col;
}

int FRAPosToIndex(int x, int y, int width, int height)
{
	int row = width * y;
	int col = x;
	return row + col;
}

mixed_vector*** Build3DArray(int width, int height, int depth)
{
	mixed_vector*** data = (mixed_vector***)malloc(width * sizeof(mixed_vector**));
	for( int x = 0; x < width; x ++ )
	{
		data[x] = (mixed_vector**)malloc(height * sizeof(mixed_vector*));
		for( int y = 0; y < height; y ++ )
		{
			data[x][y] = (mixed_vector*)malloc(depth * sizeof(mixed_vector));
			for( int z = 0; z < depth; z ++ )
			{
				data[x][y][z].weight = 0.0;
				data[x][y][z].result.bits[R] = 0;
				data[x][y][z].result.bits[G] = 0;
				data[x][y][z].result.bits[B] = 0;

			}//z
		}//y
	}//x

	return data;
}

void FRAFindMinMaxLoc(FRARawImage* input, int* min, int* max)
{
	int tmpMin = 256;
	int tmpMax = -1;

	for( int i = 0; i < input->height * input->width; i++ )
	{
		unsigned char pixel = input->bits[ i ];
		if( pixel > tmpMax )
		{
			tmpMax = (int)pixel;
		}

		if( pixel < tmpMin )
		{
			
			tmpMin = (int)pixel;
		}
	}

	printf("wow");
	(*min) = (int)tmpMin;
	(*max) = (int)tmpMax;
}

void FRAFindMinMaxLoc_RGB(FRARawImage* input, int color, int* min, int* max)
{
	int tmpMin = 256, minLoc = 0;
	int tmpMax = -1, maxLoc = 0;

	printf("height: %d, width: %d", input->height, input->width);

	for( int y = 0; y < input->height; y++ )
	{
		for( int x = 0; x < input->width; x++ )
		{
			int pos = FRAPosToIndex_RGB(x, y, input->width, input->height);
			int pixel =  input->bits[ pos + color ];
			if( pixel < tmpMin )
			{
				tmpMin = pixel;
				minLoc = pos + color; 
			}

			if( pixel > tmpMax )
			{
				tmpMax = pixel;
				maxLoc = pos + color;
			}
		}//x
	}//y
//do something
	(*min) = tmpMin;
	(*max) = tmpMax;
}

int FRAFastJointBilateralGreenChannel(FRARawImage* input, int sigmaColor, int sigmaSpace)
{
	FRARawImage* green = (FRARawImage*)malloc(sizeof(FRARawImage));

	green->width =  input->width;
	green->height =  input->height;
	green->bytesPerPixel = input->bytesPerPixel / 3;
	green->bits = (unsigned char*)malloc(green->width * green->height * sizeof(unsigned char));

	//set bits
	for( int y = 0; y < input->height; y ++ )
	{
		for( int x = 0; x < input->width; x ++ )
		{
			green->bits[ FRAPosToIndex(x, y, green->width, green->height) ] = input->bits[ FRAPosToIndex_RGB(x, y, input->width, input->height) + G ];
		}
	}
	
	return FRAFastBilateral(input, green, sigmaColor, sigmaSpace);
}

int FRAFastBilateral(FRARawImage* input, FRARawImage* base, int sigmaColor, int sigmaSpace) 
{
	int result = 0;

	const size_t padding_s = 2;
	const size_t padding_c = 2;
	const int height = input->height;
	const int width = input->width;

	size_t small_width = 0, small_height = 0, small_depth = 0;
	int color_min = 0;

	/* filter */

	int* min = (int*)malloc(sizeof(int));
	int* max = (int*)malloc(sizeof(int));
	FRAFindMinMaxLoc(base, min, max);	

	small_width = (size_t)((width - 1) / sigmaSpace ) + 1 + 2 * padding_s;
	small_height = (size_t)((height - 1) / sigmaSpace ) + 1 + 2 * padding_s;
	small_depth = (size_t)(( *max - *min ) / sigmaColor ) + 1 + 2 * padding_c;
	color_min = (*min);

	printf("small width : %zd\n", small_width);
	printf("small height : %zd\n", small_height);
	printf("small depth : %zd %d %d\n", small_depth, *max, *min);
	/*printf("input R : %zd, G: %zd, B: %zd\n", input->bits[ FRAPosToIndex_RGB(1, 1, width, height) ],  input->bits[ FRAPosToIndex_RGB(1, 1, width, height) + G ], 
		 input->bits[ FRAPosToIndex_RGB(1, 1, width, height) + B]);*/
	printf("filter good \n");
	/* downsample */

	//create and initialize 3d array
	mixed_vector*** data = Build3DArray(small_width, small_height, small_depth);
	
	for( int y = 0; y < height; y ++ )
	{
		size_t small_y = (size_t)(1.0* y / sigmaSpace + 0.5) + padding_s;
		for( int x = 0; x < width; x ++ )
		{
			size_t small_x = (size_t)(1.0* x / sigmaSpace + 0.5) + padding_s;
			size_t small_z;
			int index = FRAPosToIndex(x, y, width, height);
			
			small_z =  (size_t)((base->bits[ index ] - color_min) / sigmaColor + 0.5) + padding_c;
			//do something with cool 3d vector!

			mixed_vector tmp = data[small_x][small_y][small_z];
			int index_RGB = FRAPosToIndex_RGB(x, y, width, height);
			tmp.weight += 1.0;
			tmp.result.bits[R] += input->bits[ index_RGB  + R ];
			tmp.result.bits[G] += input->bits[ index_RGB  + G ];
			tmp.result.bits[B] += input->bits[ index_RGB  + B ];
			if( x == 5 && y == 5 )
			{
				printf("x: %zd, y: %zd, z: %zd R : %zd\n", small_x, small_y, small_z, tmp.result.bits[R]); 
			}
			data[small_x][small_y][small_z] = tmp;

		}//x
	}//y

	printf("downsample good \n");
	/* convolution */
	int offset[3];

	offset[0] = &(data[1][0][0]) - &(data[0][0][0]);
	offset[1] = &(data[0][1][0]) - &(data[0][0][0]);
	offset[2] = &(data[0][0][1]) - &(data[0][0][0]);

	mixed_vector*** buffer = Build3DArray(small_width, small_height, small_depth);

	for( int dim = 0; dim < 3; dim ++ )
	{
		const int off = offset[dim];
		for( int iter = 0; iter < 2; iter ++ )
		{
			//swap(buffer, data)
			mixed_vector*** tmp;
			tmp = data;
			data = buffer;
			buffer = tmp;

			for( int x = 1; x < small_width - 1; ++x )
			{
				for( int y = 1; y < small_height - 1; ++y )
				{
					mixed_vector* d_ptr = &(data[x][y][1]);
					mixed_vector* b_ptr = &(buffer[x][y][1]);
					for(int z = 1; z < small_depth - 1; ++z,++d_ptr,++b_ptr){
						mixed_vector b_prev;
						mixed_vector b_curr;
						mixed_vector b_next;
						mixed_vector tmp_vec;
						
						if( dim == 0 )
						{
							b_prev = data[ x - 1 ][y][z];
							b_curr = data[x][y][z];
							b_next = data[ x + 1 ][y][z];
						}
						else if( dim == 1)
						{
							b_prev = data[x][y - 1][z];
							b_curr = data[x][y][z];
							b_next = data[x][y + 1][z];
						}
						else
						{
							b_prev = data[x][y][z - 1];
							b_curr = data[x][y][z];
							b_next = data[x][y][z + 1];
						}
						//printf("dim %d, prev : %zd, curr: %zd, nex: %zd\n", dim, b_prev.result.bits[R],  b_curr.result.bits[R],  b_next.result.bits[R]);
						tmp_vec.weight = ( b_prev.weight + b_next.weight + 2.0 * b_curr.weight ) /4.0;
						//printf("x : %d, y : %d, z : %d\n", x, y, z);
						tmp_vec.result.bits[R] =  ( b_prev.result.bits[R] + b_next.result.bits[R] + 2.0 * b_curr.result.bits[R] ) /4.0;
						tmp_vec.result.bits[G] =  ( b_prev.result.bits[G] + b_next.result.bits[G] + 2.0 * b_curr.result.bits[G] ) /4.0;
						tmp_vec.result.bits[B] =  ( b_prev.result.bits[B] + b_next.result.bits[B] + 2.0 * b_curr.result.bits[B] ) /4.0;

						(*d_ptr) = tmp_vec;
					} //z
				}//y
				
			}//x

		}//iter
	}//dim
	
	printf("convolusion good\n");
	/* upsample */
	double ** weight = (double**)malloc(width * sizeof(double*));

	for( int x = 0; x < width; x ++ )
	{
		weight[x] = (double*)malloc( height * sizeof(double));
		for( int y = 0; y < height; y ++ ){
			weight[x][y] = 0;
		}
	}
/*
	for( int x = 0; x < small_width; x ++ )
	{
		for( int y = 0; y < small_width; y ++ )
		{
			for( int z = 0; z < small_depth; z ++ )
			{
				data[x][y][z].result.bits[R] /= data[x][y][z].weight != 0 ? data[x][y][z].weight : 1;
				data[x][y][z].result.bits[G] /= data[x][y][z].weight != 0 ? data[x][y][z].weight : 1;
				data[x][y][z].result.bits[B] /= data[x][y][z].weight != 0 ? data[x][y][z].weight : 1;
			}
		}
	}*/

	for( int y = 0; y < height; y ++ )
	{
		for( int x = 0; x < width; x ++ )
		{
			size_t z = base->bits[ FRAPosToIndex(x,y,base->width, base->height) ] - color_min;
			double px = (double)(x) / sigmaSpace + padding_s;
            double py = (double)(y) / sigmaSpace + padding_s;
            double pz = (double)(z) / sigmaColor + padding_c;

			mixed_vector out = trilinear_interpolation(data, px, py, pz, small_height, small_width, small_depth);
			
			weight[x][y] = out.weight;
			//printf("bits : %zd %zd %zd %f\n", out.result.bits[R], out.result.bits[G], out.result.bits[B], out.weight);
			input->bits[ FRAPosToIndex_RGB(x, y, width, height) + R ] = out.result.bits[R]/out.weight;
			input->bits[ FRAPosToIndex_RGB(x, y, width, height) + G ] = out.result.bits[G]/out.weight;
			input->bits[ FRAPosToIndex_RGB(x, y, width, height) + B ] = out.result.bits[B]/out.weight;
		}
	}
	return result;
}