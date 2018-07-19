#ifndef __ANNOTATION_TEXT_SUBSTITUTION_FILE_H__
#define __ANNOTATION_TEXT_SUBSTITUTION_FILE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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


#include <map>
#include <memory>

#include "CaretDataFile.h"
#include "EventListenerInterface.h"
#include "MapYokingGroupEnum.h"

namespace caret {

    class EventAnnotationTextSubstitutionGet;
    class SceneClassAssistant;
    
    class AnnotationTextSubstitutionFile : public CaretDataFile, public EventListenerInterface {
        
    public:
        AnnotationTextSubstitutionFile();
        
        virtual ~AnnotationTextSubstitutionFile();
        
        AnnotationTextSubstitutionFile(const AnnotationTextSubstitutionFile& obj);

        AnnotationTextSubstitutionFile& operator=(const AnnotationTextSubstitutionFile& obj);
        

        // ADD_NEW_METHODS_HERE

        void getSubstitutionValues(EventAnnotationTextSubstitutionGet* substituteEvent) const;
        
        virtual StructureEnum::Enum getStructure() const override;
        
        virtual void setStructure(const StructureEnum::Enum structure) override;
        
        virtual GiftiMetaData* getFileMetaData() override;
        
        virtual const GiftiMetaData* getFileMetaData() const override;
        
        virtual bool isEmpty() const override;
        
        virtual void clear() override;
        
        virtual void receiveEvent(Event* event) override;

        virtual void readFile(const AString& filename) override;
        
        virtual void writeFile(const AString& filename) override;
        
        virtual bool supportsWriting() const override;
        
        virtual bool isModified() const override;

        int32_t getNumberOfSubstitutions() const;
        
        int32_t getNumberOfValues() const;
        
        AString getTextSubstitution(const int32_t textSubstitutionIndex,
                                    const int32_t valueIndex) const;
        
        AString getTextSubstitution(const AString& textSubstitutionName,
                                    const int32_t valueIndex) const;
        
        MapYokingGroupEnum::Enum getMapYokingGroup() const;
        
        void setMapYokingGroup(const MapYokingGroupEnum::Enum mapYokingGroup);
        
        int32_t getSelectedValueIndex() const;
        
        void setSelectedValueIndex(const int32_t valueIndex);
        
    protected:
        virtual void saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                         SceneClass* sceneClass);
        
        virtual void restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                              const SceneClass* sceneClass);
        
    private:
        void copyHelperAnnotationTextSubstitutionFile(const AnnotationTextSubstitutionFile& obj);

        void clearPrivate();
        
        void initializeAnnotationTextSubstitutionFile();
        
        AString columnIndexToDefaultSubstitutionName(const int32_t columnIndex) const;
        
        int32_t getColumnIndexForSubstitutionName(const AString substitutionName) const;
        
        std::unique_ptr<GiftiMetaData> m_metadata;
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        std::vector<AString> m_dataValues;
        
        std::map<AString, int32_t> m_substitutionNameToIndexMap;
        
        int32_t m_numberOfSubstitutions;
        
        int32_t m_numberOfValues;
        
        MapYokingGroupEnum::Enum m_mapYokingGroup;
        
        mutable int32_t m_selectedValueIndex;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_TEXT_SUBSTITUTION_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_TEXT_SUBSTITUTION_FILE_DECLARE__

} // namespace
#endif  //__ANNOTATION_TEXT_SUBSTITUTION_FILE_H__
