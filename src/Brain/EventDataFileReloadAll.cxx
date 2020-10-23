
/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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

#define __EVENT_DATA_FILE_RELOAD_ALL_DECLARE__
#include "EventDataFileReloadAll.h"
#undef __EVENT_DATA_FILE_RELOAD_ALL_DECLARE__

#include "CaretAssert.h"
#include "CaretDataFile.h"
#include "CaretResult.h"
#include "EventManager.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventDataFileReloadAll 
 * \brief Event for reloading all data files
 * \ingroup Brain
 */

/**
 * Get all reloadable data files
 * @param brain
 *    Brain for getting files
 * @param reloadableFilesOut
 *    Output containing all reloadable data files
 * @return
 *    CaretResult with success/error
 */
std::unique_ptr<CaretResult>
EventDataFileReloadAll::getReloadableFiles(Brain* brain,
                                           std::vector<CaretDataFile*>& reloadableFilesOut)
{
    EventDataFileReloadAll event(EventDataFileReloadAll::Mode::GET_FILES,
                                 brain);
    EventManager::get()->sendEvent(event.getPointer());
    if (event.isError()) {
        return CaretResult::newInstanceError(event.getErrorMessage());
    }

    reloadableFilesOut = event.getReloadableFiles();
    return CaretResult::newInstanceSuccess();
}

/**
 * Reload data files
 * @param brain
 *    Brain that has its files reloaded
 * @return
 *    CaretResult with success/error
 */
std::unique_ptr<CaretResult>
EventDataFileReloadAll::reloadAllFiles(Brain* brain)
{
    EventDataFileReloadAll event(EventDataFileReloadAll::Mode::RELOAD_FILES,
                                 brain);
    EventManager::get()->sendEvent(event.getPointer());
    if (event.isError()) {
        return CaretResult::newInstanceError(event.getErrorMessage());
    }
    
    return CaretResult::newInstanceSuccess();
}


/**
 * Constructor.
 * @param mode
 *    The mode
 * @param brain
 *    Brain that contains files for reloading
 */
EventDataFileReloadAll::EventDataFileReloadAll(const Mode mode,
                                               Brain* brain)
: Event(EventTypeEnum::EVENT_DATA_FILE_RELOAD_ALL),
m_mode(mode),
m_brain(brain)
{
    CaretAssert(brain);
}

/**
 * Destructor.
 */
EventDataFileReloadAll::~EventDataFileReloadAll()
{
}

/**
 * @return The mode
 */
EventDataFileReloadAll::Mode
EventDataFileReloadAll::getMode()
{
    return m_mode;
}

/**
 * @return Brain that is to reload all files
 */
Brain*
EventDataFileReloadAll::getBrain()
{
    return m_brain;
}

/**
 * @return All loaded files that are reloadable.  Surfaces and volume will be in vector
 * before any other data files.
 */
std::vector<CaretDataFile*>
EventDataFileReloadAll::getReloadableFiles() const
{
    std::vector<CaretDataFile*> surfadeAndVolumeFiles;
    std::vector<CaretDataFile*> otherFiles;
    
    for (auto& df : m_reloadableFiles) {
        switch (df->getDataFileType()) {
            case DataFileTypeEnum::SURFACE:
            case DataFileTypeEnum::VOLUME:
                surfadeAndVolumeFiles.push_back(df);
                break;
            default:
                otherFiles.push_back(df);
                break;
        }
    }
    
    surfadeAndVolumeFiles.insert(surfadeAndVolumeFiles.end(),
                                 otherFiles.begin(),
                                 otherFiles.end());
    return surfadeAndVolumeFiles;
}

/**
 * Add a file to reloadable data files
 * @param caretDataFile
 *    Data file for reloading
 */
void
EventDataFileReloadAll::addReloadableFile(CaretDataFile* caretDataFile)
{
    CaretAssert(caretDataFile);
    m_reloadableFiles.push_back(caretDataFile);
}
