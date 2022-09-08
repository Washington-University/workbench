#ifndef __BRAIN_OPEN_G_L_VIEWPORT_CONTENT__H_
#define __BRAIN_OPEN_G_L_VIEWPORT_CONTENT__H_

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

#include "CaretObject.h"
#include "Matrix4x4.h"
#include "VolumeSliceViewPlaneEnum.h"
#include "VolumeSliceViewAllPlanesLayoutEnum.h"

namespace caret {

    class BrowserTabContent;
    class BrowserWindowContent;
    class GapsAndMargins;
    class GraphicsObjectToWindowTransform;
    class SpacerTabContent;
    class TileTabsLayoutGridConfiguration;
    class TileTabsLayoutManualConfiguration;
    
    class BrainOpenGLViewportContent : public CaretObject {
        
    public:
        ~BrainOpenGLViewportContent();
        
        BrainOpenGLViewportContent(const BrainOpenGLViewportContent& obj);
        
        BrainOpenGLViewportContent& operator=(const BrainOpenGLViewportContent& obj);
        
        bool getChartDataMatricesAndViewport(Matrix4x4& chartDataProjectionMatrixOut,
                                          Matrix4x4& chartDataModelViewMatrixOut,
                                          int chartViewportOut[4]) const;
        
        void setChartDataMatricesAndViewport(const Matrix4x4& chartDataProjectionMatrix,
                                          const Matrix4x4& chartDataModelViewMatrix,
                                             const int chartViewport[4]) const;
        
        void getModelViewport(int modelViewportOut[4]) const;
        
        void getSurfaceMontageModelViewport(const int32_t montageX,
                                            const int32_t montageY,
                                            int subViewportOut[4]) const;
        
        void getTabViewportBeforeApplyingMargins(int tabViewportOut[4]) const;
        
        void getTabViewportManualLayoutBeforeAspectLocking(int tabViewportOut[4]) const;
        
        void getWindowViewport(int windowViewportOut[4]) const;
        
        void getWindowBeforeAspectLockingViewport(int windowBeforeAspectLockingViewportOut[4]) const;
        
        int getWindowIndex() const;
        
        BrowserTabContent* getBrowserTabContent() const;
        
        SpacerTabContent* getSpacerTabContent() const;
        
        int32_t getTabIndex() const;
        
        bool isTabHighlighted() const;
        
        void setHistologyGraphicsObjectToWindowTransform(GraphicsObjectToWindowTransform* transform) const;
        
        const GraphicsObjectToWindowTransform* getHistologyGraphicsObjectToWindowTransform() const;
        
        void setMediaGraphicsObjectToWindowTransform(GraphicsObjectToWindowTransform* transform) const;
        
        const GraphicsObjectToWindowTransform* getMediaGraphicsObjectToWindowTransform() const;
        
        void setVolumeMprGraphicsObjectToWindowTransform(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                         GraphicsObjectToWindowTransform* transform) const;
        
        const GraphicsObjectToWindowTransform* getVolumeMprGraphicsObjectToWindowTransform(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane) const;
        
        float getTranslationStepValueForCustomViewDialog() const;
        
        float getTranslationFactorForMousePanning() const;
        
        static void adjustViewportForAspectRatio(int viewport[4],
                                                 const float aspectRatio);
        
        static void adjustWidthHeightForAspectRatio(const float aspectRatio,
                                                    int32_t& width,
                                                    int32_t& height);
        
        static std::vector<BrainOpenGLViewportContent*> createViewportContentForTileTabs(std::vector<BrowserTabContent*>& tabContents,
                                                                                         BrowserWindowContent* browserWindowContent,
                                                                                         const GapsAndMargins* gapsAndMargins,
                                                                                         const int32_t windowBeforeAspectLockingViewport[4],
                                                                                         const int32_t windowViewport[4],
                                                                                         const int32_t windowIndex,
                                                                                         const int32_t highlightTabIndex);
        
        static BrainOpenGLViewportContent* createViewportForSingleTab(std::vector<BrowserTabContent*>& allTabContents,
                                                                      BrowserTabContent* selectedTabContent,
                                                                      const GapsAndMargins* gapsAndMargins,
                                                                      const int32_t windowIndex,
                                                                      const int32_t windowBeforeAspectLockingViewport[4],
                                                                      const int32_t windowViewport[4]);
        
        static void getSliceAllViewViewport(const int32_t tabViewport[4],
                                            const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                            const VolumeSliceViewAllPlanesLayoutEnum::Enum allPlanesLayout,
                                            int32_t viewportOut[4]);
        
        static VolumeSliceViewPlaneEnum::Enum getSliceViewPlaneForVolumeAllSliceView(const int viewport[4],
                                                                                     const VolumeSliceViewAllPlanesLayoutEnum::Enum allPlanesLayout,
                                                                                     const int32_t mousePressX,
                                                                                     const int32_t mousePressY,
                                                                                     int sliceViewportOut[4]);
        
    private:
        /**
         * Assists with creation of the tile tab viewports
         */
        class TileTabsViewportSizingInfo {
        public:
            TileTabsViewportSizingInfo(BrowserTabContent* browserTabContent,
                                       SpacerTabContent* spacerTabContent,
                                       const int32_t rowIndexFromTop,
                                       const int32_t columnIndex,
                                       const float initialWidth,
                                       const float initialHeight);
            
            TileTabsViewportSizingInfo& operator=(const TileTabsViewportSizingInfo& obj);

            void print(const int32_t x,
                       const int32_t y);
            
            BrowserTabContent* m_browserTabContent;
            SpacerTabContent* m_spacerTabContent;
            int32_t m_rowIndexFromTop;
            int32_t m_columnIndex;
            
            /** size with application of tile tabs configuration */
            float m_initialWidth;
            float m_initialHeight;
            
            /** size after application of lock aspect ratio */
            int32_t m_width;
            int32_t m_height;
        };
        
        BrainOpenGLViewportContent(const int32_t windowBeforeAspectLockingViewport[4],
                                   const int windowViewport[4],
                                   const int tabViewportManualLayoutBeforeAspectLocking[4],
                                   const int tabViewport[4],
                                   const int modelViewport[4],
                                   const int windowIndex,
                                   const bool highlightTabFlag,
                                   BrowserTabContent* browserTabContent,
                                   SpacerTabContent* spacerTabContent);
        
        void copyHelperBrainOpenGLViewportContent(const BrainOpenGLViewportContent& obj);

        static void createModelViewport(const int tabViewport[4],
                                        const int32_t tabIndex,
                                        const GapsAndMargins* gapsAndMargins,
                                        int modelViewportOut[4]);
        
        void updateTabLockedAspectRatios(const int32_t windowIndex,
                                         const int32_t windowViewport[4]);

        static std::vector<BrainOpenGLViewportContent*> createViewportContentForGridTileTabs(std::vector<BrowserTabContent*>& tabContents,
                                                                                             BrowserWindowContent* browserWindowContent,
                                                                                             TileTabsLayoutGridConfiguration* gridConfiguration,
                                                                                             const GapsAndMargins* gapsAndMargins,
                                                                                             const int32_t windowBeforeAspectLockingViewport[4],
                                                                                             const int32_t windowViewport[4],
                                                                                             const int32_t windowIndex,
                                                                                             const int32_t highlightTabIndex);

        static std::vector<BrainOpenGLViewportContent*> createViewportContentForManualTileTabs(std::vector<BrowserTabContent*>& tabContents,
                                                                                               BrowserWindowContent* browserWindowContent,
                                                                                               const GapsAndMargins* gapsAndMargins,
                                                                                               const int32_t windowBeforeAspectLockingViewport[4],
                                                                                               const int32_t windowViewport[4],
                                                                                               const int32_t windowIndex,
                                                                                               const int32_t highlightTabIndex);

        void getTranslationFactors(float& customViewStepValueOut,
                                   float& mousePanningFactorOut) const;
        
        const int m_windowIndex;
        
        const bool m_highlightTab;
        
        /** Tab viewport's X-coordinate */
        int m_tabX = 0;
        /** Tab viewport's Y-coordinate */
        int m_tabY = 0;
        /** Tab viewport's Width */
        int m_tabWidth = 0;
        /** Tab viewport's Height */
        int m_tabHeight = 0;
        
        /** Manual layout tab viewport before aspect locking is applied */
        int m_tabViewportManualLayoutBeforeAspectLocking[4] = { 0, 0, 0, 0 };
        
        /** Chart data viewport's X-coordinate */
        mutable int m_chartDataX = 0;
        /** Chart data viewport's Y-coordinate */
        mutable int m_chartDataY = 0;
        /** Chart data viewport's Width */
        mutable int m_chartDataWidth = 0;
        /** Chart data viewport's Height */
        mutable int m_chartDataHeight = 0;
        /** Chart data viewport's validity */
        mutable bool m_chartDataViewportValidFlag = false;
        /** Chart data transformation matrix */
        mutable Matrix4x4 m_chartDataModelViewMatrix;
        /** Chart data transformation matrix */
        mutable Matrix4x4 m_chartDataProjectionMatrix;
        
        /** Model viewport's X-coordinate */
        int m_modelX = 0;
        /** Model viewport's Y-coordinate */
        int m_modelY = 0;
        /** Model viewport's Width */
        int m_modelWidth = 0;
        /** Model viewport's Height */
        int m_modelHeight = 0;

        /** Window viewport's X-coordinate */
        int m_windowX = 0;
        /** Window viewport's Y-coordinate */
        int m_windowY = 0;
        /** Window viewport's Width */
        int m_windowWidth = 0;
        /** Window viewport's Height */
        int m_windowHeight = 0;
        
        BrowserTabContent* m_browserTabContent = NULL;
        
        SpacerTabContent* m_spacerTabContent = NULL;
        
        /** Window viewport's X-coordinate */
        int m_windowBeforeAspectLockingX = 0;
        /** Window viewport's Y-coordinate */
        int m_windowBeforeAspectLockingY = 0;
        /** Window viewport's Width */
        int m_windowBeforeAspectLockingWidth = 0;
        /** Window viewport's Height */
        int m_windowBeforeAspectLockingHeight = 0;
        
        mutable std::unique_ptr<GraphicsObjectToWindowTransform> m_histologyGraphicsObjectToWindowTransform;
        
        mutable std::unique_ptr<GraphicsObjectToWindowTransform> m_mediaGraphicsObjectToWindowTransform;
        
        mutable std::unique_ptr<GraphicsObjectToWindowTransform> m_volumeMprAxialGraphicsObjectToWindowTransform;
        
        mutable std::unique_ptr<GraphicsObjectToWindowTransform> m_volumeMprCoronalGraphicsObjectToWindowTransform;
        
        mutable std::unique_ptr<GraphicsObjectToWindowTransform> m_volumeMprParasagittalGraphicsObjectToWindowTransform;
        
    public:
        virtual AString toString() const;
    };
    
#ifdef __BRAIN_OPEN_G_L_VIEWPORT_CONTENT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_OPEN_G_L_VIEWPORT_CONTENT_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_VIEWPORT_CONTENT__H_
