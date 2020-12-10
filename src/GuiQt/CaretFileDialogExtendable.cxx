//
///*LICENSE_START*/
///*
// *  Copyright (C) 2014  Washington University School of Medicine
// *
// *  This program is free software; you can redistribute it and/or modify
// *  it under the terms of the GNU General Public License as published by
// *  the Free Software Foundation; either version 2 of the License, or
// *  (at your option) any later version.
// *
// *  This program is distributed in the hope that it will be useful,
// *  but WITHOUT ANY WARRANTY; without even the implied warranty of
// *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// *  GNU General Public License for more details.
// *
// *  You should have received a copy of the GNU General Public License along
// *  with this program; if not, write to the Free Software Foundation, Inc.,
// *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// */
///*LICENSE_END*/
//
//#include <QPushButton>
//#include <QCloseEvent>
//#include <QLabel>
//#include <QVBoxLayout>
//
//#define __CARET_FILE_DIALOG_EXTENDABLE_DECLARE__
//#include "CaretFileDialogExtendable.h"
//#undef __CARET_FILE_DIALOG_EXTENDABLE_DECLARE__
//
//#include "CaretAssert.h"
//#include "CaretFileDialog.h"
//using namespace caret;
//
//
//    
///**
// * \class caret::CaretFileDialogExtendable 
// * \brief A File Dialog that can have a widget added to it.
// *
// * Embeds a QFileDialog inside a dialog so that a widget
// * can be inserted below the FileDialog.  All public methods
// * from QFileDialog are duplicated and go straight to 
// * the embedded QFileDialog.
// */
//
///**
// * Constructor.
// */
//CaretFileDialogExtendable::CaretFileDialogExtendable(QWidget* parent,
//                                                     Qt::WindowFlags f)
//: QDialog(parent,
//          f)
//{
//    Qt::WindowFlags flags = 0;
//    m_caretFileDialog = new CaretFileDialogPrivate(NULL,
//                                                   flags);
//
//    createDialog();
//}
///**
// * Constructor.
// */
//CaretFileDialogExtendable::CaretFileDialogExtendable(QWidget* parent,
//                                                     const QString& caption,
//                                                     const QString& directory,
//                                                     const QString& filter)
//: QDialog(parent)
//{
//    m_caretFileDialog = new CaretFileDialogPrivate(NULL,
//                                            caption,
//                                            directory,
//                                            filter);
//    
//    createDialog();
//}
//
//
///**
// * Destructor.
// */
//CaretFileDialogExtendable::~CaretFileDialogExtendable()
//{
//    
//}
//
///**
// * Create the dialog.
// */
//void 
//CaretFileDialogExtendable::createDialog()
//{
//    QObject::connect(m_caretFileDialog, SIGNAL(currentChanged(const QString&)),
//                     this, SIGNAL(currentChanged(const QString&)));
//    
//    QObject::connect(m_caretFileDialog, SIGNAL( directoryEntered(const QString&)),
//                     this, SIGNAL( directoryEntered(const QString&)));
//    
//    QObject::connect(m_caretFileDialog, SIGNAL(fileSelected(const QString&)),
//                     this, SIGNAL(fileSelected(const QString&)));
//    
//    QObject::connect(m_caretFileDialog, SIGNAL(filesSelected(const QStringList&)),
//                     this, SIGNAL(filesSelected(const QStringList&)));
//    
//    QObject::connect(m_caretFileDialog, SIGNAL(filterSelected(const QString&)),
//                     this, SIGNAL(filterSelected(const QString&)));
//    
//    QObject::connect(m_caretFileDialog, SIGNAL(finished(int)),
//                     this, SLOT(fileDialogFinished(int)));
//    
//    m_caretFileDialog->setAttribute(Qt::WA_DeleteOnClose, false);
//    m_caretFileDialog->setSizeGripEnabled(false);
//    
//    m_dialogLayout = new QVBoxLayout(this);
//    m_dialogLayout->addWidget(m_caretFileDialog);
//}
//
///**
// * Gets called when child file dialog issues its finished signal.
// * @param result
// *    Result of dialog.
// */
//void 
//CaretFileDialogExtendable::fileDialogFinished(int result)
//{
//    this->setResult(result);
//    this->done(result);
//}
//
//
///**
// * Add a widget at the bottom to this file dialog.
// * @param widget
// *   Widget that is added.
// */
//void 
//CaretFileDialogExtendable::addWidget(QWidget* widget)
//{
//    CaretAssert(widget);
//    m_dialogLayout->addWidget(widget);
//}
//
//
////==================================================================================
//
//CaretFileDialogPrivate::CaretFileDialogPrivate(QWidget* parent,
//                       Qt::WindowFlags /*f*/)
//: CaretFileDialog(parent,
//                  Qt::Widget)
//{
//}
//
//CaretFileDialogPrivate::CaretFileDialogPrivate(QWidget* parent,
//                       const QString& caption,
//                       const QString& directory,
//                       const QString& filter)
//: CaretFileDialog(parent,
//                  caption,
//                  directory,
//                  filter)
//{
//}
//
//CaretFileDialogPrivate::~CaretFileDialogPrivate()
//{
//}
//
//void
//CaretFileDialogPrivate::closeEvent(QCloseEvent* event)
//{
//    event->ignore();
//}
//
//void 
//CaretFileDialogPrivate::done(int result)
//{
//    CaretFileDialog::done(result);
//}
