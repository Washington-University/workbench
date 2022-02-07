#ifndef __GRAPHICS_OPEN_G_L_ERROR_H__
#define __GRAPHICS_OPEN_G_L_ERROR_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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



#include <memory>

#include "CaretObject.h"
#include "CaretOpenGLInclude.h"


namespace caret {
    
    class GraphicsOpenGLError : public CaretObject {
        
    public:
        GraphicsOpenGLError(const AString& userMessage);
        
        virtual ~GraphicsOpenGLError();
        
        int32_t getNumberOfErrors() const;
        
        GLenum getErrorCode(const int32_t errorIndex) const;
        
        AString getErrorDescription(const int32_t errorIndex) const;
        
        // ADD_NEW_METHODS_HERE

        AString getVerboseDescription() const;
        
    private:
//        class ErrorInfo {
//        public:
//            ErrorInfo(const GLenum errorCode,
//                      const AString errorDescription)
//            : m_errorCode(errorCode),
//            m_errorDescription(errorDescription) { }
//            
//            const GLenum m_errorCode;
//            
//            const AString m_errorDescription;
//        };
        
        GraphicsOpenGLError(const GraphicsOpenGLError& obj);
        
        GraphicsOpenGLError& operator=(const GraphicsOpenGLError& obj);
        
        const AString m_userMessage;
        
        void addError(const GLenum errorCode);
        
        std::vector<GLenum> m_errorCodes;

        SystemBacktrace m_backtrace;
        
        int32_t m_projectionStackDepth = 0;
        
        int32_t m_modelviewStackDepth = 0;
        
        int32_t m_nameStackDepth = 0;
        
        int32_t m_attribStackDepth = 0;
        
        // ADD_NEW_MEMBERS_HERE

        friend class GraphicsUtilitiesOpenGL;
    };
    
#ifdef __GRAPHICS_OPEN_G_L_ERROR_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GRAPHICS_OPEN_G_L_ERROR_DECLARE__

} // namespace
#endif  //__GRAPHICS_OPEN_G_L_ERROR_H__
