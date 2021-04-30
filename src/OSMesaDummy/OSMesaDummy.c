
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

#include "assert.h"

#define __O_S_MESA_DUMMY_DECLARE__
#include "OSMesaDummy.h"
#undef __O_S_MESA_DUMMY_DECLARE__

/*
 * THESE ARE DUMMY METHODS.
 *
 * THE COMMAND LINE USES OSMESA FOR RENDERING IMAGES USING OPENG
 * WITHOUT A DISPLAY.  HOWEVER, WITH THE GUI, IF THE OSMESA
 * LIBRARY IS LINKED, IT SOMEHOW RESULTS IN THE MESA GL (OPENGL)
 * LIBRARY BEING LINKED INSTEAD OF THE SYSTEM'S HARDWARE OPENGL
 * LIBRARY AND EVERYTHING IS SCREWED UP.
 *
 * IT IS THE DISPLAY OF HELP INFORMATION FOR COMMANDS THAT INCLUDES
 * THE SHOW SCENE COMMAND THAT REQUIRES OSMESA.
 */



/*GLAPI*/ OSMesaContext /*GLAPIENTRY*/
OSMesaCreateContextExt(GLenum format, GLint depthBits, GLint stencilBits,
                       GLint accumBits, OSMesaContext sharelist)
{
    /*
     * NOTE: These are "C" functions (NOT "C++") and so the names
     * of the parameters cannot be removed to avoid a compilation
     * warning as missing parameter names is a compiler error in "C".
     */
    if (format
        || depthBits
        || stencilBits
        || accumBits
        || sharelist) {
        
    }

    assert(0);

    return 0;
}

/*GLAPI*/ void /*GLAPIENTRY*/
OSMesaDestroyContext( OSMesaContext ctx )
{
    if (ctx) {
        
    }
    assert(0);
}

/*GLAPI*/ GLboolean /*GLAPIENTRY*/
OSMesaMakeCurrent( OSMesaContext ctx, void * buffer, GLenum type,
                  GLsizei width, GLsizei height )
{
    if (ctx
        || buffer
        || type
        || width
        || height) {
        
    }
    assert(0);
    return GL_FALSE;
}

/*GLAPI*/ void /*GLAPIENTRY*/
OSMesaGetIntegerv( GLint pname, GLint *value )
{
    if (pname
        || value) {
        
    }
    assert(0);
}

    
