//******************************************************************************
// 
// libCZI is a reader for the CZI fileformat written in C++
// Copyright (C) 2017  Zeiss Microscopy GmbH
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// 
// To obtain a commercial version please contact Zeiss Microscopy GmbH.
// 
//******************************************************************************

#include <stdexcept> 
#include <sstream>
#include <memory>
#include "JxrDecode.h"
#include "Jxr/JXRTest.h"
#include "Jxr/JXRTestWrapper.h"

using namespace JxrDecode;

//================================================================
// Encoder factory on file extension
//================================================================
ERR WmpDecAppCreateEncoderFromExt(
	PKCodecFactory* pCFactory,
	const char* szExt,
	PKImageEncode** ppIE)
{
	ERR err = WMP_errSuccess;
	const PKIID* pIID = NULL;

	//UNREFERENCED_PARAMETER(pCFactory);

	// get encod PKIID
	GetTestEncodeIID(szExt, &pIID);

	// Create encoder
	PKTestFactory_CreateCodec(pIID, (void**)ppIE);

	//Cleanup:
	return err;
}
//**********************************************************************************
//**********************************************************************************

static const char* ERR_to_string(ERR err)
{
	switch (err)
	{
	case WMP_errSuccess:return "WMP_errSuccess";
	case WMP_errFail:return "WMP_errFail";
	case WMP_errNotYetImplemented:return "WMP_errNotYetImplemented";
	case WMP_errAbstractMethod:return "WMP_errAbstractMethod";
	case WMP_errOutOfMemory:return "WMP_errOutOfMemory";
	case WMP_errFileIO:return "WMP_errFileIO";
	case WMP_errBufferOverflow:return "WMP_errBufferOverflow";
	case WMP_errInvalidParameter:return "WMP_errInvalidParameter";
	case WMP_errInvalidArgument:return "WMP_errInvalidArgument";
	case WMP_errUnsupportedFormat:return "WMP_errUnsupportedFormat";
	case WMP_errIncorrectCodecVersion:return "WMP_errIncorrectCodecVersion";
	case WMP_errIndexNotFound:return "WMP_errIndexNotFound";
	case WMP_errOutOfSequence:return "WMP_errOutOfSequence";
	case WMP_errNotInitialized:return "WMP_errNotInitialized";
	case WMP_errMustBeMultipleOf16LinesUntilLastCall:return "WMP_errMustBeMultipleOf16LinesUntilLastCall";
	case WMP_errPlanarAlphaBandedEncRequiresTempFile:return "WMP_errPlanarAlphaBandedEncRequiresTempFile";
	case WMP_errAlphaModeCannotBeTranscoded:return "WMP_errAlphaModeCannotBeTranscoded";
	case WMP_errIncorrectCodecSubVersion:return "WMP_errIncorrectCodecSubVersion";
	}

	return "unknown";
}


static void ThrowError(const char* szMsg, ERR err)
{
	std::stringstream ss;
	if (szMsg != nullptr)
	{
		ss << "Error in JXR-decoder -> \"" << szMsg << "\" code:" << err << " (" << ERR_to_string(err) << ")";
	}
	else
	{
		ss << "Error in JXR-decoder -> " << err << " (" << ERR_to_string(err) << ")";
	}

	throw std::runtime_error(ss.str());
}

static void ThrowError(ERR err)
{
	ThrowError(nullptr, err);
}

static const struct PixelFormatAndPkPixelFormat
{
	const PKPixelFormatGUID*	pGuid;
	PixelFormat					pixFmt;
}  PixelFormatAndPkPixelFormat[] =
{
	{&GUID_PKPixelFormat24bppBGR,				PixelFormat::_24bppBGR},
	{&GUID_PKPixelFormatBlackWhite,				PixelFormat::_1bppBlackWhite},
	{&GUID_PKPixelFormat8bppGray,				PixelFormat::_8bppGray },
	{&GUID_PKPixelFormat16bppGray,				PixelFormat::_16bppGray },
	{&GUID_PKPixelFormat16bppGrayFixedPoint,	PixelFormat::_16bppGrayFixedPoint },
	{&GUID_PKPixelFormat16bppGrayHalf,			PixelFormat::_16bppGrayHalf },
	{&GUID_PKPixelFormat32bppGrayFixedPoint,	PixelFormat::_32bppGrayFixedPoint },
	{&GUID_PKPixelFormat32bppGrayFloat,			PixelFormat::_32bppGrayFloat },
	{&GUID_PKPixelFormat24bppRGB,				PixelFormat::_24bppRGB },
	{&GUID_PKPixelFormat48bppRGB,				PixelFormat::_48bppRGB },
	{&GUID_PKPixelFormat48bppRGBFixedPoint,		PixelFormat::_48bppRGBFixedPoint },
	{&GUID_PKPixelFormat48bppRGBHalf,			PixelFormat::_48bppRGBHalf },
	{&GUID_PKPixelFormat96bppRGBFixedPoint,		PixelFormat::_96bppRGBFixedPoint },
	{&GUID_PKPixelFormat128bppRGBFloat,			PixelFormat::_128bppRGBFloat },
	{&GUID_PKPixelFormat32bppRGBE,				PixelFormat::_32bppRGBE },
	{&GUID_PKPixelFormat32bppCMYK,				PixelFormat::_32bppCMYK },
	{&GUID_PKPixelFormat64bppCMYK,				PixelFormat::_64bppCMYK },
	{&GUID_PKPixelFormat32bppBGRA,				PixelFormat::_32bppBGRA },
	{&GUID_PKPixelFormat64bppRGBA,				PixelFormat::_64bppRGBA },
	{&GUID_PKPixelFormat64bppRGBAFixedPoint,	PixelFormat::_64bppRGBAFixedPoint },
	{&GUID_PKPixelFormat64bppRGBAHalf,			PixelFormat::_64bppRGBAHalf },
	{&GUID_PKPixelFormat128bppRGBAFixedPoint,	PixelFormat::_128bppRGBAFixedPoint },
	{&GUID_PKPixelFormat128bppRGBAFloat,		PixelFormat::_128bppRGBAFloat },
	{&GUID_PKPixelFormat16bppRGB555,			PixelFormat::_16bppBGR555 },
	{&GUID_PKPixelFormat16bppRGB565,			PixelFormat::_16bppBGR565 },
	{&GUID_PKPixelFormat32bppRGB101010,			PixelFormat::_32bppBGR101010 },
	{&GUID_PKPixelFormat40bppCMYKAlpha,			PixelFormat::_40bppCMYKA },
	{&GUID_PKPixelFormat80bppCMYKAlpha,			PixelFormat::_80bppCMYKA },
	{&GUID_PKPixelFormat32bppBGR,				PixelFormat::_32bppBGR}
};

static PixelFormat PixelFormatFromPkPixelFormat(const PKPixelFormatGUID& pkFixelFmt)
{
	for (int i = 0; i < sizeof(PixelFormatAndPkPixelFormat) / sizeof(PixelFormatAndPkPixelFormat[0]); ++i)
	{
		if (IsEqualGUID(*(PixelFormatAndPkPixelFormat[i].pGuid), pkFixelFmt))
		{
			return PixelFormatAndPkPixelFormat[i].pixFmt;
		}
	}

	return PixelFormat::invalid;
}

static const PKPixelFormatGUID* PkPixelFormatFromPixelFormat(JxrDecode::PixelFormat pixfmt)
{
	for (int i = 0; i < sizeof(PixelFormatAndPkPixelFormat) / sizeof(PixelFormatAndPkPixelFormat[0]); ++i)
	{
		if (PixelFormatAndPkPixelFormat[i].pixFmt == pixfmt)
		{
			return PixelFormatAndPkPixelFormat[i].pGuid;
		}
	}

	throw std::invalid_argument("unknown pixfmt");
}

struct CodecHandle
{
	PKFactory* pFactory;
	PKCodecFactory* pCodecFactory;
};

JxrDecode::codecHandle JxrDecode::Initialize()
{
	PKFactory* pFactory = nullptr;
	ERR err = PKCreateFactory(&pFactory, PK_SDK_VERSION);
	if (Failed(err)) { ThrowError("PKCreateFactory failed", err); }
	std::unique_ptr<PKFactory, void(*)(PKFactory*)> upFactory(pFactory, [](PKFactory* p)->void {p->Release(&p); });

	PKCodecFactory* pCodecFactory = NULL;
	err = PKCreateCodecFactory(&pCodecFactory, WMP_SDK_VERSION);
	if (Failed(err)) { ThrowError("PKCreateCodecFactory failed", err); }
	std::unique_ptr<PKCodecFactory, void(*)(PKCodecFactory*)> upCodecFactory(pCodecFactory, [](PKCodecFactory* p)->void {p->Release(&p); });
	CodecHandle* ch = new CodecHandle{ upFactory.release(), upCodecFactory.release() };
	return reinterpret_cast<codecHandle*>(ch);
}

void JxrDecode::Destroy(codecHandle h)
{
	CodecHandle* ch = static_cast<CodecHandle*>(h);
	ch->pFactory->Release(&ch->pFactory);
	ch->pCodecFactory->Release(&ch->pCodecFactory);
	delete ch;
}

static void DeliverData(
	GUID pixeltype,
	unsigned int width,
	unsigned int height,
	unsigned int cLines,
	void* ptrData,
	unsigned int stride,
	void* userParam)
{
	auto deliver = static_cast<std::function<void(PixelFormat, uint32_t, uint32_t, uint32_t, const void*, uint32_t)>*>(userParam);
	deliver->operator()(PixelFormatFromPkPixelFormat(pixeltype), width, height, cLines, ptrData, stride);
}

void JxrDecode::Decode(codecHandle h, const WMPDECAPPARGS* decArgs, const void* ptrData, size_t size, std::function<JxrDecode::PixelFormat(const JxrDecode::PixelFormat)> selectDestPixFmt, std::function<void(PixelFormat pixFmt, std::uint32_t  width, std::uint32_t  height, std::uint32_t linesCount, const void* ptrData, std::uint32_t stride)>  deliverData)
{
	CodecHandle* ch = static_cast<CodecHandle*>(h);
	WMPStream* pStream;
	ERR err = ch->pFactory->CreateStreamFromMemory(&pStream, const_cast<void*>(ptrData), size);
	if (Failed(err)) { ThrowError("CreateStreamFromMemory failed", err); }
	std::unique_ptr<WMPStream, void(*)(WMPStream*)> upStream(pStream, [](WMPStream* p)->void {p->Close(&p); });

	PKImageDecode* pDecoder;
	ch->pCodecFactory->CreateDecoderFromStream(upStream.get(), &pDecoder);
	if (Failed(err)) { ThrowError("CreateDecoderFromStream failed", err); }
	std::unique_ptr<PKImageDecode, void(*)(PKImageDecode*)> upDecoder(pDecoder, [](PKImageDecode* p)->void {p->Release(&p); });

	PKPixelFormatGUID args_guidPixFormat;
	PKPixelInfo PI;
	if (decArgs->pixFormat == PixelFormat::dontCare)
	{
		if (selectDestPixFmt)
		{
			PI.pGUIDPixFmt = &upDecoder->guidPixFormat;
			PixelFormat pixFmtFromDecoder = PixelFormatFromPkPixelFormat(upDecoder->guidPixFormat);
			PixelFormat destFmtChosen = selectDestPixFmt(pixFmtFromDecoder);

			args_guidPixFormat = *PkPixelFormatFromPixelFormat(destFmtChosen);
			//args_guidPixFormat = selectDestPixFmt(*PI.pGUIDPixFmt);
		}
		else
		{
			// take decoder color format and try to look up better one
			// (e.g. 32bppBGR -> 24bppBGR etc.)
			PKPixelInfo newPI;
			newPI.pGUIDPixFmt = PI.pGUIDPixFmt = &upDecoder->guidPixFormat;
			PixelFormatLookup(&newPI, LOOKUP_FORWARD);
			PixelFormatLookup(&newPI, LOOKUP_BACKWARD_TIF);
			args_guidPixFormat = *newPI.pGUIDPixFmt;
		}
	}
	else
	{
		PI.pGUIDPixFmt = PkPixelFormatFromPixelFormat(decArgs->pixFormat);
		args_guidPixFormat = *PI.pGUIDPixFmt;
	}

	// == color transcoding,
	if (IsEqualGUID(args_guidPixFormat, GUID_PKPixelFormat8bppGray) || IsEqualGUID(args_guidPixFormat, GUID_PKPixelFormat16bppGray)) { // ** => Y transcoding
		upDecoder->guidPixFormat = args_guidPixFormat;
		upDecoder->WMP.wmiI.cfColorFormat = Y_ONLY;
	}
	else if (IsEqualGUID(args_guidPixFormat, GUID_PKPixelFormat24bppRGB) && upDecoder->WMP.wmiI.cfColorFormat == CMYK) { // CMYK = > RGB
		upDecoder->WMP.wmiI.cfColorFormat = CF_RGB;
		upDecoder->guidPixFormat = args_guidPixFormat;
		upDecoder->WMP.wmiI.bRGB = 1; //RGB
	}

	PixelFormatLookup(&PI, LOOKUP_FORWARD);

	std::uint8_t args_uAlphaMode = decArgs->uAlphaMode;
	if (255 == args_uAlphaMode)//user didn't set
	{
		if (!!(PI.grBit & PK_pixfmtHasAlpha))
			args_uAlphaMode = 2;//default is image & alpha for formats with alpha
		else
			args_uAlphaMode = 0;//otherwise, 0
	}

	upDecoder->WMP.wmiSCP.bfBitstreamFormat = BITSTREAMFORMAT::SPATIAL;// args.bfBitstreamFormat;	only used for transcoding?

	upDecoder->WMP.wmiSCP.uAlphaMode = args_uAlphaMode;

	upDecoder->WMP.wmiSCP.sbSubband = (SUBBAND)(std::underlying_type<Subband>::type)decArgs->sbSubband;
	upDecoder->WMP.bIgnoreOverlap = decArgs->bIgnoreOverlap ? 1 : 0;

	upDecoder->WMP.wmiI.cfColorFormat = PI.cfColorFormat;

	upDecoder->WMP.wmiI.bdBitDepth = PI.bdBitDepth;
	upDecoder->WMP.wmiI.cBitsPerUnit = PI.cbitUnit;

	//==== Validate thumbnail decode parameters =====
	upDecoder->WMP.wmiI.cThumbnailWidth = upDecoder->WMP.wmiI.cWidth;
	upDecoder->WMP.wmiI.cThumbnailHeight = upDecoder->WMP.wmiI.cHeight;
	upDecoder->WMP.wmiI.bSkipFlexbits = FALSE;
	/*if (args.tThumbnailFactor > 0 && args.tThumbnailFactor != SKIPFLEXBITS) {
		size_t tSize = ((size_t)1 << args.tThumbnailFactor);

		pDecoder->WMP.wmiI.cThumbnailWidth = (pDecoder->WMP.wmiI.cWidth + tSize - 1) / tSize;
		pDecoder->WMP.wmiI.cThumbnailHeight = (pDecoder->WMP.wmiI.cHeight + tSize - 1) / tSize;

		if (pDecoder->WMP.wmiI.cfColorFormat == YUV_420 || pDecoder->WMP.wmiI.cfColorFormat == YUV_422) { // unsupported thumbnail format
			pDecoder->WMP.wmiI.cfColorFormat = YUV_444;
		}
	}
	else if (args.tThumbnailFactor == SKIPFLEXBITS) {
		pDecoder->WMP.wmiI.bSkipFlexbits = TRUE;
	}*/

	if (decArgs->rWidth == 0 || decArgs->rHeight == 0)
	{ // no region decode
		upDecoder->WMP.wmiI.cROILeftX = 0;
		upDecoder->WMP.wmiI.cROITopY = 0;
		upDecoder->WMP.wmiI.cROIWidth = upDecoder->WMP.wmiI.cThumbnailWidth;
		upDecoder->WMP.wmiI.cROIHeight = upDecoder->WMP.wmiI.cThumbnailHeight;
	}
	else
	{
		upDecoder->WMP.wmiI.cROILeftX = decArgs->rLeftX;
		upDecoder->WMP.wmiI.cROITopY = decArgs->rTopY;
		upDecoder->WMP.wmiI.cROIWidth = decArgs->rWidth;
		upDecoder->WMP.wmiI.cROIHeight = decArgs->rHeight;
	}

	upDecoder->WMP.wmiI.oOrientation = (ORIENTATION)decArgs->oOrientation;

	upDecoder->WMP.wmiI.cPostProcStrength = decArgs->cPostProcStrength;

	upDecoder->WMP.wmiSCP.bVerbose = 0;// args.bVerbose;

	U32 cFrame;
	err = upDecoder->GetFrameCount(upDecoder.get(), &cFrame);
	if (Failed(err)) { ThrowError("GetFrameCount failed", err); }
	if (cFrame != 1)
	{
		throw std::logic_error("Not expecting to find more than one image here.");
	}

	Float rX = 0, rY = 0;
	PKRect rect = { 0, 0, 0, 0 };

	//================================
	PKFormatConverter* pConverter;
	err = ch->pCodecFactory->CreateFormatConverter(&pConverter);
	if (Failed(err)) { ThrowError("CreateFormatConverter failed", err); }
	std::unique_ptr<PKFormatConverter, void(*)(PKFormatConverter*)> upConverter(pConverter, [](PKFormatConverter* p)->void {p->Release(&p); });

	err = upConverter->Initialize(upConverter.get(), upDecoder.get(), nullptr/*pExt*/, args_guidPixFormat);
	if (Failed(err)) { ThrowError("Initialize failed", err); }

	PKImageEncode* pEncoder;
	err = WmpDecAppCreateEncoderFromExt(ch->pCodecFactory, "wrapper", &pEncoder);
	if (Failed(err)) { ThrowError("WmpDecAppCreateEncoderFromExt failed", err); }
	std::unique_ptr<PKImageEncode, void(*)(PKImageEncode*)> upEncoder(pEncoder, [](PKImageEncode* p)->void {p->Release(&p); });

	struct tagJxrTestWrapperInitializeInfo wrapperInfo;
	wrapperInfo.userParamPutData = &deliverData;
	wrapperInfo.pfnPutData = DeliverData;

	err = upEncoder->Initialize(upEncoder.get(), nullptr, &wrapperInfo, sizeof(wrapperInfo));
	if (Failed(err)) { ThrowError("Encoder::Initialize failed", err); }
	err = upEncoder->SetPixelFormat(upEncoder.get(), args_guidPixFormat);
	if (Failed(err)) { ThrowError("SetPixelFormat failed", err); }
	upEncoder->WMP.wmiSCP.bBlackWhite = upDecoder->WMP.wmiSCP.bBlackWhite;

	rect.Width = (I32)(upDecoder->WMP.wmiI.cROIWidth);
	rect.Height = (I32)(upDecoder->WMP.wmiI.cROIHeight);

	if ((std::underlying_type<Orientation>::type)decArgs->oOrientation > (std::underlying_type<Orientation>::type)Orientation::O_FLIPVH) { // allocate memory for rotated image!
		I32 bah = rect.Width;

		rect.Width = rect.Height;
		rect.Height = bah;
	}

	err = upEncoder->SetSize(upEncoder.get(), rect.Width, rect.Height);
	if (Failed(err)) { ThrowError("SetSize failed", err); }
	err = upDecoder->GetResolution(upDecoder.get(), &rX, &rY);
	if (Failed(err)) { ThrowError("GetResolution failed", err); }

	if ((std::underlying_type<Orientation>::type)decArgs->oOrientation > (std::underlying_type<Orientation>::type)O_FLIPVH)
	{
		upEncoder->SetResolution(upEncoder.get(), rY, rX);
	}
	else
	{
		upEncoder->SetResolution(upEncoder.get(), rX, rY);
	}

	//================================
	upEncoder->WriteSource = PKImageEncode_Transcode;
	err = upEncoder->WriteSource(upEncoder.get(), upConverter.get(), &rect);
	if (Failed(err)) { ThrowError("WriteSource failed", err); }

	//================================
	err = upEncoder->Terminate(upEncoder.get());
	if (Failed(err)) { ThrowError("Release (encoder) failed", err); }

	upDecoder->SelectFrame(upDecoder.get(), 1);
	if (Failed(err)) { ThrowError("SelectFrame failed", err); }
}

const char* JxrDecode::PixelFormatAsInformalString(PixelFormat pfmt)
{
	switch (pfmt)
	{
	case PixelFormat::dontCare:return "dontCare";
	case PixelFormat::_24bppBGR:return "24bppBGR";
	case PixelFormat::_1bppBlackWhite:return "1bppBlackWhite";
	case PixelFormat::_8bppGray:return "8bppGray";
	case PixelFormat::_16bppGray:return "16bppGray";
	case PixelFormat::_16bppGrayFixedPoint:return "16bppGrayFixedPoint";
	case PixelFormat::_16bppGrayHalf:return "16bppGrayHalf";
	case PixelFormat::_32bppGrayFixedPoint:return "32bppGrayFixedPoint";
	case PixelFormat::_32bppGrayFloat:return "32bppGrayFloat";
	case PixelFormat::_24bppRGB:return "24bppRGB";
	case PixelFormat::_48bppRGB:return "48bppRGB";
	case PixelFormat::_48bppRGBFixedPoint:return "48bppRGBFixedPoint";
	case PixelFormat::_48bppRGBHalf:return "48bppRGBHalf";
	case PixelFormat::_96bppRGBFixedPoint:return "96bppRGBFixedPoint";
	case PixelFormat::_128bppRGBFloat:return "128bppRGBFloat";
	case PixelFormat::_32bppRGBE:return "32bppRGBE";
	case PixelFormat::_32bppCMYK:return "32bppCMYK";
	case PixelFormat::_64bppCMYK:return "64bppCMYK";
	case PixelFormat::_32bppBGRA:return "32bppBGRA";
	case PixelFormat::_64bppRGBA:return "64bppRGBA";
	case PixelFormat::_64bppRGBAFixedPoint:return "64bppRGBAFixedPoint";
	case PixelFormat::_64bppRGBAHalf:return "64bppRGBAHalf";
	case PixelFormat::_128bppRGBAFixedPoint:return "128bppRGBAFixedPoint";
	case PixelFormat::_128bppRGBAFloat:return "128bppRGBAFloat";
	case PixelFormat::_16bppBGR555:return "16bppBGR555";
	case PixelFormat::_16bppBGR565:return "16bppBGR565";
	case PixelFormat::_32bppBGR101010:return "32bppBGR101010";
	case PixelFormat::_40bppCMYKA:return "40bppCMYKA";
	case PixelFormat::_80bppCMYKA:return "80bppCMYKA";
	case PixelFormat::_32bppBGR:return "32bppBGR";
	case PixelFormat::invalid:return "invalid";
	default:return "unknwon";
	}
}