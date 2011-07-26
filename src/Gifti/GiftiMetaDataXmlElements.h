
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
namespace caret {

namespace GiftiMetaDataXmlElements {

    /** metadata name for primary anatomical structure found in
 NIFTI_INTENT_POINTSET data arrays.
*/
     static const std::string METADATA_NAME_ANATOMICAL_STRUCTURE_PRIMARY = "AnatomicalStructurePrimary";

    /** metadata value for NIFTI_INTENT_POINTSET primary anatomical structure
 representing the left cerebral cortex.
*/
     static const std::string METADATA_NAME_ANATOMICAL_STRUCTURE_PRIMARY_VALUE_CORTEX_LEFT = "CortexLeft";

    /** metadata value for NIFTI_INTENT_POINTSET primary anatomical structure
 representing the right cerebral cortex.
*/
     static const std::string METADATA_NAME_ANATOMICAL_STRUCTURE_PRIMARY_VALUE_CORTEX_RIGHT = "CortexRight";

    /** metadata value for NIFTI_INTENT_POINTSET primary anatomical structure
 representing the both left and right cerebral cortex.
*/
     static const std::string METADATA_NAME_ANATOMICAL_STRUCTURE_PRIMARY_VALUE_CORTEX_BOTH = "CortexRightAndLeft";

    /** metadata value for NIFTI_INTENT_POINTSET primary anatomical structure
 representing the cerebellum.
*/
     static const std::string METADATA_NAME_ANATOMICAL_STRUCTURE_PRIMARY_VALUE_CEREBELLUM = "Cerebellum";

    /** metadata value for NIFTI_INTENT_POINTSET primary anatomical structure
 representing the head.
*/
     static const std::string METADATA_NAME_ANATOMICAL_STRUCTURE_PRIMARY_VALUE_HEAD = "Head";

    /** metadata value for NIFTI_INTENT_POINTSET primary anatomical structure
 representing the left hippocampus.
*/
     static const std::string METADATA_NAME_ANATOMICAL_STRUCTURE_PRIMARY_VALUE_HIPPOCAMPUS_LEFT = "HippocampusLeft";

    /**
 metadata value for NIFTI_INTENT_POINTSET primary anatomical structure
 representing the right hippocampus.
*/
     static const std::string METADATA_NAME_ANATOMICAL_STRUCTURE_PRIMARY_VALUE_HIPPOCAMPUS_RIGHT = "HippocampusRight";

    /** metadata name for for secondary anatomical structure found in
 NIFTI_INTENT_POINTSET data arrays.
*/
     static const std::string METADATA_NAME_ANATOMICAL_STRUCTURE_SECONDARY = "AnatomicalStructureSecondary";

    /** metadata value for NIFTI_INTENT_POINTSET secondary anatomical structure
 representing the gray and white boundary.
*/
     static const std::string METADATA_NAME_ANATOMICAL_STRUCTURE_SECONDARY_VALUE_GRAY_WHITE = "GrayWhite";

    /** metadata value for NIFTI_INTENT_POINTSET secondary anatomical structure
 representing the pial (gray/CSF boundary).
*/
     static const std::string METADATA_NAME_ANATOMICAL_STRUCTURE_SECONDARY_VALUE_PIAL = "Pia";

    /** metadata value for NIFTI_INTENT_POINTSET secondary anatomical structure
 representing the mid thickness (layer 4).
*/
     static const std::string METADATA_NAME_ANATOMICAL_STRUCTURE_SECONDARY_VALUE_ = "MidThickness";

    /** metadata name found in the file's metadata and indicates
 the date and time the file was written.
*/
     static const std::string METADATA_NAME_DATE = "Date";

    /** metadata name found in any metadata and provides a description of the
 entity's content.
*/
     static const std::string METADATA_NAME_DESCRIPTION = "Description";
    
    static const std::string METADATA_NAME_COMMENT = "Comment";

    /** metadata name for geometric type found in
 NIFTI_INTENT_POINTSET data arrays.
*/
     static const std::string METADATA_NAME_GEOMETRIC_TYPE = "GeometricType";


    /** metadata value for NIFTI_INTENT_POINTSET's Geometric Type
 Reconstruction with a "blocky" appearance.
*/
     static const std::string METADATA_NAME_GEOMETRIC_TYPE_VALUE_RECONSTRUCTION = "Reconstruction";

    /** metadata value for NIFTI_INTENT_POINTSET's Geometric Type
 representing true anatomical structure.
*/
     static const std::string METADATA_NAME_GEOMETRIC_TYPE_VALUE_ANATOMICAL = "Anatomica";

    /** metadata value for NIFTI_INTENT_POINTSET's Geometric Type
 for inflated surface.
*/
     static const std::string METADATA_NAME_GEOMETRIC_TYPE_VALUE_INFLATED = "Inflated";

    /** metadata value for NIFTI_INTENT_POINTSET's Geometric Type
 for very inflated surface.
*/
     static const std::string METADATA_NAME_GEOMETRIC_TYPE_VALUE_VERY_INFLATED = "VeryInflated";

    /** metadata value for NIFTI_INTENT_POINTSET's Geometric Type
 spherical surface.
*/
     static const std::string METADATA_NAME_GEOMETRIC_TYPE_VALUE_SPHERICAL = "SPHERICA";

    /** metadata value for NIFTI_INTENT_POINTSET's Geometric Type
 semi-spherical surface with one half flattened.
*/
     static const std::string METADATA_NAME_GEOMETRIC_TYPE_VALUE_SEMI_SPHERICAL = "SemiSpherica";

    /** metadata value for NIFTI_INTENT_POINTSET's Geometric Type
 ellipsoid surface.
*/
     static const std::string METADATA_NAME_GEOMETRIC_TYPE_VALUE_ELLIPSOID = "Ellipsoid";

    /** metadata value for NIFTI_INTENT_POINTSET's Geometric Type
 flat surface.
*/
     static const std::string METADATA_NAME_GEOMETRIC_TYPE_VALUE_FLAT = "Flat";

    /** metadata value for NIFTI_INTENT_POINTSET's Geometric Type
 hull surface (eg: wrapping around cortex with sulci filled but
 not necessarily convex).
*/
     static const std::string METADATA_NAME_GEOMETRIC_TYPE_VALUE_HULL = "Hul";

    /**metadata name for NIFTI Intent label found in functional data arrays. */
     static const std::string METADATA_NAME_INTENT_CODE = "Intent_code";

    /**metadata name for NIFTI Intent parameter one in functional data arrays.*/
     static const std::string METADATA_NAME_INTENT_P1 = "intent_p1";

    /**metadata name for NIFTI Intent parameter two in functional data arrays.*/
     static const std::string METADATA_NAME_INTENT_P2 = "intent_p2";

    /**metadata name for NIFTI Intent parameter three in functional data arrays.*/
     static const std::string METADATA_NAME_INTENT_P3 = "intent_p3";

    /**metadata name for name of data, often displayed in the user-interface. */
     static const std::string METADATA_NAME_NAME = "Name";

    /**metadata name for text that identifies a subjet. */
     static const std::string METADATA_NAME_SUBJECT_ID = "SubjectID";

    /**metadata name for text that uniquely defines a surface. */
     static const std::string METADATA_NAME_SURFACE_ID = "SurfaceID";

    /**metadata name for time step (TR) in NIFTI_INTENT_TIME_SERIES arrays. */
     static const std::string METADATA_NAME_TIME_STEP = "TimeStep";

    /**metadata name for topological type in NIFTI_INTENT_TRIANGLE arrays. */
     static const std::string METADATA_NAME_TOPOLOGICAL_TYPE = "TopologicalType";

    /** metadata value for NIFTI_INTENT_TRIANGLE's Topological Type
 for closed topology.
*/
     static const std::string METADATA_NAME_TOPOLOGICAL_TYPE_VALUE_CLOSED = "Closed";

    /** metadata value for NIFTI_INTENT_TRIANGLE's Topological Type
 for open topology (perhaps medial wall removed).
*/
     static const std::string METADATA_NAME_TOPOLOGICAL_TYPE_VALUE_OPEN = "Open";

    /** metadata value for NIFTI_INTENT_TRIANGLE's Topological Type
 for cut topology (typically used with flat surfaces with medial
 wall removed and cuts made to reduce distortion.
*/
     static const std::string METADATA_NAME_TOPOLOGICAL_TYPE_VALUE_CUT = "Cut";

    /** metadata name for a unique identifiers for the data array.  This ID
 is best generated using a Universal Unique Identifier function such
 as Java's java.util.uuid or C's uuid_generate().
 @see <a href="http://en.wikipedia.org/wiki/Uuid">UUID</a>
*/
     static const std::string METADATA_NAME_UNIQUE_ID = "UniqueID";

    /**metadata name for username of user that wrote the file in file metadata.*/
     static const std::string METADATA_NAME_USER_NAME = "UserName";

    /**name of study metadata link set metadata */
     static const std::string METADATA_NAME_STUDY_METADATA_LINK_SET = "StudyMetaDataLinkSet";

    /**name of palette color mapping stored in metadata */
     static const std::string METADATA_NAME_PALETTE_COLOR_MAPPING = "PaletteColorMapping";

} // namespace

} // namespace
