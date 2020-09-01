#ifndef __MATRIX4X4_H__
#define __MATRIX4X4_H__

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


#include "CaretObject.h"
#include "Matrix4x4Interface.h"
#include <stdint.h>

#include <AString.h>

namespace caret {

    class ControlPoint3D;
    class XmlWriter;

/**
 * A 4x4 homogeneous transformation matrix.
 */
class Matrix4x4 : public Matrix4x4Interface, public CaretObject {

public:
    Matrix4x4();

public:
    Matrix4x4(const Matrix4x4& o);

    Matrix4x4& operator=(const Matrix4x4& o);

    virtual ~Matrix4x4();

private:
    void copyHelper(const Matrix4x4& o);

    void initializeMembersMatrix4x4();
    
public:
    void identity();

    void getTranslation(float translatationOut[3]) const;

    void setTranslation(const float t[]);

    void setTranslation(
            const double tx,
            const double ty,
            const double tz);

    void translate(
            const double tx,
            const double ty,
            const double tz);

    void translate(const double txyz[3]);
    
    void scale(
            const double sx,
            const double sy,
            const double sz);

    void getScale(double& scaleOutX,
                  double& scaleOutY,
                  double& scaleOutZ) const;
    
    void rotateX(const double degrees);

    void rotateY(const double degrees);

    void rotateZ(const double degrees);

    void rotate(
            const double angle,
            const double vector[]);

    void rotate(
            const double angle,
            const double x,
            const double y,
            const double z);

    void getRotation(double& rotationOutX,
                     double& rotationOutY,
                     double& rotationOutZ) const;
    
    void setRotation(const double rotationX,
                     const double rotationY,
                     const double rotationZ);
    
    void premultiply(const Matrix4x4& tm);

    void postmultiply(const Matrix4x4& tm);

    void getMatrixForOpenGL(double m[16]) const;

    void setMatrixFromOpenGL(const double m[16]);

    void getMatrixForOpenGL(float m[16]) const;
    
    void setMatrixFromOpenGL(const float m[16]);
    
    void setMatrixToOpenGLRotationFromVector(const float vector[3]);
    
    void transpose();

    void setMatrix(const double m[4][4]);

    void getMatrix(double m[4][4]) const;

    void setMatrix(const float m[4][4]);
    
    void getMatrix(float m[4][4]) const;
    
    void setMatrix(const Matrix4x4& cm);

    void multiplyPoint4(float p[4]) const;

    virtual void multiplyPoint3(float p[3]) const override;
    
    void multiplyPoint3(double p[3]) const;
    
    void multiplyPoint3X3(float p[3]) const;

    AString getDataSpaceName() const;

    void setDataSpaceName(const AString& name);

    AString getTransformedSpaceName() const;

    void setTransformedSpaceName(const AString& name);

    double getMatrixElement(
            const int32_t i,
            const int32_t j) const;

    void setMatrixElement(
            const int32_t i,
            const int32_t j,
            const double e);

    bool compare(
            const Matrix4x4& m,
            const float error);

    bool invert();

    bool createLandmarkTransformMatrix(const std::vector<ControlPoint3D*>& controlPoints,
                                       AString& errorMessageOut);
    
    AString toString() const;

    AString toFormattedString(const AString& indentation) const;

    void setModified();

    void clearModified();

    bool isModified() const;

    void writeAsGiftiXML(XmlWriter& xmlWriter,
                         const AString& xmlMatrixTag,
                         const AString& xmlDataSpaceTag,
                         const AString& xmlTransformedSpaceTag,
                         const AString& xmlMatrixDataTag);
    
private:
    enum LANDMARK_TRANSFORM_MODE {
        LANDMARK_TRANSFORM_AFFINE,
        LANDMARK_TRANSFORM_RIGIDBODY,
        LANDMARK_TRANSFORM_SIMILARITY
    };
    
    double fixZero(const double f);

    void fixNumericalError();

    void Transpose(const double input[4][4], double output[4][4]) const;

    bool Inverse(const double inputMatrix[4][4], double outputMatrix[4][4]) const;

    void Adjoint(const double inputMatrix[4][4], double outputMatrix[4][4]) const;

    void UpperTriangle4(const double inputMatrix[4][4], double outputMatrix[4][4]) const;

    void UpperTriangle3(const double inputMatrix[3][3], double outputMatrix[3][3]) const;
    
    double Determinant4x4(const double matrix[4][4]) const;

    static double Determinant3x3(double A[3][3]);
    
    double Determinant3(const double matrix[3][3]) const;
    
    static double Determinant3x3(double a1, double a2, double a3,
                              double b1, double b2, double b3,
                                 double c1, double c2, double c3);
    
    static double Determinant2x2(double a, double b, double c, double d);

    static void Orthogonalize3x3(const double A[3][3], double B[3][3]);

    static void SwapVectors3(double v1[3],
                             double v2[3]);

    static void SingularValueDecomposition3x3(const double A[3][3],
                                              double U[3][3],
                                              double w[3],
                                              double VT[3][3]);
    
    static void Transpose3x3(const double A[3][3], double AT[3][3]);

    static void Multiply3x3(const double A[3][3], const double B[3][3],
                            double C[3][3]);

    static void Diagonalize3x3(const double A[3][3], double w[3], double V[3][3]);

    static int JacobiN(double **a, int n, double *w, double **v);

    bool createLandmarkTransformMatrixPrivate(const std::vector<ControlPoint3D*>& controlPoints,
                                              const LANDMARK_TRANSFORM_MODE mode,
                                              AString& errorMessageOut);
    
    float measureTransformError(const std::vector<ControlPoint3D*>& controlPoints,
                                const Matrix4x4& matrix) const;

protected:
    /**the 4x4 matrix */
    double matrix[4][4];

    /**data space name (used by GIFTI) */
    AString dataSpaceName;

    /**transformed space name (used by GIFTI) */
    AString transformedSpaceName;

private:
    /**data modification status (DO NOT CLONE) */
    bool modifiedFlag;

};

} // namespace

#endif // __MATRIX4X4_H__
