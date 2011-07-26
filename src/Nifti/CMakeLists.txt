#
# The NIFTI Project
#
project (Nifti)


#
# Create the NIFTI library
#
ADD_LIBRARY(Nifti 
Matrix4x4.h
NiftiDataType.h
NiftiException.h
NiftiIntent.h
NiftiSpacingUnits.h
NiftiTimeUnits.h
NiftiTransform.h
NiftiUtilities.h
NiftiVersion.h
PaletteColorMapping.h
PaletteScaleMode.h
PaletteThresholdTest.h
PaletteThresholdType.h


Matrix4x4.cxx
NiftiDataType.cxx
NiftiException.cxx
NiftiIntent.cxx
NiftiSpacingUnits.cxx
NiftiTimeUnits.cxx
NiftiTransform.cxx
NiftiUtilities.cxx
NiftiVersion.cxx
PaletteColorMapping.cxx
PaletteScaleMode.cxx
PaletteThresholdTest.cxx
PaletteThresholdType.cxx
)

#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Common
${CMAKE_SOURCE_DIR}/Xml
)
