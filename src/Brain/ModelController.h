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

#include "CaretWindow.h"
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
        
        virtual std::string getNameForGUI(const bool includeStructureFlag) const = 0;
        
        bool isRotationAllowed() const;
        
        bool isYokeable() const;
        
        void copyTransformations(
                                 const ModelController& controller,
                                 const CaretWindow& windowSourceID,
                                 const CaretWindow& windowTargetID);
        
        Matrix4x4* getViewingRotationMatrix(const CaretWindow& windowID) const;
        
        const float* getTranslation(const CaretWindow& windowID) const;
        
        void setTranslation(
                            const CaretWindow& windowID,
                            const float t[]);
        
        void setTranslation(
                            const CaretWindow& windowID,
                            const float tx,
                            const float ty,
                            const float tz);
        
        float getScaling(const CaretWindow& windowID) const;
        
        void setScaling(
                        const CaretWindow& windowID,
                        const float s);
        
        void resetView(const CaretWindow& windowID);
        
        void rightView(const CaretWindow& windowID);
        
        void leftView(const CaretWindow& windowID);
        
        void anteriorView(const CaretWindow& windowID);
        
        void posteriorView(const CaretWindow& windowID);
        
        void dorsalView(const CaretWindow& windowID);
        
        void ventralView(const CaretWindow& windowID);
        
        void setTransformation(
                               const CaretWindow& windowID,
                               const std::vector<float>& transformationData);
        
        virtual std::string toString() const;
        
        virtual std::string toDescriptiveString() const;
        
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
