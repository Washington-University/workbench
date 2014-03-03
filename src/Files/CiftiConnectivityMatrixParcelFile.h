#ifndef __CIFTI_CONNECTIVITY_MATRIX_PARCEL_FILE_H__
#define __CIFTI_CONNECTIVITY_MATRIX_PARCEL_FILE_H__

/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#include "ChartableInterfaceMatrix.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"

namespace caret {

    class ChartDataMatrix;
    class PaletteFile;
    
    class CiftiConnectivityMatrixParcelFile : public CiftiMappableConnectivityMatrixDataFile, public ChartableInterfaceMatrix  /*, public ChartableInterface*/ {
        
    public:
        CiftiConnectivityMatrixParcelFile();
        
        virtual ~CiftiConnectivityMatrixParcelFile();
        
    private:
        CiftiConnectivityMatrixParcelFile(const CiftiConnectivityMatrixParcelFile&);

        CiftiConnectivityMatrixParcelFile& operator=(const CiftiConnectivityMatrixParcelFile&);

        virtual ChartDataMatrix* getMatrixChart(const PaletteFile* paletteFile);
        
//        virtual CaretMappableDataFile* getCaretMappableDataFile() ;
//
//        virtual const CaretMappableDataFile* getCaretMappableDataFile() const;
//
//        virtual bool isChartingEnabled() const;
//
//        virtual bool isChartingSupported() const;
//
//        virtual void setChartingEnabled(const bool enabled);
//
//        virtual ChartTypeEnum::Enum getDefaultChartType() const;
//
//        virtual void getSupportedChartTypes(std::vector<ChartTypeEnum::Enum> &list) const;
//
//        virtual bool loadAverageChartForSurfaceNodes(const StructureEnum::Enum /*structure*/,
//                                                     const std::vector<int32_t>& /*nodeIndices*/,
//                                                     TimeLine& /*timeLineOut*/) throw (DataFileException) {this->isChartingEnabled();return false;};
//        
//        
//         
//        virtual bool loadChartForSurfaceNode(const StructureEnum::Enum /*structure*/,
//                                             const int32_t /*nodeIndex*/,
//                                             TimeLine& /*timeLineOut*/) throw (DataFileException) {return false;};
//        
//       
//        virtual bool loadChartForVoxelAtCoordinate(const float* /* xyz[3]*/,
//            TimeLine& /*timeLineOut*/) throw (DataFileException)  {return false;};

        
    public:

        // ADD_NEW_METHODS_HERE

    private:

        // ADD_NEW_MEMBERS_HERE

        bool m_chartingEnabled;

    };
    
#ifdef __CIFTI_CONNECTIVITY_MATRIX_PARCEL_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CIFTI_CONNECTIVITY_MATRIX_PARCEL_FILE_DECLARE__

} // namespace
#endif  //__CIFTI_CONNECTIVITY_MATRIX_PARCEL_FILE_H__
