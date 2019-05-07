#ifndef __ALGORITHM_ANNOTATION_RESAMPLE_H__
#define __ALGORITHM_ANNOTATION_RESAMPLE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#include "AbstractAlgorithm.h"

namespace caret {

    class Annotation;
    
    class AlgorithmAnnotationResample : public AbstractAlgorithm {

    private:
        AlgorithmAnnotationResample(); 

    protected:
        static float getSubAlgorithmWeight();

        static float getAlgorithmInternalWeight();

    public:
        AlgorithmAnnotationResample(ProgressObject* myProgObj,
                                    AnnotationFile* annotationFile,
                                    const AString& annotationFileName,
                                    const std::vector<const SurfaceFile*>& sourceSurfaces,
                                    const std::vector<const SurfaceFile*>& targetSurfaces);

        static OperationParameters* getParameters();

        static void useParameters(OperationParameters* myParams, 
                                  ProgressObject* myProgObj);

        static AString getCommandSwitch();

        static AString getShortDescription();

    private:
        class SourceTargetSurface {
        public:
            SourceTargetSurface(const SurfaceFile* source)
            : m_source(source),
            m_target(NULL) { }
            
            const SurfaceFile* m_source;
            const SurfaceFile* m_target;
        };
        
        std::map<StructureEnum::Enum, SourceTargetSurface*> m_surfaces;
        
        void setupSurfaces(const std::vector<const SurfaceFile*>& sourceSurfaces,
                           const std::vector<const SurfaceFile*>& targetSurfaces);
        
        void resampleAnnotation(Annotation* ann);
    };

    typedef TemplateAutoOperation<AlgorithmAnnotationResample> AutoAlgorithmAnnotationResample;

} // namespace

#endif  //__ALGORITHM_ANNOTATION_RESAMPLE_H__

