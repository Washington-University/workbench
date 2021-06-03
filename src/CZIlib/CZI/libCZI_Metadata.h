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

#include "libCZI_DimCoordinate.h"
#include "libCZI_Pixels.h"
#include <limits>
#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace libCZI
{
	/// General document information - corresponding to Information/Document.
	struct GeneralDocumentInfo
	{
		GeneralDocumentInfo() :rating(0) {}
		std::wstring name;				///< Name of the document.
		std::wstring title;				///< Title of the document.
		std::wstring userName;			///< Name of the user who created the document.
		std::wstring description;		///< A text describing the document.
		std::wstring comment;			///< A text with comments on the document.
		std::wstring keywords;			///< List of keywords (should be separated by semicolons)
		int rating;						///< An integer specifying a "five-star-rating" (should be between 0 and 5).
		std::wstring creationDateTime;	///< The creation date of the document (formatted as xml-datatype "dateTime").
	};

	/// Scaling information - gives the size of a pixel.
	struct ScalingInfo
	{
		ScalingInfo() : scaleX(std::numeric_limits<double>::quiet_NaN()), scaleY(std::numeric_limits<double>::quiet_NaN()), scaleZ(std::numeric_limits<double>::quiet_NaN()) {}
		double scaleX;	///< The length of a pixel in x-direction in the unit meters. If unknown/invalid, this value is numeric_limits<double>::quiet_NaN().
		double scaleY;	///< The length of a pixel in y-direction in the unit meters. If unknown/invalid, this value is numeric_limits<double>::quiet_NaN().
		double scaleZ;	///< The length of a pixel in y-direction in the unit meters. If unknown/invalid, this value is numeric_limits<double>::quiet_NaN().
	};

	/// Base class for information about the dimension. (not yet implemented)
	class IDimensionInfo
	{
	public:

		/// Gets the dimension index.
		/// \return The dimension index.
		virtual DimensionIndex GetDimension() const = 0;

		/// Gets the interval.
		/// \param [out] start If non-null, it will receive the start index.
		/// \param [out] end   If non-null, it will receive the end index.
		virtual void GetInterval(int* start, int* end) const = 0;

		virtual ~IDimensionInfo() {}
	};

	class IChannelDisplaySetting;

	/// The display settings.
	class IDisplaySettings
	{
	public:
		/// The (normalized) control points of a spline.
		struct SplineControlPoint
		{
			double x;	///< The normalized x-coordinate of a spline control point.
			double y;	///< The normalized y-coordinate of a spline control point.
		};

		/// Values that represent the gradation curve modes.
		enum class GradationCurveMode
		{
			Linear,		///< The gradation curve is a straight line (from white point to black point).
			Gamma,		///< The gradation curve is defined by a gamma.
			Spline		///< The gradation curve is defined by piecewise splines.
		};

		/// The coefficients of a cubic spline defined by \f$a\,x^3 + b\,x^2 + c\,x + d =y\f$.
		struct CubicSplineCoefficients
		{
			double a; ///< The coefficient of the cube.
			double b; ///< The coefficient of the square.
			double c; ///< The coefficient of the linear term.
			double d; ///< The constant.

			/// Gets the coefficients by an index (where a is 0, b is 1, c is 2 and d is 3). If the index is out-of-range, the method returns NaN.
			///
			/// \param index The index of the coefficient to get. 
			///
			/// \return The specified coefficient if the index is valid, NaN otherwise.
			double Get(int index) const
			{
				switch (index)
				{
				case 0:return this->a;
				case 1:return this->b;
				case 2:return this->c;
				case 3:return this->d;
				}

				return std::numeric_limits<double>::quiet_NaN();
			}
		};

		/// The defintion of the (piecewise) spline. The spline starts at <tt>xPos</tt> which is the normalized position (between 0 and 1).
		struct SplineData
		{
			/// The (normalized) position for which this spline definition is valid.
			double xPos;

			/// The spline coefficients for this piece.
			CubicSplineCoefficients coefficients;
		};

		/// Enum all channels (which are described by the display-settings object).
		///
		/// \param func The functor to be called (passing in the channel index). If the functor returns false, the 
		/// 			enumeration is stopped.
		virtual void EnumChannels(std::function<bool(int chIndex)> func) const = 0;

		/// Gets channel display settings for the specified channel. If the channel index is not valid, then
		/// an empty shared_ptr is returned.
		///
		/// \param chIndex The channel index.
		///
		/// \return The channel display settings object (if the channel index was valid), and empty shared_ptr otherwise.
		virtual std::shared_ptr<libCZI::IChannelDisplaySetting> GetChannelDisplaySettings(int chIndex) const = 0;

		virtual ~IDisplaySettings() {}
	};

	/// The display-settings for a channel.
	class IChannelDisplaySetting
	{
	public:

		/// Gets a boolean indicating whether the corresponding channel is 'active' in the multi-channel-composition.
		///
		/// \return True if the corresponding channel is 'active', false otherwise.
		virtual bool	GetIsEnabled() const = 0;

		/// Gets the weight of the channel (for multi-channel-composition).
		///
		/// \return The weight.
		virtual float	GetWeight() const = 0;

		/// Attempts to get the RGB24-tinting color for the corresponding channel. If tinting is not enabled, then
		/// this method will return false.
		///
		/// \param [out] pColor If tinting is enabled for the corresponding channel, then (if non-null) will receive the tinting-color.
		///
		/// \return True if tinting is enabled for the corresponding channel (and in this case <tt>pColor</tt> will be set), false otherwise (and <tt>pColor</tt> will not be set).
		virtual bool	TryGetTintingColorRgb8(libCZI::Rgb8Color* pColor) const = 0;

		/// Gets the black point and the white point.
		///
		/// \param [out] pBlack If non-null, the black point will be returned.
		/// \param [out] pWhite If non-null, the white point will be returned.
		virtual void	GetBlackWhitePoint(float* pBlack, float* pWhite) const = 0;

		/// Gets gradation curve mode.
		///
		/// \return The gradation curve mode.
		virtual IDisplaySettings::GradationCurveMode GetGradationCurveMode() const = 0;

		/// Attempts to get the gamma - this will only be available if gradation curve mode is <tt>Gamma</tt>.
		///
		/// \param [out] gamma If non-null and applicable, the gamma will be returned.
		///
		/// \return True if the corresponding channel uses gradation curve mode <tt>Gamma</tt> (and a value for gamma is available), false otherwise.
		virtual bool	TryGetGamma(float* gamma)const = 0;

		/// Attempts to get spline control points - this will only be available if gradation curve mode is <tt>Spline</tt>.
		/// \remark
		/// We make no promises that both the control-points and the spline-data are always available. It might be plausible that
		/// the spline is defined in a different way (different than control-points), so in that case only the "spline-data" would
		/// be available. So - be careful is using this interface in a context different thant "CZI-metadata" where it might be
		/// the case the 'TryGetSplineControlPoints' will fail but 'TryGetSplineData' might succeed.
		/// Maybe better should remove 'TryGetSplineData' from this interface.
		///
		/// \param [in,out] ctrlPts If non-null, the control points will be written to this vector.
		///
		/// \return True if it succeeds, false if it fails.
		virtual bool	TryGetSplineControlPoints(std::vector<libCZI::IDisplaySettings::SplineControlPoint>* ctrlPts) const = 0;

		/// Attempts to get the spline data - this will only be available if gradation curve mode is <tt>Spline</tt>.
		///
		/// \param [in,out] data If non-null, the spline data will be written to this vector.
		///
		/// \return True if it the corresponding channels uses gradation curve mode <tt>Spline</tt>, false otherwise.
		virtual bool	TryGetSplineData(std::vector<libCZI::IDisplaySettings::SplineData>* data) const = 0;

		virtual ~IChannelDisplaySetting() {}
	};

	/// The top-level interface for the CZI-metadata object.
	class ICziMultiDimensionDocumentInfo
	{
	public:

		/// Gets "general document information".
		/// \return The "general document information".
		virtual GeneralDocumentInfo GetGeneralDocumentInfo() const = 0;

		/// Gets "scaling information".
		/// \return The "scaling information".
		virtual libCZI::ScalingInfo GetScalingInfo() const = 0;

		/// Enumerate the dimensions (defined in the metadata).
		/// /remark
		/// Not yet implemented.
		/// \param enumDimensions The functor which will be called for each dimension. If the functor returns false, the enumeration is cancelled.
		virtual void EnumDimensions(std::function<bool(DimensionIndex)> enumDimensions) = 0;

		/// Gets the dimension information for the specified dimension.
		/// /remark
		/// Not yet implemented.
		/// \param dim The dimension to retrieve the information for.
		/// \return The dimension information if available.
		virtual std::shared_ptr<IDimensionInfo> GetDimensionInfo(DimensionIndex dim) = 0;

		/// Gets the display settings.
		/// \remark
		/// This method may return an empty shared_ptr in case that display-settings are not present in the metadata.
		/// \return The display settings object.
		virtual std::shared_ptr<IDisplaySettings> GetDisplaySettings() const = 0;

		virtual ~ICziMultiDimensionDocumentInfo() {}

		/// Gets a vector with all dimensions (found in metadata).
		/// \return The vector containing all dimensions.
		std::vector<DimensionIndex> GetDimensions()
		{
			std::vector<DimensionIndex> vec;
			this->EnumDimensions([&](DimensionIndex i)->bool {vec.push_back(i); return true; });
			return vec;
		}
	};

	/// Representation of the CZI-metadata.
	class ICziMetadata
	{
	public:
		/// Gets the metadata as an unprocessed UTF8-encoded XML-string.
		///
		/// \return The metadata (unprocessed UTF8-encoded XML).
		virtual std::string GetXml() = 0;

		/// Gets the "document information" part of the metadata.
		///
		/// \return The "document information".
		virtual std::shared_ptr<libCZI::ICziMultiDimensionDocumentInfo> GetDocumentInfo() = 0;

		virtual ~ICziMetadata() {}
	};
}


