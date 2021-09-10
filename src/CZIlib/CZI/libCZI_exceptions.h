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

#include <exception>
#include <stdexcept>

namespace libCZI
{
	/// Base class for all libCZI-specific exceptions.
	class LibCZIException : public std::runtime_error
	{
	public:
		/// Constructor.
		/// \param szErrMsg Message describing the error.
		explicit LibCZIException(const char* szErrMsg)
			: std::runtime_error(szErrMsg)
		{}
	};

	/// Exception for signalling errors specific for accessors.
	class LibCZIAccessorException : public LibCZIException
	{
	public:
		/// Values that represent error types.
		enum class ErrorType
		{
			CouldntDeterminePixelType,	///< The pixeltype could not be determined.
			Unspecified					///< Unspecified error.
		};
	private:
		ErrorType errorType;
	public:
		/// Constructor for the LibCZIAccessorException.
		/// \param szErrMsg  Message describing the error. This type is used
		/// to signal error that are specific for accessors.
		/// \param errorType Type of the error.
		LibCZIAccessorException(const char* szErrMsg, ErrorType errorType)
			: LibCZIException(szErrMsg), errorType(errorType)
		{}

		/// Gets error type.
		/// \return The error type.
		ErrorType GetErrorType() const { return this->errorType; };
	};

	/// Exception for signalling that a string did not parse correctly.
	class LibCZIStringParseException : public LibCZIException
	{
	public:
		/// Values that represent error types.
		enum class ErrorType
		{
			InvalidSyntax,		///< The string parsed has an invalid syntax.
			DuplicateDimension, ///< When parsing a string representation of a coordinate, it was detected, that a dimension occured more than once.
			FromGreaterThanTo,  ///< A range was parsed, and the start value is bigger than the end value.
			Unspecified			///< General error.
		};
	private:
		ErrorType errorType;
		int numberOfCharsParsedOk;
	public:

		/// Constructor for the LibCZIStringParseException. This type is used
		/// to signal that a string did not parse correctly.
		/// \param szErrMsg				 Message describing the error.
		/// \param numberOfCharsParsedOk Number of characters parsed ok. The parse error occured after this position. If negative, then this information is not available.
		/// \param errorType			 Type of the error.
		LibCZIStringParseException(const char* szErrMsg, int numberOfCharsParsedOk, ErrorType errorType)
			: LibCZIException(szErrMsg), errorType(errorType), numberOfCharsParsedOk(numberOfCharsParsedOk)
		{}

		/// Gets error type.
		/// \return The error type.
		ErrorType GetErrorType() const { return this->errorType; };

		/// Gets number of characters that parsed correctly. The parse error occured after this position.
		/// If this number is negative, then this information is not available and valid.
		/// \return The number of characters parsed that parsed correctly. If the number is negative, this information is not available.
		int GetNumberOfCharsParsedOk() const { return this->numberOfCharsParsedOk; }
	};


	/// Exception for signalling an I/O error. If the problem originates from the
	/// (external) stream-object, then the original exception is enclosed here as
	/// a nested exception. In order to access the nested exception, use something
	/// like this:
	/// @code
	/// try
	///		{
	///			spReader->Open(stream);
	///		}
	///		catch (LibCZIIOException& excp)
	///		{
	///			try
	///			{
	///				excp.rethrow_nested();
	///			}
	///			catch (std::ios_base::failure& innerExcp) // assuming that is the exception you 
	///			{                                         // expect to be thrown from the stream-object
	///			 ....
	///			}
	///		}
	///	@endcode
	class LibCZIIOException : public LibCZIException, public std::nested_exception
	{
	private:
		std::uint64_t offset;
		std::uint64_t size;
	public:
		/// Constructor for the LibCZIIOException. This type is used
		/// to signal an I/O-error from the underlying stream.
		///
		/// \param szErrMsg Message describing the error.
		/// \param offset   The offset (into the stream) at which the I/O-error occured.
		/// \param size	    The size of data we have attempted to read (when the I/O-error occured).
		LibCZIIOException(const char* szErrMsg, std::uint64_t offset, std::uint64_t size)
			: LibCZIException(szErrMsg), offset(offset), size(size)  {}

		/// Gets the offset (in bytes) into the stream at which
		/// the I/O-error occured.
		///
		/// \return The offset  (in bytes).
		std::uint64_t GetOffset() const { return this->offset; }

		/// Gets the size of data (in bytes) we attempted to read
		/// when the I/O-error occured.
		///
		/// \return The size (in bytes).
		std::uint64_t GetSize() const {return this->size; }
	};


	/// Exception for signalling errors parsing the CZI-stream.
	struct LibCZICZIParseException : public LibCZIException
	{
		/// Values that represent different error conditions.
		enum class ErrorCode
		{
			NotEnoughData,	///< An enum constant representing that not the expected amount of data could be read.
			CorruptedData,  ///< An enum constant representing that the data was detected to be bogus.
			InternalError	///< An internal error was detected.
		};

		/// Constructor for the LibCZICZIParseException. This type is used
		/// to signal that there was a parsing error.
		/// \param szErrMsg Message describing the error.
		/// \param code	    The error code.
		LibCZICZIParseException(const char* szErrMsg, ErrorCode code)
			: LibCZIException(szErrMsg), errorCode(code)
		{
		}

		/// Gets error code.
		/// \return The error code.
		ErrorCode GetErrorCode() const { return this->errorCode; }
	private:
		ErrorCode errorCode;
	};

	/// Exception for signalling an incorrect plane-coordinate object.
	struct LibCZIInvalidPlaneCoordinateException : public LibCZIException
	{
		/// Values that represent different error conditions.
		enum class ErrorCode
		{
			SurplusDimension,	 ///< An enum constant representing a dimension was specified which is not found in the document.
			MissingDimension,    ///< An enum constant representing that the plane-coordinate did not contain a coordinate which is required to specify a plane.
			InvalidDimension,	 ///< An enum constant representing that the plane-coordinate contained a dimension which is not used to specify a plane.
			CoordinateOutOfRange ///< An enum constant representing that a coordinate was given which is out-of-range.
		};

		/// Constructor for the LibCZIInvalidPlaneCoordinateException. This type is used
		/// to signal that a plane-coordinate was determined to be invalid.
		/// \param szErrMsg Message describing the error.
		/// \param code	    The error code.
		LibCZIInvalidPlaneCoordinateException(const char* szErrMsg, ErrorCode code)
			: LibCZIException(szErrMsg), errorCode(code)
		{
		}
        /// Gets error code.
        /// \return The error code.
        ErrorCode GetErrorCode() const { return this->errorCode; }
	private:
		ErrorCode errorCode;
	};
}

