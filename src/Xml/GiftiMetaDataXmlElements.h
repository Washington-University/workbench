
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
namespace caret {

namespace GiftiMetaDataXmlElements {

    /** metadata name for primary anatomical structure found in
 NIFTI_INTENT_POINTSET data arrays.
*/
     static const AString METADATA_NAME_ANATOMICAL_STRUCTURE_PRIMARY = "AnatomicalStructurePrimary";

    /** metadata value for NIFTI_INTENT_POINTSET primary anatomical structure
 representing the left cerebral cortex.
*/
     static const AString METADATA_NAME_ANATOMICAL_STRUCTURE_PRIMARY_VALUE_CORTEX_LEFT = "CortexLeft";

    /** metadata value for NIFTI_INTENT_POINTSET primary anatomical structure
 representing the right cerebral cortex.
*/
     static const AString METADATA_NAME_ANATOMICAL_STRUCTURE_PRIMARY_VALUE_CORTEX_RIGHT = "CortexRight";

    /** metadata value for NIFTI_INTENT_POINTSET primary anatomical structure
 representing the both left and right cerebral cortex.
*/
     static const AString METADATA_NAME_ANATOMICAL_STRUCTURE_PRIMARY_VALUE_CORTEX_BOTH = "CortexRightAndLeft";

    /** metadata value for NIFTI_INTENT_POINTSET primary anatomical structure
 representing the cerebellum.
*/
     static const AString METADATA_NAME_ANATOMICAL_STRUCTURE_PRIMARY_VALUE_CEREBELLUM = "Cerebellum";

    /** metadata value for NIFTI_INTENT_POINTSET primary anatomical structure
 representing the head.
*/
     static const AString METADATA_NAME_ANATOMICAL_STRUCTURE_PRIMARY_VALUE_HEAD = "Head";

    /** metadata value for NIFTI_INTENT_POINTSET primary anatomical structure
 representing the left hippocampus.
*/
     static const AString METADATA_NAME_ANATOMICAL_STRUCTURE_PRIMARY_VALUE_HIPPOCAMPUS_LEFT = "HippocampusLeft";

    /**
 metadata value for NIFTI_INTENT_POINTSET primary anatomical structure
 representing the right hippocampus.
*/
     static const AString METADATA_NAME_ANATOMICAL_STRUCTURE_PRIMARY_VALUE_HIPPOCAMPUS_RIGHT = "HippocampusRight";

    /** metadata name for for secondary anatomical structure found in
 NIFTI_INTENT_POINTSET data arrays.
*/
     static const AString METADATA_NAME_ANATOMICAL_STRUCTURE_SECONDARY = "AnatomicalStructureSecondary";

    /** metadata value for NIFTI_INTENT_POINTSET secondary anatomical structure
 representing the gray and white boundary.
*/
     static const AString METADATA_NAME_ANATOMICAL_STRUCTURE_SECONDARY_VALUE_GRAY_WHITE = "GrayWhite";

    /** metadata value for NIFTI_INTENT_POINTSET secondary anatomical structure
 representing the pial (gray/CSF boundary).
*/
     static const AString METADATA_NAME_ANATOMICAL_STRUCTURE_SECONDARY_VALUE_PIAL = "Pial";

    /** metadata value for NIFTI_INTENT_POINTSET secondary anatomical structure
 representing the mid thickness (layer 4).
*/
     static const AString METADATA_NAME_ANATOMICAL_STRUCTURE_SECONDARY_VALUE_ = "MidThickness";

    /** metadata name found in the file's metadata and indicates
 the date and time the file was written.
*/
     static const AString METADATA_NAME_DATE = "Date";

    /** metadata name found in any metadata and provides a description of the
 entity's content.
*/
     static const AString METADATA_NAME_DESCRIPTION = "Description";
    
    static const AString METADATA_NAME_COMMENT = "Comment";

    /** metadata name for geometric type found in
 NIFTI_INTENT_POINTSET data arrays.
*/
     static const AString METADATA_NAME_GEOMETRIC_TYPE = "GeometricType";


    /** metadata value for NIFTI_INTENT_POINTSET's Geometric Type
 Reconstruction with a "blocky" appearance.
*/
     static const AString METADATA_NAME_GEOMETRIC_TYPE_VALUE_RECONSTRUCTION = "Reconstruction";

    /** metadata value for NIFTI_INTENT_POINTSET's Geometric Type
 representing true anatomical structure.
*/
     static const AString METADATA_NAME_GEOMETRIC_TYPE_VALUE_ANATOMICAL = "Anatomical";

    /** metadata value for NIFTI_INTENT_POINTSET's Geometric Type
 for inflated surface.
*/
     static const AString METADATA_NAME_GEOMETRIC_TYPE_VALUE_INFLATED = "Inflated";

    /** metadata value for NIFTI_INTENT_POINTSET's Geometric Type
 for very inflated surface.
*/
     static const AString METADATA_NAME_GEOMETRIC_TYPE_VALUE_VERY_INFLATED = "VeryInflated";

    /** metadata value for NIFTI_INTENT_POINTSET's Geometric Type
 spherical surface.
*/
     static const AString METADATA_NAME_GEOMETRIC_TYPE_VALUE_SPHERICAL = "Spherical";

    /** metadata value for NIFTI_INTENT_POINTSET's Geometric Type
 semi-spherical surface with one half flattened.
*/
     static const AString METADATA_NAME_GEOMETRIC_TYPE_VALUE_SEMI_SPHERICAL = "SemiSpherical";

    /** metadata value for NIFTI_INTENT_POINTSET's Geometric Type
 ellipsoid surface.
*/
     static const AString METADATA_NAME_GEOMETRIC_TYPE_VALUE_ELLIPSOID = "Ellipsoid";

    /** metadata value for NIFTI_INTENT_POINTSET's Geometric Type
 flat surface.
*/
     static const AString METADATA_NAME_GEOMETRIC_TYPE_VALUE_FLAT = "Flat";

    /** metadata value for NIFTI_INTENT_POINTSET's Geometric Type
 hull surface (eg: wrapping around cortex with sulci filled but
 not necessarily convex).
*/
     static const AString METADATA_NAME_GEOMETRIC_TYPE_VALUE_HULL = "Hull";

    /**metadata name for NIFTI Intent label found in functional data arrays. */
     static const AString METADATA_NAME_INTENT_CODE = "Intent_code";

    /**metadata name for NIFTI Intent parameter one in functional data arrays.*/
     static const AString METADATA_NAME_INTENT_P1 = "intent_p1";

    /**metadata name for NIFTI Intent parameter two in functional data arrays.*/
     static const AString METADATA_NAME_INTENT_P2 = "intent_p2";

    /**metadata name for NIFTI Intent parameter three in functional data arrays.*/
     static const AString METADATA_NAME_INTENT_P3 = "intent_p3";

    /**metadata name for name of data, often displayed in the user-interface. */
     static const AString METADATA_NAME_NAME = "Name";

    /**metadata name for text that identifies a subjet. */
     static const AString METADATA_NAME_SUBJECT_ID = "SubjectID";

    /**metadata name for text that uniquely defines a surface. */
     static const AString METADATA_NAME_SURFACE_ID = "SurfaceID";

    /**metadata name for time step (TR) in NIFTI_INTENT_TIME_SERIES arrays. */
     static const AString METADATA_NAME_TIME_STEP = "TimeStep";

    /**metadata name for topological type in NIFTI_INTENT_TRIANGLE arrays. */
     static const AString METADATA_NAME_TOPOLOGICAL_TYPE = "TopologicalType";

    /** metadata value for NIFTI_INTENT_TRIANGLE's Topological Type
 for closed topology.
*/
     static const AString METADATA_NAME_TOPOLOGICAL_TYPE_VALUE_CLOSED = "Closed";

    /** metadata value for NIFTI_INTENT_TRIANGLE's Topological Type
 for open topology (perhaps medial wall removed).
*/
     static const AString METADATA_NAME_TOPOLOGICAL_TYPE_VALUE_OPEN = "Open";

    /** metadata value for NIFTI_INTENT_TRIANGLE's Topological Type
 for cut topology (typically used with flat surfaces with medial
 wall removed and cuts made to reduce distortion.
*/
     static const AString METADATA_NAME_TOPOLOGICAL_TYPE_VALUE_CUT = "Cut";

    /** metadata name for a unique identifiers for the data array.  This ID
 is best generated using a Universal Unique Identifier function such
 as Java's java.util.uuid or C's uuid_generate().
 @see <a href="http://en.wikipedia.org/wiki/Uuid">UUID</a>
*/
     static const AString METADATA_NAME_UNIQUE_ID = "UniqueID";

    /**metadata name for username of user that wrote the file in file metadata.*/
     static const AString METADATA_NAME_USER_NAME = "UserName";

    /**name of study metadata link set metadata */
     static const AString METADATA_NAME_STUDY_METADATA_LINK_SET = "StudyMetaDataLinkSet";

    /**name of palette color mapping stored in metadata */
     static const AString METADATA_NAME_PALETTE_COLOR_MAPPING = "PaletteColorMapping";
    
    /** name of palette normalization mode */
    static const AString METADATA_PALETTE_NORMALIZATION_MODE = "PaletteNormalizationMode";
    
    /** name of histogram number of buckets */
    static const AString HISTOGRAM_NUMBER_OF_BUCKETS = "HistogramNumberOfBuckets";
    
    static const AString SAMPLES_ALT_SHORTHAND_ID = "Alt_Shorthand_id";
    
    static const AString SAMPLES_ALT_ATLAS_DESCRIPTION = "Alt_Atlas_description";
    
    static const AString SAMPLES_CASE_ID = "Case_id";
    
    static const AString SAMPLES_DING_DESCRIPTION = "Ding_description";
    
    static const AString SAMPLES_DISSECTION_DATE = "Dissection_date";
    
    static const AString SAMPLES_DONOR_ID = "Donor_id";
    
    static const AString SAMPLES_HEMISPHERE = "Hemisphere";
    
    static const AString SAMPLES_LOCATION_ID = "Location";
    
    static const AString SAMPLES_SAMPLE_ID = "Sample_id";

    static const AString SAMPLES_SAMPLE_SLAB_FACE = "Slab_face";
    
    static const AString SAMPLES_SAMPLE_TYPE = "Sample_type";

    static const AString SAMPLES_SHORTHAND_ID = "Shorthand_id";
    
    static const AString SAMPLES_SLAB_ID = "Slab_id";
    
    /** Used for converting a QDate to a string */
    const AString METADATA_QT_DATE_FORMAT = "dd MMM yyyy";


} // namespace

} // namespace
