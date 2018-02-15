
/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#define __BEST_PRACTICES_DIALOG_DECLARE__
#include "BestPracticesDialog.h"
#undef __BEST_PRACTICES_DIALOG_DECLARE__

#include <QLabel>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BestPracticesDialog 
 * \brief Dialog that displays "Best Practices" information.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param infoMode
 *     Information Mode of dialog
 * @param textMode
 *     Text mode
 * @param parent
 *     Parent widget.
 */
BestPracticesDialog::BestPracticesDialog(const InfoMode infoMode,
                                         QWidget* parent)
: WuQDialogNonModal("Best Practices",
                    parent),
m_infoMode(infoMode),
m_textMode(TextMode::FULL)
{
    setApplyButtonText("");
    setDeleteWhenClosed(true);
    
    QLabel* label = new QLabel(getTextForInfoMode(infoMode,
                                                  m_textMode));
    label->setWordWrap(true);
    
    QWidget* widget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(label);
    
//    widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    setCentralWidget(widget, WuQDialog::SCROLL_AREA_AS_NEEDED_VERT_NO_HORIZ);

    WuQtUtilities::limitWindowSizePercentageOfMaximum(this, 90.0, 80.0);
}

/**
 * Destructor.
 */
BestPracticesDialog::~BestPracticesDialog()
{
}

/**
 * Get the text for the given information mode and text mode.
 *
 * @param infoMode
 *     Information Mode of dialog
 * @param textMode
 *     Text mode
 */
AString
BestPracticesDialog::getTextForInfoMode(const InfoMode infoMode,
                                        const TextMode textMode)
{
    AString text;
    
    switch (infoMode) {
        case InfoMode::LOCK_ASPECT_BEST_PRACTICES:
            text = getLockAspectText(textMode);
            break;
    }
    return text;
}

AString
BestPracticesDialog::getLockAspectText(const TextMode textMode)
{
    AString introText;
    {
        switch (textMode) {
            case BRIEF:
                introText = ("Aspect is unlocked and it is <b>strongly recommended</b> that aspect is locked and remains locked "
                             "when annotations are present.  Failing to lock aspect or unlocking the aspect may "
                             "cause annotations to move from their original locations when the graphics region size changes."
                             "<p>"
                             "When creating annotations and scenes, follow these best practices:");
                break;
            case FULL:
                introText = ("Describing annotations and scenes is best done using an analogy with Microsoft PowerPoint.  "
                             "PowerPoint allows one to create a presentation consisting of slides saved into a PowerPoint file.  "
                             "These slides are annotated with text and various shapes.  In wb_view, a scene is analogous "
                             "to a PowerPoint slide and a Scene File contains multiple scenes just like a PowerPoint "
                             "file contains multiple slides.  In wb_view, one annotates a scene similar to the way one "
                             "annotates a slide in PowerPoint.  A Scene saves ‘the state’ of wb_view so that when "
                             "the scene is loaded at a later time, by either the creator of the scene or another "
                             "user, the user-interface and the orientation of the brain models replicates ‘the state’ at the time the "
                             "scene was created."
                             "<p>"
                             "When creating scenes and annotations, following a set of “best practices” ensures "
                             "that one efficiently creates scenes that are properly displayed at a later time "
                             "and when shared with other users.");
                break;
        }
    }
    
    std::vector<AString> bulletLines;
    
    {
        AString text("If a multi-tab scene will be created, enter Tile Tabs (View Menu->Enter Tile Tabs).  ");
        switch (textMode) {
            case BRIEF:
                break;
            case FULL:
                text.append("Tile Tabs displays all of the window's tabs in a grid pattern.  "
                            "The number of row and columns in the Tile Tabs view is edited on the "
                            "Tile Tabs Configuration Dialog (View Menu->Tile Tabs Configuration->Create and Edit).  "
                            "Advanced parameters include “stretch factors” that allow one to increase the "
                            "size of a row or column relative to other rows and columns.  One may also save the "
                            "configuration for future use.  If a scene is created, the configuration is added to the scene "
                            "so that the configuration is available when the scene is displayed.");
                break;
        }
        
        
        bulletLines.push_back(text);
    }
    
    {
        AString text("Adjust size of window.  ");
        switch (textMode) {
            case BRIEF:
                break;
            case FULL:
                text.append("It is important to adust the size of the window prior to entering Annotations Mode "
                            "(further down in this list).");
                break;
        }
        bulletLines.push_back(text);
    }
    
    {
        AString text;
        switch (textMode) {
            case BRIEF:
                text = ("Setup view of the models (pan/rotate/zoom).  ");
                break;
            case FULL:
                text = ("Setup viewa of the models.  "
                        "Pan (drag mouse with SHIFT key down), "
                        "Rotate (drag mouse), "
                        "Zoom (drag mouse with Control Key.  Use the Command Key on Mac.");
                break;
        }
        bulletLines.push_back(text);
    }
    
    {
        AString text("Lock Aspect Ratio (click Lock Aspect button in right side of Toolbar).  ");
        switch (textMode) {
            case BRIEF:
                break;
            case FULL:
                text.append("As a general rule, once the aspect ratio is locked, it should remain locked.  "
                            "The aspect ratio is the ratio of height divided by width.  "
                            "Annotations in tab and window spaces "
                            "are positioned using percentage coordinates that range from 0% to 100% with "
                            "0% at the left (x) and bottom (y) and 100% at the right (x) and top (y).  "
                            "If the window is resized with the aspect ratio unlocked and there is a change "
                            "in the aspect ratio, annotations in tab/window space may no longer "
                            "appear in the correct location (such as over an anatomical feature).  Locking "
                            "the aspect ratio adds padding to the left and right or bottom "
                            "and top of the graphics region so that the model’s region maintains the same "
                            "aspect ratio (ratio of height to width).  This ensures that "
                            "annotations remain in the correct location relative to the viewed brain model. ");
                break;
        }
        bulletLines.push_back(text);
    }
    
    {
        AString text("Enter Annotations Mode (click Annotate button in Toolbar).  ");
        switch (textMode) {
            case BRIEF:
                break;
            case FULL:
                break;
        }
        bulletLines.push_back(text);
    }
    
    {
        AString text;
        switch (textMode) {
            case BRIEF:
                text = ("Add Annotations.");
                break;
            case FULL:
                text = ("To add annotations, click any of the Space and/or Type icons in the Insert New section "
                        "of the Annotation Toolbar and then click in graphics region to create the annotation.  "
                        "A new annotation is automatically selected so that it can be moved/resized with "
                        "the mouse or its attributes changed using controls in the toolbar.");
                break;
        }
        bulletLines.push_back(text);
    }
    
    {
        AString text("Display the Scene Dialog (click Clapboard icon in Toolbar or select Window Menu->Scenes) and "
                     "add a Scene (click Add button on Scene Dialog).  ");
        switch (textMode) {
            case BRIEF:
                break;
            case FULL:
                text.append("In the Create New Scene Dialog, edit the name and description for the new scene and then "
                            "press the OK button.  Note that by default, new annotations are added to "
                            "the current scene and stored in the scene file.  If one wants to place annotations into a disk file, "
                            "click the small arrow to the right of the Scene button in the Insert New "
                            "section of the Toolbar.  When the arrow is clicked, a menu is displayed "
                            "for creating and/or selecting a disk annotation file.");
                break;
        }
        bulletLines.push_back(text);
    }
    
    {
        AString text("Save the Scene File.  Click Save As button the first time to set the path and name of the scene file.  "
                     "Once path and name of scene file has been set, use the Save button.");
        switch (textMode) {
            case BRIEF:
                break;
            case FULL:
                text.append("To set the name of the New Scene File, click the Save As button.  "
                            "In the file selection dialog, choose the directory, and enter a descriptive "
                            "name for the Scene File.  It is best for the Scene File to be in the same "
                            "directory or the parent directory of the directory containing your data files.  "
                            "If the data files are scattered about many directories, it may cause problems "
                            "if the Scene File and its data files are zipped for distribution to others "
                            "or when uploading the BALSA Database");
                break;
        }
        bulletLines.push_back(text);
    }
    
    {
        AString text("Repeat last few steps to edit annotations and create additional scenes.");
        switch (textMode) {
            case BRIEF:
                break;
            case FULL:
                break;
        }
        bulletLines.push_back(text);
    }
    
    {
        AString text;
        switch (textMode) {
            case BRIEF:
                text = ("Be sure to save the Scene File (click Save button or Save As if Save is disabled at top of Scene Dialog).");
                break;
            case FULL:
                text = ("If the Save button at the top of the Scene Dialog is enabled, the Scene File is in a "
                        "modified state.  Click the Save button to save the Scene File and avoid losing your work.");
                break;
        }
        bulletLines.push_back(text);
    }
    
    
    AString msg;
    msg.reserve(32000);
    
    msg.appendWithNewLine("<html>"
                          + introText
                          + "<p>");
    
    AString text;
    switch (textMode) {
        case BRIEF:
        {
            msg.appendWithNewLine("<ol>");
            for (const auto& line : bulletLines) {
                msg.appendWithNewLine("<li> ");
                msg.append(line);
            }
            msg.appendWithNewLine("</ol>");
        }
            break;
        case FULL:
        {
            int32_t counter = 1;
            for (const auto& line : bulletLines) {
                msg.appendWithNewLine("<p>");
                msg.append(AString::number(counter) + ".  ");
                msg.append(line);
                ++counter;
            }
        }
            break;
    }
    
    {
        switch (textMode) {
            case BRIEF:
                break;
            case FULL:
                text.append("<p>"
                            "Understanding the Tab and Window Coordinate Spaces. "
                            "<p>"
                            "In PowerPoint, each slide maintains a fixed aspect ratio (the ratio of "
                            "height to width remains constant) with a spatial coordinate system "
                            "(typically inches).  As a result, each annotation in PowerPoint is places "
                            "its annotions at a spatial coordinate (X and Y in inches).  "
                            "In contrast, wb_view defaults to a "
                            "variable aspect ratio so that the window is resizable to best fit the brain "
                            "model (surface, volume, chart).  This variable aspect ratio is problematic "
                            "for annotations in Tab or Window Space both of which use a percentage "
                            "coordinate system (0% at left/bottom and 100% at right/top).  "
                            "<p>"
                            "If one creates a Tab Space annotation with the variable aspect enabled and "
                            "changes the size of the window, the Tab Space annotation may no longer be in "
                            "the correct location.  To correct this problem, wb_view supports “locking "
                            "of the aspect ratio”.  When the aspect ratio is locked, the window (and "
                            "each tab within the window) are fixed to the aspect ratio at the time "
                            "aspect locking is enabled.  If the size of the window changes while "
                            "the aspect is locked, the contents of window and each tab will expand or "
                            "contract in size but maintain a fixed aspect by adding space to sides "
                            "of the model when needed.");
                text.append("<p>"
                            "Scenes"
                            "<p>"
                            "Scenes are closely related to annotations.  Scenes allow one to preserve "
                            "'the state’ of wb_view.  Thus one if one saves a scene, exits wb_view, later restarts "
                            "wb_view, and loads the scene, wb_view appears identical to when the scene was "
                            "originally saved.  Scenes are frequently used for figures in publications.  "
                            "Scenes are also uploaded to the BALSA database for sharing with other users."
                            "<p>"
                            "While annotations can be saved in disk files, annotations are most commonly saved "
                            "to a scene.  When creating annotations for a scene, it is a best practice to "
                            "save the annotations to the scene instead of creating a disk annotation file.  ");
                break;
        }
        msg.append(text);
    }
    
    msg.appendWithNewLine("</html>");
    
    return msg;
}

