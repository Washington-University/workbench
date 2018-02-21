
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

#include <QDialogButtonBox>
#include <QLabel>
#include <QTextBrowser>
#include <QTextDocument>
#include <QVBoxLayout>
#include <QPushButton>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>

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
m_infoMode(infoMode)
{
    
    setApplyButtonText("");
    setDeleteWhenClosed(true);
    
    m_text = getTextForInfoMode(infoMode,
                                     TextMode::BRIEF_AND_FULL);

    m_printPushButton = addUserPushButton("Print...",
                                          QDialogButtonBox::NoRole);
    
    QPushButton* closeButton = getDialogButtonBox()->button(QDialogButtonBox::Close);
    CaretAssert(closeButton);
    closeButton->setAutoDefault(true);
    closeButton->setDefault(true);

    QTextBrowser* browser = new QTextBrowser();
    browser->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    browser->setHtml(m_text);
    browser->setMinimumSize(600, 400);
    
    QWidget* widget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(browser);
    
    setCentralWidget(widget,
                     WuQDialog::SCROLL_AREA_AS_NEEDED_VERT_NO_HORIZ);

    WuQtUtilities::limitWindowSizePercentageOfMaximum(this, 90.0, 80.0);
}

/**
 * Destructor.
 */
BestPracticesDialog::~BestPracticesDialog()
{
}

/**
 * Gets called when a user pushbutton is pressed.
 *
 * @param userPushButton
 *     User push button that was pressed.
 * @return
 *     Action dialog should take in response to push button being pressed.
 */
WuQDialog::DialogUserButtonResult
BestPracticesDialog::userButtonPressed(QPushButton* userPushButton)
{
    if (userPushButton == m_printPushButton) {
        QPrinter printer;
        QPrintDialog printDialog(&printer, this);
        if (printDialog.exec() == QPrintDialog::Accepted) {
            QTextDocument textDocument;
            textDocument.setHtml(m_text);
            textDocument.print(&printer);
        }
    }
    else {
        CaretAssert(0);
    }
    
    return DialogUserButtonResult::RESULT_NONE;
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
    const AString briefIntro ("<h2>Best Practices Summary</h2>");
    
    const AString fullIntro("<h2>Introduction</h2>"
                            "Describing annotations and scenes is best done using an analogy with Microsoft PowerPoint.  "
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
    
    
    std::vector<std::pair<AString,AString>> summaryDetails;
    
    {
        const AString summary("Verify data files are in a common directory tree and file names are reasonable.  ");
        const AString details(summary
                              + "Scenes contain the directory path and filename for each data file that is loaded "
                              "at the time the scene is created.  Once a scene has been created, moving or "
                              "renaming a data file referenced by the scene will cause an error when the scene is displayed.  "
                              "In addition, if data files are scattered about many directories, this may cause "
                              "problems when zipping a scene file and its data files or uploading a scene file "
                              "to the BALSA database.");
        
        summaryDetails.push_back(std::make_pair(summary, details));
    }
    
    {
        const AString summary("If a multi-tab scene will be created, enter Tile Tabs (View Menu->Enter Tile Tabs).  ");
        const AString details(summary
                              + "Tile Tabs displays all of the window's tabs in a grid pattern.  "
                              "The number of row and columns in the Tile Tabs view is edited on the "
                              "Tile Tabs Configuration Dialog (View Menu->Tile Tabs Configuration->Create and Edit).  "
                              "Advanced parameters include “stretch factors” that allow one to increase the "
                              "size of a row or column relative to other rows and columns.  One may also save the "
                              "configuration for future use.  If a scene is created, the configuration is added to the scene "
                              "so that the configuration is available when the scene is displayed.");
        
        summaryDetails.push_back(std::make_pair(summary, details));
    }
    
    {
        const AString summary("Adjust size of window.  ");
        const AString details(summary +
                              "It is important to adust the size of the window prior to entering Annotations Mode "
                              "(further down in this list).");
        
        summaryDetails.push_back(std::make_pair(summary, details));
    }
    
    {
        const AString summary("Setup view of the models (pan/rotate/zoom).  ");
        const AString details("Setup view of the models.  "
                              "Pan (drag mouse with SHIFT key down), "
                              "Rotate (drag mouse), "
                              "Zoom (drag mouse with Control Key.  Use the Command Key on Mac.");
        
        summaryDetails.push_back(std::make_pair(summary, details));
    }
    
    {
        const AString summary("Lock Aspect Ratio (click Lock Aspect button in right side of Toolbar).  ");
        const AString details(summary
                              + "As a general rule, once the aspect ratio is locked, it should remain locked.  "
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
        
        summaryDetails.push_back(std::make_pair(summary, details));
    }
    
    {
        const AString summary("Enter Annotations Mode (click Annotate button in Toolbar).  ");
        const AString details(summary);
        
        summaryDetails.push_back(std::make_pair(summary, details));
    }
    
    {
        const AString summary = ("Add Annotations.");
        const AString details("To add annotations, click any of the Space and/or Type icons in the Insert New section "
                              "of the Annotation Toolbar and then click in graphics region to create the annotation.  "
                              "A new annotation is automatically selected so that it can be moved/resized with "
                              "the mouse or its attributes changed using controls in the toolbar.");
        
        summaryDetails.push_back(std::make_pair(summary, details));
    }
    
    {
        const AString summary("Display the Scene Dialog (click Clapboard icon in Toolbar or select Window Menu->Scenes) and "
                              "add a Scene (click Add button on Scene Dialog).  ");
        const AString details(summary
                              + "In the Create New Scene Dialog, edit the name and description for the new scene and then "
                              "press the OK button.  Note that by default, new annotations are added to "
                              "the current scene and stored in the scene file.  If one wants to place annotations into a disk file, "
                              "click the small arrow to the right of the Scene button in the Insert New "
                              "section of the Toolbar.  When the arrow is clicked, a menu is displayed "
                              "for creating and/or selecting a disk annotation file.");
        
        summaryDetails.push_back(std::make_pair(summary, details));
    }
    
    {
        const AString summary("Save the Scene File.  Click Save As button the first time to set the path and name of the scene file.  "
                              "Once path and name of scene file has been set, use the Save button.");
        const AString details(summary
                              + "To set the name of the New Scene File, click the Save As button.  "
                              "In the file selection dialog, choose the directory, and enter a descriptive "
                              "name for the Scene File.  It is best for the Scene File to be in the same "
                              "directory or the parent directory of the directory containing your data files.  "
                              "If the data files are scattered about many directories, it may cause problems "
                              "if the Scene File and its data files are zipped for distribution to others "
                              "or when uploading the BALSA Database");
        
        summaryDetails.push_back(std::make_pair(summary, details));
    }
    
    {
        const AString summary("Repeat last few steps to edit annotations and create additional scenes.");
        const AString details(summary);
        
        summaryDetails.push_back(std::make_pair(summary, details));
    }
    
    {
        const AString summary = ("Be sure to save the Scene File (click Save button or Save As if Save is disabled at top of Scene Dialog).");
        const AString details("If the Save button at the top of the Scene Dialog is enabled, the Scene File is in a "
                              "modified state.  Click the Save button to save the Scene File and avoid losing your work.");
        
        summaryDetails.push_back(std::make_pair(summary, details));
    }
    
    
    bool showBriefFlag = false;
    bool showFullFlag  = false;
    switch (textMode) {
        case BRIEF_AND_FULL:
            showBriefFlag = true;
            showFullFlag  = true;
            break;
        case BRIEF:
            showBriefFlag = true;
            break;
        case FULL:
            showFullFlag = true;
            break;
    }
    
    AString briefText;
    if (showBriefFlag) {
        briefText.appendWithNewLine(briefIntro);
    }
    
    AString fullText;
    if (showFullFlag) {
        fullText.appendWithNewLine(fullIntro);
        fullText.appendWithNewLine("<h2>Best Practices Details</h2>");
    }
    
    int32_t counter = 1;
    for (const auto sd : summaryDetails) {
        const AString linkName("n" + AString::number(counter));
        
        if (showBriefFlag) {
            briefText.append("<a href=\"#" + linkName + "\">"
                             + AString::number(counter)
                             + "</a>. "
                             + sd.first
                             + "<br>");
        }
        if (showFullFlag) {
            fullText.append("<a name=\""
                            + linkName
                            + "\">"
                            + AString::number(counter) + "</a>.  ");
            fullText.append(sd.second);
            fullText.append("<p>");
        }
        ++counter;
    }

    if (showFullFlag) {
        fullText.append("<p>"
                    "<h2>Understanding the Tab and Window Coordinate Spaces</h2> "
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
        fullText.append("<p>"
                    "<h2>Scenes</h2>"
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
    }
    
    AString msg;
    msg.reserve(64 * 1024);
    
    msg.appendWithNewLine("<html>");
    if (showFullFlag) {
        msg.appendWithNewLine("<h1>Scenes and Annotations</h1>");
    }
    msg.appendWithNewLine(briefText);
    msg.appendWithNewLine(fullText);
    msg.appendWithNewLine("</html>");
    
    return msg;
}

