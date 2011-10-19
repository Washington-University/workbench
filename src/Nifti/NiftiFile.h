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
#ifndef NIFTIFILE_H
#define NIFTIFILE_H

#include <QtCore>
#include "iostream"
#include "NiftiException.h"
#include "NiftiHeaderIO.h"
#include "ByteSwapping.h"
#include "NiftiMatrix.h"
#include "VolumeFile.h"

/** TODOS: there are a BUNCH...
  * 1.  create class for dealing with and parsing extensions, borrow as much as possible from caret5
  * 2.  when an extension is added, update header to reflect new vox offset
  * 3.  decide how to deal with in place read/write, for now one can edit the volume of a nifti in place, but
        how do we deal with extensions, which would shift the volume offset, simply not allow it for in-place
        edits?
  * 4.  Add more control for writing out different layouts, currently, it's set up to honor the original
  *     layout and byte order when doing in place edits, or write out to the default format, little endian (floats),
        but we may want to add more control
  * 5.  Currently only reads and write a frames at a time, and the underlying matrix only does this, it would be
        good to add functionality for reading a voxel at a time, a row at a time, a slice at a time, or the
        entire time series at a time, this would involve creating separate syntax for the following (the current
        matrix reader writer blurs the lines between concepts a bit):
        a.  setting current "area of interest on disk" (we don't just want to "read" it, since we may be dealing
        with a new volume, in case "read" as it currently is used.
        b.  loading "area of interest" into memory (when reading/updating), not necessary when writing.
        c.  reading/writing to data loaded in memory (specifying component, slice, frame, or time series), when the
            data requested is larger than, or outside the range of "area of interest", should we automatically
            deal with it, throw an exception, or is there another approach?
        d.  setting size of "area of interest", in the case that we automatically load data when requested
            read/write is out of bounds, this will set the size of chunk that is loaded (component, row, slice,
            or frame, in the case of the entire time series, going out of bounds "shouldn't" happnen)
  * 6.  Functions for getting at data should expose different data types than just float, internal storage
        should be more flexible than simply storing floats
  * 7.  For matrix reader, need to set up efficient ways of reading data that isn't packed tightly together,
        and generic ways of controlling how this happens, especially important for cifti.
  * 8.  Think a bit more about data organization.  i.e.  Should a nifti header "know" that it needs to be swapped
        (convenient for deriving matrix layouts from a nifti header), or does that only belong in the reader writer
        for the header?  Try to eliminate areas of unnecessary data duplication to eliminate potential bugs, and
        also add more code to keep things in sync (i.e. matrix offset in NiftiMatrix, and voxoffset in nifti header).
  * 9.  Finally, should headers, matrix readers, and extensions exist outside nifti file, or should all properties
        be expose through nifti files, so that they can be synchronized internally?
**/



/** TODOS: there are a BUNCH...
  * 1.  create class for dealing with and parsing extensions, borrow as much as possible from caret5
  * 2.  when an extension is added, update header to reflect new vox offset
  * 3.  decide how to deal with in place read/write, for now one can edit the volume of a nifti in place, but
        how do we deal with extensions, which would shift the volume offset, simply not allow it for in-place
        edits?
  * 4.  Add more control for writing out different layouts, currently, it's set up to honor the original
  *     layout and byte order when doing in place edits, or write out to the default format, little endian (floats),
        but we may want to add more control
  * 5.  Currently only reads and write a frames at a time, and the underlying matrix only does this, it would be
        good to add functionality for reading a voxel at a time, a row at a time, a slice at a time, or the
        entire time series at a time, this would involve creating separate syntax for the following (the current
        matrix reader writer blurs the lines between concepts a bit):
        a.  setting current "area of interest on disk" (we don't just want to "read" it, since we may be dealing
        with a new volume, in case "read" as it currently is used.
        b.  loading "area of interest" into memory (when reading/updating), not necessary when writing.
        c.  reading/writing to data loaded in memory (specifying component, slice, frame, or time series), when the
            data requested is larger than, or outside the range of "area of interest", should we automatically
            deal with it, throw an exception, or is there another approach?
        d.  setting size of "area of interest", in the case that we automatically load data when requested
            read/write is out of bounds, this will set the size of chunk that is loaded (component, row, slice,
            or frame, in the case of the entire time series, going out of bounds "shouldn't" happnen)
  * 6.  Functions for getting at data should expose different data types than just float, internal storage
        should be more flexible than simply storing floats
  * 7.  For matrix reader, need to set up efficient ways of reading data that isn't packed tightly together,
        and generic ways of controlling how this happens, especially important for cifti.
  * 8.  Think a bit more about data organization.  i.e.  Should a nifti header "know" that it needs to be swapped
        (convenient for deriving matrix layouts from a nifti header), or does that only belong in the reader writer
        for the header?  Try to eliminate areas of unnecessary data duplication to eliminate potential bugs, and
        also add more code to keep things in sync (i.e. matrix offset in NiftiMatrix, and voxoffset in nifti header).
  * 9.  Finally, should headers, matrix readers, and extensions exist outside nifti file, or should all properties
        be expose through nifti files, so that they can be synchronized internally?
**/



/// Class for opening, reading, and writing generic Nifti1/2 Data, doesn't support extensions (yet)

namespace caret {

//we specify here whether to write in native byte order, or to honor the original
//byte order, we could also get clever and try to determine whether or not we are
//a big or little-endian machine, but the key concept to keep track of is whether or
//not we want to honor the byte order of the original file (for in place read/write)
//not the actual byte order.  This should also function portably.

class NiftiFile
{
public:
   /// Constructor
   NiftiFile() throw (NiftiException);
   /// Constructor
   NiftiFile(const QString &fileName) throw (NiftiException);
   /// Open the Nifti File
   virtual void openFile(const QString &fileName) throw (NiftiException);
   /// Write the Nifti File
   virtual void writeFile(const QString &fileName, NIFTI_BYTE_ORDER byteOrder = NATIVE_BYTE_ORDER) throw (NiftiException);

   /// Header Functions
   /// set Nifti1Header
   virtual void setHeader(const Nifti1Header &header) throw (NiftiException);
   /// get Nifti1Header
   void getHeader(Nifti1Header &header) throw (NiftiException);
   /// set Nifti2Header
   virtual void setHeader(const Nifti2Header &header) throw (NiftiException);
   /// get Nifti2Header
   void getHeader(Nifti2Header &header) throw (NiftiException);

   // TODO: This will eventually be handled by the extension reader/writer object
   void swapExtensionsBytes(int8_t *bytes, const int64_t &extensionLength);

   /// volume file read/write Functions
   /// get VolumeFrame
   void getVolumeFrame(VolumeFile &frameOut, const int64_t timeSlice, const int64_t component=0);
   /// set VolumeFrame
   void setVolumeFrame(VolumeFile &frameIn, const int64_t & timeSlice, const int64_t component=0);

   /// Read the entire nifti file into a volume file
   void readVolumeFile(VolumeFile &vol, const AString &filename) throw (NiftiException);
   /// Write the entire Volume File to a nifti file
   void writeVolumeFile(VolumeFile &vol, const AString &filename) throw (NiftiException);

   /// Gets a Nifti1Header from a previously defined volume file
   void getHeaderFromVolumeFile(VolumeFile &vol, Nifti1Header & header);
   /// Gets a Nifti2Header from a previously defined volume file
   void getHeaderFromVolumeFile(VolumeFile &vol, Nifti2Header & header);

   void getLayout(LayoutType &layout) throw(NiftiException);

   /// Destructor
   virtual ~NiftiFile();
protected:
   virtual void init();

   AString m_fileName;
   NiftiHeaderIO headerIO;
   NiftiMatrix matrix;
   int8_t * extension_bytes;
};


}

#endif // NIFTIFILE_H

