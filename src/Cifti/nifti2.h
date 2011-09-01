#include "nifti1.h"
/*! \struct nifti_2_header
    \brief Data structure defining the fields in the nifti2 header.
           This binary header should be found at the beginning of a valid
           NIFTI-2 header file.
 */
#ifndef __NIFTI2_HEADER
#define __NIFTI2_HEADER
/*=================*/
#ifdef  __cplusplus
extern "C" {
#endif
/*=================*/
/*extended nifti intent codes*/
#define NIFTI_INTENT_CONNECTIVITY_DENSE 3001
#define NIFTI_INTENT_CONNECTIVITY_DENSE_TIME 3002
#define NIFTI_INTENT_CONNECTIVITY_PARCELLATED 3003
#define NIFTI_INTENT_CONNECTIVITY_PARCELLATED_TIME 3004
#define NIFTI_INTENT_CONNECTIVITY_CONNECTIVITY_TRAJECTORY 3005
//#define NIFTI_INTENT_COMPRESSED_SPARSE_CRS 3006
//#define NIFTI_INTENT_COMPRESSED_GZIP_ROWS 300
#define NIFTI_ECODE_CIFTI 32
#define NIFTI2_HEADER_SIZE 540

#define NIFTI2_VERSION(h) \
   (h).sizeof_hdr == 348 ? 1 : \
   (h).sizeof_hdr == 1543569408? 1 : \
   (h).sizeof_hdr == 540 ? 2 : \
   (h).sizeof_hdr == 469893120 ? 2 : 0
   
#define NIFTI2_NEEDS_SWAP(h) \
   (h).sizeof_hdr == 469893120 ? 1 : \
   (h).sizeof_hdr == 1543569408 ? 1 : 0
   
/*#define NIFTI_VERSION(h)                               \
 ( ( (h).magic[0]=='n' && (h).magic[3]=='\0'    &&     \
     ( (h).magic[1]=='i' || (h).magic[1]=='+' ) &&     \
     ( (h).magic[2]>='1' && (h).magic[2]<='9' )   )    \
? (h).magic[2]-'0' : 0 )*/

                        /*************************/  /************************/ /************/
struct nifti_2_header { /* NIFTI-2 usage         */  /* NIFTI-1 usage        */ /*  offset  */
                        /*************************/  /************************/ /************/
int   sizeof_hdr;     /*!< MUST be 540           */  /* int sizeof_hdr; (348) */  /*   0 */
char  magic[8] ;      /*!< MUST be valid signature. */  /* char magic[4];     */  /*   4 */
short datatype;       /*!< Defines data type!    */  /* short datatype;       */  /*  12 */
short bitpix;         /*!< Number bits/voxel.    */  /* short bitpix;         */  /*  14 */
long long dim[8];       /*!< Data array dimensions.*/  /* short dim[8];         */  /*  16 */
double intent_p1 ;    /*!< 1st intent parameter. */  /* float intent_p1;      */  /*  80 */
double intent_p2 ;    /*!< 2nd intent parameter. */  /* float intent_p2;      */  /*  88 */
double intent_p3 ;    /*!< 3rd intent parameter. */  /* float intent_p3;      */  /*  96 */
double pixdim[8];     /*!< Grid spacings.        */  /* float pixdim[8];      */  /* 104 */
long long vox_offset;   /*!< Offset into .nii file */  /* float vox_offset;     */  /* 168 */
double scl_slope ;    /*!< Data scaling: slope.  */  /* float scl_slope;      */  /* 176 */
double scl_inter ;    /*!< Data scaling: offset. */  /* float scl_inter;      */  /* 184 */
double cal_max;       /*!< Max display intensity */  /* float cal_max;        */  /* 192 */
double cal_min;       /*!< Min display intensity */  /* float cal_min;        */  /* 200 */
double slice_duration;/*!< Time for 1 slice.     */  /* float slice_duration; */  /* 208 */
double toffset;       /*!< Time axis shift.      */  /* float toffset;        */  /* 216 */
long long slice_start;  /*!< First slice index.    */  /* short slice_start;    */  /* 224 */
long long slice_end;    /*!< Last slice index.     */  /* short slice_end;      */  /* 232 */
char  descrip[80];    /*!< any text you like.    */  /* char descrip[80];     */  /* 240 */
char  aux_file[24];   /*!< auxiliary filename.   */  /* char aux_file[24];    */  /* 320 */
int qform_code ;      /*!< NIFTI_XFORM_* code.   */ /* short qform_code;      */  /* 344 */
int sform_code ;      /*!< NIFTI_XFORM_* code.   */ /* short sform_code;      */  /* 348 */
double quatern_b ;    /*!< Quaternion b param.   */ /* float quatern_b;       */  /* 352 */
double quatern_c ;    /*!< Quaternion c param.   */ /* float quatern_c;       */  /* 360 */
double quatern_d ;    /*!< Quaternion d param.   */ /* float quatern_d;       */  /* 368 */
double qoffset_x ;    /*!< Quaternion x shift.   */ /* float qoffset_x;       */  /* 376 */
double qoffset_y ;    /*!< Quaternion y shift.   */ /* float qoffset_y;       */  /* 384 */
double qoffset_z ;    /*!< Quaternion z shift.   */ /* float qoffset_z;       */  /* 392 */
double srow_x[4] ;    /*!< 1st row affine transform. */  /* float srow_x[4];  */  /* 400 */
double srow_y[4] ;    /*!< 2nd row affine transform. */  /* float srow_y[4];  */  /* 432 */
double srow_z[4] ;    /*!< 3rd row affine transform. */  /* float srow_z[4];  */  /* 464 */
int slice_code ;      /*!< Slice timing order.   */  /* char slice_code;      */  /* 496 */
int xyzt_units ;      /*!< Units of pixdim[1..4] */  /* char xyzt_units;      */  /* 500 */
int intent_code ;     /*!< NIFTI_INTENT_* code.  */  /* short intent_code;    */  /* 504 */
char intent_name[16]; /*!< 'name' or meaning of data. */ /* char intent_name[16]; */  /* 508 */
char dim_info;        /*!< MRI slice ordering.   */      /* char dim_info;        */  /* 524 */
char unused_str[15];  /*!< unused, filled with \0 */                                  /* 525 */
} ;                   /**** 540 bytes total ****/
typedef struct nifti_2_header nifti_2_header ;

/*=================*/
#ifdef  __cplusplus
}
#endif
/*=================*/
#endif //__NIFTI2_HEADER
