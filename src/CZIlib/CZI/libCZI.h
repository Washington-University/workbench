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
#include "ImportExport.h"

#include "priv_guiddef.h"
#include "libCZI_exceptions.h"
#include "libCZI_DimCoordinate.h"
#include "libCZI_Pixels.h"
#include "libCZI_Metadata.h"
#include "libCZI_Utilities.h"
#include "libCZI_Compositor.h"
#include "libCZI_Site.h"

#include <functional>
#include <memory>
#include <map>
#include <limits>

// virtual d'tor -> https://isocpp.org/wiki/faq/virtual-functions#virtual-dtors

/// External interfaces, classes, functions and structs are found in the namespace "libCZI".
namespace libCZI
{
	/// Values that represent site-object-types.
	/// On Windows, we provide on Site-object that uses the WIC-codec and one that uses
	/// an internal JPEG-XR decoder (JXRLib).
	enum class SiteObjectType
	{
		Default,		///< An enum constant representing the default option (which is JXRLib)
		WithJxrDecoder  ///< An enum constant representing a Site-object using the internal JXRLib.
#if defined(_WIN32)
		, WithWICDecoder///< An enum constant representing a Site-object using the Windows WIC-codec.
#endif
	};

	class ISite;

	/// Gets one of the available Site-objects. The objects returned are static objects with an
	/// unbounded lifetime.
	/// \param type The Site-object type.
	/// \return nullptr if it fails, else the default site object (of the specified type).
	LIBCZI_API ISite* GetDefaultSiteObject(libCZI::SiteObjectType type);

	/// Sets the global Site-object. This function must only be called once and before any other
	/// function in this library is called. The object passed in here must have a lifetime greater
	/// than any usage of the library.
	/// If no Site-object is set, then at first usage a default Site-object is created and used.
	/// \param [in] pSite The Site-object to use. It must not be nullptr.
	LIBCZI_API void SetSiteObject(libCZI::ISite* pSite);

	class ICZIReader;
	class IStream;
	class ISubBlock;
	class IMetadataSegment;
	class ISubBlockRepository;
	class IAttachment;

	/// Gets the version of the library.
	///
	/// \param [out] pMajor If non-null, will receive the major version number.
	/// \param [out] pMinor If non-null, will receive the minor version number.
	LIBCZI_API void GetLibCZIVersion(int* pMajor, int* pMinor);

	/// Creates a new instance of the CZI-reader class.
	/// \return The newly created CZI-reader.
	LIBCZI_API std::shared_ptr<ICZIReader> CreateCZIReader();

	/// Creates bitmap from sub block.
	/// \param [in] subBlk The sub-block.
	/// \return The newly allocated bitmap containing the image from the sub-block.
	LIBCZI_API std::shared_ptr<IBitmapData>  CreateBitmapFromSubBlock(ISubBlock* subBlk);

	/// Creates metadata-object from a metadata segment.
	/// \param [in] metadataSegment The metadata segment object.
	/// \return The newly created metadata object.
	LIBCZI_API std::shared_ptr<ICziMetadata> CreateMetaFromMetadataSegment(IMetadataSegment* metadataSegment);

	/// Creates an accessor of the specified type which uses the specified sub-block repository.
	/// \param repository   The sub-block repository.
	/// \param accessorType Type of the accessor.
	/// \return The newly created accessor object.
	LIBCZI_API std::shared_ptr<IAccessor> CreateAccesor(std::shared_ptr<ISubBlockRepository> repository, AccessorType accessorType);

	/// Creates a stream-object for the specified file.
	/// A stock-implementation of a stream-object (for reading a file from disk) is provided here.
	/// \param szFilename Filename of the file.
	/// \return The new stream object.
	LIBCZI_API std::shared_ptr<IStream> CreateStreamFromFile(const wchar_t* szFilename);

	/// Creates a stream-object on a memory-block.
	/// \param ptr	Shared pointer to a memory-block.
	/// \param dataSize Size of the memory-block.
	/// \return			The new stream object.
	LIBCZI_API std::shared_ptr<IStream> CreateStreamFromMemory(std::shared_ptr<const void> ptr, size_t dataSize);

	/// Creates a stream-object on the memory-block in an attachment.
	/// \param attachment	Pointer to attachment.
	/// \return			The new stream object.
	LIBCZI_API std::shared_ptr<IStream> CreateStreamFromMemory(IAttachment* attachment);



	/// Interface used for accessing the data-stream.
	///
	class IStream
	{
	public:
		/// Reads the specified amount of data from the stream at the specified position.
		///
		/// \param offset				 The offset to start reading from.
		/// \param [out] pv				 The caller-provided buffer for the data. Must be non-null.
		/// \param size					 The size of the buffer.
		/// \param [out] ptrBytesRead	 If non-null, the variable pointed to will receive the number of bytes actually read.
		virtual void Read(std::uint64_t offset, void *pv, std::uint64_t size, std::uint64_t* ptrBytesRead) = 0;

		virtual ~IStream() {}
	};

	/// Information about a sub-block.
	struct SubBlockInfo
	{
		/// The compression mode of the sub-block.
		CompressionMode			mode;
		/// The pixel type of the sub-block.
		PixelType				pixelType;
		/// The coordinate of the sub-block.
		libCZI::CDimCoordinate	coordinate;
		/// The rectangle where the bitmap (in this sub-block) is located.
		libCZI::IntRect			logicalRect;
		/// The physical size of the bitmap (which may be different to the size of logicalRect).
		libCZI::IntSize			physicalSize;
		/// The M-index of the sub-block (if available). If not available, it has the value std::numeric_limits<int>::max().
		int						mIndex;

		/// Calculate a zoom-factor from the physical- and logical size.
		/// \remark
		/// This calculation not really well-defined.
		/// \return The zoom factor.
		double					GetZoom() const
		{
			if (this->physicalSize.w > this->physicalSize.h)
			{
				return double(this->physicalSize.w) / this->logicalRect.w;
			}

			return double(this->physicalSize.h) / this->logicalRect.h;
		}
	};

	/// Representation of a sub-block. A sub-block can contain three types of data: the bitmap-data,
	/// an attachment and metadata. The presence of an attachment is optional.
	class ISubBlock
	{
	public:
		/// Values that represent the three different data types found in a sub-block.
		enum MemBlkType
		{
			Metadata,	///< An enum constant representing the metadata.
			Data,		///< An enum constant representing the bitmap-data.
			Attachment	///< An enum constant representing the attachment (of a sub-block).
		};

		/// Gets sub-block information.
		/// \return The sub-block information.
		virtual const SubBlockInfo& GetSubBlockInfo() const = 0;

		///  Get a pointer to the raw data. Note that the pointer returned is only valid during the
		///  lifetime of the sub-block-object.
		/// \param type			 The sub-block data-type.
		/// \param [out] ptr 	 The pointer to the data is stored here.
		/// \param [out] size	 The size of the data.
		virtual void DangerousGetRawData(MemBlkType type, const void*& ptr, size_t& size) const = 0;

		/// Gets raw data.
		/// \param type			    The type.
		/// \param [out] ptrSize	If non-null, size of the data buffer is stored here.
		/// \return The raw data.
		virtual std::shared_ptr<const void> GetRawData(MemBlkType type, size_t* ptrSize) = 0;

		/// Creates a bitmap (from the data of this sub-block).
		/// \remark
		/// Within this call the bitmap is decoded (if necessary).
		/// In current implementation, the sub-block does not hold a reference to the returned
		/// bitmap here (and, if called twice, a new bitmap is created). One should not rely
		/// on this behavior, it is conceivable that in a later version the sub-block will
		/// keep a reference (and return the same bitmap if called twice).
		/// In current version this method is equivalant to calling CreateBitmapFromSubBlock.
		/// \return The bitmap (contained in this sub-block).
		virtual std::shared_ptr<IBitmapData> CreateBitmap() = 0;

		virtual ~ISubBlock() {};

		/// A helper method used to cast the pointer to a specific type.
		/// \param type			 The sub-block data-type.
		/// \param [out] ptr 	 The pointer to the data is stored here.
		/// \param [out] size	 The size of the data.
		template <class Q>
		void DangerousGetRawData(MemBlkType type, const Q*& ptr, size_t& size) const
		{
			const void* p;
			this->DangerousGetRawData(type, p, size);
			ptr = (Q*)p;
		}
	};

	/// Information about an attachment.
	struct AttachmentInfo
	{
		GUID		contentGuid;			///< A Guid identifying the content of the attachment.
		char		contentFileType[9];		///< A null-terminated character array identifying the content of the attachment.
		std::string name;					///< A string identifying the content of the attachment.
	};

	/// Representation of an attachment. An attachment is a binary blob, its inner structure is opaque.
	class IAttachment
	{
	public:
		/// Gets information about the attachment.
		/// \return The attachment information.
		virtual const AttachmentInfo& GetAttachmentInfo() const = 0;

		///  Get a pointer to the raw data. Note that the pointer returned is only valid during the
		///  lifetime of the sub-block-object.
		/// \param [out] ptr 	 The pointer to the data is stored here.
		/// \param [out] size	 The size of the data.
		virtual void DangerousGetRawData(const void*& ptr, size_t& size) const = 0;

		/// Gets raw data.
		/// \param [out] ptrSize	If non-null, size of the data buffer is stored here.
		/// \return The raw data.
		virtual std::shared_ptr<const void> GetRawData(size_t* ptrSize) = 0;

		/// A helper method used to cast the pointer to a specific type.
		/// \param [out] ptr 	 The pointer to the data is stored here.
		/// \param [out] size	 The size of the data.
		template <class Q>
		void DangerousGetRawData(const Q*& ptr, size_t& size) const
		{
			const void* p;
			this->DangerousGetRawData(p, size);
			ptr = (Q*)p;
		}
	};

	/// Interface representing the metadata-segment.
	class IMetadataSegment
	{
	public:
		/// Values that represent the two different data types found in the metadata-segment.
		enum MemBlkType
		{
			XmlMetadata,	///< The metadata (in UTF8-XML-format)
			Attachment		///< The attachment (not currently used).
		};

		/// Gets raw data.
		/// \param type			    The metadata-segment memory-block type.
		/// \param [out] ptrSize	If non-null, thus size of the data (in bytes) is stored here.
		/// \return The raw data.
		virtual std::shared_ptr<const void> GetRawData(MemBlkType type, size_t* ptrSize) = 0;

		///  Get a pointer to the raw data. Note that the pointer returned is only valid during the
		///  lifetime of the sub-block-object.
		/// \param type			 The metadata-segment memory-block type.
		/// \param [out] ptr 	 The pointer to the data is stored here.
		/// \param [out] size	 The size of the data.
		virtual void DangerousGetRawData(MemBlkType type, const void*& ptr, size_t& size) const = 0;

		virtual ~IMetadataSegment() {}

		/// Creates metadata object from this metadata segment.
		/// \return The newly created metadata object.
		std::shared_ptr<ICziMetadata> CreateMetaFromMetadataSegment() { return libCZI::CreateMetaFromMetadataSegment(this); }
	};

	/// This structure gathers the bounding-boxes determined from all sub-blocks and only be those on pyramid-layer 0.
	struct BoundingBoxes
	{
		/// The bounding-box determined from all sub-blocks.
		IntRect	boundingBox;

		/// The bounding-boxes determined only from sub-blocks of pyramid-layer 0.
		IntRect boundingBoxLayer0;
	};

	/// Statistics about all sub-blocks found in a CZI-document.
	struct SubBlockStatistics
	{
		/// The total number of sub-blocks in the CZI-document.
		/// We are counting here all sub-block (no matter on which pyramid-layer).
		int subBlockCount;

		/// The minimum M-index (determined from all sub-blocks in the document with a valid M-index).
		/// If no valid M-index was present, then this member will have the value std::numeric_limits<int>::max().
		int minMindex;

		/// The maximum M-index (determined from all sub-blocks in the document with a valid M-index).
		/// If no valid M-index was present, then this member will have the value std::numeric_limits<int>::min().
		int maxMindex;

		/// The bounding box determined from all sub-blocks in the 
		/// document.
		IntRect boundingBox;

		/// The bounding box determined only from the sub-blocks of pyramid-layer0 in the 
		/// document.
		IntRect boundingBoxLayer0Only;

		/// The dimension bounds - the minimum and maximum dimension index determined
		/// from all sub-blocks in the CZI-document.
		CDimBounds dimBounds;

		/// A map with key scene-index and value bounding box of the scene.
		/// Two bounding-boxes are determined - one from checking all sub-blocks (with the specific scene-index)
		/// and another one by only considering sub-blocks on pyramid-layer 0.
		/// If no scene-indices are present, this map is empty.
		std::map<int, BoundingBoxes> sceneBoundingBoxes;

		/// Query if the members minMindex and maxMindex are valid. They may be
		/// invalid in the case that the sub-blocks do not define an M-index.
		///
		/// \return True if minMindex and maxMindex are valid, false if not.
		bool IsMIndexValid() const
		{
			return this->minMindex < this->maxMindex ? true : false;
		}

		/// Invalidates this object.
		void Invalidate()
		{
			this->subBlockCount = -1;
			this->boundingBox.Invalidate();
			this->boundingBoxLayer0Only.Invalidate();
			this->dimBounds.Clear();
			this->sceneBoundingBoxes.clear();
			this->minMindex = (std::numeric_limits<int>::max)();
			this->maxMindex = (std::numeric_limits<int>::min)();
		}
	};

	/// Statistics about the pyramid-layers.
	struct PyramidStatistics
	{
		/// Information about the pyramid-layer.
		/// It consists of two parts: the minification factor and the layer number.
		/// The minification factor specifies by which factor two adjacent pyramid-layers are shrunk. Commonly used in
		/// CZI are 2 or 3.
		/// The layer number starts with 0 with the highest resolution layer.
		/// The lowest level (layer 0) is denoted by pyramidLayerNo == 0 AND minificationFactor==0.
		/// Another special case is pyramidLayerNo == 0xff AND minificationFactor==0xff which means that the
		/// pyramid-layer could not be determined (=the minification factor could not unambigiuosly correlated to
		/// a pyramid-layer).
		struct PyramidLayerInfo
		{
			std::uint8_t minificationFactor;	///< Factor by which adjacent pyramid-layers are shrunk. Commonly used in CZI are 2 or 3.
			std::uint8_t pyramidLayerNo;		///< The pyramid layer number.

			/// Query if this object represents layer 0 (=no minification).
			///
			/// \return True if representing layer 0, false if not.
			bool IsLayer0() const { return this->minificationFactor == 0 && this->pyramidLayerNo == 0; }

			/// Query if this object represents the set of subblocks which cannot be represented as pyramid-layers.
			///
			/// \return True if the set of "not representable as pyramid-layer" is represented by this object, false if not.
			bool IsNotIdentifiedAsPyramidLayer() const { return this->minificationFactor == 0xff && this->pyramidLayerNo == 0xff; }
		};
	
		/// Information about a pyramid-layer.
		struct PyramidLayerStatistics
		{
			PyramidLayerInfo	layerInfo;	///< This identifies the pyramid-layer.
			int					count;		///< The number of sub-blocks which are present in the pyramid-layer.
            
            /*
             * Fix "Your comparator is not a valid strict-weak ordering" assertion failure.
             */
            bool operator==(const PyramidLayerStatistics& rhs) const {
                if ((layerInfo.minificationFactor == rhs.layerInfo.minificationFactor)
                    && (layerInfo.pyramidLayerNo == rhs.layerInfo.pyramidLayerNo)
                    && (count == rhs.count)) {
                    return true;
                }
                return false;
            }
		};

		/// A map with key "scene-index" and value "list of subblock-counts per pyramid-layer".
		/// A key with value std::numeric_limits<int>::max() is used in case that the scene-index is not valid.
		std::map<int, std::vector<PyramidLayerStatistics>> scenePyramidStatistics;
	};

	/// Interface for sub-block repository. This interface is used to access the sub-blocks in a CZI-file.
	class ISubBlockRepository
	{
	public:
		/// Enumerate all sub-blocks. 
		/// \param funcEnum The functor which will be called for every sub-block. If the return value of the
		/// 				functor is true, the enumeration is continued, otherwise it is stopped.
		/// 				The first argument is the index of the sub-block and the second is providing
		/// 				information about the sub-block.
		virtual void EnumerateSubBlocks(std::function<bool(int index, const SubBlockInfo& info)> funcEnum) = 0;

		/// Enumerate the subset of sub-blocks defined by the parameters.
		/// \param planeCoordinate The plane coordinate. Only sub-blocks on this plane will be considered.
		/// \param roi			   The ROI - only sub-blocks which intersects with this ROI will be considered.
		/// \param onlyLayer0	   If true, then only sub-blocks on pyramid-layer 0 will be considered.
		/// \param funcEnum The functor which will be called for every sub-block. If the return value of the
		/// 				functor is true, the enumeration is continued, otherwise it is stopped.
		/// 				The first argument is the index of the sub-block and the second is providing
		/// 				information about the sub-block.
		virtual void EnumSubset(const IDimCoordinate* planeCoordinate, const IntRect* roi, bool onlyLayer0, std::function<bool(int index, const SubBlockInfo& info)> funcEnum) = 0;

		/// Reads the sub-block identified by the specified index. If there is no sub-block present (for
		/// the specified index) then an empty shared_ptr is returned. If a different kind of problem
		/// occurs (e. g. I/O error or corrupted data) an exception is thrown.
		/// \param index Index of the sub-block (as reported by the Enumerate-methods).
		/// \return If successful, the sub-block object; otherwise an empty shared_ptr.
		virtual std::shared_ptr<ISubBlock> ReadSubBlock(int index) = 0;

		/// Attempts to get subblock information of an arbitrary subblock in of the specified channel.
		/// The purpose is that it is quite often necessary to determine the pixeltype of a channel - and
		/// if we do not want to/cannot rely on metadata for determining this, then the obvious way is to
		/// look at an (arbitrary) subblock. In order to allow the repository to have this information
		/// available fast (i. e. cached) we introduce a specific method for this purpose. A cornerstone
		/// case is when no subblock has a channel-index - the rule is: if no subblock has channel-
		/// information, then a channelIndex of 0 fits. Otherwise a subblock is a match if the channel-
		/// index is an exact match.
		/// \param channelIndex  The channel index.
		/// \param [out] info The sub-block information (will be set only if the method is successful).
		/// \return true if it succeeds, false if it fails.
		virtual bool TryGetSubBlockInfoOfArbitrarySubBlockInChannel(int channelIndex, SubBlockInfo& info) = 0;

		/// Gets the statistics about the sub-blocks (determined from examining all sub-blocks).
		/// \return The sub-block statistics.
		virtual SubBlockStatistics GetStatistics() = 0;

		/// Gets the statistics about the pyramid-layers. This information is constructed from all T, Z, C, ...
		/// Pyramids are constructed per scene in CZI.
		///
		/// \return The pyramid statistics.
		virtual PyramidStatistics GetPyramidStatistics() = 0;

		virtual ~ISubBlockRepository() {}
	};

	/// Interface for the attachment repository. This interface is used to access the attachments in a CZI-file.
	class IAttachmentRepository
	{
	public:

		/// Enumerate all attachments.
		///
		/// \param funcEnum The functor which will be called for every attachment. If the return value of the
		/// 				functor is true, the enumeration is continued, otherwise it is stopped.
		/// 				The first argument is the index of the attachment and the second is providing
		/// 				information about the attachment.
		virtual void EnumerateAttachments(std::function<bool(int index, const AttachmentInfo& info)> funcEnum) = 0;

		/// Enumerate the subset of the attachments defined by the parameters.
		/// \param contentFileType If non-null, only attachments with this contentFileType will be considered.
		/// \param name 		   If non-null, only attachments with this name will be considered.
		/// \param funcEnum The functor which will be called for every attachment (within the subset). If the return value of the
		/// 				functor is true, the enumeration is continued, otherwise it is stopped.
		/// 				The first argument is the index of the attachment and the second is providing
		/// 				information about the attachment.
		virtual void EnumerateSubset(const char* contentFileType, const char* name, std::function<bool(int index, const AttachmentInfo& infi)> funcEnum) = 0;

		/// Reads the attachment identified by the specified index. If there is no attachment present (for
		/// the specified index) then an empty shared_ptr is returned. If a different kind of problem
		/// occurs (e. g. I/O error or corrupted data) an exception is thrown.
		/// \param index Index of the attachment (as reported by the Enumerate-methods).
		/// \return If successful, the attachment object; otherwise an empty shared_ptr.
		virtual std::shared_ptr<IAttachment> ReadAttachment(int index) = 0;
	};

	/// This interface is used to represent the CZI-file.
	class ICZIReader : public ISubBlockRepository, public IAttachmentRepository
	{
	public:
		/// Opens the specified stream and reads the global information from the CZI-document.
		/// The stream passed in will have its refcount incremented, a reference is held until Close
		/// is called (or the instance is destroyed).
		/// \remark
		/// If this method is called twice, then an exception of type std::logic_error is thrown.
		///
		/// \param stream The stream object.
		virtual void Open(std::shared_ptr<IStream> stream) = 0;

		/// Reads the metadata segment from the stream.
		/// \remark
		/// If the class is not operational (i. e. Open was not called or Open was not successfull), then an exception of type std::logic_error is thrown.
		///
		/// \return The metadata segment.
		virtual std::shared_ptr<IMetadataSegment> ReadMetadataSegment() = 0;

		/// Creates an accessor for the sub-blocks.
		/// See also the various typed methods: `CreateSingleChannelTileAccessor`, `CreateSingleChannelPyramidLayerTileAccessor` and `CreateSingleChannelScalingTileAccessor`.
		/// \remark
		/// If the class is not operational (i. e. Open was not called or Open was not successfull), then an exception of type std::logic_error is thrown.
		///
		/// \param accessorType The type of the accessor.
		///
		/// \return The accessor (of the requested type).
		virtual std::shared_ptr<IAccessor> CreateAccessor(AccessorType accessorType) = 0;

		/// Closes CZI-reader. The underlying stream-object will be released, and further calls to
		/// other methods will fail. The stream is also closed when the object is destroyed, so it
		/// is usually not neccesary to explitely call `Close`. Also, take care that the ownership of
		/// the class must be defined when calling `Close`.
		virtual void Close() = 0;
	public:
		/// Creates a single channel tile accessor.
		/// \return The new single channel tile accessor.
		std::shared_ptr<ISingleChannelTileAccessor>  CreateSingleChannelTileAccessor()
		{
			return std::dynamic_pointer_cast<ISingleChannelTileAccessor, IAccessor>(this->CreateAccessor(libCZI::AccessorType::SingleChannelTileAccessor));
		}

		/// Creates a single channel pyramid-layer accessor.
		/// \return The new single channel tile accessor.
		std::shared_ptr<ISingleChannelPyramidLayerTileAccessor>  CreateSingleChannelPyramidLayerTileAccessor()
		{
			return std::dynamic_pointer_cast<ISingleChannelPyramidLayerTileAccessor, IAccessor>(this->CreateAccessor(libCZI::AccessorType::SingleChannelPyramidLayerTileAccessor));
		}

		/// Creates a single channel scaling tile accessor.
		/// \return The new single channel scaling tile accessor.
		std::shared_ptr<ISingleChannelScalingTileAccessor> CreateSingleChannelScalingTileAccessor()
		{
			return std::dynamic_pointer_cast<ISingleChannelScalingTileAccessor, IAccessor>(this->CreateAccessor(libCZI::AccessorType::SingleChannelScalingTileAccessor));
		}
	};
}

#include "libCZI_Helpers.h"
