#
# The NIFTI Project
#
project (Nifti)


#
# Create the NIFTI library
#
ADD_LIBRARY(Nifti 
Matrix4x4.h
NiftiDataTypeEnum.h
NiftiException.h
NiftiIntentEnum.h
NiftiSpacingUnitsEnum.h
NiftiTimeUnitsEnum.h
NiftiTransformEnum.h
NiftiUtilities.h
NiftiVersionEnum.h
PaletteColorMapping.h
PaletteScaleModeEnum.h
PaletteThresholdTestEnum.h
PaletteThresholdTypeEnum.h


Matrix4x4.cxx
NiftiDataTypeEnum.cxx
NiftiException.cxx
NiftiIntentEnum.cxx
NiftiSpacingUnitsEnum.cxx
NiftiTimeUnitsEnum.cxx
NiftiTransformEnum.cxx
NiftiUtilities.cxx
NiftiVersionEnum.cxx
PaletteColorMapping.cxx
PaletteScaleModeEnum.cxx
PaletteThresholdTestEnum.cxx
PaletteThresholdTypeEnum.cxx
)

#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Common
${CMAKE_SOURCE_DIR}/Xml
)
