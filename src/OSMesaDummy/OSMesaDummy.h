#ifndef __O_S_MESA_DUMMY_H__
#define __O_S_MESA_DUMMY_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#include "CaretOpenGLInclude.h"

typedef struct osmesa_context *OSMesaContext;

/*
 * THESE ARE DUMMY METHODS.
 *
 * See the ".c" file for more info.
 */


/*GLAPI*/ OSMesaContext /*GLAPIENTRY*/
OSMesaCreateContextExt(GLenum format, GLint depthBits, GLint stencilBits,
                       GLint accumBits, OSMesaContext sharelist);

/*GLAPI*/ void /*GLAPIENTRY*/
OSMesaDestroyContext( OSMesaContext ctx );

/*GLAPI*/ GLboolean /*GLAPIENTRY*/
OSMesaMakeCurrent( OSMesaContext ctx, void *buffer, GLenum type,
                  GLsizei width, GLsizei height );



#endif  //__O_S_MESA_DUMMY_H__
