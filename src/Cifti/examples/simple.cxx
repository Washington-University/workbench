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
#include <stdio.h>
#include <QtCore>
#include "CiftiFile.h"
#include "iostream"
#include "CiftiXMLElements.h"
#include "CiftiXMLReader.h"
#include "CiftiXMLWriter.h"
#include <QFile>
#include <qiodevice.h>

CiftiRootElement root;


int main(int argc, char **argv)
{
   if(argc != 2)
   {
      std::cout << "Usage: " << std::endl;
      std::cout << "simple <inputCiftiFileName> <outputCiftiFileName>" << std::endl;
      std::cout << "To download example Cifti Data, please go to the following URL:" << std::endl;
      std::cout << "http://www.nitrc.org/plugins/mwiki/index.php/cifti:ConnectivityMatrixFileFormats" << std::endl;
   }
   
   QString inFileName(argv[1]);
   QString outFileName(argv[2]);
   CiftiFile *cf = new CiftiFile(inFileName);
   if(!cf) std::cout << "There was an error opening the Cifti File." << std::endl;
   
   CiftiXML *xml = cf->getCiftiXML();
   Nifti2Header *header = cf->getHeader();
   CiftiMatrix *matrix = cf->getCiftiMatrix();
   
   // print out Nifti Header in human readable format
   QString *string = header->getHeaderAsString();
   std::cout << string->toAscii().data() << std::endl;
   delete string;
   string = NULL;
   
   // Print out number of Brain Model Elements
   CiftiRootElement root;
   xml->getXMLRoot(root);
   std::cout << "Number of Brain Elements is: " << root.m_matrices.at(0).m_matrixIndicesMap.at(0).m_brainModels.size() << std::endl;
   
   // Print out matrix dimensions
   float *data = NULL;
   std::vector<int> dimensions;
   try {
      matrix->getMatrixData(data, dimensions);
   }
   catch (CiftiFileException e)
   {
      std::cout << "There was an error getting the Cifti Matrix data" << e.what() << std::endl;
   }
   for(unsigned int i = 0;i<dimensions.size();i++)
      std::cout << "Dimension " << i << " is:" << dimensions[i] << std::endl;   
   
   // write to a new file
   cf->writeFile(outFileName);
   return 0;
   
}


