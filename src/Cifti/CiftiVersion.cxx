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

#include "CiftiVersion.h"

#include "DataFileException.h"

using namespace caret;

CiftiVersion::CiftiVersion()
{
    m_major = 2;
    m_minor = 0;
}

CiftiVersion::CiftiVersion(const int16_t& major, const int16_t& minor)
{
    m_major = major;
    m_minor = minor;
}

CiftiVersion::CiftiVersion(const QString& versionString)
{
    int result = versionString.indexOf('.');
    bool ok = false;
    if (result < 0)
    {
        m_minor = 0;
        m_major = versionString.toShort(&ok);
        if (!ok) throw DataFileException("improperly formatted CIFTI version string: '" + versionString + "'");
    } else {
        if (result == 0) throw DataFileException("improperly formatted CIFTI version string: '" + versionString + "'");
        m_major = versionString.mid(0, result).toShort(&ok);
        if (!ok) throw DataFileException("improperly formatted CIFTI version string: '" + versionString + "'");
        m_minor = versionString.mid(result + 1).toShort(&ok);
        if (!ok) throw DataFileException("improperly formatted CIFTI version string: '" + versionString + "'");
    }
}

bool CiftiVersion::hasReversedFirstDims() const
{
    if (m_major == 1 && m_minor == 0) return true;
    return false;
}

bool CiftiVersion::operator<(const CiftiVersion& rhs) const
{
    if (m_major < rhs.m_major) return true;
    if (m_major == rhs.m_major && m_minor < rhs.m_minor) return true;
    return false;
}

bool CiftiVersion::operator<=(const CiftiVersion& rhs) const
{
    if (m_major < rhs.m_major) return true;
    if (m_major == rhs.m_major && m_minor <= rhs.m_minor) return true;
    return false;
}

bool CiftiVersion::operator==(const CiftiVersion& rhs) const
{
    if (m_major == rhs.m_major && m_minor == rhs.m_minor) return true;
    return false;
}

bool CiftiVersion::operator!=(const CiftiVersion& rhs) const
{
    return !(*this == rhs);
}

bool CiftiVersion::operator>(const caret::CiftiVersion& rhs) const
{
    if (m_major > rhs.m_major) return true;
    if (m_major == rhs.m_major && m_minor > rhs.m_minor) return true;
    return false;
}

bool CiftiVersion::operator>=(const caret::CiftiVersion& rhs) const
{
    if (m_major > rhs.m_major) return true;
    if (m_major == rhs.m_major && m_minor >= rhs.m_minor) return true;
    return false;
}

QString CiftiVersion::toString() const
{
    QString ret = QString::number(m_major);
    if (m_minor != 0) ret += "." + QString::number(m_minor);
    return ret;
}
