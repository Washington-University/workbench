#include <stdlib.h>
#include <string.h>

#include "JXRGlue.h"
//#include "JXRTestWrapper.h"
#include "strcodec.h"
#include "JXRTest.h"

ERR PKImageEncode_Initialize_Wrapper(PKImageEncode* pIE, struct WMPStream* stream, void* vp, size_t size)
{
	if (size!=sizeof(JxrTestWrapperInitializeInfo))
	{
		return WMP_errInvalidArgument;
	}

	JxrTestWrapperInitializeInfo* ptrInfo = (JxrTestWrapperInitializeInfo*)vp;
	pIE->WRAPPER.info = *ptrInfo;
	return WMP_errSuccess;
}

ERR PKImageEncode_WritePixels_Wrapper(
	PKImageEncode* pIE,
	U32 cLine,
	U8* pbPixel,
	U32 cbStride)
{
	pIE->WRAPPER.info.pfnPutData(
		pIE->guidPixFormat,
		pIE->uWidth,
		pIE->uHeight,
		cLine, 
		pbPixel, 
		cbStride, 
		pIE->WRAPPER.info.userParamPutData);
	return WMP_errSuccess;
}
 
ERR PKImageEncode_Create_Wrapper(PKImageEncode** ppIE)
{
	ERR err = WMP_errSuccess;

	PKImageEncode* pIE = NULL;

	Call(PKImageEncode_Create(ppIE));

	pIE = *ppIE;
	pIE->Initialize = PKImageEncode_Initialize_Wrapper;
	pIE->WritePixels = PKImageEncode_WritePixels_Wrapper;

Cleanup:
	return err;
}