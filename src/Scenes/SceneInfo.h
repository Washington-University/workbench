#ifndef __SCENE_INFO_H__
#define __SCENE_INFO_H__

/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#include <stdint.h>
#include "CaretObject.h"



namespace caret {
    class XmlWriter;
    
    class SceneInfo : public CaretObject {
        
    public:
        SceneInfo();
        
        virtual ~SceneInfo();
        
        AString getName() const;
        
        void setName(const AString& sceneName);
        
        AString getDescription() const;
        
        void setDescription(const AString& description);
        
        void getImageBytes(QByteArray& imageBytesOut,
                                    AString& imageFormatOut) const;

        void setImageBytes(const QByteArray& imageBytes,
                                    const AString& imageFormat);
        
        bool hasImage() const;
        
        void writeSceneInfo(XmlWriter& xmlWriter,
                            const int32_t sceneInfoIndex) const;

        void setImageFromText(const AString& text,
                                       const AString& encoding,
                                       const AString& imageFormat);
        
        void writeSceneInfoImage(XmlWriter& xmlWriter,
                                 const AString& xmlTag,
                                 const QByteArray& imageBytes,
                                 const AString& imageFormat) const;
        
    private:
        SceneInfo(const SceneInfo&);

        SceneInfo& operator=(const SceneInfo&);
        
        /** name of scene*/
        AString m_sceneName;
        
        /** description of scene */
        AString m_sceneDescription;
        
        /** thumbnail image bytes */
        QByteArray m_imageBytes;
        
        /** format of thumbnail image (eg: jpg, ppm, etc.) */
        AString m_imageFormat;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SCENE_INFO_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SCENE_INFO_DECLARE__

} // namespace
#endif  //__SCENE_INFO_H__
