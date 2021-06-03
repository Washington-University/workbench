#pragma once

typedef struct tagJxrTestWrapperInitializeInfo
{
	void(*pfnPutData)(
			PKPixelFormatGUID pixeltype,
			unsigned int width,
			unsigned int height,
			unsigned int cLines, 
			void* ptrData, 
			unsigned int stride, 
			void* userParam);
	void* userParamPutData;
} JxrTestWrapperInitializeInfo;
