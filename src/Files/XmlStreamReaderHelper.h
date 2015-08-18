#ifndef __XML_STREAM_READER_HELPER_H__
#define __XML_STREAM_READER_HELPER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#include "CaretObject.h"

class QXmlStreamAttributes;
class QXmlStreamReader;

namespace caret {

    class GiftiMetaData;
    
    class XmlStreamReaderHelper : public CaretObject {
        
    public:
        XmlStreamReaderHelper(const QString& filename,
                              QXmlStreamReader* stream);
        
        virtual ~XmlStreamReaderHelper();
        
        QString getOptionalAttributeStringValue(const QXmlStreamAttributes& attributes,
                                                const QString& elementName,
                                                const QString& attributeName,
                                                const QString& defaultValue);
        
        QString getRequiredAttributeStringValue(const QXmlStreamAttributes& attributes,
                                                       const QString& elementName,
                                                       const QString& attributeName);
        
        int getRequiredAttributeIntValue(const QXmlStreamAttributes& attributes,
                                                const QString& elementName,
                                                const QString& attributeName);
        
        float getOptionalAttributeFloatValue(const QXmlStreamAttributes& attributes,
                                            const QString& elementName,
                                            const QString& attributeName,
                                            const float defaultValue);
        
        float getRequiredAttributeFloatValue(const QXmlStreamAttributes& attributes,
                                                    const QString& elementName,
                                                    const QString& attributeName);
        
        bool getOptionalAttributeBoolValue(const QXmlStreamAttributes& attributes,
                                           const QString& elementName,
                                           const QString& attributeName,
                                           const bool defaultValue);
        
        bool getRequiredAttributeBoolValue(const QXmlStreamAttributes& attributes,
                                           const QString& elementName,
                                           const QString& attributeName);
        
        void readMetaData(GiftiMetaData* metadata);
        
        void throwDataFileException(const QString message);
        
    private:
        XmlStreamReaderHelper(const XmlStreamReaderHelper&);

        XmlStreamReaderHelper& operator=(const XmlStreamReaderHelper&);
        
        void readMetaDataElement(GiftiMetaData* metadata);
        
        const QString m_filename;
        
        QXmlStreamReader* m_stream;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __XML_STREAM_READER_HELPER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __XML_STREAM_READER_HELPER_DECLARE__

} // namespace
#endif  //__XML_STREAM_READER_HELPER_H__
