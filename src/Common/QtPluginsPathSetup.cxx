
/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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

#define __QT_PLUGINS_PATH_SETUP_DECLARE__
#include "QtPluginsPathSetup.h"
#undef __QT_PLUGINS_PATH_SETUP_DECLARE__

#include <iostream>

#include <QCoreApplication>
#include <QDir>
#include <QtGlobal>

#include "AString.h"
#include "CaretAssert.h"
#include "CaretLogger.h"

using namespace caret;
    
/**
 * \class caret::QtPluginsPathSetup 
 * \brief Setup Qt Plugins Path for MacOS
 * \ingroup Common
 */

/**
 * Constructor.
 */
QtPluginsPathSetup::QtPluginsPathSetup()
{
    
}

/**
 * Destructor.
 */
QtPluginsPathSetup::~QtPluginsPathSetup()
{
}

/**
 * Setup the plugins path
 */
void
QtPluginsPathSetup::setupPluginsPath()
{
#ifdef CARET_OS_LINUX
#endif // CARET_OS_LINUX
    
#ifdef CARET_OS_MACOSX
    const AString errorPrefix("Setup Plugins for MacOS.  ");
    
    /*
     * App path is something like <some-path>/wb_view.app/Contents/MacOS
     * and that directory contains the 'wb_view' application
     */
    const AString appPath(QCoreApplication::applicationDirPath());
    std::cout << "App path: " << appPath << std::endl;
    QDir appDir(appPath);
    
    /*
     * Move up one directory into "Contents"
     */
    if ( ! appDir.cdUp()) {
        CaretLogSevere(errorPrefix
                       + "Failed to cdUp() from MacOS App Path: "
                       + appPath);
        return;
    }
    std::cout << "Contents directory: " << appDir.canonicalPath().toStdString() << std::endl;
    
    /*
     * cd into the plugins directory
     */
    const AString pluginsDirName("PlugIns");
    if ( ! appDir.cd(pluginsDirName)) {
        CaretLogSevere(errorPrefix
                       + "Failed to cd into "
                       + pluginsDirName);
        return;
    }
    const AString pluginsPath(appDir.canonicalPath());
    std::cout << "Plugins path: " << pluginsPath << std::endl;
    
    /*
     * Add the plugins directory to the applications library paths
     */
//    QCoreApplication::addLibraryPath(pluginsPath);
    
    qputenv("QT_PLUGIN_PATH",
            pluginsPath.toLatin1());
#endif // CARET_OS_MACOSX
    
#ifdef CARET_OS_WINDOWS
#endif // CARET_OS_MACOSX
}

