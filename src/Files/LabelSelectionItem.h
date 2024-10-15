#ifndef __LABEL_SELECTION_ITEM_H__
#define __LABEL_SELECTION_ITEM_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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
#include <memory>
#include <vector>

#include <QStandardItem>

#include "CaretObject.h"
#include "Cluster.h"
#include "SceneableInterface.h"


namespace caret {
    class SceneClassAssistant;

    class LabelSelectionItem : public QStandardItem, public SceneableInterface {
        
    public:
        class COG : public CaretObject {
        public:
            COG(const AString& title,
                const Vector3D& xyz,
                const int64_t numberOfBrainordinates)
            : m_title(title),
            m_xyz(xyz),
            m_numberOfBrainordinates(numberOfBrainordinates) { }
            
            ~COG() { }
            
            AString getTitle() const { return m_title; }
            
            const Vector3D& getXYZ() const { return m_xyz; }
            
            int64_t getNumberOfBrainordinates() const { return m_numberOfBrainordinates; }

            void merge(const COG& cog) {
                const Vector3D sum((m_xyz * m_numberOfBrainordinates)
                                   + (cog.m_xyz * cog.m_numberOfBrainordinates));
                m_numberOfBrainordinates += cog.m_numberOfBrainordinates;
                if (m_numberOfBrainordinates >= 1) {
                    m_xyz = (sum / static_cast<float>(m_numberOfBrainordinates));
                }
                else {
                    m_xyz.fill(0.0);
                }
            }
            
            void updateTitle(const AString& titlePrefix) {
                m_title = (titlePrefix
                           + ": "
                           + AString::number(m_numberOfBrainordinates)
                           + " Brainordinates; COG "
                           + m_xyz.toString());
            }
        private:
            AString m_title;
            
            Vector3D m_xyz;
            
            int64_t m_numberOfBrainordinates;
        };
        
        class CogSet : public CaretObject {
        public:
            CogSet() { }
            
            CogSet(COG* allCOG,
                   COG* centralCOG,
                   COG* leftCOG,
                   COG* rightCOG)
            : m_allCOG(allCOG),
            m_centralCOG(centralCOG),
            m_leftCOG(leftCOG),
            m_rightCOG(rightCOG) { }

            std::vector<const COG*> getCOGs() const {
                std::vector<const COG*> cogsOut;
                if (m_allCOG != NULL)     cogsOut.push_back(m_allCOG.get());
                if (m_centralCOG != NULL) cogsOut.push_back(m_centralCOG.get());
                if (m_leftCOG != NULL)    cogsOut.push_back(m_leftCOG.get());
                if (m_rightCOG != NULL)   cogsOut.push_back(m_rightCOG.get());
                return cogsOut;
            }
            
            void merge(const CogSet* cogSet) {
                if (cogSet->m_allCOG != NULL) {
                    if (m_allCOG != NULL) {
                        m_allCOG->merge(*cogSet->m_allCOG);
                    }
                    else {
                        m_allCOG.reset(new COG("Sum All",
                                               cogSet->m_allCOG->getXYZ(),
                                               cogSet->m_allCOG->getNumberOfBrainordinates()));
                    }
                    m_allCOG->updateTitle("Sum All");
                }

                if (cogSet->m_centralCOG != NULL) {
                    if (m_centralCOG != NULL) {
                        m_centralCOG->merge(*cogSet->m_centralCOG);
                    }
                    else {
                        m_centralCOG.reset(new COG("Sum Central",
                                               cogSet->m_centralCOG->getXYZ(),
                                               cogSet->m_centralCOG->getNumberOfBrainordinates()));
                    }
                    m_centralCOG->updateTitle("Sum Central");
                }
                
                if (cogSet->m_leftCOG != NULL) {
                    if (m_leftCOG != NULL) {
                        m_leftCOG->merge(*cogSet->m_leftCOG);
                    }
                    else {
                        m_leftCOG.reset(new COG("Sum Left",
                                               cogSet->m_leftCOG->getXYZ(),
                                               cogSet->m_leftCOG->getNumberOfBrainordinates()));
                    }
                    m_leftCOG->updateTitle("Sum Left");
                }
                
                if (cogSet->m_rightCOG != NULL) {
                    if (m_rightCOG != NULL) {
                        m_rightCOG->merge(*cogSet->m_rightCOG);
                    }
                    else {
                        m_rightCOG.reset(new COG("Sum Right",
                                               cogSet->m_rightCOG->getXYZ(),
                                               cogSet->m_rightCOG->getNumberOfBrainordinates()));
                    }
                    m_rightCOG->updateTitle("Sum Right");
                }
            }
        private:
            std::unique_ptr<COG> m_allCOG;
            std::unique_ptr<COG> m_centralCOG;
            std::unique_ptr<COG> m_leftCOG;
            std::unique_ptr<COG> m_rightCOG;
        };
        
        /*
         * Type of item returned by override type() method
         */
        enum class ItemType : int {
            /** Item is a label that that is turned on/off by user */
            ITEM_LABEL = QStandardItem::UserType + 1,
            /** Item is in the hierarchy and IS NOT a label */
            ITEM_HIERARCHY = QStandardItem::UserType + 2
        };
        
        LabelSelectionItem(const AString& text,
                           const AString& ontologyID,
                           const int32_t labelIndex,
                           const std::array<uint8_t, 4>& labelRGBA);
        
        LabelSelectionItem(const AString& text,
                           const AString& ontologyID);
        
        virtual ~LabelSelectionItem();
        
        virtual QStandardItem* clone() const;
        
        virtual int type() const;

        AString getOntologyID() const;
        
        AString getTextForInfoDisplay() const;
        
        int32_t getLabelIndex() const;
        
        void setAllChildrenChecked(const bool checked);

        Qt::CheckState setCheckStateFromChildren();
        
        std::vector<LabelSelectionItem*> getThisAndAllDescendantsOfType(const LabelSelectionItem::ItemType itemType);
        
        std::vector<LabelSelectionItem*> getThisAndAllDescendants();
        
        AString toFormattedString(const AString& indentation) const;
        
        const CogSet* getCentersOfGravity() const;
        
        const CogSet* getMyAndChildrenCentersOfGravity() const;
        
        void setClusters(const std::vector<const Cluster*>& clusters);
        
        void appendToToolTip(const QString& text);
        
        // ADD_NEW_METHODS_HERE

        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

    protected:
        LabelSelectionItem(const LabelSelectionItem& other);
        
        LabelSelectionItem& operator=(const LabelSelectionItem& other); 
        
    private:
        void copyHelper(const LabelSelectionItem& other);
        
        void initializeInstance();
        
        void setCheckedStatusOfAllChildren(QStandardItem* item,
                                           const Qt::CheckState checkState);
        
        void setMyAndChildrenMergedClusters();

        const ItemType m_itemType;
        
        AString m_ontologyID;
        
        int32_t m_labelIndex;
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        std::unique_ptr<CogSet> m_centersOfGravity;
        
        std::unique_ptr<CogSet> m_myAndChildrenCentersOfGravity;
        
        friend class LabelSelectionItemModel;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __LABEL_SELECTION_ITEM_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __LABEL_SELECTION_ITEM_DECLARE__

} // namespace
#endif  //__LABEL_SELECTION_ITEM_H__
