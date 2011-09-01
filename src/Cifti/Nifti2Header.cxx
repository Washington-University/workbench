/*LICENSE_START*/
/*
 *  Copyright 1995-2011 Washington University School of Medicine
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
/*LICENSE_END*/
#include "Nifti2Header.h"
#include <vector>

/**
 * Constructor
 * 
 * Constructor that opens and reads from the specified file 
 * @param inputFileName the name of the input file that starts with the Nifti2Header
 */
Nifti2Header::Nifti2Header(const QString& inputFileName) throw (CiftiFileException)
{
   readFile(inputFileName);   
}

/**
 * Constructor
 * 
 * Constructor that reads from the specified file handle 
 * @param inputFile the input handle that starts with the Nifti2Header
 */
Nifti2Header::Nifti2Header(QFile &inputFile) throw (CiftiFileException)
{
   readFile(inputFile);
}


/**
 * readFile
 * 
 * read Nifti2Header data from file name, closes file when finished loading data
 * @param inputFileName name and path of Nifti2 Header file
 */
void Nifti2Header::readFile(const QString& inputFileName) throw (CiftiFileException)
{
   QFile inputFile;
   inputFile.setFileName(inputFileName);
   inputFile.open(QIODevice::ReadOnly);
   this->readFile(inputFile);
   inputFile.close(); 
   
}

/**
 * getSwapNeed
 * 
 * determines whether the byte order needs to be swapped, i.e. if
 * the Nifti file is in big endian format, but the machine architecture
 * is little-endian, then a swap would be necessary
 * @return swapNeeded true if swap is needed
 */
bool Nifti2Header::getSwapNeeded()
{
   return m_swapNeeded;
}

/**
 * Constructor
 * 
 * Constructor that takes an input nifti_2_header struct * 
 * @param header 
 */
void Nifti2Header::readFile(QFile &inputFile) throw (CiftiFileException)
{
   int bytes_read = 0;
   m_swapNeeded=false;
   bytes_read = inputFile.read((char *)&m_header, NIFTI2_HEADER_SIZE);
   if(bytes_read < NIFTI2_HEADER_SIZE) {
      throw CiftiFileException("Error reading Cifti header, file is too short.");
   }
   if(NIFTI2_NEEDS_SWAP(m_header))
   {
      m_swapNeeded = true;
      CiftiByteSwap::swapBytes(&(m_header.sizeof_hdr),1);
      CiftiByteSwap::swapBytes(&(m_header.datatype),1);
      CiftiByteSwap::swapBytes(&(m_header.bitpix),1);
      CiftiByteSwap::swapBytes(&(m_header.dim[0]),8);      
      CiftiByteSwap::swapBytes(&(m_header.intent_p1),1);
      CiftiByteSwap::swapBytes(&(m_header.intent_p2),1);
      CiftiByteSwap::swapBytes(&(m_header.intent_p3),1);
      CiftiByteSwap::swapBytes(&(m_header.pixdim[0]),8);
      CiftiByteSwap::swapBytes(&(m_header.vox_offset),1);
      CiftiByteSwap::swapBytes(&(m_header.scl_slope),1);
      CiftiByteSwap::swapBytes(&(m_header.scl_inter),1);
      CiftiByteSwap::swapBytes(&(m_header.cal_max),1);
      CiftiByteSwap::swapBytes(&(m_header.cal_min),1);
      CiftiByteSwap::swapBytes(&(m_header.slice_duration),1);
      CiftiByteSwap::swapBytes(&(m_header.toffset),1);
      CiftiByteSwap::swapBytes(&(m_header.slice_start),1);
      CiftiByteSwap::swapBytes(&(m_header.slice_end),1);
      CiftiByteSwap::swapBytes(&(m_header.qform_code),1);
      CiftiByteSwap::swapBytes(&(m_header.sform_code),1);
      CiftiByteSwap::swapBytes(&(m_header.quatern_b),1);
      CiftiByteSwap::swapBytes(&(m_header.quatern_c),1);
      CiftiByteSwap::swapBytes(&(m_header.quatern_d),1);
      CiftiByteSwap::swapBytes(&(m_header.qoffset_x),1);
      CiftiByteSwap::swapBytes(&(m_header.qoffset_y),1);
      CiftiByteSwap::swapBytes(&(m_header.qoffset_z),1);
      CiftiByteSwap::swapBytes(&(m_header.srow_x[0]),4);
      CiftiByteSwap::swapBytes(&(m_header.srow_y[0]),4);
      CiftiByteSwap::swapBytes(&(m_header.srow_z[0]),4);
      CiftiByteSwap::swapBytes(&(m_header.slice_code),1);
      CiftiByteSwap::swapBytes(&(m_header.xyzt_units),1);      
      CiftiByteSwap::swapBytes(&(m_header.intent_code),1);
      
      //throw CiftiFileException("The NIFTI standard only supports little-endian byte order.");
      std::cout <<"It is recommended that Cifti files are stored in little-endian byte order." << std::endl;
   }
   if((NIFTI2_VERSION(m_header))!=2)
   {
      throw CiftiFileException("This is not a valid Nifti2/Cifti File.");
   }
}

/**
 * Constructor
 * 
 * Constructor that takes an input nifti_2_header struct
 * 
 * @param header 
 */
Nifti2Header::Nifti2Header(const nifti_2_header &header) throw (CiftiFileException)
{
   memcpy((void *)&m_header,&header,sizeof(m_header));   
}

/**
 * Default Constructor
 * 
 * Default Constructor 
 */
Nifti2Header::Nifti2Header() throw (CiftiFileException)
{
   initHeaderStruct(m_header);
}

/**
 * Destructor
 * 
 * Destructor
 */
Nifti2Header::~Nifti2Header()
{   
}

/**
 * getHeaderAsString
 * 
 * creates a formatted string containing the Nifti2 Header data, this
 * is useful for printing out the Nifti2 Header in a human readable
 * format.
 * @return string containing human readable Nifti 2 Header
 */
QString *Nifti2Header::getHeaderAsString()
{
   QString *string = new QString;   
   *string += "header size: " + QString::number (m_header.sizeof_hdr) + "\n";   
   *string += "magic : " + QString::fromAscii(m_header.magic) + "\n";
   *string += "Data Type: " + QString::number(m_header.datatype) + "\n";
   *string += "bitpix: " + QString::number(m_header.bitpix) + "\n";
   *string += "dim[0]: " + QString::number(m_header.dim[0]) + "\n";
   *string += "intent_p1: " + QString::number(m_header.intent_p1) + "\n";
   *string += "intent_p2: " + QString::number(m_header.intent_p2) + "\n";
   *string += "intent_p3: " + QString::number(m_header.intent_p3) + "\n";
   *string += "pixdim[0]: " + QString::number(m_header.pixdim[0] ) + "\n";
   *string += "pixdim[1]: " + QString::number(m_header.pixdim[1]) + "\n";
   *string += "pixdim[2]: " + QString::number(m_header.pixdim[2]) + "\n";
   *string += "pixdim[3]: " + QString::number(m_header.pixdim[3]) + "\n";
   *string += "pixdim[4]: " + QString::number(m_header.pixdim[4]) + "\n";
   *string += "pixdim[5]: " + QString::number(m_header.pixdim[5]) + "\n";
   *string += "pixdim[6]: " + QString::number(m_header.pixdim[6]) + "\n";
   *string += "pixdim[7]: " + QString::number(m_header.pixdim[7]) + "\n";
   *string += "vox_offset: " + QString::number(m_header.vox_offset) + "\n";
   *string += "scl_scope: " + QString::number(m_header.scl_slope) + "\n";
   *string += "scl_inter: " + QString::number(m_header.scl_inter) + "\n";
   *string += "cal_max: " + QString::number(m_header.cal_max) + "\n";
   *string += "cal_min: " + QString::number(m_header.cal_min) + "\n";
   *string += "slice_duration: " + QString::number(m_header.slice_duration) + "\n";
   *string += "toffset: " + QString::number(m_header.toffset) + "\n";
   *string += "slice_start: " + QString::number(m_header.slice_start) + "\n";
   *string += "slice_end: " + QString::number(m_header.slice_end) + "\n";
   *string += "descrip: " + QString::fromAscii(m_header.descrip) + "\n";
   *string += "aux_file: " + QString::fromAscii(m_header.aux_file) + "\n";
   *string += "qform_code: " + QString::number(m_header.qform_code) + "\n";
   *string += "sform_code: " + QString::number(m_header.sform_code) + "\n";
   *string += "quatern_b: " + QString::number(m_header.quatern_b) + "\n";
   *string += "quatern_c: " + QString::number(m_header.quatern_c) + "\n";
   *string += "quatern_d: " + QString::number(m_header.quatern_d) + "\n";
   *string += "qoffset_x: " + QString::number(m_header.qoffset_x) + "\n";
   *string += "qoffset_y: " + QString::number(m_header.qoffset_y) + "\n";
   *string += "qoffset_z: " + QString::number(m_header.qoffset_z) + "\n";
   *string += "srow_x[0]: " + QString::number(m_header.srow_x[0]) + "\n";
   *string += "srow_x[1]: " + QString::number(m_header.srow_x[1]) + "\n";
   *string += "srow_x[2]: " + QString::number(m_header.srow_x[2]) + "\n";
   *string += "srow_x[3]: " + QString::number(m_header.srow_x[3]) + "\n";
   *string += "srow_y[0]: " + QString::number(m_header.srow_y[0]) + "\n";
   *string += "srow_y[1]: " + QString::number(m_header.srow_y[1]) + "\n";
   *string += "srow_y[2]: " + QString::number(m_header.srow_y[2]) + "\n";
   *string += "srow_y[3]: " + QString::number(m_header.srow_y[3]) + "\n";
   *string += "srow_z[0]: " + QString::number(m_header.srow_z[0]) + "\n";
   *string += "srow_z[1]: " + QString::number(m_header.srow_z[1]) + "\n";
   *string += "srow_z[2]: " + QString::number(m_header.srow_z[2]) + "\n";
   *string += "srow_z[3]: " + QString::number(m_header.srow_z[3]) + "\n";
   *string += "slice_code: " + QString::number(m_header.slice_code) + "\n";
   *string += "xyzt_units: " + QString::number(m_header.xyzt_units) + "\n";
   *string += "intent_code: " + QString::number(m_header.intent_code) + "\n";
   *string += "intent_name: " + QString::fromAscii(m_header.intent_name) + "\n";
   *string += "dim_info: " + QString::number(m_header.dim_info) + "\n";
   //string + m_header.unused_str + "\n";
   return string;
}

/**
 * getHeaderStruct
 * 
 * get the raw nifti_2_header struct, as defined in nifti2.h
 * @param header
 */
void Nifti2Header::getHeaderStruct(nifti_2_header &header) const throw (CiftiFileException)
{   
   memcpy(&header, &m_header, sizeof(m_header));
}

/** 
 * setHeaderStruct
 * 
 * sets the raw nifti_2_header struct, as defined in nifti2.h
 * @param header
 */
void Nifti2Header::setHeaderStuct(const nifti_2_header &header) throw (CiftiFileException)
{
   memcpy(&m_header, &header, sizeof(m_header));   
}

/** 
 * getCiftiType
 * 
 * gets the Cifti intent code from the supplied nifti_2_header struct
 * @param header
 * @return int containing the nifti header intent_code
 */
int Nifti2Header::getCiftiType(const nifti_2_header &header)   const throw (CiftiFileException)
{
   return header.intent_code;
}

/** 
 * getCiftiType
 * 
 * gets the Cifti intent code
 * @return int containing the nifti header intent_code
 */
int Nifti2Header::getCiftiType() const throw (CiftiFileException)
{
   return m_header.intent_code;
}

/**
 * writeFile
 * 
 * writes the nifti 2 header to the output file handle
 * @param outputFile
 */
void Nifti2Header::writeFile(QFile &outputFile) const throw (CiftiFileException)
{
   if(!outputFile.isOpen()) 
   { 
      if(!outputFile.open(QIODevice::WriteOnly))
      {
            throw CiftiFileException("There was an error opening the file for writing.");
      }
   }
   if(outputFile.write((char *)&m_header,NIFTI2_HEADER_SIZE) != NIFTI2_HEADER_SIZE)
   {
         throw CiftiFileException("The was an error writing the header.");
   }
   //outputFile.close();
}

/**
 * writeFile
 * 
 * writes the nifti 2 header to the output file name
 * @param outputFileName
 */
void Nifti2Header::writeFile(const QString &outputFileName) const throw (CiftiFileException)
{
   QFile outputFile;
   outputFile.setFileName(outputFileName);
   this->writeFile(outputFile);
   outputFile.close();
}

void Nifti2Header::initHeaderStruct()
{
   initHeaderStruct(this->m_header);
}

/**
 * initHeaderStruct
 * 
 * initializes the supplied nifti 2 header struct to sensible defaults for Cifti
 * @param header
 */
void Nifti2Header::initHeaderStruct(nifti_2_header &header)
{
   header.sizeof_hdr = NIFTI2_HEADER_SIZE;
   memcpy(header.magic, "n+2\0\r\n\032\n",8);
   header.datatype = 0;//CIFTI_NONE;
   header.bitpix = 0;//TODO
   header.dim[0] = 0;//TODO
   header.intent_p1 = 0;
   header.intent_p2 = 0;
   header.intent_p3 = 0;
   header.pixdim[0] = 0.0;header.pixdim[1] = 1.0;
   header.pixdim[2] = 1.0;header.pixdim[3] = 1.0;
   header.pixdim[4] = 1.0;header.pixdim[5] = 1.0;
   header.pixdim[6] = 1.0;header.pixdim[7] = 1.0;
   header.vox_offset = 544;//TODO, currently set to minimum value
   header.scl_slope = 1;
   header.scl_inter = 0;
   header.cal_max = 0;
   header.cal_min = 0;
   header.slice_duration = 0;
   header.toffset = 0;
   header.slice_start = 0;
   header.slice_end = 0;
   memset(header.descrip,0x00,80);
   memset(header.aux_file,0x00,24);
   header.qform_code =0.0;
   header.sform_code =0.0;
   header.quatern_b = 0.0;
   header.quatern_c = 0.0;
   header.quatern_d = 0.0;
   header.qoffset_x = 0.0;
   header.qoffset_y = 0.0;
   header.qoffset_z = 0.0;
   header.srow_x[0] = 0.0;
   header.srow_x[1] = 0.0;
   header.srow_x[2] = 0.0;
   header.srow_x[3] = 0.0;
   header.srow_y[0] = 0.0;
   header.srow_y[1] = 0.0;
   header.srow_y[2] = 0.0;
   header.srow_y[3] = 0.0;
   header.srow_z[0] = 0.0;
   header.srow_z[1] = 0.0;
   header.srow_z[2] = 0.0;
   header.srow_z[3] = 0.0;
   header.slice_code = 0;
   header.xyzt_units = 0;//TODO
   header.intent_code = NIFTI_INTENT_NONE;
   memset(header.intent_name,0x00,16);
   header.dim_info = 0;
   memset(header.unused_str,0x00,15);
}

/**
 * initTimeSeriesHeaderStruct
 * 
 * initializes the supplied nifti 2 header struct to sensible defaults for Cifti
 * @param header
 */
void Nifti2Header::initTimeSeriesHeaderStruct()
{
   initTimeSeriesHeaderStruct(m_header);
}

void Nifti2Header::initTimeSeriesHeaderStruct(nifti_2_header &header)
{
   initHeaderStruct(header);
   header.intent_code = NIFTI_INTENT_CONNECTIVITY_DENSE;
   header.datatype = NIFTI_TYPE_FLOAT32;
   header.dim[0] = 6;header.dim[1] = 1;
   header.dim[2] = 1;header.dim[3] = 1;
   header.dim[4] = 1;header.dim[5] = 0;
   header.dim[6] = 0;header.dim[7] = 1;
   header.bitpix = 32;
   strcpy(header.intent_name, "ConnDenseTime\0");
}

void Nifti2Header::getCiftiDimensions(std::vector< int >& dimensions)
{
   if(m_header.dim[5]!=1) dimensions.push_back(m_header.dim[5]);
   if(m_header.dim[6]!=1) dimensions.push_back(m_header.dim[6]);
   if(m_header.dim[7]!=1) dimensions.push_back(m_header.dim[7]);
}

void Nifti2Header::setCiftiDimensions(std::vector< int >& dimensions)
{
   for (int i = 0; i < (int)dimensions.size() && i < 3; ++i)
   {
      m_header.dim[5 + i] = dimensions[i];
   }
}
