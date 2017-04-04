/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/

#include "CiftiSeriesMap.h"

#include "CaretAssert.h"
#include "DataFileException.h"
#include "CaretLogger.h"

#include <cmath>

using namespace caret;
using namespace std;

void CiftiSeriesMap::setLength(const int64_t& length)
{
    CaretAssert(length > 0);
    m_length = length;
}

CiftiSeriesMap::Unit CiftiSeriesMap::stringToUnit(const QString& string, bool& ok)
{
    ok = true;
    if (string == "SECOND")
    {
        return SECOND;
    } else if (string == "HERTZ") {
        return HERTZ;
    } else if (string == "METER") {
        return METER;
    } else if (string == "RADIAN") {
        return RADIAN;
    }
    ok = false;
    return SECOND;
}

QString CiftiSeriesMap::unitToString(const CiftiSeriesMap::Unit& theUnit)
{
    switch (theUnit)
    {
        case SECOND:
            return "SECOND";
        case HERTZ:
            return "HERTZ";
        case METER:
            return "METER";
        case RADIAN:
            return "RADIAN";
    }
    CaretAssert(false);
    return "UNKNOWN";
}

vector<CiftiSeriesMap::Unit> CiftiSeriesMap::getAllUnits()
{
    vector<Unit> ret;
    ret.push_back(SECOND);
    ret.push_back(HERTZ);
    ret.push_back(METER);
    ret.push_back(RADIAN);
    return ret;
}

void CiftiSeriesMap::readXML1(QXmlStreamReader& xml)
{
    QXmlStreamAttributes attrs = xml.attributes();
    float newStart = 0.0f, newStep = -1.0f, mult = 0.0f;
    bool ok = false;
    if (!attrs.hasAttribute("TimeStepUnits"))
    {
        throw DataFileException("timepoints mapping is missing required attribute TimeStepUnits");
    }
    QStringRef unitString = attrs.value("TimeStepUnits");
    if (unitString == "NIFTI_UNITS_SEC")
    {
        mult = 1.0f;
    } else if (unitString == "NIFTI_UNITS_MSEC") {
        mult = 0.001f;
    } else if (unitString == "NIFTI_UNITS_USEC") {
        mult = 0.000001f;
    } else {
        throw DataFileException("unrecognized value for TimeStepUnits: " + unitString.toString());
    }
    if (attrs.hasAttribute("TimeStart"))//optional and nonstandard
    {
        newStart = mult * attrs.value("TimeStart").toString().toFloat(&ok);
        if (!ok)
        {
            throw DataFileException("unrecognized value for TimeStart: " + attrs.value("TimeStart").toString());
        }
    }
    if (!attrs.hasAttribute("TimeStep"))
    {
        throw DataFileException("timepoints mapping is missing required attribute TimeStep");
    }
    newStep = mult * attrs.value("TimeStep").toString().toFloat(&ok);
    if (!ok)
    {
        throw DataFileException("unrecognized value for TimeStep: " + attrs.value("TimeStep").toString());
    }
    if (xml.readNextStartElement())
    {
        throw DataFileException("unexpected element in timepoints map: " + xml.name().toString());
    }
    m_length = -1;//cifti-1 doesn't know length in xml, must be set by checking the matrix
    m_start = newStart;
    m_step = newStep;
    m_unit = SECOND;
    CaretAssert(xml.isEndElement() && xml.name() == "MatrixIndicesMap");
}

void CiftiSeriesMap::readXML2(QXmlStreamReader& xml)
{
    QXmlStreamAttributes attrs = xml.attributes();
    float newStart = 0.0f, newStep = -1.0f, mult = 0.0f;
    int64_t newLength = -1;
    Unit newUnit;
    bool ok = false;
    if (!attrs.hasAttribute("SeriesUnit"))
    {
        throw DataFileException("series mapping is missing required attribute SeriesUnit");
    }
    QStringRef unitString = attrs.value("SeriesUnit");
    if (unitString == "HERTZ")
    {
        newUnit = HERTZ;
    } else if (unitString == "METER") {
        newUnit = METER;
    } else if (unitString == "RADIAN") {
        newUnit = RADIAN;
    } else if (unitString == "SECOND") {
        newUnit = SECOND;
    } else {
        throw DataFileException("unrecognized value for SeriesUnit: " + unitString.toString());
    }
    if (!attrs.hasAttribute("SeriesExponent"))
    {
        throw DataFileException("series mapping is missing required attribute SeriesExponent");
    }
    int exponent = attrs.value("SeriesExponent").toString().toInt(&ok);
    if (!ok)
    {
        throw DataFileException("unrecognized value for SeriesExponent: " + attrs.value("SeriesExponent").toString());
    }
    mult = pow(10.0f, exponent);
    if (!attrs.hasAttribute("SeriesStart"))
    {
        throw DataFileException("series mapping is missing required attribute SeriesStart");
    }
    newStart = mult * attrs.value("SeriesStart").toString().toFloat(&ok);
    if (!ok)
    {
        throw DataFileException("unrecognized value for SeriesStart: " + attrs.value("SeriesStart").toString());
    }
    if (!attrs.hasAttribute("SeriesStep"))
    {
        throw DataFileException("series mapping is missing required attribute SeriesStep");
    }
    newStep = mult * attrs.value("SeriesStep").toString().toFloat(&ok);
    if (!ok)
    {
        throw DataFileException("unrecognized value for SeriesStep: " + attrs.value("SeriesStep").toString());
    }
    if (!attrs.hasAttribute("NumberOfSeriesPoints"))
    {
        throw DataFileException("series mapping is missing required attribute NumberOfSeriesPoints");
    }
    newLength = attrs.value("NumberOfSeriesPoints").toString().toLongLong(&ok);
    if (!ok)
    {
        throw DataFileException("unrecognized value for NumberOfSeriesPoints: " + attrs.value("NumberOfSeriesPoints").toString());
    }
    if (newLength < 1)
    {
        throw DataFileException("NumberOfSeriesPoints must be positive");
    }
    if (xml.readNextStartElement())
    {
        throw DataFileException("unexpected element in series map: " + xml.name().toString());
    }
    m_length = newLength;
    m_start = newStart;
    m_step = newStep;
    m_unit = newUnit;
    CaretAssert(xml.isEndElement() && xml.name() == "MatrixIndicesMap");
}

void CiftiSeriesMap::writeXML1(QXmlStreamWriter& xml) const
{
    CaretAssert(m_length != -1);
    if (m_unit != SECOND)
    {
        CaretLogWarning("changing series units to seconds for CIFTI-1 XML");
    }
    xml.writeAttribute("IndicesMapToDataType", "CIFTI_INDEX_TYPE_TIME_POINTS");
    float mult = 1.0f;
    QString unitString = "NIFTI_UNITS_SEC";
    float test = m_step;
    if (test == 0.0f) test = m_start;
    if (test != 0.0f)
    {
        if (abs(test) < 0.00005f)
        {
            mult = 1000000.0f;
            unitString = "NIFTI_UNITS_USEC";
        } else if (abs(test) < 0.05f) {
            mult = 1000.0f;
            unitString = "NIFTI_UNITS_MSEC";
        }
    }
    xml.writeAttribute("TimeStepUnits", unitString);
    xml.writeAttribute("TimeStart", QString::number(mult * m_start, 'f', 7));//even though it is nonstandard, write it, always
    xml.writeAttribute("TimeStep", QString::number(mult * m_step, 'f', 7));
}

void CiftiSeriesMap::writeXML2(QXmlStreamWriter& xml) const
{
    CaretAssert(m_length != -1);
    xml.writeAttribute("IndicesMapToDataType", "CIFTI_INDEX_TYPE_SERIES");
    int exponent = 0;
    float test = m_step;
    if (test == 0.0f) test = m_start;
    if (test != 0.0f)
    {
        exponent = 3 * (int)floor((log10(test) - log10(0.05f)) / 3.0f);//some magic to get the exponent that is a multiple of 3 that puts the test value in [0.05, 50]
    }
    float mult = pow(10.0f, -exponent);
    QString unitString;
    switch (m_unit)
    {
        case HERTZ:
            unitString = "HERTZ";
            break;
        case METER:
            unitString = "METER";
            break;
        case RADIAN:
            unitString = "RADIAN";
            break;
        case SECOND:
            unitString = "SECOND";
            break;
    }
    xml.writeAttribute("NumberOfSeriesPoints", QString::number(m_length));
    xml.writeAttribute("SeriesExponent", QString::number(exponent));
    xml.writeAttribute("SeriesStart", QString::number(mult * m_start, 'f', 7));
    xml.writeAttribute("SeriesStep", QString::number(mult * m_step, 'f', 7));
    xml.writeAttribute("SeriesUnit", unitString);
}
