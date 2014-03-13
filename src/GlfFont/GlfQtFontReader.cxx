
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

#define __GLF_QT_FONT_READER_DECLARE__
#include "GlfQtFontReader.h"
#undef __GLF_QT_FONT_READER_DECLARE__

#include <QDataStream>
#include <QFile>

#include "CaretLogger.h"
#include "glf.h"


/**
 * Read a font file with Qt.  The file may be a file in Qt's
 * resource system.
 *
 * @param fontName
 *    Name of the font.
 * @param glff
 *    Pointer to structure into which font is loaded.
 * @return
 *    GLF_OK if read successfully, else GLF_ERROR.
 */
int
GlfQtFontReader::readFontFile(const char *fontFileName,
             struct glf_font *glff)
{
    QFile file(fontFileName);
    if (! file.open(QFile::ReadOnly)) {
        CaretLogSevere("Reading Font File "
                       + QString(fontFileName)
                       + ": ");
        return GLF_ERROR;
    }
    QDataStream dataStream(&file);
    
    char buffer[64];
    int i, j;
    unsigned char temp, code, verts, fcets, lns;
    float tempfx, tempfy;
    unsigned char *tp;
    int LEndian; /* True if little endian machine */
    
    if (dataStream.readRawData(buffer, 3) != 3) {
        CaretLogSevere("Reading Font File "
                       + QString(fontFileName)
                       + ": first three bytes.");
        file.close();
        return GLF_ERROR;
    }
    buffer[3] = 0;
    if (strcmp(buffer, "GLF"))
    {
        file.close();
        CaretLogSevere("Reading Font File "
                       + QString(fontFileName)
                       + ": incorrect file format should be GLF "
                       + QString(buffer));
        //if (console_msg) printf("Error reading font file: incorrect file format\n");
        return GLF_ERROR;
    }
    
    /* Check for machine */
    LEndian = LittleEndian();
    
    if (dataStream.readRawData(glff->font_name, 96) != 96) {
        CaretLogSevere("Reading Font Name from"
                       + QString(fontFileName));
        file.close();
        return GLF_ERROR;
    }
    glff->font_name[96] = 0;
    
    /*
     * total symbols in font
     */
    if (dataStream.readRawData((char*)&glff->sym_total, 1) != 1) {
        CaretLogSevere("Reading total symbols in font"
                       + QString(fontFileName));
        file.close();
        return GLF_ERROR;
    }
    
    for (i=0; i<MAX_FONTS; i++) glff->symbols[i] = NULL;
    
    /*
     * Read unused data
     */
    for (i=0; i<28; i++) {
        if (dataStream.readRawData((char*)&temp, 1) != 1) {
            CaretLogSevere("Reading unused data "
                           + QString(fontFileName));
            file.close();
            return GLF_ERROR;
        }
    }
    
    /* Now start to read font data */
    
    for (i=0; i<glff->sym_total; i++)
    {
        /* read symbol code */
        if (dataStream.readRawData((char*)&code, 1) != 1) {
            CaretLogSevere("Reading symbol code "
                           + QString(fontFileName));
            file.close();
            return GLF_ERROR;
        }
        /* read vertices count */
        if (dataStream.readRawData((char*)&verts, 1) != 1) {
            CaretLogSevere("Reading vertices count "
                           + QString(fontFileName));
            file.close();
            return GLF_ERROR;
        }
        /* read facet count */
        if (dataStream.readRawData((char*)&fcets, 1) != 1) {
            CaretLogSevere("Reading facet count "
                           + QString(fontFileName));
            file.close();
            return GLF_ERROR;
        }
        /* read lines count */
        if (dataStream.readRawData((char*)&lns, 1) != 1) {
            CaretLogSevere("Reading lines count"
                           + QString(fontFileName));
            file.close();
            return GLF_ERROR;
        }

        
        if (glff->symbols[code] != NULL)
        {
            CaretLogSevere("Error reading font file: encountered symbols in font in "
                           + QString(fontFileName));
            file.close();
            return GLF_ERROR;
        }
        
        glff->symbols[code] = (struct one_symbol *)malloc(sizeof(struct one_symbol));
        glff->symbols[code]->vdata = (float *)malloc(8*verts);
        glff->symbols[code]->fdata = (unsigned char *)malloc(3*fcets);
        glff->symbols[code]->ldata = (unsigned char *)malloc(lns);
        
        glff->symbols[code]->vertexs = verts;
        glff->symbols[code]->facets = fcets;
        glff->symbols[code]->lines = lns;
        
        /* Read vertexs data */
        glff->symbols[code]->leftx = 10;
        glff->symbols[code]->rightx = -10;
        glff->symbols[code]->topy = 10;
        glff->symbols[code]->bottomy = -10;
        
        for (j=0; j<verts; j++)
        {
            if (dataStream.readRawData((char*)&tempfx, 4) != 4) {
                CaretLogSevere("Reading x vertex"
                               + QString(fontFileName));
                file.close();
                return GLF_ERROR;
            }
            if (dataStream.readRawData((char*)&tempfy, 4) != 4) {
                CaretLogSevere("Reading y vertex"
                               + QString(fontFileName));
                file.close();
                return GLF_ERROR;
            }
            
            /* If machine is bigendian -> swap low and high words in
             tempfx and tempfy */
            if (!LEndian)
            {
                tp = (unsigned char *)&tempfx;
                temp = tp[0]; tp[0] = tp[3]; tp[3] = temp;
                temp = tp[1]; tp[1] = tp[2]; tp[2] = temp;
                tp = (unsigned char *)&tempfy;
                temp = tp[0]; tp[0] = tp[3]; tp[3] = temp;
                temp = tp[1]; tp[1] = tp[2]; tp[2] = temp;
            }
            glff->symbols[code]->vdata[j*2] = tempfx;
            glff->symbols[code]->vdata[j*2+1] = tempfy;
            
            if (tempfx < glff->symbols[code]->leftx) glff->symbols[code]->leftx = tempfx;
            if (tempfx > glff->symbols[code]->rightx) glff->symbols[code]->rightx = tempfx;
            if (tempfy < glff->symbols[code]->topy) glff->symbols[code]->topy = tempfy;
            if (tempfy > glff->symbols[code]->bottomy) glff->symbols[code]->bottomy = tempfy;
        }
        for (j=0; j<fcets; j++) {
            if (dataStream.readRawData((char*)&glff->symbols[code]->fdata[j*3], 3) != 3) {
                CaretLogSevere("Reading fdata"
                               + QString(fontFileName));
                file.close();
                return GLF_ERROR;
            }
        }
        for (j=0; j<lns; j++) {
            if (dataStream.readRawData((char*)&glff->symbols[code]->ldata[j*3], 1) != 1) {
                CaretLogSevere("Reading ldata"
                               + QString(fontFileName));
                file.close();
                return GLF_ERROR;
            }
        }
    }
    
    file.close();

    return GLF_OK;
    
}

