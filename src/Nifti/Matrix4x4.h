#ifndef __MATRIX4X4_H__
#define __MATRIX4X4_H__

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
#include "XmlException.h"
#include <stdint.h>

#include <AString.h>

namespace caret {

class XmlWriter;

/**
 * A 4x4 homogeneous transformation matrix.
 */
class Matrix4x4 : public CaretObject {

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

    void scale(
            const double sx,
            const double sy,
            const double sz);

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

    void premultiply(const Matrix4x4& tm);

    void postmultiply(const Matrix4x4& tm);

    void getMatrixForOpenGL(double m[16]) const;

    void setMatrixFromOpenGL(const double m[16]);

    void transpose();

    void setMatrix(const double m[4][4]);

    void getMatrix(double m[4][4]) const;

    void setMatrix(const Matrix4x4& cm);

    void multiplyPoint4(float p[4]) const;

    void multiplyPoint3(float p[3]) const;
    
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

    AString toString() const;

    AString toFormattedString(const AString& indentation);

    void setModified();

    void clearModified();

    bool isModified() const;

    void writeAsXML(XmlWriter& xmlWriter) throw (XmlException);
    
private:
    double fixZero(const double f);

    void fixNumericalError();

    void Transpose(const double input[4][4], double output[4][4]) const;

    bool Inverse(const double inputMatrix[4][4], double outputMatrix[4][4]) const;

    void Adjoint(const double inputMatrix[4][4], double outputMatrix[4][4]) const;

    void UpperTriangle4(const double inputMatrix[4][4], double outputMatrix[4][4]) const;

    void UpperTriangle3(const double inputMatrix[3][3], double outputMatrix[3][3]) const;
    
    double Determinant4x4(const double matrix[4][4]) const;

    double Determinant3(const double matrix[3][3]) const;
    
    static double Determinant3x3(double a1, double a2, double a3,
                              double b1, double b2, double b3,
                                 double c1, double c2, double c3);
    
    static double Determinant2x2(double a, double b, double c, double d);

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
