#ifndef __IDENTIFIED_ITEM_UNIVERSAL_H__
#define __IDENTIFIED_ITEM_UNIVERSAL_H__

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

#include <array>

#include "CaretColorEnum.h"
#include "CaretObject.h"
#include "HistologyCoordinate.h"
#include "IdentifiedItemBase.h"
#include "IdentificationSymbolSizeTypeEnum.h"
#include "IdentifiedItemUniversalTypeEnum.h"
#include "PixelLogicalIndex.h"
#include "SceneableInterface.h"
#include "StructureEnum.h"
#include "Vector3D.h"

namespace caret {
    class IdentifiedItemUniversal  : public CaretObject, public SceneableInterface {
        
    public:

        static IdentifiedItemUniversal* newInstanceInvalidIdentification();

        static IdentifiedItemUniversal* newInstanceTextNoSymbolIdentification(const AString& simpleText,
                                                                              const AString& formattedText);
        
        static IdentifiedItemUniversal* newInstanceStereotaxicIdentification(const AString& simpleText,
                                                                             const AString& formattedText,
                                                                             const Vector3D& stereotaxicXYZ);

        static IdentifiedItemUniversal* newInstanceHistologyCoordinateIdentification(const AString& simpleText,
                                                                                     const AString& formattedText,
                                                                                     const HistologyCoordinate& histologyCoordinate);

        static IdentifiedItemUniversal* newInstanceHistologyStereotaxicCoordinateIdentification(const AString& simpleText,
                                                                                                const AString& formattedText,
                                                                                                const HistologyCoordinate& histologyCoordinate);

        static IdentifiedItemUniversal* newInstanceMediaLogicalCoordinateIdentification(const AString& simpleText,
                                                                                        const AString& formattedText,
                                                                                        const AString& dataFileName,
                                                                                        const PixelLogicalIndex& pixelLogicalIndex,
                                                                                        const Vector3D& stereotaxicXYZ,
                                                                                        const bool stereotaxicXYZValidFlag);

        static IdentifiedItemUniversal* newInstanceMediaPlaneCoordinateIdentification(const AString& simpleText,
                                                                                      const AString& formattedText,
                                                                                      const AString& dataFileName,
                                                                                      const Vector3D& pixelPlaneCoordinate,
                                                                                      const Vector3D& stereotaxicXYZ,
                                                                                      const bool stereotaxicXYZValidFlag);

        static IdentifiedItemUniversal* newInstanceSurfaceIdentification(const AString& simpleText,
                                                                         const AString& formattedText,
                                                                         const AString& dataFileName,
                                                                         const StructureEnum::Enum structure,
                                                                         const int32_t surfaceNumberOfVertices,
                                                                         const int32_t surfaceVertexIndex,
                                                                         const Vector3D& stereotaxicXYZ);

        static IdentifiedItemUniversal* newInstanceVolumeIdentification(const AString& simpleText,
                                                                        const AString& formattedText,
                                                                        const AString& dataFileName,
                                                                        const std::array<int64_t, 3>& voxelIJK,
                                                                        const Vector3D& stereotaxicXYZ);
        
        static IdentifiedItemUniversal* newInstanceFromOldIdentification(const IdentifiedItemBase* oldItem);

        virtual ~IdentifiedItemUniversal();
        
        IdentifiedItemUniversal(const IdentifiedItemUniversal& obj);

        IdentifiedItemUniversal& operator=(const IdentifiedItemUniversal& obj);
        
        IdentifiedItemUniversalTypeEnum::Enum getType() const;
        
        void setTypeToTextNoSymbol();
        
        // ADD_NEW_METHODS_HERE

        virtual bool isValid() const;
        
        void appendText(const AString& simpleText,
                        const AString& formattedText);

        void clearAllText();
        
        AString getSimpleText() const;
        
        AString getFormattedText() const;
        
        AString getDataFileName() const;
        
        StructureEnum::Enum getStructure() const;
        
        StructureEnum::Enum getContralateralStructure() const;
        
        void setContralateralStructure(const StructureEnum::Enum contralateralStructure);

        int32_t getSurfaceNumberOfVertices() const;
        
        int32_t getSurfaceVertexIndex() const;
        
        int64_t getUniqueIdentifier() const;
        
        PixelLogicalIndex getPixelLogicalIndex() const;
        
        Vector3D getPixelPlaneCoordinate() const;
        
        std::array<int64_t, 3> getVoxelIJK() const;
        
        Vector3D getStereotaxicXYZ() const;
        
        void setStereotaxicXYZ(const Vector3D& xyz);
        
        bool isStereotaxicXYZValid() const;
        
        const HistologyCoordinate& getHistologyCoordinate() const;
        
        bool isOldIdentification() const;
        
        AString getToolTip() const;
        
        virtual AString toString() const override;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName) override;
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass) override;
        
        static void resetUniqueIdentifierGenerator();
        
    private:
        IdentifiedItemUniversal();
        
        IdentifiedItemUniversal(const IdentifiedItemUniversalTypeEnum::Enum type,
                                const AString& simpleText,
                                const AString& formattedText,
                                const AString& dataFileName,
                                const StructureEnum::Enum structure,
                                const int32_t surfaceNumberOfVertices,
                                const int32_t surfaceVertexIndex,
                                const PixelLogicalIndex& pixelLogicalIndex,
                                const Vector3D& pixelPlaneCoordinate,
                                const std::array<int64_t, 3>& voxelIJK,
                                const HistologyCoordinate& histologyCoordinate,
                                const Vector3D& stereotaxicXYZ,
                                const bool stereotaxicXYZValidFlag);
        
        void copyHelperIdentifiedItemUniversal(const IdentifiedItemUniversal& obj);

        void initializeInstance();
        
        // ADD_NEW_MEMBERS_HERE
        
        IdentifiedItemUniversalTypeEnum::Enum m_type = IdentifiedItemUniversalTypeEnum::INVALID;
        
        AString m_simpleText;
        
        AString m_formattedText;
        
        AString m_dataFileName;
        
        StructureEnum::Enum m_structure = StructureEnum::INVALID;
        
        StructureEnum::Enum m_contralateralStructure = StructureEnum::INVALID;
        
        int32_t m_surfaceNumberOfVertices = -1;
        
        int32_t m_surfaceVertexIndex = - 1;
        
        PixelLogicalIndex m_pixelLogicalIndex;
        
        Vector3D m_pixelPlaneCoordinate;
        
        std::array<int64_t, 3> m_voxelIJK;
        
        HistologyCoordinate m_histologyCoordinate;
        
        Vector3D m_stereotaxicXYZ;
        
        bool m_stereotaxicXYZValidFlag = false;

        CaretColorEnum::Enum m_symbolColor = CaretColorEnum::WHITE;
        
        CaretColorEnum::Enum m_contralateralSymbolColor = CaretColorEnum::LIME;
        
        float m_symbolSize = 3.0;
        
        IdentificationSymbolSizeTypeEnum::Enum m_symbolSizeType = IdentificationSymbolSizeTypeEnum::MILLIMETERS;
        
        /**
         * Unique ID for identified items.
         * NOT saved to scenes.
         */
        int64_t m_uniqueIdentifier = -1;
        
        SceneClassAssistant* m_sceneAssistant;
        
        /*
         * Identification is from an older version prior to creation of this class
         */
        bool m_oldIdentificationFlag = false;
        
        /**
         * Generates unique IDs for identified items
         */
        static int64_t s_uniqueIdentifierGenerator;
    };
    
#ifdef __IDENTIFIED_ITEM_UNIVERSAL_DECLARE__
    int64_t IdentifiedItemUniversal::s_uniqueIdentifierGenerator = 0;
#endif // __IDENTIFIED_ITEM_UNIVERSAL_DECLARE__

} // namespace
#endif  //__IDENTIFIED_ITEM_UNIVERSAL_H__
