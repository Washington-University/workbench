#ifndef __DATAFILEINTERFACE_H__
#define __DATAFILEINTERFACE_H__

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


#include <AString.h>

#include "TracksModificationInterface.h"

namespace caret {

    /**
     * Abstract class for data file behavior.
     */
    class DataFileInterface : public TracksModificationInterface {
        
    protected:
        DataFileInterface() { }
        
        virtual ~DataFileInterface() { }

    private:
        DataFileInterface(const DataFileInterface& s);
        DataFileInterface& operator=(const DataFileInterface&);
        
    public:
        /**
         * Is the file empty (contains no data)?
         *
         * @return 
         *    true if the file is empty, else false.
         */
        virtual bool isEmpty() const = 0;
        
        /**
         * Get the name of the data file.
         *
         * @return Name of the data file.
         */
        virtual AString getFileName() const = 0;
        
        /**
         * Set the name of the data file.
         *
         * @param filename
         *     New name of data file.
         */
        virtual void setFileName(const AString& filename) = 0;
        
        /**
         * Read the data file.
         *
         * @param filename
         *    Name of the data file.
         * @throws DataFileException
         *    If the file was not successfully read.
         */
        virtual void readFile(const AString& filename) = 0;
        
        /**
         * Write the data file.
         *
         * @param filename
         *    Name of the data file.
         * @throws DataFileException
         *    If the file was not successfully written.
         */
        virtual void writeFile(const AString& filename) = 0;
        
    };
    
} // namespace

#endif // __DATAFILEINTERFACE_H__
