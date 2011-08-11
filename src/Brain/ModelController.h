#ifndef __MODEL_CONTROLLER_H__
#define __MODEL_CONTROLLER_H__

/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 


#include "CaretObject.h"

#include "CaretWindowEnum.h"
#include "Matrix4x4.h"

namespace caret {

    class Brain;
    
    /**
     * Controls some type of object.
     */
    class ModelController : public CaretObject {
        
    protected:
        ModelController(Brain* brain,
               const bool allowsYokingFlag,
               const bool allowsRotationFlag);
        
        virtual ~ModelController();
        
        
    private:        
        ModelController(const ModelController& o);
        ModelController& operator=(const ModelController& o);
        
        void initializeMembersModelController();
        
    public:
        Brain* getBrain() const;
        
        virtual AString getNameForGUI(const bool includeStructureFlag) const = 0;
        
        bool isRotationAllowed() const;
        
        bool isYokeable() const;
        
        void copyTransformations(const ModelController& controller,
                                 const int32_t windowIndexSource,
                                 const int32_t windowIndexTarget);
        
        Matrix4x4* getViewingRotationMatrix(const int32_t windowIndex) const;
        
        const float* getTranslation(const int32_t windowIndex) const;
        
        void setTranslation(const int32_t windowIndex,
                            const float t[]);
        
        void setTranslation(const int32_t windowIndex,
                            const float tx,
                            const float ty,
                            const float tz);
        
        float getScaling(const int32_t windowIndex) const;
        
        void setScaling(const int32_t windowIndex,
                        const float s);
        
        void resetView(const int32_t windowIndex);
        
        void rightView(const int32_t windowIndex);
        
        void leftView(const int32_t windowIndex);
        
        void anteriorView(const int32_t windowIndex);
        
        void posteriorView(const int32_t windowIndex);
        
        void dorsalView(const int32_t windowIndex);
        
        void ventralView(const int32_t windowIndex);
        
        void setTransformation(
                               const int32_t windowIndex,
                               const std::vector<float>& transformationData);
        
        virtual AString toString() const;
        
        virtual AString toDescriptiveString() const;
        
    private:
        void initializeTransformations();
        
    protected:
        float defaultModelScaling;
        
        /**Brain to which this controller belongs. */
        Brain* brain;
        
        /**the viewing rotation matrix. */
        std::vector<Matrix4x4*> viewingRotationMatrix;
        
        /**translation. */
        std::vector<float> translation;;
        
        /**scaling. */
        std::vector<float> scaling;
        
    private:
        bool allowsYokingFlag;
        
        bool allowsRotationFlag;
        
    };

} // namespace

#endif // __MODEL_CONTROLLER_H__
