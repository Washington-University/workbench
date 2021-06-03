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

#pragma once

#include <cstdint>
#include <memory>
#include <algorithm>
#include <ostream>

namespace libCZI
{
	/// A rectangle (with integer coordinates).
	struct IntRect
	{
		int x;	///< The x-coordinate of the upper-left point of the rectangle.
		int y;  ///< The y-coordinate of the upper-left point of the rectangle.
		int w;	///< The width of the rectangle.
		int h;  ///< The height of the rectangle.

		/// Invalidates this object.
		void Invalidate() { this->w = this->h = -1; }

		/// Returns a boolean indicating whether this rectangle contains valid information.
		bool IsValid() const { return this->w >= 0 && this->h >= 0; }

		/// Determine whether this rectangle intersects with the specified one.
		/// \param r The other rectangle.
		/// \return True if the two rectangles intersect, false otherwise.
		bool IntersectsWith(const IntRect& r) const
		{
			IntRect is = this->Intersect(r);
			if (is.w <= 0 || is.h <= 0)
			{
				return false;
			}

			return true;
		}

		/// Calculate the intersection with the specified rectangle.
		///
		/// \param r The rectangle for which the intersection is to be calculated.
		///
		/// \return A rectangle which is the intersection of the two rectangles. If the two rectangles do not intersect, an empty rectangle is returned (width=height=0).
		IntRect Intersect(const IntRect& r) const
		{
			return IntRect::Intersect(*this, r);
		}

		/// Calculate the intersection of the two specified rectangle.
		///
		/// \param a The first rectangle.
		/// \param b The second rectangle.
		///
		/// \return A rectangle which is the intersection of the two rectangles. If the two rectangles do not intersect, an empty rectangle is returned (width=height=0).
		static IntRect Intersect(const IntRect& a, const IntRect& b)
		{
			int x1 = (std::max)(a.x, b.x);
			int x2 = (std::min)(a.x + a.w, b.x + b.w);
			int y1 = (std::max)(a.y, b.y);
			int y2 = (std::min)(a.y + a.h, b.y + b.h);
			if (x2 >= x1&& y2 >= y1)
			{
				return IntRect{ x1, y1, x2 - x1, y2 - y1 };
			}

			return IntRect{ 0,0,0,0 };
		}
	};

	/// A rectangle (with double coordinates).
	struct DblRect
	{
		double x;	///< The x-coordinate of the upper-left point of the rectangle.
		double y;   ///< The y-coordinate of the upper-left point of the rectangle.
		double w;	///< The width of the rectangle.
		double h;	///< The height of the rectangle.

		/// Invalidates this object.
		void Invalidate() { this->w = this->h = -1; }
	};

	///  A structure representing a size (width and height) in integers.
	struct IntSize
	{
		std::uint32_t w;	///< The width
		std::uint32_t h;	///< The height
	};

	/// A structure representing an R-G-B-color triple (as bytes).
	struct Rgb8Color
	{
		std::uint8_t r;	///< The red component.
		std::uint8_t g; ///< The green component.
		std::uint8_t b; ///< The blue component.
	};

	/// A structure representing an R-G-B-color triple (as floats).
	struct RgbFloatColor
	{
		float r; ///< The red component.
		float g; ///< The green component.
		float b; ///< The blue component.
	};

	/// An enum representing a pixel-type.
	enum class PixelType : std::uint8_t
	{
		Invalid = 0xff,				///< Invalid pixel type.
		Gray8 = 0,					///< Grayscale 8-bit unsinged.
		Gray16 = 1,					///< Grayscale 16-bit unsinged.
		Gray32Float = 2,			///< Grayscale 4 byte float.
		Bgr24 = 3,					///< BGR-color 8-bytes triples (memory order B, G, R).
		Bgr48 = 4,					///< BGR-color 16-bytes triples (memory order B, G, R).
		Bgr96Float = 8,				///< BGR-color 4 byte float triples (memory order B, G, R).
		Bgra32 = 9,					///< Currently not supported in libCZI.
		Gray64ComplexFloat = 10,	///< Currently not supported in libCZI.
		Bgr192ComplexFloat = 11,	///< Currently not supported in libCZI.
		Gray32 = 12,				///< Currently not supported in libCZI.
		Gray64Float = 13,			///< Currently not supported in libCZI.
	};

	/// An enum specifying the compression method.
	enum class CompressionMode : std::uint8_t
	{
		Invalid = 0xff,		///< Invalid compression type.
		UnCompressed = 0,	///< The data is uncompressed.
		Jpg = 1,			///< The data is JPG-compressed.
		JpgXr = 4			///< The data is JPG-XR-compressed.
	};

	/// Information about a locked bitmap - allowing direct access to the image data in memory.
	struct BitmapLockInfo
	{
		void*				ptrData;	///< Not currently used, to be ignored.
		void*				ptrDataRoi;	///< The pointer to the first (top-left) pixel of the bitmap.
		std::uint32_t		stride;		///< The stride of the bitmap data (pointed to by `ptrDataRoi`).
		std::uint64_t		size;		///< The size of the bitmap data (pointed to by `ptrDataRoi`) in bytes.
	};

	/// This interface is used to represent a bitmap.
	///
	/// In order to access the pixel data, the Lock-method must be called. The information returned 
	/// from the Lock-method is to be considered valid only until Unlock is called. If a bitmap is
	/// destroyed while it is locked, this is considered to be a fatal error. It is legal to call Lock
	/// multiple times, but the calls to Lock and Unlock must be balanced.
	class IBitmapData
	{
	public:

		/// Gets pixel type.
		///
		/// \return The pixel type.
		virtual PixelType		GetPixelType() const = 0;

		/// Gets the size of the bitmap (i. e. its width and height in pixels).
		///
		/// \return The size (in pixels).
		virtual IntSize			GetSize() const = 0;

		/// Gets a data structure allowing for direct access of the bitmap.
		/// 
		/// The BitmapLockInfo returned must only considered to be valid until Unlock is called.
		/// It is legal to call Lock multiple time (also from different threads concurrently).
		/// In any case, calls to Lock and Unlock must be balanced. It is considered to be a
		/// fatal error if the object is destroyed when it is locked.
		///
		/// \return The BitmapLockInfo allowing to directly access the data representing the bitmap.
		virtual BitmapLockInfo	Lock() = 0;

		/// Inform the bitmap object that the data (previously retrieved by a call to Lock)
		/// is not longer used.
		/// 
		/// The BitmapLockInfo returned must only considered to be valid until Unlock is called.
		virtual void			Unlock() = 0;

		virtual ~IBitmapData() {};

		/// Gets the width of the bitmap in pixels.
		///
		/// \return The width in pixels.
		std::uint32_t GetWidth() const { return this->GetSize().w; }

		/// Gets the height of the bitmap in pixels.
		///
		/// \return The height in pixels
		std::uint32_t GetHeight() const { return this->GetSize().h; }
	};

	/// A helper class used to scope the lock state of a bitmap.
	///
	/// It is intended to be used like this:
	/// \code{.cpp}
	/// 		 
	/// libCZI::IBitmapData* bm = ... // assume that we have a pointer to a bitmap
	/// 
	/// // this example assumes that the pixel type is Gray8 and nothing else...
	/// if (bm->GetPixelType() != libCZI::PixelType::Gray8) throw std::exception();
	/// 
	/// {
	///     // access the bitmap's pixels directly within this scope
	///     libCZI::ScopedBitmapLocker<libCZI::IBitmapData*> lckBm{ bm };	// <- will call bm->Lock here
	///     for (std::uint32_t y  = 0; y < bm->GetHeight(); ++y)
	///     {
	///         const std::uint8_t* ptrLine = ((const std::uint8_t*)lckBm.ptrDataRoi) + y * lckBm.stride;
	///         for (std::uint32_t x  = 0; x < bm->GetWidth(); ++x)
	///         {
	///             auto pixelVal = ptrLine[x];
	///             // do something with the pixel...
	///         }
	///     }
	///     
	///     // when lckBm goes out of scope, bm->Unlock will be called
	///  }
	///
	/// \endcode
	/// 
	/// For convenience two typedef are provided: `ScopedBitmapLockerP` and `ScopedBitmapLockerSP` for
	/// use with the types `IBitmapData*` and `std::shared_ptr<IBitmapData>`.
	///
	///  \code{.cpp}
	/// typedef ScopedBitmapLocker<IBitmapData*> ScopedBitmapLockerP;
	/// typedef ScopedBitmapLocker<std::shared_ptr<IBitmapData>> ScopedBitmapLockerSP;
	/// \endcode
	/// 
	/// So in above sample we could have used 
	/// \code{.cpp}
	/// libCZI::ScopedBitmapLockerP lckBm{ bm };
	/// \endcode
	/// 
	/// This utility is intended to help adhering to the RAII-pattern, since it makes writing exception-safe
	/// code easier - in case of an exception (within the scope of the ScopedBitmapLocker object) the bitmap's
	/// Unlock method will be called (which is cumbersome to achieve otherwise).
	template <typename tBitmap>
	class ScopedBitmapLocker : public BitmapLockInfo
	{
	private:
		tBitmap bmData;
	public:
		ScopedBitmapLocker() = delete;

		/// Constructor taking the object for which we provide the scope-guard.
		/// \param bmData The object for which we are to provide the scope-guard.
		explicit ScopedBitmapLocker(tBitmap bmData) : bmData(bmData)
		{
			auto lockInfo = bmData->Lock();
			this->ptrData = lockInfo.ptrData;
			this->ptrDataRoi = lockInfo.ptrDataRoi;
			this->stride = lockInfo.stride;
			this->size = lockInfo.size;
		}

		/// Copy-Constructor .
		/// \param other The other object.
		ScopedBitmapLocker(const ScopedBitmapLocker<tBitmap>& other) : bmData(other.bmData)
		{
			auto lockInfo = other.bmData->Lock();
			this->ptrData = lockInfo.ptrData;
			this->ptrDataRoi = lockInfo.ptrDataRoi;
			this->stride = lockInfo.stride;
			this->size = lockInfo.size;
		}

		/// move constructor
		ScopedBitmapLocker(ScopedBitmapLocker<tBitmap>&& other) noexcept : bmData(tBitmap())
		{
			*this = std::move(other);
		}

		/// move assignment
		ScopedBitmapLocker<tBitmap>& operator=(ScopedBitmapLocker<tBitmap>&& other) noexcept
		{
			if (this != &other)
			{
				if (this->bmData)
				{
					this->bmData->Unlock();
				}

				this->bmData = std::move(other.bmData);
				this->ptrData = other.ptrData;
				this->ptrDataRoi = other.ptrDataRoi;
				this->stride = other.stride;
				this->size = other.size;
				other.ptrData = other.ptrDataRoi = nullptr;
				other.bmData = tBitmap();
			}

			return *this;
		}

		~ScopedBitmapLocker()
		{
			if (this->bmData)
			{
				this->bmData->Unlock();
			}
		}
	};

	/// Defines an alias representing the scoped bitmap locker for use with libCZI::IBitmapData.
	typedef ScopedBitmapLocker<IBitmapData*> ScopedBitmapLockerP;

	/// Defines an alias representing the scoped bitmap locker for use with a shared_ptr of type libCZI::IBitmapData.
	typedef ScopedBitmapLocker<std::shared_ptr<IBitmapData>> ScopedBitmapLockerSP;

	//-------------------------------------------------------------------------

	/// Stream insertion operator for the libCZI::IntRect type. A string of the form '(x,y,width,height)' is output to the stream.
	/// \param [in] os	   The stream to output the rect to.
	/// \param rect		   The rectangle.
	/// \return The ostream object <tt>os</tt>.
	inline std::ostream& operator<<(std::ostream& os, const IntRect& rect)
	{
		os << "(" << rect.x << "," << rect.y << "," << rect.w << "," << rect.h << ")";
		return os;
	}

	/// Stream insertion operator for the libCZI::IntSize type. A string of the form '(width,height)' is output to the stream.
	/// \param [in] os	   The stream to output the size to.
	/// \param size		   The size structure.
	/// \return The ostream object <tt>os</tt>.
	inline std::ostream& operator<<(std::ostream& os, const IntSize& size)
	{
		os << "(" << size.w << "," << size.h << ")";
		return os;
	}
}
