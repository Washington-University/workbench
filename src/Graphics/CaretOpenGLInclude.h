#ifndef __CARET_OPEN_GL_INCLUDE_H__
#define __CARET_OPEN_GL_INCLUDE_H__

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

/**
 * \file CaretOpenGLInclude.h
 * \brief Contains OpenGL include declarations that work for all operating systems.
 * \ingroup Common
 */

/*
 * OpenGL is deprecated on MacOS 10.15, Catalina
 * This definition suppress the deprecation warning that will
 * appear on every OpenGL function call
 */
#ifdef CARET_OS_MACOSX
#define GL_SILENCE_DEPRECATION 1
#endif


/*
 * When using GLEW, GL/glew.h MUST be included before Gl/gl.h.
 * Note: Windows.h includes Gl/gl.h.
 *
 * Must define GLEW_STATIC when GLEW is in a static library
 * or linking errors will occur with GLEW functions
 */
#ifdef HAVE_GLEW
    #ifdef HAVE_GLEW_STATIC_LIB
        #define GLEW_STATIC
    #endif

    #include <GL/glew.h>
#endif

#ifdef CARET_OS_WINDOWS
    #include <Windows.h>
#endif

#ifdef CARET_OS_MACOSX
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
#else
    #ifndef HAVE_GLEW
        #ifdef CARET_OS_LINUX
            /*
             * On linux, adds support for newer OpenGL 
             * as described at https://www.khronos.org/registry/OpenGL/ABI/
             */
            #define GL_GLEXT_PROTOTYPES
        #endif
    #endif

    #include <GL/gl.h>
    #include <GL/glu.h>
#endif

#endif  //__CARET_OPEN_GL_INCLUDE_H__
