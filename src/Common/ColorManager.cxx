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

#include "ColorManager.h"
using namespace caret;

ColorManager::ColorManager()
{
    init();
}


void ColorManager::removeColor(ColorId colorId)
{
    takenColors[colorId] = false;
}

ColorId ColorManager::getNewColor()
{    

    for(ColorId i = 0;i< takenColors.size();i++)
    {
        if(takenColors[i] == false)
        {
            takenColors[i] = true;
            return i;
        }
    }
    return -1;
}

int64_t ColorManager::getMaxColors()
{
    return colors.size();
}

Qt::GlobalColor ColorManager::getColor(ColorId colorId)
{
    return colors[colorId];
}

void ColorManager::init()
{
    colors.push_back(Qt::red);
    colors.push_back(Qt::green);
    colors.push_back(Qt::blue);
    colors.push_back(Qt::cyan);
    colors.push_back(Qt::magenta);
    colors.push_back(Qt::yellow);
    colors.push_back(Qt::darkRed);
    colors.push_back(Qt::darkGreen);
    colors.push_back(Qt::darkBlue);
    colors.push_back(Qt::darkCyan);
    colors.push_back(Qt::darkMagenta);
    colors.push_back(Qt::darkYellow);

    for(int i = 0;i<(int)colors.size();i++)
    {
        takenColors.push_back(false);
    }
}


