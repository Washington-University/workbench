
/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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

#include "MacDarkTheme.h"

#import <Cocoa/Cocoa.h>

bool macDarkThemeAvailable()
{
    if (__builtin_available(macOS 10.14, *))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool macIsInDarkTheme()
{
    if (__builtin_available(macOS 10.14, *))
    {
        auto appearance = [NSApp.effectiveAppearance bestMatchFromAppearancesWithNames:
                           @[ NSAppearanceNameAqua, NSAppearanceNameDarkAqua ]];
        return [appearance isEqualToString:NSAppearanceNameDarkAqua];
    }
    return false;
}

void macSetToDarkTheme()
{
    // https://stackoverflow.com/questions/55925862/how-can-i-set-my-os-x-application-theme-in-code
    if (__builtin_available(macOS 10.14, *))
    {
        [NSApp setAppearance:[NSAppearance appearanceNamed:NSAppearanceNameDarkAqua]];
    }
}

void macSetToLightTheme()
{
    // https://stackoverflow.com/questions/55925862/how-can-i-set-my-os-x-application-theme-in-code
    if (__builtin_available(macOS 10.14, *))
    {
        [NSApp setAppearance:[NSAppearance appearanceNamed:NSAppearanceNameAqua]];
    }
}

void macSetToAutoTheme()
{
    if (__builtin_available(macOS 10.14, *))
    {
        [NSApp setAppearance:nil];
    }
}
