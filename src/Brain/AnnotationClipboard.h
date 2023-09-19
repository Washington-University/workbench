#ifndef __ANNOTATION_CLIPBOARD_H__
#define __ANNOTATION_CLIPBOARD_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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


#include <memory>

#include "AnnotationAndFile.h"
#include "CaretObject.h"
#include "UserInputModeEnum.h"
#include "Vector3D.h"

namespace caret {

    class Annotation;
    class AnnotationFile;
    class Brain;

    class AnnotationClipboard : public CaretObject {
        
    public:
        AnnotationClipboard(const UserInputModeEnum::Enum userInputMode,
                            Brain* brain);
        
        virtual ~AnnotationClipboard();
        
        AnnotationClipboard(const AnnotationClipboard&) = delete;

        AnnotationClipboard& operator=(const AnnotationClipboard&) = delete;

        void clear();
        
        bool isEmpty() const;
        
        int32_t getNumberOfAnnotations() const;
        
        const Annotation* getAnnotation(const int32_t index) const;
        
        Annotation* getCopyOfAnnotation(const int32_t index) const;
        
        AnnotationGroupKey getAnnotationGroupKey(const int32_t index) const;
        
        const Vector3D& getMouseWindowCoordinates() const;
        
        const std::vector<Vector3D>& getAnnotationWindowCoordinates() const;
        
        bool areAllAnnotationsInSameUserGroup() const;
        
        bool setContent(const std::vector<AnnotationAndFile>& annotationsAndFile,
                        std::vector<Vector3D>& annotationWindowCoordinates,
                        const Vector3D& mouseWindowCoordinates);

        void setContent(AnnotationFile* annotationFile,
                        const Annotation* annotation,
                        std::vector<Vector3D>& annotationWindowCoordinates,
                        Vector3D& mouseWindowCoordinates);

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        static bool areAnnotationsClipboardEligible(const std::vector<AnnotationAndFile>& annotationsAndFile);
        
    private:
        const UserInputModeEnum::Enum m_userInputMode;
        
        Brain* m_brain = NULL;
        
        std::vector<Vector3D> m_annotationWindowCoordinates;
        
        std::vector<AnnotationAndFile> m_clipboardContent;
        
        Vector3D m_mouseWindowCoordinates;
        
        bool m_allAnnotationsInSameUserGroupFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_CLIPBOARD_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_CLIPBOARD_DECLARE__

} // namespace
#endif  //__ANNOTATION_CLIPBOARD_H__
