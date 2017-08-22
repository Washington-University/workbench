#ifndef __CHART_TWO_TITLE_H__
#define __CHART_TWO_TITLE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

#include "CaretObjectTracksModification.h"

#include "SceneableInterface.h"


namespace caret {
    class SceneClassAssistant;

    class ChartTwoTitle : public CaretObjectTracksModification, public SceneableInterface {
        
    public:
        ChartTwoTitle();
        
        virtual ~ChartTwoTitle();
        
        ChartTwoTitle(const ChartTwoTitle& obj);

        ChartTwoTitle& operator=(const ChartTwoTitle& obj);
        
        float getPaddingSize() const;
        
        void setPaddingSize(const float paddingSize);
        
        float getTextSize() const;
        
        void setTextSize(const float textSize);
        
        AString getText() const;
        
        void setText(const AString& text);
        
        bool isDisplayed() const;
        
        void setDisplayed(const bool displayed);
        
        void reset();
        
        // ADD_NEW_METHODS_HERE

        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

          
          
          
          
          
// If there will be sub-classes of this class that need to save
// and restore data from scenes, these pure virtual methods can
// be uncommented to force their implementation by sub-classes.
//    protected: 
//        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
//                                             SceneClass* sceneClass) = 0;
//
//        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
//                                                  const SceneClass* sceneClass) = 0;

    private:
        void copyHelperChartTwoTitle(const ChartTwoTitle& obj);

        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        /** Padding between title and edge of viewport*/
        float m_paddingSize;
        
        /** Size of title characters when drawing*/
        float m_textSize;
        
        /** text displayed as title*/
        AString m_text;
        
        /** display status of title*/
        bool m_displayed;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHART_TWO_TITLE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_TWO_TITLE_DECLARE__

} // namespace
#endif  //__CHART_TWO_TITLE_H__
