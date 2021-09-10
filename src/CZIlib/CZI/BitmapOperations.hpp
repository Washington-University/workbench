#include "BitmapOperations.h"
#include "CziUtils.h"

#include <cmath>
#if defined(_DEBUG)
#include <assert.h>
#endif

template <libCZI::PixelType tSrcDstPixelType>
inline void CBitmapOperations::CopySamePixelType(const void* srcPtr, int srcStride, void* dstPtr, int dstStride, int width, int height, bool drawTileBorder)
{
	auto bytesPerPel = CziUtils::BytesPerPel<tSrcDstPixelType>();
	int bytesToCopy = width * bytesPerPel;

	if (drawTileBorder == false)
	{
		for (int y = 0; y < height; ++y)
		{
			char* dest = ((char*)dstPtr) + y*((std::ptrdiff_t)dstStride);
			const char* src = ((const char*)srcPtr) + y*((std::ptrdiff_t)srcStride);
			memcpy(dest, src, bytesToCopy);
		}
	}
	else
	{
		memset(dstPtr, 0, bytesToCopy);
		for (int y = 1; y < height - 1; ++y)
		{
			char* dest = ((char*)dstPtr) + y*((std::ptrdiff_t)dstStride);
			const char* src = ((const char*)srcPtr) + y*((std::ptrdiff_t)srcStride);
			memcpy(dest + bytesPerPel, src, bytesToCopy - 2 * bytesPerPel);
			memset(dest, 0, bytesPerPel);
			memset(dest + bytesToCopy - bytesPerPel, 0, bytesPerPel);
		}

		memset(((char*)dstPtr) + (height - 1)*dstStride, 0, bytesToCopy);
	}
}

template <libCZI::PixelType tSrcPixelType, libCZI::PixelType tDstPixelType, typename tPixelConverter>
inline void CBitmapOperations::Copy(const tPixelConverter& conv, const void* srcPtr, int srcStride, void* dstPtr, int dstStride, int width, int height, bool /*drawTileBorder*/)
{
	// TODO: -implement "drawBorder"
	for (int y = 0; y < height; ++y)
	{
		char* dest = ((char*)dstPtr) + y*((std::ptrdiff_t)dstStride);
		const char* src = ((const char*)srcPtr) + y*((std::ptrdiff_t)srcStride);
		for (int x = 0; x < width; ++x)
		{
			conv.ConvertPixel(dest, src);
			dest += CziUtils::BytesPerPel<tDstPixelType>();;
			src += CziUtils::BytesPerPel<tSrcPixelType>();;
		}
	}
}

struct CConvBgr24ToGray8
{
	void ConvertPixel(void* ptrDest, const void* ptrSrc) const
	{
		const std::uint8_t* src = (const std::uint8_t*)ptrSrc;
		std::uint8_t* dst = (std::uint8_t*)ptrDest;
		std::uint8_t p = (std::uint8_t)((((int)src[0]) + ((int)src[1]) + ((int)src[2]) + 1) / 3);
		*dst = p;
	}
};

struct CConvBgr24ToGray16
{
	void ConvertPixel(void* ptrDest, const void* ptrSrc) const
	{
		const std::uint8_t* src = (const std::uint8_t*)ptrSrc;
		std::uint16_t* dst = (std::uint16_t*)ptrDest;
		std::uint16_t p = (std::uint8_t)((((int)src[0]) + ((int)src[1]) + ((int)src[2]) + 1) / 3);
		*dst = p;
	}
};

struct CConvBgr24ToBgr48
{
	void ConvertPixel(void* ptrDest, const void* ptrSrc) const
	{
		const std::uint16_t* src = (const std::uint16_t*)ptrSrc;
		std::uint16_t* dst = (std::uint16_t*)ptrDest;
		*dst = *src;
		*(dst + 1) = *(src + 1);
		*(dst + 2) = *(src + 2);
	}
};

struct CConvGray8ToBgr24
{
	void ConvertPixel(void* ptrDest, const void* ptrSrc) const
	{
		const std::uint8_t* src = (const std::uint8_t*)ptrSrc;
		std::uint8_t* dst = (std::uint8_t*)ptrDest;
		dst[0] = dst[1] = dst[2] = *src;
	}
};

struct CConvGray8ToBgr48
{
	void ConvertPixel(void* ptrDest, const void* ptrSrc) const
	{
		const std::uint8_t* src = (const std::uint8_t*)ptrSrc;
		std::uint16_t* dst = (std::uint16_t*)ptrDest;
		dst[0] = dst[1] = dst[2] = *src;
	}
};

struct CConvGray8ToGray16
{
	void ConvertPixel(void* ptrDest, const void* ptrSrc) const
	{
		const std::uint8_t* src = (const std::uint8_t*)ptrSrc;
		std::uint16_t* dst = (std::uint16_t*)ptrDest;
		*dst = *src;
	}
};

struct CConvGray8ToGray8
{
	void ConvertPixel(void* ptrDest, const void* ptrSrc) const
	{
		const std::uint8_t* src = (const std::uint8_t*)ptrSrc;
		std::uint8_t* dst = (std::uint8_t*)ptrDest;
		*dst = *src;
	}
};

struct CConvGray8ToGray32Float
{
	void ConvertPixel(void* ptrDest, const void* ptrSrc) const
	{
		const std::uint8_t* src = (const std::uint8_t*)ptrSrc;
		float* dst = (float*)ptrDest;
		*dst = *src;
	}
};

struct CConvGray16ToGray32Float
{
	void ConvertPixel(void* ptrDest, const void* ptrSrc) const
	{
		const std::uint16_t* src = (const std::uint16_t*)ptrSrc;
		float* dst = (float*)ptrDest;
		*dst = *src;
	}
};

struct CConvGray16ToBgr24
{
	void ConvertPixel(void* ptrDest, const void* ptrSrc) const
	{
		const std::uint16_t* src = (const std::uint16_t*)ptrSrc;
		std::uint8_t* dst = (std::uint8_t*)ptrDest;
		*(dst + 0) = *(dst + 1) = *(dst + 2) = (std::uint8_t)((*src) >> 8);
	}
};

struct CConvGray16ToBgr48
{
	void ConvertPixel(void* ptrDest, const void* ptrSrc) const
	{
		const std::uint16_t* src = (const std::uint16_t*)ptrSrc;
		std::uint16_t* dst = (std::uint16_t*)ptrDest;
		*(dst + 0) = *(dst + 1) = *(dst + 2) = (*src);
	}
};

struct CConvGray16ToGray16
{
	void ConvertPixel(void* ptrDest, const void* ptrSrc) const
	{
		const std::uint16_t* src = (const std::uint16_t*)ptrSrc;
		std::uint16_t* dst = (std::uint16_t*)ptrDest;
		*dst = *src;
	}
};

struct CConvGray16ToGray8
{
	void ConvertPixel(void* ptrDest, const void* ptrSrc) const
	{
		const std::uint16_t* src = (const std::uint16_t*)ptrSrc;
		std::uint8_t* dst = (std::uint8_t*)ptrDest;
		*dst = (std::uint8_t)((*src) >> 8);
	}
};

struct CConvBgr24ToGray32Float
{
	void ConvertPixel(void* ptrDest, const void* ptrSrc) const
	{
		const std::uint8_t* src = (const std::uint8_t*)ptrSrc;
		float* dst = (float*)ptrDest;
		float p = (float)(src[0] + src[1] + src[2]);
		p /= 3;
		*dst = p;
	}
};

struct CConvBgr24ToBgr24
{
	void ConvertPixel(void* ptrDest, const void* ptrSrc) const
	{
		const std::uint8_t* src = (const std::uint8_t*)ptrSrc;
		std::uint8_t* dst = (std::uint8_t*)ptrDest;
		*(dst + 0) = *(src + 0);
		*(dst + 1) = *(src + 1);
		*(dst + 2) = *(src + 2);
	}
};

struct CConvBgr48ToBgr48
{
	void ConvertPixel(void* ptrDest, const void* ptrSrc) const
	{
		const std::uint16_t* src = (const std::uint16_t*)ptrSrc;
		std::uint16_t* dst = (std::uint16_t*)ptrDest;
		*(dst + 0) = *(src + 0);
		*(dst + 1) = *(src + 1);
		*(dst + 2) = *(src + 2);
	}
};

struct CConvBgr48ToBgr24
{
	void ConvertPixel(void* ptrDest, const void* ptrSrc) const
	{
		const std::uint16_t* src = (const std::uint16_t*)ptrSrc;
		std::uint8_t* dst = (std::uint8_t*)ptrDest;
		*(dst + 0) = (std::uint8_t)((*(src + 0)) >> 8);
		*(dst + 1) = (std::uint8_t)((*(src + 1)) >> 8);
		*(dst + 2) = (std::uint8_t)((*(src + 2)) >> 8);
	}
};

struct CConvBgr48ToGray32Float
{
	void ConvertPixel(void* ptrDest, const void* ptrSrc) const
	{
		const std::uint16_t* src = (const std::uint16_t*)ptrSrc;
		float* dst = (float*)ptrDest;
		*(dst) = float(int(*(src + 0)) + int(*(src + 1)) + int(*(src + 2))) / 3;
	}
};


struct CConvBgr48ToGray16
{
	void ConvertPixel(void* ptrDest, const void* ptrSrc) const
	{
		const std::uint16_t* src = (const std::uint16_t*)ptrSrc;
		std::uint16_t* dst = (std::uint16_t*)ptrDest;
		*(dst) = (std::uint16_t)((int(*(src + 0)) + int(*(src + 1)) + int(*(src + 2))) / 3);
	}
};

struct CConvBgr48ToGray8
{
	void ConvertPixel(void* ptrDest, const void* ptrSrc) const
	{
		const std::uint16_t* src = (const std::uint16_t*)ptrSrc;
		std::uint8_t* dst = (std::uint8_t*)ptrDest;
		*(dst) = (std::uint8_t)(((int(*(src + 0)) + int(*(src + 1)) + int(*(src + 2))) / 3) >> 8);
	}
};

struct CConvGray32FloatToGray32Float
{
	void ConvertPixel(void* ptrDest, const void* ptrSrc) const
	{
		const float* src = (const float*)ptrSrc;
		float* dst = (float*)ptrDest;
		*(dst) = *(src);
	}
};

template <>
inline void CBitmapOperations::Copy<libCZI::PixelType::Gray8, libCZI::PixelType::Gray8>(const void* srcPtr, int srcStride, void* dstPtr, int dstStride, int width, int height, bool drawTileBorder)
{
	CopySamePixelType<libCZI::PixelType::Gray8>(srcPtr, srcStride, dstPtr, dstStride, width, height, drawTileBorder);
}
template <>
inline void CBitmapOperations::Copy<libCZI::PixelType::Gray16, libCZI::PixelType::Gray16>(const void* srcPtr, int srcStride, void* dstPtr, int dstStride, int width, int height, bool drawTileBorder)
{
	CopySamePixelType<libCZI::PixelType::Gray16>(srcPtr, srcStride, dstPtr, dstStride, width, height, drawTileBorder);
}
template <>
inline void CBitmapOperations::Copy<libCZI::PixelType::Gray32Float, libCZI::PixelType::Gray32Float>(const void* srcPtr, int srcStride, void* dstPtr, int dstStride, int width, int height, bool drawTileBorder)
{
	CopySamePixelType<libCZI::PixelType::Gray32Float>(srcPtr, srcStride, dstPtr, dstStride, width, height, drawTileBorder);
}
template <>
inline void CBitmapOperations::Copy<libCZI::PixelType::Bgr24, libCZI::PixelType::Bgr24>(const void* srcPtr, int srcStride, void* dstPtr, int dstStride, int width, int height, bool drawTileBorder)
{
	CopySamePixelType<libCZI::PixelType::Bgr24>(srcPtr, srcStride, dstPtr, dstStride, width, height, drawTileBorder);
}
template <>
inline void CBitmapOperations::Copy<libCZI::PixelType::Bgr48, libCZI::PixelType::Bgr48>(const void* srcPtr, int srcStride, void* dstPtr, int dstStride, int width, int height, bool drawTileBorder)
{
	CopySamePixelType<libCZI::PixelType::Bgr48>(srcPtr, srcStride, dstPtr, dstStride, width, height, drawTileBorder);
}

// TODO: -implement the missing conversions
//       -can we make IPP an option?
template <>
inline void CBitmapOperations::Copy<libCZI::PixelType::Bgr24, libCZI::PixelType::Gray8>(const void* srcPtr, int srcStride, void* dstPtr, int dstStride, int width, int height, bool drawTileBorder)
{
	Copy<libCZI::PixelType::Bgr24, libCZI::PixelType::Gray8, CConvBgr24ToGray8>(CConvBgr24ToGray8(), srcPtr, srcStride, dstPtr, dstStride, width, height, drawTileBorder);
}
template <>
inline void CBitmapOperations::Copy<libCZI::PixelType::Gray8, libCZI::PixelType::Bgr24>(const void* srcPtr, int srcStride, void* dstPtr, int dstStride, int width, int height, bool drawTileBorder)
{
	Copy<libCZI::PixelType::Gray8, libCZI::PixelType::Bgr24, CConvGray8ToBgr24>(CConvGray8ToBgr24(), srcPtr, srcStride, dstPtr, dstStride, width, height, drawTileBorder);
}
template <>
inline void CBitmapOperations::Copy<libCZI::PixelType::Gray8, libCZI::PixelType::Bgr48>(const void* srcPtr, int srcStride, void* dstPtr, int dstStride, int width, int height, bool drawTileBorder)
{
	Copy<libCZI::PixelType::Gray8, libCZI::PixelType::Bgr48, CConvGray8ToBgr48>(CConvGray8ToBgr48(), srcPtr, srcStride, dstPtr, dstStride, width, height, drawTileBorder);
}
template <>
inline void CBitmapOperations::Copy<libCZI::PixelType::Gray8, libCZI::PixelType::Gray16>(const void* srcPtr, int srcStride, void* dstPtr, int dstStride, int width, int height, bool drawTileBorder)
{
	Copy<libCZI::PixelType::Gray8, libCZI::PixelType::Gray16, CConvGray8ToGray16>(CConvGray8ToGray16(), srcPtr, srcStride, dstPtr, dstStride, width, height, drawTileBorder);
}
template <>
inline void CBitmapOperations::Copy<libCZI::PixelType::Gray8, libCZI::PixelType::Gray32Float>(const void* srcPtr, int srcStride, void* dstPtr, int dstStride, int width, int height, bool drawTileBorder)
{
	Copy<libCZI::PixelType::Gray8, libCZI::PixelType::Gray32Float, CConvGray8ToGray32Float>(CConvGray8ToGray32Float(), srcPtr, srcStride, dstPtr, dstStride, width, height, drawTileBorder);
}
template <>
inline void CBitmapOperations::Copy<libCZI::PixelType::Bgr24, libCZI::PixelType::Gray32Float>(const void* srcPtr, int srcStride, void* dstPtr, int dstStride, int width, int height, bool drawTileBorder)
{
	Copy<libCZI::PixelType::Bgr24, libCZI::PixelType::Gray32Float, CConvBgr24ToGray32Float>(CConvBgr24ToGray32Float(), srcPtr, srcStride, dstPtr, dstStride, width, height, drawTileBorder);
}
template <>
inline void CBitmapOperations::Copy<libCZI::PixelType::Bgr24, libCZI::PixelType::Gray16>(const void* srcPtr, int srcStride, void* dstPtr, int dstStride, int width, int height, bool drawTileBorder)
{
	Copy<libCZI::PixelType::Bgr24, libCZI::PixelType::Gray16, CConvBgr24ToGray16>(CConvBgr24ToGray16(), srcPtr, srcStride, dstPtr, dstStride, width, height, drawTileBorder);
}
template <>
inline void CBitmapOperations::Copy<libCZI::PixelType::Bgr24, libCZI::PixelType::Bgr48>(const void* srcPtr, int srcStride, void* dstPtr, int dstStride, int width, int height, bool drawTileBorder)
{
	Copy<libCZI::PixelType::Bgr24, libCZI::PixelType::Bgr48, CConvBgr24ToBgr48>(CConvBgr24ToBgr48(), srcPtr, srcStride, dstPtr, dstStride, width, height, drawTileBorder);
}


//------------------------------------------------------------------------------------------------------------

template <libCZI::PixelType tSrcPixelType, libCZI::PixelType tDstPixelType, typename tPixelConverter, typename tFlt>
inline void CBitmapOperations::InternalNNScale2(const tPixelConverter& conv, const NNResizeInfo2<tFlt>& resizeInfo)
{
	auto bytesPerPelSrc = CziUtils::BytesPerPel<tSrcPixelType>();
	auto bytesPerPelDest = CziUtils::BytesPerPel<tDstPixelType>();

	int dstXStart = (std::max)((int)resizeInfo.dstRoiX, 0);
	int dstXEnd = (std::min)((int)(resizeInfo.dstRoiX + resizeInfo.dstRoiW), resizeInfo.dstWidth - 1);

	int dstYStart = (std::max)((int)resizeInfo.dstRoiY, 0);
	int dstYEnd = (std::min)((int)(resizeInfo.dstRoiY + resizeInfo.dstRoiH), resizeInfo.dstHeight - 1);

	auto yMin = ((0 - resizeInfo.srcRoiY)*resizeInfo.dstRoiH) / (resizeInfo.srcRoiH) + resizeInfo.dstRoiY;
	auto yMax = ((resizeInfo.srcHeight - 1 - resizeInfo.srcRoiY)*(resizeInfo.dstRoiH)) / resizeInfo.srcRoiH + resizeInfo.dstRoiY;
	auto xMin = ((0 - resizeInfo.srcRoiX)*resizeInfo.dstRoiW) / (resizeInfo.srcRoiW) + resizeInfo.dstRoiX;
	auto xMax = ((resizeInfo.srcWidth - 1 - resizeInfo.srcRoiX)*(resizeInfo.dstRoiW)) / resizeInfo.srcRoiW + resizeInfo.dstRoiX;

	int dstXStartClipped = (std::max)((int)std::ceil(xMin), dstXStart);
	int dstXEndClipped = (std::min)((int)std::ceil(xMax), dstXEnd);
	int dstYStartClipped = (std::max)((int)std::ceil(yMin), dstYStart);
	int dstYEndClipped = (std::min)((int)std::ceil(yMax), dstYEnd);

	for (int y = dstYStartClipped; y <= dstYEndClipped; ++y)
	{
		tFlt srcY = ((y - resizeInfo.dstRoiY) / resizeInfo.dstRoiH)*resizeInfo.srcRoiH + resizeInfo.srcRoiY;
		int srcYInt = (int)srcY;
		if (srcYInt < 0)
		{
			srcYInt = 0;
		}
		else if (srcYInt >= resizeInfo.srcHeight)
		{
			srcYInt = resizeInfo.srcHeight - 1;
		}

		const char* pSrcLine = (((const char*)resizeInfo.srcPtr) + srcYInt * ((std::ptrdiff_t)resizeInfo.srcStride));
		char* pDstLine = ((char*)resizeInfo.dstPtr) + y * ((std::ptrdiff_t)resizeInfo.dstStride);
		for (int x = dstXStartClipped; x <= dstXEndClipped; ++x)
		{
			// now transform this pixel into the source-ROI
			tFlt srcX = ((x - resizeInfo.dstRoiX) / resizeInfo.dstRoiW)*resizeInfo.srcRoiW + resizeInfo.srcRoiX;
			int srcXInt = (int)srcX;
			if (srcXInt < 0)
			{
				srcXInt = 0;
			}
			else if (srcXInt >= resizeInfo.srcWidth)
			{
				srcXInt = resizeInfo.srcWidth - 1;
			}

			const char* pSrc = pSrcLine + srcXInt * bytesPerPelSrc;
			char* pDst = pDstLine + x * bytesPerPelDest;
			conv.ConvertPixel(pDst, pSrc);
		}
	}
}

template <typename tFlt>
inline void CBitmapOperations::NNSCale2(libCZI::PixelType srcPixelType, libCZI::PixelType dstPixelType, const NNResizeInfo2<tFlt>& resizeInfo)
{
	switch (srcPixelType)
	{
	case libCZI::PixelType::Gray8:
		switch (dstPixelType)
		{
		case libCZI::PixelType::Gray8:
			InternalNNScale2<libCZI::PixelType::Gray8, libCZI::PixelType::Gray8, CConvGray8ToGray8>(resizeInfo);
			break;
		case libCZI::PixelType::Gray16:
			InternalNNScale2<libCZI::PixelType::Gray8, libCZI::PixelType::Gray16, CConvGray8ToGray16>(resizeInfo);
			break;
		case libCZI::PixelType::Bgr24:
			InternalNNScale2<libCZI::PixelType::Gray8, libCZI::PixelType::Bgr24, CConvGray8ToBgr24>(resizeInfo);
			break;
		case libCZI::PixelType::Bgr48:
			InternalNNScale2<libCZI::PixelType::Gray8, libCZI::PixelType::Bgr48, CConvGray8ToBgr48>(resizeInfo);
			break;
		case libCZI::PixelType::Gray32Float:
			InternalNNScale2<libCZI::PixelType::Gray8, libCZI::PixelType::Gray32Float, CConvGray8ToGray32Float>(resizeInfo);
			break;
		default:
			ThrowUnsupportedConversion(srcPixelType, dstPixelType);
		}

		break;
	case libCZI::PixelType::Bgr24:
		switch (dstPixelType)
		{
		case libCZI::PixelType::Gray8:
			InternalNNScale2<libCZI::PixelType::Bgr24, libCZI::PixelType::Gray8, CConvBgr24ToGray8>(resizeInfo);
			break;
		case libCZI::PixelType::Gray16:
			InternalNNScale2<libCZI::PixelType::Bgr24, libCZI::PixelType::Gray16, CConvBgr24ToGray16>(resizeInfo);
			break;
		case libCZI::PixelType::Gray32Float:
			InternalNNScale2<libCZI::PixelType::Bgr24, libCZI::PixelType::Gray32Float, CConvBgr24ToGray32Float>(resizeInfo);
			break;
		case libCZI::PixelType::Bgr24:
			InternalNNScale2<libCZI::PixelType::Bgr24, libCZI::PixelType::Bgr24, CConvBgr24ToBgr24>(resizeInfo);
			break;
		case libCZI::PixelType::Bgr48:
			InternalNNScale2<libCZI::PixelType::Bgr24, libCZI::PixelType::Bgr48, CConvBgr24ToBgr48>(resizeInfo);
			break;
		default:
			ThrowUnsupportedConversion(srcPixelType, dstPixelType);
		}

		break;
	case libCZI::PixelType::Gray16:
		switch (dstPixelType)
		{
		case libCZI::PixelType::Gray8:
			InternalNNScale2<libCZI::PixelType::Gray16, libCZI::PixelType::Gray8, CConvGray16ToGray8>(resizeInfo);
			break;
		case libCZI::PixelType::Gray16:
			InternalNNScale2<libCZI::PixelType::Gray16, libCZI::PixelType::Gray16, CConvGray16ToGray16>(resizeInfo);
			break;
		case libCZI::PixelType::Gray32Float:
			InternalNNScale2<libCZI::PixelType::Gray16, libCZI::PixelType::Gray32Float, CConvGray16ToGray32Float>(resizeInfo);
			break;
		case libCZI::PixelType::Bgr24:
			InternalNNScale2<libCZI::PixelType::Gray16, libCZI::PixelType::Bgr24, CConvGray16ToBgr24>(resizeInfo);
			break;
		case libCZI::PixelType::Bgr48:
			InternalNNScale2<libCZI::PixelType::Gray16, libCZI::PixelType::Bgr48, CConvGray16ToBgr48>(resizeInfo);
			break;
		default:
			ThrowUnsupportedConversion(srcPixelType, dstPixelType);
		}

		break;
	case libCZI::PixelType::Bgr48:
		switch (dstPixelType)
		{
		case libCZI::PixelType::Gray8:
			InternalNNScale2<libCZI::PixelType::Bgr48, libCZI::PixelType::Gray8, CConvBgr48ToGray8>(resizeInfo);
			break;
		case libCZI::PixelType::Gray16:
			InternalNNScale2<libCZI::PixelType::Bgr48, libCZI::PixelType::Gray16, CConvBgr48ToGray16>(resizeInfo);
			break;
		case libCZI::PixelType::Gray32Float:
			InternalNNScale2<libCZI::PixelType::Bgr48, libCZI::PixelType::Gray32Float, CConvBgr48ToGray32Float>(resizeInfo);
			break;
		case libCZI::PixelType::Bgr24:
			InternalNNScale2<libCZI::PixelType::Bgr48, libCZI::PixelType::Bgr24, CConvBgr48ToBgr24>(resizeInfo);
			break;
		case libCZI::PixelType::Bgr48:
			InternalNNScale2<libCZI::PixelType::Bgr48, libCZI::PixelType::Bgr48, CConvBgr48ToBgr48>(resizeInfo);
			break;
		default:
			ThrowUnsupportedConversion(srcPixelType, dstPixelType);
		}

		break;
	case libCZI::PixelType::Gray32Float:
		switch (dstPixelType)
		{
		case libCZI::PixelType::Gray32Float:
			InternalNNScale2<libCZI::PixelType::Gray32Float, libCZI::PixelType::Gray32Float, CConvGray32FloatToGray32Float>(resizeInfo);
			break;
		default:
			ThrowUnsupportedConversion(srcPixelType, dstPixelType);
		}
	default:
		ThrowUnsupportedConversion(srcPixelType, dstPixelType);
	}
}
