#ifndef __VIEWING_TRANSFORMATIONS_H__
#define __VIEWING_TRANSFORMATIONS_H__

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

#include <memory>

#include "CaretObject.h"
#include "Matrix4x4.h"
#include "SceneableInterface.h"

namespace caret {
    class BoundingBox;
    class BrowserTabContent;
    class CaretUndoStack;
    class GraphicsObjectToWindowTransform;
    class SceneClassAssistant;
    class Vector3D;

    class ViewingTransformations : public CaretObject, public SceneableInterface  {
        
    public:
        ViewingTransformations();
        
        virtual ~ViewingTransformations();
        
        ViewingTransformations(const ViewingTransformations& obj);

        ViewingTransformations& operator=(const ViewingTransformations& obj);
        
        void copyFromOther(const ViewingTransformations& viewingTransform);
        
        const float* getTranslation() const;
        
        void getTranslation(float translationOut[3]) const;
        
        void setTranslation( const float translation[3]);
        
        void setTranslation(const float translationX,
                            const float translationY,
                            const float translationZ);
        
        float getScaling() const;
        
        void setScaling(const float scaling);
        
        virtual void scaleAboutMouse(const GraphicsObjectToWindowTransform* transform,
                                     const int32_t mousePressX,
                                     const int32_t mousePressY,
                                     const float mouseDY,
                                     const float dataX,
                                     const float dataY,
                                     const bool dataXYValidFlag);

        virtual void setHistologyScaling(const GraphicsObjectToWindowTransform* transform,
                                         const float scaling);
        
        virtual void setMediaScaling(const GraphicsObjectToWindowTransform* transform,
                                     const float scaling);
        
        Matrix4x4 getRotationMatrix() const;
        
        void setRotationMatrix(const Matrix4x4& rotationMatrix);
        
        void getRightCortexFlatMapOffset(float& rightCortexFlatMapOffsetX,
                                         float& rightCortexFlatMapOffsetY) const;
        
        void setRightCortexFlatMapOffset(const float rightCortexFlatMapOffsetX,
                                         const float rightCortexFlatMapOffsetY);

        Matrix4x4 getFlatRotationMatrix() const;
        
        void setFlatRotationMatrix(const Matrix4x4& flatRotationMatrix);
        
        float getRightCortexFlatMapZoomFactor() const;
        
        void setRightCortexFlatMapZoomFactor(const float rightCortexFlatMapZoomFactor);
        
        virtual void resetView();
        
        virtual void rightView();
        
        virtual void leftView();
        
        virtual void anteriorView();
        
        virtual void posteriorView();
        
        virtual void dorsalView();
        
        virtual void ventralView();
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;

        CaretUndoStack* getRedoUndoStack();

    protected:
        /** Rotation matrix. */
        Matrix4x4* m_rotationMatrix;
        
        /** Translation */
        float m_translation[3];
        
        /** Scaling. */
        float m_scaling;
        
        /** Rotation matrix for flat surface */
        Matrix4x4* m_flatRotationMatrix;
        
        /** Offset for right cortex flat map */
        float m_rightCortexFlatMapOffset[2];
        
        float m_rightCortexFlatMapZoomFactor;
        
    private:
        void copyHelperViewingTransformations(const ViewingTransformations& obj);

        SceneClassAssistant* m_sceneAssistant;
        
        std::unique_ptr<CaretUndoStack> m_undoStack;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __VIEWING_TRANSFORMATIONS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VIEWING_TRANSFORMATIONS_DECLARE__

} // namespace
#endif  //__VIEWING_TRANSFORMATIONS_H__
