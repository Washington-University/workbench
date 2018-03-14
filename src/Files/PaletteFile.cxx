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

#include "CaretLogger.h"
#include "DataFileException.h"
#include "GiftiLabel.h"
#include "GiftiMetaData.h"
#include "Palette.h"
#include "PaletteColorMapping.h"
#include "PaletteFile.h"
#include "PaletteScalarAndColor.h"

#include <limits>

using namespace caret;

/**
 * Constructor.
 *
 */
PaletteFile::PaletteFile()
: CaretDataFile(DataFileTypeEnum::PALETTE)
{
    this->metadata = new GiftiMetaData();
    this->initializeMembersPaletteFile();
    this->addDefaultPalettes();
    this->clearModified();
}

/**
 * Destructor
 */
PaletteFile::~PaletteFile()
{
    this->clearAll();
    delete this->metadata;
}

void
PaletteFile::initializeMembersPaletteFile()
{
}

/**
 * Get the label table used for color storage.
 * @return  LabelTable used for color storage.
 *
 */
GiftiLabelTable*
PaletteFile::getLabelTable()
{
    return &this->labelTable;
}

/**
 * Clear everything.
 */
void 
PaletteFile::clearAll()
{
    int64_t numberOfPalettes = this->palettes.size();
    for (int64_t i = 0; i < numberOfPalettes; i++) {
        delete this->palettes[i];
    }
    this->palettes.clear();
    this->labelTable.clear();   
    this->metadata->clear();
}

/**
 * Clear the file but add default palettes.
 */
void
PaletteFile::clear()
{
    this->clearAll();
    this->addDefaultPalettes();
}

/**
 * Add a palette color.
 * 
 * @param pc - color to add.
 *
 */
void
PaletteFile::addColor(const GiftiLabel& pc)
{
    this->labelTable.addLabel(&pc);
}

/**
 * Add a palette color.
 * 
 * @param name  - name of color.
 * @param red   - red component.
 * @param green - red component.
 * @param blue  - red component.
 *
 */
void
PaletteFile::addColor(
                   const AString& name,
                   const int32_t red,
                   const int32_t green,
                   const int32_t blue)
{
    this->labelTable.addLabel(name, red, green, blue);
}

/**
 * Add a palette color.
 * 
 * @param name - Name of color.
 * @param rgb  - RGB components of color.
 *
 */
void
PaletteFile::addColor(
                   const AString& name,
                   const int32_t rgb[])
{
    this->addColor(name, rgb[0], rgb[1], rgb[2]);
}

/**
 * Get a color via its index.
 * 
 * @param index - index of color.
 * @return  Reference to color at index or the default color 
 *          if the index is invalid.
 *
 */
const GiftiLabel*
PaletteFile::getColor(const int32_t indx) const
{
    return this->labelTable.getLabel(indx);
}

/**
 * Get a color via its index.
 * 
 * @param colorName - Name of color.
 * @return  Reference to color with name or the default color 
 *          if the name does not match any colors.
 *
 */
const GiftiLabel*
PaletteFile::getColorByName(const AString& colorName) const
{
    const GiftiLabel* gl = this->labelTable.getLabel(colorName);
    return gl;
}

/**
 * Get index for a color.
 * 
 * @param colorName - Name of color.
 * @return  Index to color or -1 if not found. 
 *
 */
int32_t
PaletteFile::getColorIndex(const AString& colorName) const
{
    return this->labelTable.getLabelKeyFromName(colorName);
}

/**
 * Get the number of palettes.
 * 
 * @return The number of palettes.
 *
 */
int32_t
PaletteFile::getNumberOfPalettes() const
{
    return this->palettes.size();
}

/**
 * Add a palette.
 * 
 * @param p - palette to add.
 *
 */
void
PaletteFile::addPalette(const Palette& p)
{
    Palette* pal = new Palette(p);
    this->assignColorsToPalette(*pal);
    this->palettes.push_back(pal);
    this->setModified();
}

/**
 * Get a palette.
 * 
 * @param index - index of palette.
 * @return  Reference to palette or null if invalid index.
 *
 */
Palette*
PaletteFile::getPalette(const int32_t indx) const
{
    return this->palettes[indx];
}

/**
 * Find a palette by the specified name.
 * 
 * @param name  Name of palette to search for.
 * @return  Reference to palette with name or null if not found.
 *
 */
Palette*
PaletteFile::getPaletteByName(const AString& name) const
{
    int64_t numberOfPalettes = this->palettes.size();
    for (int64_t i = 0; i < numberOfPalettes; i++) {
        if (this->palettes[i]->getName() == name) {
            return this->palettes[i];
        }
    }
    return NULL;
}

/**
 * Remove a palette.
 * 
 * @param index - index of palette to remove.
 *
 */
void
PaletteFile::removePalette(const int32_t indx)
{
    this->palettes.erase(this->palettes.begin() + indx);
    this->setModified();
}

/**
 * Is this file empty?
 * 
 * @return true if the file is empty, else false.
 *
 */
bool
PaletteFile::isEmpty() const
{
    return this->palettes.empty();
}

/**
 * String description of this class. 
 */
AString
PaletteFile::toString() const
{
    AString s;
    
    int64_t numberOfPalettes = this->palettes.size();
    for (int64_t i = 0; i < numberOfPalettes; i++) {
        s += (this->palettes[i]->toString() + "\n");
    }
    
    return s;
}

/**
 * Is this palette modified?
 * @return
 *   true if modified, else false.
 */
bool 
PaletteFile::isModified() const
{
    if (DataFile::isModified()) {
        return true;
    }
    if (this->labelTable.isModified()) {
        return true;
    }
    
    const int64_t numberOfPalettes = this->getNumberOfPalettes();
    for (int i = 0; i < numberOfPalettes; i++) {
        if (this->palettes[i]->isModified()) {
            return true;
        }
    }
    return false;
}

/**
 * Set this object as not modified.  Object should also
 * clear the modification status of its children.
 *
 */
void
PaletteFile::clearModified()
{
    DataFile::clearModified();
    
    const int64_t numberOfPalettes = this->getNumberOfPalettes();
    for (int i = 0; i < numberOfPalettes; i++) {
        this->palettes[i]->clearModified();
    }
    
    this->labelTable.clearModified();
}

/**
 * Assign colors to the palette.
 * @param
 *    p Palette to which colors are assigned.
 */
void 
PaletteFile::assignColorsToPalette(Palette& p)
{
    int64_t numberOfScalars = p.getNumberOfScalarsAndColors();
    for (int64_t i = 0; i < numberOfScalars; i++) {
        PaletteScalarAndColor* psac = p.getScalarAndColor(i);
        const AString& colorName = psac->getColorName();
        const GiftiLabel* gl = this->getColorByName(colorName);
        if (gl != NULL) {
            float rgba[4];
            gl->getColor(rgba);
            psac->setColor(rgba);
        } else {
            CaretLogSevere(("Missing color \""
                            + colorName
                            + "\" in palette \""
                            + p.getName()
                            + "\""));
        }
    }
}

/**
 * Read the data file.
 *
 * @param filename
 *    Name of the data file.
 * @throws DataFileException
 *    If the file was not successfully read.
 */
void 
PaletteFile::readFile(const AString& filename)
{
    clear();
//    checkFileReadability(filename);
    
    throw DataFileException(filename,
                            "Reading of PaletteFile not implemented.");
}

/**
 * Write the data file.
 *
 * @param filename
 *    Name of the data file.
 * @throws DataFileException
 *    If the file was not successfully written.
 */
void 
PaletteFile::writeFile(const AString& filename)
{
//    checkFileWritability(filename);
    
    throw DataFileException(filename,
                            "Reading of PaletteFile not implemented.");
}

/**
 * Add the default palettes.
 *
 */
void
PaletteFile::addDefaultPalettes()
{
    bool modifiedStatus = this->isModified();
    
    this->addColor("none",  0xff, 0xff, 0xff );
    this->addColor("_yellow",  0xff, 0xff, 0x00 );
    this->addColor("_black",  0x00, 0x00, 0x00 );
    this->addColor("_orange",  0xff, 0x69, 0x00 );
    
    if (this->getPaletteByName(Palette::ROY_BIG_BL_PALETTE_NAME) == NULL) {
        this->addColor("_RGB_255_255_0",  255, 255, 0 ); //#ffff00
        this->addColor("_RGB_255_200_0",  255, 200, 0 ); //#ffc800
        this->addColor("_RGB_255_120_0",  255, 120, 0 ); //#ff7800
        this->addColor("_RGB_255_0_0",  255, 0, 0 ); //#ff0000
        this->addColor("_RGB_200_0_0",  200, 0, 0 ); //#c80000
        this->addColor("_RGB_150_0_0",  150, 0, 0 ); //#960000
        this->addColor("_RGB_100_0_0",  100, 0, 0 ); //#640000
        this->addColor("_RGB_60_0_0",  60, 0, 0 ); //#3c0000
        this->addColor("_RGB_0_0_80",  0, 0, 80 ); //#000050
        this->addColor("_RGB_0_0_170",  0, 0, 170 ); //#0000aa
        this->addColor("_RGB_75_0_125",  75, 0, 125 ); //#4b007d
        this->addColor("_RGB_125_0_160",  125, 0, 160 ); //#7d00a0
        this->addColor("_RGB_75_125_0",  75, 125, 0 ); //#4b7d00
        this->addColor("_RGB_0_200_0",  0, 200, 0 ); //#00c800
        this->addColor("_RGB_0_255_0",  0, 255, 0 ); //#00ff00
        this->addColor("_RGB_0_255_255",  0, 255, 255 ); //#00ffff
        
        Palette royBigBL;
        royBigBL.setName(Palette::ROY_BIG_BL_PALETTE_NAME);
        
        royBigBL.addScalarAndColor(1.00f, "_RGB_255_255_0");
        royBigBL.addScalarAndColor(0.875f, "_RGB_255_200_0");
        royBigBL.addScalarAndColor(0.750f, "_RGB_255_120_0");
        royBigBL.addScalarAndColor(0.625f, "_RGB_255_0_0");
        royBigBL.addScalarAndColor(0.500f, "_RGB_200_0_0");
        royBigBL.addScalarAndColor(0.375f, "_RGB_150_0_0");
        royBigBL.addScalarAndColor(0.250f, "_RGB_100_0_0");
        royBigBL.addScalarAndColor(0.125f, "_RGB_60_0_0");
        royBigBL.addScalarAndColor(0.000f, "_black");
        royBigBL.addScalarAndColor(-0.125f, "_RGB_0_0_80");
        royBigBL.addScalarAndColor(-0.250f, "_RGB_0_0_170");
        royBigBL.addScalarAndColor(-0.375f, "_RGB_75_0_125");
        royBigBL.addScalarAndColor(-0.500f, "_RGB_125_0_160");
        royBigBL.addScalarAndColor(-0.625f, "_RGB_75_125_0");
        royBigBL.addScalarAndColor(-0.750f, "_RGB_0_200_0");
        royBigBL.addScalarAndColor(-0.875f, "_RGB_0_255_0");
        royBigBL.addScalarAndColor(-0.990f, "_RGB_0_255_255");
        royBigBL.addScalarAndColor(-1.00f, "_RGB_0_255_255");
        
        addPalette(royBigBL);
    }
    
    //------------------------------------------------------------------------
    //
    // Palette by David Van Essen
    //
    int oran_yell[3] = { 0xff, 0x99, 0x00 };
    this->addColor("_oran-yell", oran_yell);
    int red[3] = { 0xff, 0x00, 0x00 };
    this->addColor("_red", red);
    int cyan[3] = { 0x00, 0xff, 0xff };
    this->addColor("_cyan", cyan);
    int green[3] = { 0x00, 0xff, 0x00 };
    this->addColor("_green", green);
    int limegreen[3] = { 0x10, 0xb0, 0x10 };
    this->addColor("_limegreen", limegreen);
    int violet[3] = { 0xe2, 0x51, 0xe2 };
    this->addColor("_violet", violet);
    int hotpink[3] = { 0xff, 0x38, 0x8d };
    this->addColor("_hotpink", hotpink);
    int white[3] = { 0xff, 0xff, 0xff };
    this->addColor("_white", white);
    int gry_dd[3] = { 0xdd, 0xdd, 0xdd };
    this->addColor("_gry-dd", gry_dd );
    int gry_bb[3] = { 0xbb, 0xbb, 0xbb };
    this->addColor("_gry-bb", gry_bb);
    int purple2[3] = { 0x66, 0x00, 0x33 };
    this->addColor("_purple2", purple2);
    int blue_videen11[3] = { 0x33, 0x33, 0x4c };
    this->addColor("_blue_videen11", blue_videen11);
    int blue_videen9[3] = { 0x4c, 0x4c, 0x7f };
    this->addColor("_blue_videen9", blue_videen9);
    int blue_videen7[3] = { 0x7f, 0x7f, 0xcc };
    this->addColor("_blue_videen7", blue_videen7);
    
    if (this->getPaletteByName("videen_style") == NULL) {
        Palette videenStyle;
        videenStyle.setName("videen_style");
        videenStyle.addScalarAndColor(1.0f, "_red");
        videenStyle.addScalarAndColor(0.9f, "_orange");
        videenStyle.addScalarAndColor(0.8f, "_oran-yell");
        videenStyle.addScalarAndColor(0.7f, "_yellow");
        videenStyle.addScalarAndColor(0.6f, "_limegreen");
        videenStyle.addScalarAndColor(0.5f, "_green");
        videenStyle.addScalarAndColor(0.4f, "_blue_videen7");
        videenStyle.addScalarAndColor(0.3f, "_blue_videen9");
        videenStyle.addScalarAndColor(0.2f, "_blue_videen11");
        videenStyle.addScalarAndColor(0.1f, "_purple2");
        videenStyle.addScalarAndColor(0.0f, "_black");
        videenStyle.addScalarAndColor(-0.1f, "_cyan");
        videenStyle.addScalarAndColor(-0.2f, "_green");
        videenStyle.addScalarAndColor(-0.3f, "_limegreen");
        videenStyle.addScalarAndColor(-0.4f, "_violet");
        videenStyle.addScalarAndColor(-0.5f, "_hotpink");
        videenStyle.addScalarAndColor(-0.6f, "_white");
        videenStyle.addScalarAndColor(-0.7f, "_gry-dd");
        videenStyle.addScalarAndColor(-0.8f, "_gry-bb");
        videenStyle.addScalarAndColor(-0.9f, "_black");
        addPalette(videenStyle);
    }
    
    //
    // Create a palette with just white and black designed to be used
    // with the interpolate option
    //   
    if (this->getPaletteByName(Palette::GRAY_INTERP_PALETTE_NAME) == NULL) {
        this->addColor("_white_gray_interp",  255, 255, 255 );
        this->addColor("_black_gray_interp",  0, 0, 0 );

        
        Palette palGrayPositiveInterp;
        palGrayPositiveInterp.setName(Palette::GRAY_INTERP_POSITIVE_PALETTE_NAME);        
        palGrayPositiveInterp.addScalarAndColor( 1.0f, "_white_gray_interp");
        palGrayPositiveInterp.addScalarAndColor(0.0f, "_black_gray_interp");
        addPalette(palGrayPositiveInterp);   
        
        Palette palGrayInterp;
        palGrayInterp.setName(Palette::GRAY_INTERP_PALETTE_NAME);        
        palGrayInterp.addScalarAndColor( 1.0f, "_white_gray_interp");
        palGrayInterp.addScalarAndColor(-1.0f, "_black_gray_interp");
        addPalette(palGrayInterp);   
    }
    
    //----------------------------------------------------------------------
    // fixed Psych palette
    //
    if (this->getPaletteByName("PSYCH-FIXED") == NULL) {
        this->addColor("_pyell-oran",  0xff, 0xcc, 0x00 );
        this->addColor("_poran-red",  0xff, 0x44, 0x00 );
        this->addColor("_pred",  0xff, 0x00, 0x00 );
        this->addColor("_pblue",  0x00, 0x44, 0xff );
        this->addColor("_pltblue1",  0x00, 0x69, 0xff );
        this->addColor("_pltblue2",  0x00, 0x99, 0xff );
        this->addColor("_pbluecyan",  0x00, 0xcc, 0xff );
        this->addColor("_pcyan",  0x00, 0xff, 0xff );
        
        Palette psychFixed;
        psychFixed.setName("PSYCH-FIXED");
        //psych.setPositiveOnly(false);
        
        psychFixed.addScalarAndColor(1.00f, "_yellow");
        psychFixed.addScalarAndColor(0.75f, "_pyell-oran");
        psychFixed.addScalarAndColor(0.50f, "_orange");
        psychFixed.addScalarAndColor(0.25f, "_poran-red");
        psychFixed.addScalarAndColor(0.00001f, "_pred");//0.00001f is a special range reserved by data normalization for zero values, see PaletteColorMapping.cxx:1590
        psychFixed.addScalarAndColor(0.0000099f, "_black");
        psychFixed.addScalarAndColor(-0.0000099f, "_black");
        psychFixed.addScalarAndColor(-0.00001f, "_pblue");
        psychFixed.addScalarAndColor(-0.25f, "_pltblue1");
        psychFixed.addScalarAndColor(-0.50f, "_pltblue2");
        psychFixed.addScalarAndColor(-0.75f, "_pbluecyan");
        psychFixed.addScalarAndColor(-1.0f, "_pcyan");
        
        addPalette(psychFixed);
    }
    
    //------------------------------------------------------------------------
    //
    // Palette by Jon Wieser @ mcw
    //
    int rbgyr20_01[3] = { 0xCC, 0x10, 0x33 };
    this->addColor("_rbgyr20_01", rbgyr20_01);
    int rbgyr20_02[3] = { 0x99, 0x20, 0x66 };
    this->addColor("_rbgyr20_02", rbgyr20_02);
    int rbgyr20_03[3] = { 0x66, 0x31, 0x99 };
    this->addColor("_rbgyr20_03", rbgyr20_03);
    int rbgyr20_04[3] = { 0x34, 0x41, 0xCC };
    this->addColor("_rbgyr20_04", rbgyr20_04);
    int rbgyr20_05[3] = { 0x00, 0x51, 0xFF };
    this->addColor("_rbgyr20_05", rbgyr20_05);
    int rbgyr20_06[3] = { 0x00, 0x74, 0xCC };
    this->addColor("_rbgyr20_06", rbgyr20_06);
    int rbgyr20_07[3] = { 0x00, 0x97, 0x99 };
    this->addColor("_rbgyr20_07", rbgyr20_07);
    int rbgyr20_08[3] = { 0x00, 0xB9, 0x66 };
    this->addColor("_rbgyr20_08", rbgyr20_08);
    int rbgyr20_09[3] = { 0x00, 0xDC, 0x33 };
    this->addColor("_rbgyr20_09", rbgyr20_09);
    int rbgyr20_10[3] = { 0x00, 0xFF, 0x00 };
    this->addColor("_rbgyr20_10", rbgyr20_10);
    int rbgyr20_11[3] = { 0x33, 0xFF, 0x00 };
    this->addColor("_rbgyr20_11", rbgyr20_11);
    int rbgyr20_12[3] = { 0x66, 0xFF, 0x00 };
    this->addColor("_rbgyr20_12", rbgyr20_12);
    int rbgyr20_13[3] = { 0x99, 0xFF, 0x00 };
    this->addColor("_rbgyr20_13", rbgyr20_13);
    int rbgyr20_14[3] = { 0xCC, 0xFF, 0x00 };
    this->addColor("_rbgyr20_14", rbgyr20_14);
    int rbgyr20_15[3] = { 0xFF, 0xFF, 0x00 };
    this->addColor("_rbgyr20_15", rbgyr20_15);
    int rbgyr20_16[3] = { 0xFF, 0xCC, 0x00 };
    this->addColor("_rbgyr20_16", rbgyr20_16);
    int rbgyr20_17[3] = { 0xFF, 0x99, 0x00 };
    this->addColor("_rbgyr20_17", rbgyr20_17);
    int rbgyr20_18[3] = { 0xFF, 0x66, 0x00 };
    this->addColor("_rbgyr20_18", rbgyr20_18);
    int rbgyr20_19[3] = { 0xFF, 0x33, 0x00 };
    this->addColor("_rbgyr20_19", rbgyr20_19);
    int rbgyr20_20[3] = { 0xFF, 0x00, 0x00 };
    this->addColor("_rbgyr20_20", rbgyr20_20);
    
    if (this->getPaletteByName("RBGYR20") == NULL) {
        Palette pal2;
        pal2.setName("RBGYR20");
        pal2.addScalarAndColor( 1.0f, "_rbgyr20_01");
        pal2.addScalarAndColor( 0.9f, "_rbgyr20_02");
        pal2.addScalarAndColor( 0.8f, "_rbgyr20_03");
        pal2.addScalarAndColor( 0.7f, "_rbgyr20_04");
        pal2.addScalarAndColor( 0.6f, "_rbgyr20_05");
        pal2.addScalarAndColor( 0.5f, "_rbgyr20_06");
        pal2.addScalarAndColor( 0.4f, "_rbgyr20_07");
        pal2.addScalarAndColor( 0.3f, "_rbgyr20_08");
        pal2.addScalarAndColor( 0.2f, "_rbgyr20_09");
        pal2.addScalarAndColor( 0.1f, "_rbgyr20_10");
        pal2.addScalarAndColor( 0.0f, "_rbgyr20_11");
        pal2.addScalarAndColor(-0.1f, "_rbgyr20_12");
        pal2.addScalarAndColor(-0.2f, "_rbgyr20_13");
        pal2.addScalarAndColor(-0.3f, "_rbgyr20_14");
        pal2.addScalarAndColor(-0.4f, "_rbgyr20_15");
        pal2.addScalarAndColor(-0.5f, "_rbgyr20_16");
        pal2.addScalarAndColor(-0.6f, "_rbgyr20_17");
        pal2.addScalarAndColor(-0.7f, "_rbgyr20_18");
        pal2.addScalarAndColor(-0.8f, "_rbgyr20_19");
        pal2.addScalarAndColor(-0.9f, "_rbgyr20_20");
        addPalette(pal2);
        
        Palette pal3;
        pal3.setName("RBGYR20P");
        pal3.addScalarAndColor(1.00f, "_rbgyr20_01");
        pal3.addScalarAndColor(0.95f, "_rbgyr20_02");
        pal3.addScalarAndColor(0.90f, "_rbgyr20_03");
        pal3.addScalarAndColor(0.85f, "_rbgyr20_04");
        pal3.addScalarAndColor(0.80f, "_rbgyr20_05");
        pal3.addScalarAndColor(0.75f, "_rbgyr20_06");
        pal3.addScalarAndColor(0.70f, "_rbgyr20_07");
        pal3.addScalarAndColor(0.65f, "_rbgyr20_08");
        pal3.addScalarAndColor(0.60f, "_rbgyr20_09");
        pal3.addScalarAndColor(0.55f, "_rbgyr20_10");
        pal3.addScalarAndColor(0.50f, "_rbgyr20_11");
        pal3.addScalarAndColor(0.45f, "_rbgyr20_12");
        pal3.addScalarAndColor(0.40f, "_rbgyr20_13");
        pal3.addScalarAndColor(0.35f, "_rbgyr20_14");
        pal3.addScalarAndColor(0.30f, "_rbgyr20_15");
        pal3.addScalarAndColor(0.25f, "_rbgyr20_16");
        pal3.addScalarAndColor(0.20f, "_rbgyr20_17");
        pal3.addScalarAndColor(0.15f, "_rbgyr20_18");
        pal3.addScalarAndColor(0.10f, "_rbgyr20_19");
        pal3.addScalarAndColor(0.05f, "_rbgyr20_20");
        pal3.addScalarAndColor(0.0f, "none");
        addPalette(pal3);
    }
    
    //----------------------------------------------------------------------
    // Orange-Yellow palette
    //
    if (this->getPaletteByName("Orange-Yellow") == NULL) {
        this->addColor("_oy1",  0, 0, 0 );
        this->addColor("_oy2",  130, 2, 0 );
        this->addColor("_oy3",  254, 130, 2 );
        this->addColor("_oy4",  254, 254, 126 );
        this->addColor("_oy5",  254, 254, 254 );
        
        Palette orangeYellow;
        orangeYellow.setName("Orange-Yellow");
        orangeYellow.addScalarAndColor( 1.0f, "_oy5");
        orangeYellow.addScalarAndColor( 0.5f, "_oy4");
        orangeYellow.addScalarAndColor( 0.0f, "_oy3");
        orangeYellow.addScalarAndColor(-0.5f, "_oy2");
        orangeYellow.addScalarAndColor(-1.0f, "_oy1");
        addPalette(orangeYellow);
    }
    
    //----------------------------------------------------------------------
    // Positive/Negative/Zero palette
    //
    if (this->getPaletteByName("POS_NEG_ZERO") == NULL) {
        this->addColor("pos_neg_blue",  0x00, 0x00, 0xff );
        this->addColor("pos_neg_red",  0xff, 0x00, 0x00 );
        
        Palette posNegZero;
        posNegZero.setName("POS_NEG_ZERO");
        
        posNegZero.addScalarAndColor(1.0f, "pos_neg_red");
        posNegZero.addScalarAndColor(0.00001f, "pos_neg_red");
        posNegZero.addScalarAndColor(0.0000099f, "_black");
        posNegZero.addScalarAndColor(-0.0000099f, "_black");
        posNegZero.addScalarAndColor(-0.00001f, "pos_neg_blue");
        posNegZero.addScalarAndColor(-1.0f, "pos_neg_blue");
        
        addPalette(posNegZero);
    }
    
    if (this->getPaletteByName("red-yellow") == NULL) {
        this->addColor("_red_yellow_interp_red",  255, 0, 0 );
        this->addColor("_red_yellow_interp_yellow",  255, 255, 0 );
        this->addColor("_blue_lightblue_interp_blue",  0, 0, 255 );
        this->addColor("_blue_lightblue_interp_lightblue",  0, 255, 255 );
        this->addColor("_fslview_zero", 0, 0, 0);

        Palette palRedYellowInterp;
        palRedYellowInterp.setName("red-yellow");
        palRedYellowInterp.addScalarAndColor(1.0f, "_red_yellow_interp_yellow");
        palRedYellowInterp.addScalarAndColor(0.0f, "_red_yellow_interp_red");
        addPalette(palRedYellowInterp);

        Palette palBlueLightblueInterp;
        palBlueLightblueInterp.setName("blue-lightblue");
        palBlueLightblueInterp.addScalarAndColor(1.0f, "_blue_lightblue_interp_lightblue");
        palBlueLightblueInterp.addScalarAndColor(0.0f, "_blue_lightblue_interp_blue");
        addPalette(palBlueLightblueInterp);
        
        Palette palFSLView;
        palFSLView.setName("FSL");
        palFSLView.addScalarAndColor( 1.0f, "_red_yellow_interp_yellow");
        palFSLView.addScalarAndColor( 0.00001f, "_red_yellow_interp_red");
        palFSLView.addScalarAndColor( 0.0000099f, "_fslview_zero");
        palFSLView.addScalarAndColor(-0.0000099f, "_fslview_zero");
        palFSLView.addScalarAndColor(-0.00001f, "_blue_lightblue_interp_blue");
        palFSLView.addScalarAndColor(-1.0f, "_blue_lightblue_interp_lightblue");
        addPalette(palFSLView);
    }

    if (this->getPaletteByName("power_surf") == NULL) {
        this->addColor("_ps_0",    1.0 *255.0,   0.0 * 255.0,  0.0 * 255.0 );
        this->addColor("_ps_059",  0.0 * 255.0,  0.0 * 255.0,  0.6 * 255.0 );
        this->addColor("_ps_118",  1.0 * 255.0,  1.0 * 255.0,  0.0 * 255.0 );
        this->addColor("_ps_176",  1.0 * 255.0,  0.7 * 255.0,  0.4 * 255.0);
        this->addColor("_ps_235",  0.0 * 255.0,  0.8 * 255.0,  0.0 * 255.0 );
        this->addColor("_ps_294",  1.0 * 255.0,  0.6 * 255.0,  1.0 * 255.0 );
        this->addColor("_ps_353",  0.0 * 255.0,  0.6 * 255.0,  0.6 * 255.0 );
        this->addColor("_ps_412",  0.0 * 255.0,  0.0 * 255.0,  0.0 * 255.0 );
        this->addColor("_ps_471",  0.3 * 255.0,  0.0 * 255.0,  0.6 * 255.0 );
        this->addColor("_ps_529",  0.2 * 255.0,  1.0 * 255.0,  1.0 * 255.0 );
        this->addColor("_ps_588",  1.0 * 255.0,  0.5 * 255.0,  0.0 * 255.0 );
        this->addColor("_ps_647",  0.6 * 255.0,  0.2 * 255.0,  1.0 * 255.0 );
        this->addColor("_ps_706",  0.0 * 255.0,  0.2 * 255.0,  0.4 * 255.0 );
        this->addColor("_ps_765",  0.2 * 255.0,  1.0 * 255.0,  0.2 * 255.0 );
        this->addColor("_ps_824",  0.0 * 255.0,  0.0 * 255.0,  1.0 * 255.0 );
        this->addColor("_ps_882",  1.0 * 255.0,  1.0 * 255.0,  0.8 * 255.0 );
        this->addColor("_ps_941",  0.0 * 255.0,  0.4 * 255.0,  0.0 * 255.0 );
        this->addColor("_ps_1000", 0.25 * 255.0, 0.25 * 255.0, 0.25 * 255.0 );
        
        Palette powerSurf;
        powerSurf.setName("power_surf");
        powerSurf.addScalarAndColor( 1.0, "_ps_1000");
        powerSurf.addScalarAndColor( 0.941, "_ps_941");
        powerSurf.addScalarAndColor( 0.882, "_ps_882");
        powerSurf.addScalarAndColor( 0.824, "_ps_824");
        powerSurf.addScalarAndColor( 0.765, "_ps_765");
        powerSurf.addScalarAndColor( 0.706, "_ps_706");
        powerSurf.addScalarAndColor( 0.647, "_ps_647");
        powerSurf.addScalarAndColor( 0.588, "_ps_588");
        powerSurf.addScalarAndColor( 0.529, "_ps_529");
        powerSurf.addScalarAndColor( 0.471, "_ps_471");
        powerSurf.addScalarAndColor( 0.412, "_ps_412");
        powerSurf.addScalarAndColor( 0.353, "_ps_353");
        powerSurf.addScalarAndColor( 0.294, "_ps_294");
        powerSurf.addScalarAndColor( 0.235, "_ps_235");
        powerSurf.addScalarAndColor( 0.176, "_ps_176");
        powerSurf.addScalarAndColor( 0.118, "_ps_118");
        powerSurf.addScalarAndColor( 0.059, "_ps_059");
        powerSurf.addScalarAndColor( 0.0, "_ps_0");
        addPalette(powerSurf);
    }
    
    /*
     * FSL Red palette from WB-289
     *
     * float offset = 100.0;
     * float step = (255.0 - offset) / 255.0;
     * for(unsigned char i = 0; i < 255; ++i)
     * { int red = int(((i + 1) * step) + offset); lut->pushValue(red, 0, 0, i); }
     *
     * lut->m_lutName = std::string("Red");
     */
    //TSC: no "lookup tables" for purely interpolated palettes!  bad for performance.
    if (this->getPaletteByName("fsl_red") == NULL) {
        Palette fslRed;
        fslRed.setName("fsl_red");
        this->addColor("fsl_red_0", 100, 0, 0);
        this->addColor("fsl_red_1", 255, 0, 0);
        fslRed.addScalarAndColor(1.0f, "fsl_red_1");
        fslRed.addScalarAndColor(0.0f, "fsl_red_0");
        addPalette(fslRed);
    }
    
    if (this->getPaletteByName("fsl_green") == NULL) {
        Palette fslGreen;
        fslGreen.setName("fsl_green");
        this->addColor("fsl_green_0", 0, 100, 0);
        this->addColor("fsl_green_1", 0, 255, 0);
        fslGreen.addScalarAndColor(1.0f, "fsl_green_1");
        fslGreen.addScalarAndColor(0.0f, "fsl_green_0");
        addPalette(fslGreen);
    }
    
    if (this->getPaletteByName("fsl_blue") == NULL) {
        Palette fslBlue;
        fslBlue.setName("fsl_blue");
        this->addColor("fsl_blue_0", 0, 0, 100);
        this->addColor("fsl_blue_1", 0, 0, 255);
        fslBlue.addScalarAndColor(1.0f, "fsl_blue_1");
        fslBlue.addScalarAndColor(0.0f, "fsl_blue_0");
        addPalette(fslBlue);
    }
    
    if (this->getPaletteByName("fsl_yellow") == NULL) {
        Palette fslYellow;
        fslYellow.setName("fsl_yellow");
        this->addColor("fsl_yellow_0", 100, 100, 0);
        this->addColor("fsl_yellow_1", 255, 255, 0);
        fslYellow.addScalarAndColor(1.0f, "fsl_yellow_1");
        fslYellow.addScalarAndColor(0.0f, "fsl_yellow_0");
        addPalette(fslYellow);
    }
    
    //
    // Create a palette with red (positive), white (zero),
    // and blue (negative) for Alan A.
    //
    if (this->getPaletteByName("RedWhiteBlue") == NULL) {
        Palette redWhiteBlue;
        redWhiteBlue.setName("RedWhiteBlue");
        this->addColor("rwbBlue", 0, 0, 255);
        this->addColor("rwbBlueMiddle", 127, 127, 255);
        this->addColor("rwbWhite", 255, 255, 255);
        this->addColor("rwbRedMiddle", 255, 127, 127);
        this->addColor("rwbRed", 255, 0, 0);
        redWhiteBlue.addScalarAndColor( 1.0f, "rwbRed");
        redWhiteBlue.addScalarAndColor( 0.5f, "rwbRedMiddle");
        redWhiteBlue.addScalarAndColor( 0.0f, "rwbWhite");
        redWhiteBlue.addScalarAndColor(-0.5f, "rwbBlueMiddle");
        redWhiteBlue.addScalarAndColor(-1.0f, "rwbBlue");
        addPalette(redWhiteBlue);
    }
    
    //coolwarm, http://www.kennethmoreland.com/color-maps/
    //"Diverging Color Maps for Scientific Visualization." Kenneth Moreland. In Proceedings of the 5th International Symposium on Visual Computing, December 2009. DOI 10.1007/978-3-642-10520-3_9.
    //this palette was interpolated in CIELAB space, and then translated to sRGB, so we need to keep a substantial number of points
    //use the 33 csv file
    if (this->getPaletteByName("cool-warm") == NULL) {
        Palette coolwarm;
        coolwarm.setName("cool-warm");
        this->addColor("cool-warm-0", 59,76,192);
        this->addColor("cool-warm-1", 68,90,204);
        this->addColor("cool-warm-2", 77,104,215);
        this->addColor("cool-warm-3", 87,117,225);
        this->addColor("cool-warm-4", 98,130,234);
        this->addColor("cool-warm-5", 108,142,241);
        this->addColor("cool-warm-6", 119,154,247);
        this->addColor("cool-warm-7", 130,165,251);
        this->addColor("cool-warm-8", 141,176,254);
        this->addColor("cool-warm-9", 152,185,255);
        this->addColor("cool-warm-10", 163,194,255);
        this->addColor("cool-warm-11", 174,201,253);
        this->addColor("cool-warm-12", 184,208,249);
        this->addColor("cool-warm-13", 194,213,244);
        this->addColor("cool-warm-14", 204,217,238);
        this->addColor("cool-warm-15", 213,219,230);
        this->addColor("cool-warm-16", 221,221,221);
        this->addColor("cool-warm-17", 229,216,209);
        this->addColor("cool-warm-18", 236,211,197);
        this->addColor("cool-warm-19", 241,204,185);
        this->addColor("cool-warm-20", 245,196,173);
        this->addColor("cool-warm-21", 247,187,160);
        this->addColor("cool-warm-22", 247,177,148);
        this->addColor("cool-warm-23", 247,166,135);
        this->addColor("cool-warm-24", 244,154,123);
        this->addColor("cool-warm-25", 241,141,111);
        this->addColor("cool-warm-26", 236,127,99);
        this->addColor("cool-warm-27", 229,112,88);
        this->addColor("cool-warm-28", 222,96,77);
        this->addColor("cool-warm-29", 213,80,66);
        this->addColor("cool-warm-30", 203,62,56);
        this->addColor("cool-warm-31", 192,40,47);
        this->addColor("cool-warm-32", 180,4,38);
        coolwarm.addScalarAndColor(16.0f / 16.0f, "cool-warm-32");
        coolwarm.addScalarAndColor(15.0f / 16.0f, "cool-warm-31");
        coolwarm.addScalarAndColor(14.0f / 16.0f, "cool-warm-30");
        coolwarm.addScalarAndColor(13.0f / 16.0f, "cool-warm-29");
        coolwarm.addScalarAndColor(12.0f / 16.0f, "cool-warm-28");
        coolwarm.addScalarAndColor(11.0f / 16.0f, "cool-warm-27");
        coolwarm.addScalarAndColor(10.0f / 16.0f, "cool-warm-26");
        coolwarm.addScalarAndColor(9.0f / 16.0f, "cool-warm-25");
        coolwarm.addScalarAndColor(8.0f / 16.0f, "cool-warm-24");
        coolwarm.addScalarAndColor(7.0f / 16.0f, "cool-warm-23");
        coolwarm.addScalarAndColor(6.0f / 16.0f, "cool-warm-22");
        coolwarm.addScalarAndColor(5.0f / 16.0f, "cool-warm-21");
        coolwarm.addScalarAndColor(4.0f / 16.0f, "cool-warm-20");
        coolwarm.addScalarAndColor(3.0f / 16.0f, "cool-warm-19");
        coolwarm.addScalarAndColor(2.0f / 16.0f, "cool-warm-18");
        coolwarm.addScalarAndColor(1.0f / 16.0f, "cool-warm-17");
        coolwarm.addScalarAndColor(0.0f / 16.0f, "cool-warm-16");
        coolwarm.addScalarAndColor(-1.0f / 16.0f, "cool-warm-15");
        coolwarm.addScalarAndColor(-2.0f / 16.0f, "cool-warm-14");
        coolwarm.addScalarAndColor(-3.0f / 16.0f, "cool-warm-13");
        coolwarm.addScalarAndColor(-4.0f / 16.0f, "cool-warm-12");
        coolwarm.addScalarAndColor(-5.0f / 16.0f, "cool-warm-11");
        coolwarm.addScalarAndColor(-6.0f / 16.0f, "cool-warm-10");
        coolwarm.addScalarAndColor(-7.0f / 16.0f, "cool-warm-9");
        coolwarm.addScalarAndColor(-8.0f / 16.0f, "cool-warm-8");
        coolwarm.addScalarAndColor(-9.0f / 16.0f, "cool-warm-7");
        coolwarm.addScalarAndColor(-10.0f / 16.0f, "cool-warm-6");
        coolwarm.addScalarAndColor(-11.0f / 16.0f, "cool-warm-5");
        coolwarm.addScalarAndColor(-12.0f / 16.0f, "cool-warm-4");
        coolwarm.addScalarAndColor(-13.0f / 16.0f, "cool-warm-3");
        coolwarm.addScalarAndColor(-14.0f / 16.0f, "cool-warm-2");
        coolwarm.addScalarAndColor(-15.0f / 16.0f, "cool-warm-1");
        coolwarm.addScalarAndColor(-16.0f / 16.0f, "cool-warm-0");
        addPalette(coolwarm);
    }
    
    //matplotlib's spectral is from colorbrewer
    //Colors from www.ColorBrewer.org by Cynthia A. Brewer, Geography, Pennsylvania State University.
    //Copyright (c) 2002 Cynthia Brewer, Mark Harrower, and The Pennsylvania State University.
    if (this->getPaletteByName("spectral") == NULL) {
        Palette spectral;
        spectral.setName("spectral");
        this->addColor("spectral-0", 158,1,66);
        this->addColor("spectral-1", 213,62,79);
        this->addColor("spectral-2", 244,109,67);
        this->addColor("spectral-3", 253,174,97);
        this->addColor("spectral-4", 254,224,139);
        this->addColor("spectral-5", 255,255,191);
        this->addColor("spectral-6", 230,245,152);
        this->addColor("spectral-7", 171,221,164);
        this->addColor("spectral-8", 102,194,165);
        this->addColor("spectral-9", 50,136,189);
        this->addColor("spectral-10", 94,79,162);
        spectral.addScalarAndColor(5.0f / 5.0f, "spectral-0");
        spectral.addScalarAndColor(4.0f / 5.0f, "spectral-1");
        spectral.addScalarAndColor(3.0f / 5.0f, "spectral-2");
        spectral.addScalarAndColor(2.0f / 5.0f, "spectral-3");
        spectral.addScalarAndColor(1.0f / 5.0f, "spectral-4");
        spectral.addScalarAndColor(0.0f / 5.0f, "spectral-5");
        spectral.addScalarAndColor(-1.0f / 5.0f, "spectral-6");
        spectral.addScalarAndColor(-2.0f / 5.0f, "spectral-7");
        spectral.addScalarAndColor(-3.0f / 5.0f, "spectral-8");
        spectral.addScalarAndColor(-4.0f / 5.0f, "spectral-9");
        spectral.addScalarAndColor(-5.0f / 5.0f, "spectral-10");
        addPalette(spectral);
    }
    
    //palette from Alan Anticevic
    if (this->getPaletteByName("RY-BC-BL") == NULL) {
        Palette RY_BC_BL;
        RY_BC_BL.setName("RY-BC-BL");//named like "ROY-BIG-BL"
        this->addColor("rybcbl-y", 255, 255, 0);
        this->addColor("rybcbl-r", 255, 0, 0);
        this->addColor("rybcbl-bl", 0, 0, 0);
        this->addColor("rybcbl-b", 0, 0, 255);
        this->addColor("rybcbl-c", 0, 255, 255);
        RY_BC_BL.addScalarAndColor(1.0f, "rybcbl-y");
        RY_BC_BL.addScalarAndColor(0.5f, "rybcbl-r");
        RY_BC_BL.addScalarAndColor(0.0f, "rybcbl-bl");
        RY_BC_BL.addScalarAndColor(-0.5f, "rybcbl-b");
        RY_BC_BL.addScalarAndColor(-1.0f, "rybcbl-c");
        addPalette(RY_BC_BL);
    }
    
    //matplotlib's magma, approximated by reducing their massive 256-entry lookup table to 32 entries
    //Copyright (c) 2012-2015 Matplotlib Development Team; All Rights Reserved
    //see https://github.com/matplotlib/matplotlib/blob/master/LICENSE/LICENSE or debian/copyright for the matplotlib license, "All Rights Reserved" is required language by the license, but it is actually open source
    if (this->getPaletteByName("magma") == NULL) {
        Palette magma;
        magma.setName("magma");
        this->addColor("magma-black", 0, 0, 0);//negative and zero
        this->addColor("magma-1", 3, 3, 15);
        this->addColor("magma-2", 9, 7, 32);
        this->addColor("magma-3", 18, 13, 49);
        this->addColor("magma-4", 28, 16, 68);
        this->addColor("magma-5", 39, 18, 88);
        this->addColor("magma-6", 52, 16, 105);
        this->addColor("magma-7", 66, 15, 117);
        this->addColor("magma-8", 79, 18, 123);
        this->addColor("magma-9", 92, 22, 127);
        this->addColor("magma-10", 104, 28, 129);
        this->addColor("magma-11", 117, 33, 129);
        this->addColor("magma-12", 129, 37, 129);
        this->addColor("magma-13", 142, 42, 129);
        this->addColor("magma-14", 155, 46, 127);
        this->addColor("magma-15", 168, 50, 125);
        this->addColor("magma-16", 181, 54, 122);
        this->addColor("magma-17", 194, 59, 117);
        this->addColor("magma-18", 207, 64, 112);
        this->addColor("magma-19", 219, 71, 106);
        this->addColor("magma-20", 229, 80, 100);
        this->addColor("magma-21", 238, 91, 94);
        this->addColor("magma-22", 244, 105, 92);
        this->addColor("magma-23", 249, 120, 93);
        this->addColor("magma-24", 251, 135, 97);
        this->addColor("magma-25", 253, 150, 104);
        this->addColor("magma-26", 254, 165, 113);
        this->addColor("magma-27", 254, 180, 123);
        this->addColor("magma-28", 254, 194, 135);
        this->addColor("magma-29", 254, 209, 148);
        this->addColor("magma-30", 253, 224, 161);
        this->addColor("magma-31", 252, 238, 176);
        this->addColor("magma-32", 252, 253, 191);
        magma.addScalarAndColor(32.0f / 32.0f, "magma-32");
        magma.addScalarAndColor(31.0f / 32.0f, "magma-31");
        magma.addScalarAndColor(30.0f / 32.0f, "magma-30");
        magma.addScalarAndColor(29.0f / 32.0f, "magma-29");
        magma.addScalarAndColor(28.0f / 32.0f, "magma-28");
        magma.addScalarAndColor(27.0f / 32.0f, "magma-27");
        magma.addScalarAndColor(26.0f / 32.0f, "magma-26");
        magma.addScalarAndColor(25.0f / 32.0f, "magma-25");
        magma.addScalarAndColor(24.0f / 32.0f, "magma-24");
        magma.addScalarAndColor(23.0f / 32.0f, "magma-23");
        magma.addScalarAndColor(22.0f / 32.0f, "magma-22");
        magma.addScalarAndColor(21.0f / 32.0f, "magma-21");
        magma.addScalarAndColor(20.0f / 32.0f, "magma-20");
        magma.addScalarAndColor(19.0f / 32.0f, "magma-19");
        magma.addScalarAndColor(18.0f / 32.0f, "magma-18");
        magma.addScalarAndColor(17.0f / 32.0f, "magma-17");
        magma.addScalarAndColor(16.0f / 32.0f, "magma-16");
        magma.addScalarAndColor(15.0f / 32.0f, "magma-15");
        magma.addScalarAndColor(14.0f / 32.0f, "magma-14");
        magma.addScalarAndColor(13.0f / 32.0f, "magma-13");
        magma.addScalarAndColor(12.0f / 32.0f, "magma-12");
        magma.addScalarAndColor(11.0f / 32.0f, "magma-11");
        magma.addScalarAndColor(10.0f / 32.0f, "magma-10");
        magma.addScalarAndColor(9.0f / 32.0f, "magma-9");
        magma.addScalarAndColor(8.0f / 32.0f, "magma-8");
        magma.addScalarAndColor(7.0f / 32.0f, "magma-7");
        magma.addScalarAndColor(6.0f / 32.0f, "magma-6");
        magma.addScalarAndColor(5.0f / 32.0f, "magma-5");
        magma.addScalarAndColor(4.0f / 32.0f, "magma-4");
        magma.addScalarAndColor(3.0f / 32.0f, "magma-3");
        magma.addScalarAndColor(2.0f / 32.0f, "magma-2");
        magma.addScalarAndColor(1.0f / 32.0f, "magma-1");
        magma.addScalarAndColor(0.0f, "magma-black");//technically magma doesn't quite reach black at the minimum end (0 0 4), or ~(0 0 2) if extended by one, as it is a 256 element lookup, but it is close enough
        magma.addScalarAndColor(-1.0f, "magma-black");
        addPalette(magma);
    }

    if (this->getPaletteByName("JET256") == NULL) {
        Palette JET256;
        JET256.setName("JET256");

        //summary of original slow "lookup table" (if closer to previous implementation is desired):
        //start: 0 -> (0 0 132)
        //change: 0.121 -> (0 0 255)
        //change: 0.372 -> (0 255 255)
        //change: 0.623 -> (255 255 0)
        //change: 0.874 -> (255 0 0)
        //end: 1 -> (127 0 0)
        
        //alternative round-valued version via https://gist.github.com/bagrow/805122
        /*(0 0.0 0.0 0.5, \
           1 0.0 0.0 1.0, \
           2 0.0 0.5 1.0, \ <- redundant
           3 0.0 1.0 1.0, \
           4 0.5 1.0 0.5, \ <- redundant
           5 1.0 1.0 0.0, \
           6 1.0 0.5 0.0, \ <- redundant
           7 1.0 0.0 0.0, \
           8 0.5 0.0 0.0 )*/
        
        this->addColor("_J0", 0, 0, 127);//rounding to probably-intended colors
        this->addColor("_J1", 0, 0, 255);
        this->addColor("_J3", 0, 255, 255);//skipping redundant points
        this->addColor("_J5", 255, 255, 0);
        this->addColor("_J7", 255, 0, 0);
        this->addColor("_J8", 127, 0, 0);
        
        JET256.addScalarAndColor(1.0f, "_J8");
        JET256.addScalarAndColor(0.875f, "_J7");//also rounding to probably-intended control points
        JET256.addScalarAndColor(0.625f, "_J5");
        JET256.addScalarAndColor(0.375f, "_J3");
        JET256.addScalarAndColor(0.125f, "_J1");
        JET256.addScalarAndColor(0.0f, "_J0");
        
        addPalette(JET256);
    }
    
    //TSC: palettes that use "none" or are broken (psych no none) start here - exception: rbgyr20p, to keep it close to rbgyr20
    //----------------------------------------------------------------------
    // Psych palette
    //
    if (this->getPaletteByName("PSYCH") == NULL) {
        this->addColor("_pyell-oran",  0xff, 0xcc, 0x00 );
        this->addColor("_poran-red",  0xff, 0x44, 0x00 );
        this->addColor("_pblue",  0x00, 0x44, 0xff );
        this->addColor("_pltblue1",  0x00, 0x69, 0xff );
        this->addColor("_pltblue2",  0x00, 0x99, 0xff );
        this->addColor("_pbluecyan",  0x00, 0xcc, 0xff );
        
        Palette psych;
        psych.setName("PSYCH");
        //psych.setPositiveOnly(false);
        
        psych.addScalarAndColor(1.00f, "_yellow");
        psych.addScalarAndColor(0.75f, "_pyell-oran");
        psych.addScalarAndColor(0.50f, "_orange");
        psych.addScalarAndColor(0.25f, "_poran-red");
        psych.addScalarAndColor(0.05f, "none");
        psych.addScalarAndColor(-0.05f, "_pblue");
        psych.addScalarAndColor(-0.25f, "_pltblue1");
        psych.addScalarAndColor(-0.50f, "_pltblue2");
        psych.addScalarAndColor(-0.75f, "_pbluecyan");
        
        addPalette(psych);
    }
    //----------------------------------------------------------------------
    // Psych no-none palette
    //
    if (this->getPaletteByName("PSYCH-NO-NONE") == NULL) {
        this->addColor("_pyell-oran",  0xff, 0xcc, 0x00 );
        this->addColor("_poran-red",  0xff, 0x44, 0x00 );
        this->addColor("_pblue",  0x00, 0x44, 0xff );
        this->addColor("_pltblue1",  0x00, 0x69, 0xff );
        this->addColor("_pltblue2",  0x00, 0x99, 0xff );
        this->addColor("_pbluecyan",  0x00, 0xcc, 0xff );
        
        Palette psychNoNone;
        psychNoNone.setName("PSYCH-NO-NONE");
        //psychNoNone.setPositiveOnly(false);
        
        psychNoNone.addScalarAndColor(1.00f, "_yellow");
        psychNoNone.addScalarAndColor(0.75f, "_pyell-oran");
        psychNoNone.addScalarAndColor(0.50f, "_orange");
        psychNoNone.addScalarAndColor(0.25f, "_poran-red");
        psychNoNone.addScalarAndColor(0.0f, "_pblue");
        psychNoNone.addScalarAndColor(-0.25f, "_pltblue1");
        psychNoNone.addScalarAndColor(-0.50f, "_pltblue2");
        psychNoNone.addScalarAndColor(-0.75f, "_pbluecyan");
        
        addPalette(psychNoNone);
    }
    
    //----------------------------------------------------------------------
    // ROY-BIG palette
    //
    if (this->getPaletteByName("ROY-BIG") == NULL) {
        Palette royBig;
        royBig.setName("ROY-BIG");
        
        royBig.addScalarAndColor(1.00f, "_RGB_255_255_0");
        royBig.addScalarAndColor(0.875f, "_RGB_255_200_0");
        royBig.addScalarAndColor(0.750f, "_RGB_255_120_0");
        royBig.addScalarAndColor(0.625f, "_RGB_255_0_0");
        royBig.addScalarAndColor(0.500f, "_RGB_200_0_0");
        royBig.addScalarAndColor(0.375f, "_RGB_150_0_0");
        royBig.addScalarAndColor(0.250f, "_RGB_100_0_0");
        royBig.addScalarAndColor(0.125f, "_RGB_60_0_0");
        royBig.addScalarAndColor(0.000f, "none");
        royBig.addScalarAndColor(-0.125f, "_RGB_0_0_80");
        royBig.addScalarAndColor(-0.250f, "_RGB_0_0_170");
        royBig.addScalarAndColor(-0.375f, "_RGB_75_0_125");
        royBig.addScalarAndColor(-0.500f, "_RGB_125_0_160");
        royBig.addScalarAndColor(-0.625f, "_RGB_75_125_0");
        royBig.addScalarAndColor(-0.750f, "_RGB_0_200_0");
        royBig.addScalarAndColor(-0.875f, "_RGB_0_255_0");
        royBig.addScalarAndColor(-0.990f, "_RGB_0_255_255");
        royBig.addScalarAndColor(-1.00f, "_RGB_0_255_255");
        
        addPalette(royBig);
    }
    
    if (this->getPaletteByName("clear_brain") == NULL) {
        Palette clearBrain;
        clearBrain.setName("clear_brain");
        clearBrain.addScalarAndColor(1.0f , "_red");
        clearBrain.addScalarAndColor(0.9f , "_orange");
        clearBrain.addScalarAndColor(0.8f , "_oran-yell");
        clearBrain.addScalarAndColor(0.7f , "_yellow");
        clearBrain.addScalarAndColor(0.6f , "_limegreen");
        clearBrain.addScalarAndColor(0.5f , "_green");
        clearBrain.addScalarAndColor(0.4f , "_blue_videen7");
        clearBrain.addScalarAndColor(0.3f , "_blue_videen9");
        clearBrain.addScalarAndColor(0.2f , "_blue_videen11");
        clearBrain.addScalarAndColor(0.1f , "_purple2");
        clearBrain.addScalarAndColor(0.0f , "none");
        clearBrain.addScalarAndColor(-0.1f , "_cyan");
        clearBrain.addScalarAndColor(-0.2f , "_green");
        clearBrain.addScalarAndColor(-0.3f , "_limegreen");
        clearBrain.addScalarAndColor(-0.4f , "_violet");
        clearBrain.addScalarAndColor(-0.5f , "_hotpink");
        clearBrain.addScalarAndColor(-0.6f , "_white");
        clearBrain.addScalarAndColor(-0.7f , "_gry-dd");
        clearBrain.addScalarAndColor(-0.8f , "_gry-bb");
        clearBrain.addScalarAndColor(-0.9f , "_black");
        addPalette(clearBrain);
    }
    
    if (this->getPaletteByName("fidl") == NULL) {
        int Bright_Yellow[3] = { 0xee, 0xee, 0x55 };
        this->addColor("_Bright_Yellow", Bright_Yellow);
        int Mustard[3] = { 0xdd, 0xdd, 0x66 };
        this->addColor("_Mustard", Mustard);
        int Brown_Mustard[3] = { 0xdd, 0x99, 0x00 };
        this->addColor("_Brown_Mustard", Brown_Mustard);
        int Bright_Red[3] = { 0xff, 0x00, 0x00 };
        this->addColor("_Bright_Red", Bright_Red);
        int Fire_Engine_Red[3] = { 0xdd, 0x00, 0x00 };
        this->addColor("_Fire_Engine_Red", Fire_Engine_Red);
        int Brick[3] = { 0xbb, 0x00, 0x00 };
        this->addColor("_Brick", Brick);
        int Beet[3] = { 0x99, 0x00, 0x00 };
        this->addColor("_Beet", Beet);
        int Beaujolais[3] = { 0x77, 0x00, 0x00 };
        this->addColor("_Beaujolais", Beaujolais);
        int Burgundy[3] = { 0x55, 0x00, 0x00 };
        this->addColor("_Burgundy", Burgundy);
        int Thrombin[3] = { 0x11, 0x00, 0x00 };
        this->addColor("_Thrombin", Thrombin);
        int Deep_Green[3] = { 0x00, 0x11, 0x00 };
        this->addColor("_Deep_Green", Deep_Green);
        int British_Racing_Green[3] = { 0x00, 0x55, 0x00 };
        this->addColor("_British_Racing_Green", British_Racing_Green);
        int Kelp[3] = { 0x00, 0x77, 0x00 };
        this->addColor("_Kelp", Kelp);
        int Lime[3] = { 0x00, 0x99, 0x00 };
        this->addColor("_Lime", Lime);
        int Mint[3] = { 0x00, 0xbb, 0x00 };
        this->addColor("_Mint", Mint);
        int Brussell_Sprout[3] = { 0x00, 0xdd, 0x00 };
        this->addColor("_Brussell_Sprout", Brussell_Sprout);
        int Bright_Green[3] = { 0x00, 0xff, 0x00 };
        this->addColor("_Bright_Green", Bright_Green);
        int Periwinkle[3] = { 0x66, 0x66, 0xbb };
        this->addColor("_Periwinkle", Periwinkle);
        int Azure[3] = { 0x88, 0x88, 0xee };
        this->addColor("_Azure", Azure);
        int Turquoise[3] = { 0x00, 0xcc, 0xcc };
        this->addColor("_Turquoise", Turquoise);
        
        Palette fidl;
        fidl.setName("fidl");
        fidl.addScalarAndColor(1.0f, "_Bright_Yellow");
        fidl.addScalarAndColor(0.9f, "_Mustard");
        fidl.addScalarAndColor(0.8f, "_Brown_Mustard");
        fidl.addScalarAndColor(0.7f, "_Bright_Red");
        fidl.addScalarAndColor(0.6f, "_Fire_Engine_Red");
        fidl.addScalarAndColor(0.5f, "_Brick");
        fidl.addScalarAndColor(0.4f, "_Beet");
        fidl.addScalarAndColor(0.3f, "_Beaujolais");
        fidl.addScalarAndColor(0.2f, "_Burgundy");
        fidl.addScalarAndColor(0.1f, "_Thrombin");
        fidl.addScalarAndColor(0.0f, "none");
        fidl.addScalarAndColor(-0.1f, "_Deep_Green");
        fidl.addScalarAndColor(-0.2f, "_British_Racing_Green");
        fidl.addScalarAndColor(-0.3f, "_Kelp");
        fidl.addScalarAndColor(-0.4f, "_Lime");
        fidl.addScalarAndColor(-0.5f, "_Mint");
        fidl.addScalarAndColor(-0.6f, "_Brussell_Sprout");
        fidl.addScalarAndColor(-0.7f, "_Bright_Green");
        fidl.addScalarAndColor(-0.8f, "_Periwinkle");
        fidl.addScalarAndColor(-0.9f, "_Azure");
        fidl.addScalarAndColor(-1.0f, "_Turquoise");
        addPalette(fidl);
    }
    
    //------------------------------------------------------------------------
    //
    // Colors by Russ H.
    //
    int _rbgyr20_10[3] = { 0x00, 0xff, 0x00 };
    this->addColor("_rbgyr20_10", _rbgyr20_10);
    int _rbgyr20_15[3] = { 0xff, 0xff, 0x00 };
    this->addColor("_rbgyr20_15", _rbgyr20_15);
    int _rbgyr20_20[3] = { 0xff, 0x00, 0x00 };
    this->addColor("_rbgyr20_20", _rbgyr20_20);
    
    int _rbgyr20_21[3] = { 0x9d, 0x22, 0xc1 };
    this->addColor("_rbgyr20_21", _rbgyr20_21);
    int _rbgyr20_22[3] = { 0x81, 0x06, 0xa5 };
    this->addColor("_rbgyr20_22", _rbgyr20_22);
    int _rbgyr20_23[3] = { 0xff, 0xec, 0x00 };
    this->addColor("_rbgyr20_23", _rbgyr20_23);
    int _rbgyr20_24[3] = { 0xff, 0xd6, 0x00 };
    this->addColor("_rbgyr20_24", _rbgyr20_24);
    int _rbgyr20_25[3] = { 0xff, 0xbc, 0x00 };
    this->addColor("_rbgyr20_25", _rbgyr20_25);
    int _rbgyr20_26[3] = { 0xff, 0x9c, 0x00 };
    this->addColor("_rbgyr20_26", _rbgyr20_26);
    int _rbgyr20_27[3] = { 0xff, 0x7c, 0x00 };
    this->addColor("_rbgyr20_27", _rbgyr20_27);
    int _rbgyr20_28[3] = { 0xff, 0x5c, 0x00 };
    this->addColor("_rbgyr20_28", _rbgyr20_28);
    int _rbgyr20_29[3] = { 0xff, 0x3d, 0x00 };
    this->addColor("_rbgyr20_29", _rbgyr20_29);
    int _rbgyr20_30[3] = { 0xff, 0x23, 0x00 };
    this->addColor("_rbgyr20_30", _rbgyr20_30);
    int _rbgyr20_31[3] = { 0x00, 0xed, 0x12 };
    this->addColor("_rbgyr20_31", _rbgyr20_31);
    int _rbgyr20_32[3] = { 0x00, 0xd5, 0x2a };
    this->addColor("_rbgyr20_32", _rbgyr20_32);
    int _rbgyr20_33[3] = { 0x00, 0xb9, 0x46 };
    this->addColor("_rbgyr20_33", _rbgyr20_33);
    int _rbgyr20_34[3] = { 0x00, 0x9b, 0x64 };
    this->addColor("_rbgyr20_34", _rbgyr20_34);
    int _rbgyr20_35[3] = { 0x00, 0x7b, 0x84 };
    this->addColor("_rbgyr20_35", _rbgyr20_35);
    int _rbgyr20_36[3] = { 0x00, 0x5b, 0xa4 };
    this->addColor("_rbgyr20_36", _rbgyr20_36);
    int _rbgyr20_37[3] = { 0x00, 0x44, 0xbb };
    this->addColor("_rbgyr20_37", _rbgyr20_37);
    int _rbgyr20_38[3] = { 0x00, 0x24, 0xdb };
    this->addColor("_rbgyr20_38", _rbgyr20_38);
    int _rbgyr20_39[3] = { 0x00, 0x00, 0xff };
    this->addColor("_rbgyr20_39", _rbgyr20_39);
    
    int _rbgyr20_40[3] = { 0xff, 0xf1, 0x00 };
    this->addColor("_rbgyr20_40", _rbgyr20_40);
    int _rbgyr20_41[3] = { 0xff, 0xdc, 0x00 };
    this->addColor("_rbgyr20_41", _rbgyr20_41);
    int _rbgyr20_42[3] = { 0xff, 0xcb, 0x00 };
    this->addColor("_rbgyr20_42", _rbgyr20_42);
    int _rbgyr20_43[3] = { 0xff, 0xc2, 0x00 };
    this->addColor("_rbgyr20_43", _rbgyr20_43);
    int _rbgyr20_44[3] = { 0xff, 0xae, 0x00 };
    this->addColor("_rbgyr20_44", _rbgyr20_44);
    int _rbgyr20_45[3] = { 0xff, 0x9f, 0x00 };
    this->addColor("_rbgyr20_45", _rbgyr20_45);
    int _rbgyr20_46[3] = { 0xff, 0x86, 0x00 };
    this->addColor("_rbgyr20_46", _rbgyr20_46);
    int _rbgyr20_47[3] = { 0xff, 0x59, 0x00 };
    this->addColor("_rbgyr20_47", _rbgyr20_47);
    int _rbgyr20_48[3] = { 0x00, 0xff, 0x2d };
    this->addColor("_rbgyr20_48", _rbgyr20_48);
    int _rbgyr20_49[3] = { 0x00, 0xff, 0x65 };
    this->addColor("_rbgyr20_49", _rbgyr20_49);
    int _rbgyr20_50[3] = { 0x00, 0xff, 0xa5 };
    this->addColor("_rbgyr20_50", _rbgyr20_50);
    int _rbgyr20_51[3] = { 0x00, 0xff, 0xdd };
    this->addColor("_rbgyr20_51", _rbgyr20_51);
    int _rbgyr20_52[3] = { 0x00, 0xff, 0xff };
    this->addColor("_rbgyr20_52", _rbgyr20_52);
    int _rbgyr20_53[3] = { 0x00, 0xe9, 0xff };
    this->addColor("_rbgyr20_53", _rbgyr20_53);
    int _rbgyr20_54[3] = { 0x00, 0xad, 0xff };
    this->addColor("_rbgyr20_54", _rbgyr20_54);
    int _rbgyr20_55[3] = { 0x00, 0x69, 0xff };
    this->addColor("_rbgyr20_55", _rbgyr20_55);
    int _rbgyr20_56[3] = { 0xff, 0x00, 0xb9 };
    this->addColor("_rbgyr20_56", _rbgyr20_56);
    int _rbgyr20_57[3] = { 0xff, 0x00, 0x63 };
    this->addColor("_rbgyr20_57", _rbgyr20_57);
    int _rbgyr20_58[3] = { 0xff, 0x05, 0x00 };
    this->addColor("_rbgyr20_58", _rbgyr20_58);
    int _rbgyr20_59[3] = { 0xff, 0x32, 0x00 };
    this->addColor("_rbgyr20_59", _rbgyr20_59);
    int _rbgyr20_60[3] =  { 0xff, 0x70, 0x00 };
    this->addColor("_rbgyr20_60", _rbgyr20_60);
    int _rbgyr20_61[3] = { 0xff, 0xa4, 0x00 };
    this->addColor("_rbgyr20_61", _rbgyr20_61);
    int _rbgyr20_62[3] = { 0xff, 0xba, 0x00 };
    this->addColor("_rbgyr20_62", _rbgyr20_62);
    int _rbgyr20_63[3] = { 0xff, 0xd3, 0x00 };
    this->addColor("_rbgyr20_63", _rbgyr20_63);
    int _rbgyr20_64[3] = { 0x42, 0x21, 0xdb };
    this->addColor("_rbgyr20_64", _rbgyr20_64);
    int _rbgyr20_65[3] = { 0x10, 0x08, 0xf6 };
    this->addColor("_rbgyr20_65", _rbgyr20_65);
    int _rbgyr20_66[3] = { 0x00, 0x13, 0xff };
    this->addColor("_rbgyr20_66", _rbgyr20_66);
    int _rbgyr20_67[3] = { 0x00, 0x5b, 0xff };
    this->addColor("_rbgyr20_67", _rbgyr20_67);
    int _rbgyr20_68[3] = { 0x00, 0xb3, 0xff };
    this->addColor("_rbgyr20_68", _rbgyr20_68);
    int _rbgyr20_69[3] = { 0x00, 0xfc, 0xff };
    this->addColor("_rbgyr20_69", _rbgyr20_69);
    int _rbgyr20_70[3] = { 0x00, 0xff, 0xcd };
    this->addColor("_rbgyr20_70", _rbgyr20_70);
    int _rbgyr20_71[3] = { 0x00, 0xff, 0x74 };
    this->addColor("_rbgyr20_71", _rbgyr20_71);
    int _rbgyr20_72[3] = { 0xff, 0x00, 0xf9 };
    this->addColor("_rbgyr20_72", _rbgyr20_72);
    int _rbgyr20_73[3] = { 0x62, 0x31, 0xc9 };
    this->addColor("_rbgyr20_73", _rbgyr20_73);
    
    //------------------------------------------------------------------------
    //
    // Palette by Russ H.
    //
    if (this->getPaletteByName("raich4_clrmid") == NULL) {
        Palette r4;
        r4.setName("raich4_clrmid");
        r4.addScalarAndColor(1.000000f, "_rbgyr20_20");
        r4.addScalarAndColor(0.900000f, "_rbgyr20_30");
        r4.addScalarAndColor(0.800000f, "_rbgyr20_29");
        r4.addScalarAndColor(0.700000f, "_rbgyr20_28");
        r4.addScalarAndColor(0.600000f, "_rbgyr20_27");
        r4.addScalarAndColor(0.500000f, "_rbgyr20_26");
        r4.addScalarAndColor(0.400000f, "_rbgyr20_25");
        r4.addScalarAndColor(0.300000f, "_rbgyr20_24");
        r4.addScalarAndColor(0.200000f, "_rbgyr20_23");
        r4.addScalarAndColor(0.100000f, "_rbgyr20_15");
        r4.addScalarAndColor(0.000000f, "none");
        r4.addScalarAndColor(-0.100000f, "_rbgyr20_10");
        r4.addScalarAndColor(-0.200000f, "_rbgyr20_31");
        r4.addScalarAndColor(-0.300000f, "_rbgyr20_32");
        r4.addScalarAndColor(-0.400000f, "_rbgyr20_33");
        r4.addScalarAndColor(-0.500000f, "_rbgyr20_34");
        r4.addScalarAndColor(-0.600000f, "_rbgyr20_35");
        r4.addScalarAndColor(-0.700000f, "_rbgyr20_36");
        r4.addScalarAndColor(-0.800000f, "_rbgyr20_37");
        r4.addScalarAndColor(-0.900000f, "_rbgyr20_38");
        r4.addScalarAndColor(-1.000000f, "_rbgyr20_39");
        addPalette(r4);
    }
    
    //------------------------------------------------------------------------
    //
    // Palette by Russ H.
    //
    if (this->getPaletteByName("raich6_clrmid") == NULL) {
        Palette r6;
        r6.setName("raich6_clrmid");
        r6.addScalarAndColor(1.000000f, "_rbgyr20_20");
        r6.addScalarAndColor(0.900000f, "_rbgyr20_47");
        r6.addScalarAndColor(0.800000f, "_rbgyr20_46");
        r6.addScalarAndColor(0.700000f, "_rbgyr20_45");
        r6.addScalarAndColor(0.600000f, "_rbgyr20_44");
        r6.addScalarAndColor(0.500000f, "_rbgyr20_43");
        r6.addScalarAndColor(0.400000f, "_rbgyr20_42");
        r6.addScalarAndColor(0.300000f, "_rbgyr20_41");
        r6.addScalarAndColor(0.200000f, "_rbgyr20_40");
        r6.addScalarAndColor(0.100000f, "_rbgyr20_15");
        r6.addScalarAndColor(0.000000f, "none");
        r6.addScalarAndColor(-0.100000f, "_rbgyr20_10");
        r6.addScalarAndColor(-0.200000f, "_rbgyr20_48");
        r6.addScalarAndColor(-0.300000f, "_rbgyr20_49");
        r6.addScalarAndColor(-0.400000f, "_rbgyr20_50");
        r6.addScalarAndColor(-0.500000f, "_rbgyr20_51");
        r6.addScalarAndColor(-0.600000f, "_rbgyr20_52");
        r6.addScalarAndColor(-0.700000f, "_rbgyr20_53");
        r6.addScalarAndColor(-0.800000f, "_rbgyr20_54");
        r6.addScalarAndColor(-0.900000f, "_rbgyr20_55");
        r6.addScalarAndColor(-1.000000f, "_rbgyr20_39");
        addPalette(r6);
    }
    
    //------------------------------------------------------------------------
    //
    // Palette by Russ H.
    //
    if (this->getPaletteByName("HSB8_clrmid") == NULL) {
        Palette hsb8;
        hsb8.setName("HSB8_clrmid");
        hsb8.addScalarAndColor(1.000000f, "_rbgyr20_15");
        hsb8.addScalarAndColor(0.900000f, "_rbgyr20_63");
        hsb8.addScalarAndColor(0.800000f, "_rbgyr20_62");
        hsb8.addScalarAndColor(0.700000f, "_rbgyr20_61");
        hsb8.addScalarAndColor(0.600000f, "_rbgyr20_60");
        hsb8.addScalarAndColor(0.500000f, "_rbgyr20_59");
        hsb8.addScalarAndColor(0.400000f, "_rbgyr20_58");
        hsb8.addScalarAndColor(0.300000f, "_rbgyr20_57");
        hsb8.addScalarAndColor(0.200000f, "_rbgyr20_56");
        hsb8.addScalarAndColor(0.100000f, "_rbgyr20_72");
        hsb8.addScalarAndColor(0.000000f, "none");
        hsb8.addScalarAndColor(-0.100000f, "_rbgyr20_73");
        hsb8.addScalarAndColor(-0.200000f, "_rbgyr20_64");
        hsb8.addScalarAndColor(-0.300000f, "_rbgyr20_65");
        hsb8.addScalarAndColor(-0.400000f, "_rbgyr20_66");
        hsb8.addScalarAndColor(-0.500000f, "_rbgyr20_67");
        hsb8.addScalarAndColor(-0.600000f, "_rbgyr20_68");
        hsb8.addScalarAndColor(-0.700000f, "_rbgyr20_69");
        hsb8.addScalarAndColor(-0.800000f, "_rbgyr20_70");
        hsb8.addScalarAndColor(-0.900000f, "_rbgyr20_71");
        hsb8.addScalarAndColor(-1.000000f, "_rbgyr20_10");
        addPalette(hsb8);
    }
    
    //----------------------------------------------------------------------
    // Positive/Negative palette
    //
    if (this->getPaletteByName("POS_NEG") == NULL) {
        this->addColor("pos_neg_blue",  0x00, 0x00, 0xff );
        this->addColor("pos_neg_red",  0xff, 0x00, 0x00 );
        
        Palette posNeg;
        posNeg.setName("POS_NEG");
        
        posNeg.addScalarAndColor(1.00f, "pos_neg_red");
        posNeg.addScalarAndColor(0.0001f, "none");
        posNeg.addScalarAndColor(-0.0001f, "pos_neg_blue");
        
        addPalette(posNeg);
    }
    
    if (modifiedStatus == false) {
        this->clearModified();//this clears modified status on all members, to match our status
    }
}

/**
 * @return The structure for this file.
 */
StructureEnum::Enum 
PaletteFile::getStructure() const
{
    // palette files do not have structure
    return StructureEnum::INVALID;
}

/**
 * Set the structure for this file.
 * @param structure
 *   New structure for this file.
 */
void 
PaletteFile::setStructure(const StructureEnum::Enum /*structure*/)
{
    // palette files do not have structure
}

/**
 * @return Get access to the file's metadata.
 */
GiftiMetaData* 
PaletteFile::getFileMetaData()
{
    return this->metadata;
}

/**
 * @return Get access to unmodifiable file's metadata.
 */
const GiftiMetaData* 
PaletteFile::getFileMetaData() const
{
    return this->metadata;
}

/**
 * Set the palette mapping based upon the given file type,
 * file name, data name, and data.
 *
 * @param paletteColorMapping
 *    Palette color mapping that is setup.
 * @param dataFileType
 *    Type of data file.
 * @param fileName
 *    Name of file.
 * @param dataName
 *    Name of data.
 * @param data
 *    The data.
 * @param numberOfDataElements
 *    Number of elements in data.
 */
void
PaletteFile::setDefaultPaletteColorMapping(PaletteColorMapping* paletteColorMapping,
                                           const DataFileTypeEnum::Enum& dataFileType,
                                           const AString& fileNameIn,
                                           const AString& dataNameIn,
                                           const float* data,
                                           const int32_t numberOfDataElements)
{
    bool isShapeCurvatureData = false;
    bool isShapeDepthData = false;
    bool isShapeData = false;
    bool isVolumeAnatomyData = false;
    
    const AString fileName = fileNameIn.toLower();
    const AString dataName = dataNameIn.toLower();
    
    bool invalid = false;
    bool checkShapeFile = false;
    bool checkVolume = false;
    switch (dataFileType) {
        case DataFileTypeEnum::ANNOTATION:
            invalid = true;
            break;
        case DataFileTypeEnum::BORDER:
            invalid = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
            invalid = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
            checkShapeFile = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
            invalid = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
            invalid = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
            break;
        case DataFileTypeEnum::FOCI:
            invalid = true;
            break;
        case DataFileTypeEnum::IMAGE:
            invalid = true;
            break;
        case DataFileTypeEnum::LABEL:
            invalid = true;
            break;
        case DataFileTypeEnum::METRIC:
            checkShapeFile = true;
            break;
        case DataFileTypeEnum::PALETTE:
            invalid = true;
            break;
        case DataFileTypeEnum::RGBA:
            invalid = true;
            break;
        case DataFileTypeEnum::SCENE:
            invalid = true;
            break;
        case DataFileTypeEnum::SPECIFICATION:
            invalid = true;
            break;
        case DataFileTypeEnum::SURFACE:
            invalid = true;
            break;
        case DataFileTypeEnum::UNKNOWN:
            invalid = true;
            break;
        case DataFileTypeEnum::VOLUME:
            checkVolume = true;
            break;
    }
    
    if (invalid) {
        return;
    }
    
    if (checkShapeFile) {
        if (dataName.contains("curv")) {
            isShapeData = true;
            isShapeCurvatureData = true;
        }
        else if (dataName.contains("depth")) {
            isShapeData = true;
            isShapeDepthData = true;
        }
        else if (dataName.contains("shape")) {
            isShapeData = true;
        }
        else if (fileName.contains("curv")) {
            isShapeData = true;
            isShapeCurvatureData = true;
        }
        else if (fileName.contains("depth")) {
            isShapeData = true;
            isShapeDepthData = true;
        }
        else if (fileName.contains("shape")) {
            isShapeData = true;
        }
    }
    
    float minValue = std::numeric_limits<float>::max();
    float maxValue = -minValue;
    
    for (int32_t i = 0; i < numberOfDataElements; i++) {
        const float d = data[i];
        if (d > maxValue) {
            maxValue = d;
        }
        if (d < minValue) {
            minValue = d;
        }
    }
    //bool havePositiveData = (maxValue > 0);//unused, commenting out to prevent compiler warning
    bool haveNegativeData = (minValue < 0);
    
    if (checkVolume) {
        if ((minValue >= 0)
            && (maxValue <= 255.0)) {
            isVolumeAnatomyData = true;
        }
    }
    
    if (isVolumeAnatomyData) {
        paletteColorMapping->setThresholdType(PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF);
        paletteColorMapping->setSelectedPaletteName("Gray_Interp_Positive");
        paletteColorMapping->setInterpolatePaletteFlag(true);
        paletteColorMapping->setScaleMode(PaletteScaleModeEnum::MODE_AUTO_SCALE_PERCENTAGE);
        paletteColorMapping->setAutoScalePercentageNegativeMaximum(98.0);
        paletteColorMapping->setAutoScalePercentageNegativeMinimum(2.0);
        paletteColorMapping->setAutoScalePercentagePositiveMinimum(2.0);
        paletteColorMapping->setAutoScalePercentagePositiveMaximum(98.0);
    }
    else if (isShapeData) {
        paletteColorMapping->setThresholdType(PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF);
        paletteColorMapping->setSelectedPaletteName("Gray_Interp");
        paletteColorMapping->setInterpolatePaletteFlag(true);
        if (isShapeDepthData) {
            paletteColorMapping->setScaleMode(PaletteScaleModeEnum::MODE_USER_SCALE);
            paletteColorMapping->setUserScaleNegativeMaximum(-30.0);
            paletteColorMapping->setUserScaleNegativeMinimum(0.0);
            paletteColorMapping->setUserScalePositiveMinimum(0.0);
            paletteColorMapping->setUserScalePositiveMaximum(10.0);
        }
        else if (isShapeCurvatureData) {
//            paletteColorMapping->setScaleMode(PaletteScaleModeEnum::MODE_USER_SCALE);
//            paletteColorMapping->setUserScaleNegativeMaximum(-1.5);
//            paletteColorMapping->setUserScaleNegativeMinimum(0.0);
//            paletteColorMapping->setUserScalePositiveMinimum(0.0);
//            paletteColorMapping->setUserScalePositiveMaximum(1.5);
            paletteColorMapping->setScaleMode(PaletteScaleModeEnum::MODE_AUTO_SCALE_PERCENTAGE);
            paletteColorMapping->setAutoScalePercentageNegativeMaximum(98.0);
            paletteColorMapping->setAutoScalePercentageNegativeMinimum(2.0);
            paletteColorMapping->setAutoScalePercentagePositiveMinimum(2.0);
            paletteColorMapping->setAutoScalePercentagePositiveMaximum(98.0);
        }
        else {
            paletteColorMapping->setScaleMode(PaletteScaleModeEnum::MODE_AUTO_SCALE_PERCENTAGE);
            paletteColorMapping->setAutoScalePercentageNegativeMaximum(98.0);
            paletteColorMapping->setAutoScalePercentageNegativeMinimum(2.0);
            paletteColorMapping->setAutoScalePercentagePositiveMinimum(2.0);
            paletteColorMapping->setAutoScalePercentagePositiveMaximum(98.0);
        }
        paletteColorMapping->setDisplayNegativeDataFlag(true);
        paletteColorMapping->setDisplayPositiveDataFlag(true);
        paletteColorMapping->setDisplayZeroDataFlag(true);
    }
    else {
        if (haveNegativeData) {
            paletteColorMapping->setThresholdType(PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF);
            paletteColorMapping->setSelectedPaletteName("videen-style");
            paletteColorMapping->setSelectedPaletteName("ROY-BIG-BL");
            paletteColorMapping->setInterpolatePaletteFlag(true);
            paletteColorMapping->setScaleMode(PaletteScaleModeEnum::MODE_AUTO_SCALE_PERCENTAGE);
            paletteColorMapping->setAutoScalePercentageNegativeMaximum(98.0);
            paletteColorMapping->setAutoScalePercentageNegativeMinimum(2.0);
            paletteColorMapping->setAutoScalePercentagePositiveMinimum(2.0);
            paletteColorMapping->setAutoScalePercentagePositiveMaximum(98.0);
            paletteColorMapping->setDisplayNegativeDataFlag(true);
            paletteColorMapping->setDisplayPositiveDataFlag(true);
            paletteColorMapping->setDisplayZeroDataFlag(false);
        }
        else {
            paletteColorMapping->setThresholdType(PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF);
            paletteColorMapping->setSelectedPaletteName("videen-style");
            paletteColorMapping->setSelectedPaletteName("ROY-BIG-BL");
            paletteColorMapping->setInterpolatePaletteFlag(true);
            paletteColorMapping->setScaleMode(PaletteScaleModeEnum::MODE_AUTO_SCALE_PERCENTAGE);
            paletteColorMapping->setAutoScalePercentageNegativeMaximum(98.0);
            paletteColorMapping->setAutoScalePercentageNegativeMinimum(2.0);
            paletteColorMapping->setAutoScalePercentagePositiveMinimum(2.0);
            paletteColorMapping->setAutoScalePercentagePositiveMaximum(98.0);
            paletteColorMapping->setDisplayNegativeDataFlag(true);
            paletteColorMapping->setDisplayPositiveDataFlag(true);
            paletteColorMapping->setDisplayZeroDataFlag(false);
        }
    }
    paletteColorMapping->clearModified();
}

