#pragma once

typedef enum {
	FRA_MEM_NULL_ERROR = -2,
	FRA_MEM_ALLOC_ERROR = -1,
	FRA_SUCCESS = 0,
	FRA_FILE_ERROR,
	FRA_NOT_SUPPORTED_FORMAT,
	FRA_NO_CL_KERNEL_FILE,
	FRA_NO_CL_PLATORM,
	FRA_CL_DEVICE_ERROR,
	FRA_CL_PROGRAM_BUILD_ERROR
} FRAError;