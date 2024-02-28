/*******************************************************************************

Copyright The University of Auckland

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*******************************************************************************/

//==============================================================================
// macOS utilities
//==============================================================================

#include "macos.h"

//==============================================================================

#import <AppKit/NSWindow.h>

//==============================================================================

namespace OpenCOR {

//==============================================================================

void removeMacosSpecificMenuItems()
{
    // Remove (disable) the "Start Dictation..." and "Emoji & Symbols" menu
    // items from the "Edit" menu

    [[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"NSDisabledDictationMenuItem"];
    [[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"NSDisabledCharacterPaletteMenuItem"];

//#ifdef AVAILABLE_MAC_OS_X_VERSION_10_12_AND_LATER
//    // Remove (don't allow) the "Show Tab Bar" menu item from the "View" menu,
//    // if supported
//
//    if ([NSWindow respondsToSelector:@selector(allowsAutomaticWindowTabbing)])
//        NSWindow.allowsAutomaticWindowTabbing = NO;
//#endif

    // Remove (don't have) the "Enter Full Screen" menu item from the "View"
    // menu

    [[NSUserDefaults standardUserDefaults] setBool:NO forKey:@"NSFullScreenMenuItemEverywhere"];
}

//==============================================================================

}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================

