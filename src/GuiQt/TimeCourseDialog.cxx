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

#include <algorithm>
#include "QApplication"
#include "QDesktopWidget"
#include "QImageWriter"
#include "QFileDialog"
#include "QMessageBox"
#include "QPainter"
#include "QVBoxLayout"
#include "qwt_plot_renderer.h"

#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "SessionManager.h"
#include "TimeCourseDialog.h"
#include "ui_TimeCourseDialog.h"
#include "TimeCoursePlotter.h"

using namespace caret;

TimeCourseDialog::TimeCourseDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TimeCourseDialog)
{
    ui->setupUi(this);
#ifdef CARET_OS_WINDOWS
    this->setWindowFlags(windowFlags() | Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
#endif
    this->plot = new caret::TimeCoursePlotter();
    isEnabled = true;
    this->ui->TDMinTime->setEnabled(true);
    this->ui->TDMaxTime->setEnabled(true);
    this->ui->TDMinActivity->setEnabled(true);
    this->ui->TDMaxActivity->setEnabled(true);
    this->ui->TDMinTime->setMinimum(-1000000.0);
    this->ui->TDMaxTime->setMinimum(-1000000.0);
    this->ui->TDMinActivity->setMinimum(-1000000.0);
    this->ui->TDMaxActivity->setMinimum(-1000000.0);
    this->ui->TDMinTime->setMaximum(1000000.0);
    this->ui->TDMaxTime->setMaximum(1000000.0);
    this->ui->TDMinActivity->setMaximum(1000000.0);
    this->ui->TDMaxActivity->setMaximum(1000000.0);
    /*QObject::connect(this->plot, SIGNAL(timeStartValueChanged(double)),
                    this->ui->TDMinTime, SLOT(setValue(double)));
    QObject::connect(this->plot, SIGNAL(timeEndValueChanged(double)),
                    this->ui->TDMaxTime, SLOT(setValue(double)));*/
    QObject::connect(this->plot, SIGNAL(activityMinValueChanged(double)),
                    this, SLOT(plotActivityMinValueChanged(double)));
    QObject::connect(this->plot, SIGNAL(activityMaxValueChanged(double)),
                    this, SLOT(plotActivityMaxValueChanged(double)));
    QObject::connect(this->plot, SIGNAL(timeStartValueChanged(double)),
                    this, SLOT(plotTimeStartValueChanged(double)));
    QObject::connect(this->plot, SIGNAL(timeEndValueChanged(double)),
                    this, SLOT(plotTimeEndValueChanged(double)));
    this->layout()->setContentsMargins(0,0,0,0);
    //this->layout()->addWidget(plot);
    QVBoxLayout *layout = (QVBoxLayout *)this->layout();
    layout->insertWidget(0,plot,100);
    

    //ui->verticalLayout_4->setContentsMargins(0,0,0,0);
    //ui->verticalLayout_4->insertWidget(0,plot,100);
    CaretPreferences *prefs = SessionManager::get()->getCaretPreferences();
    double time = 0.0;    
    this->setAnimationStartTime(time);
}



TimeCourseDialog::~TimeCourseDialog()
{
    delete ui;
}

void TimeCourseDialog::addTimeLine(TimeLine &tl)
{    
    tlV.push_back(tl);
    //readjust x values to account for timestep
    double timeStep = ((tlV.last().timeStep>0.0001) ? tlV.last().timeStep : 1.0);
    for(int j = 0;j<tlV.last().x.size();j++)
    {
        tlV.last().x[j] = startOffset + timeStep*(double)j;
    }
}

void TimeCourseDialog::addTimeLines(QList <TimeLine> &tlVIn)
{
    for(int i = 0;i<tlVIn.size();i++)
    {
        this->addTimeLine(tlV[i]);
    }
}

void TimeCourseDialog::updateDialog(const bool &forceUpdate, const bool &forceDisableAutoScale)
{
    if(tlV.isEmpty() && !forceUpdate) return;
    if(!this->isEnabled) return;
    if(!(this->filename.length())) 
    {   
        if(this->tlV.count())
        {
            this->filename = tlV[0].filename;
            AString temp = filename;
            if(temp.length() > 80) temp = "..." + temp.right(80);
            this->setWindowTitle("Time Course " + AString::number(tlV[0].clmID) + AString(" - ") + temp);
        }
        else 
        {
            this->filename = " ";
            this->setWindowTitle("Time Course");
        }
        
    }
    plot->detachItems();
    plot->populate(tlV,forceDisableAutoScale);    
    this->populateHistory();
    this->setVisible(true);
    this->show();
    this->activateWindow();
    plot->update();
    plot->replot();
    plot->setFocus();
    this->setAttribute(Qt::WA_NoMousePropagation,true);    
    plot->legend()->setVisible(false);
    plot->legend()->setDisabled(true);
    plot->legend()->clear();
}

void TimeCourseDialog::populateHistory()
{
    if(tlV.isEmpty()) return;
    this->ui->TDHistoryList->clear();
    for(int i =0;i<tlV.size();i++)
    { 
        
//        this->ui->TDHistoryList->setTextColor(plot->colors.getColor(tlV[i].colorID));
//        this->ui->TDHistoryList->append(tlV[i].label);
        QListWidgetItem *item = new QListWidgetItem(tlV[i].label);
        item->setTextColor(plot->colors.getColor(tlV[i].colorID));
        this->ui->TDHistoryList->addItem(item);
        this->ui->TDHistoryList->show();
    }
}

void TimeCourseDialog::setAnimationStartTime(const double &time)
{
    startOffset = time;
    if(tlV.isEmpty()) return;
    
    for(int i =0;i<tlV.size();i++)
    { 
        double timeStep = ((tlV[i].timeStep>0.0001) ? tlV[i].timeStep : 1.0);
        for(int j = 0;j<tlV[i].x.size();j++)
        {
            tlV[i].x[j] = startOffset + timeStep*(double)j;
        }
    }
    plot->detachItems();
    plot->populate(tlV,true);
    plot->update();
    plot->replot();    
}




void TimeCourseDialog::on_TDClose_clicked()
{
    this->hide();
}

void TimeCourseDialog::on_TDClearChart_clicked()
{
    this->plot->clear(tlV);
    tlV.clear();
    this->ui->TDHistoryList->clear();
    this->updateDialog(true);
}

void TimeCourseDialog::on_TDMinTime_valueChanged(double arg1)
{
    QwtScaleDiv *sd = this->plot->axisScaleDiv(QwtPlot::xBottom);
    plot->setAxisScale(QwtPlot::xBottom,arg1,sd->upperBound());
    plot->QwtPlot::replot();
}

void TimeCourseDialog::on_TDMaxTime_valueChanged(double arg1)
{
    QwtScaleDiv *sd = this->plot->axisScaleDiv(QwtPlot::xBottom);
    plot->setAxisScale(QwtPlot::xBottom,sd->lowerBound(),arg1);
    plot->QwtPlot::replot();
}

void TimeCourseDialog::on_TDMinActivity_valueChanged(double arg1)
{
    QwtScaleDiv *sd = this->plot->axisScaleDiv(QwtPlot::yLeft);
    plot->setAxisScale(QwtPlot::yLeft,arg1,sd->upperBound());
    plot->QwtPlot::replot();
}

void TimeCourseDialog::on_TDMaxActivity_valueChanged(double arg1)
{
    QwtScaleDiv *sd = this->plot->axisScaleDiv(QwtPlot::yLeft);
    plot->setAxisScale(QwtPlot::yLeft,sd->lowerBound(),arg1);
    plot->QwtPlot::replot();
}

void TimeCourseDialog::plotTimeEndValueChanged(double time)
{
    this->ui->TDMaxTime->blockSignals(true);
    this->ui->TDMaxTime->setValue(time);
    this->ui->TDMaxTime->blockSignals(false);

}
void TimeCourseDialog::plotTimeStartValueChanged(double time)
{
    this->ui->TDMinTime->blockSignals(true);
    this->ui->TDMinTime->setValue(time);
    this->ui->TDMinTime->blockSignals(false);
}
void TimeCourseDialog::plotActivityMaxValueChanged(double time)
{
    this->ui->TDMaxActivity->blockSignals(true);
    this->ui->TDMaxActivity->setValue(time);
    this->ui->TDMaxActivity->blockSignals(false);
}
void TimeCourseDialog::plotActivityMinValueChanged(double time)
{
    this->ui->TDMinActivity->blockSignals(true);
    this->ui->TDMinActivity->setValue(time);
    this->ui->TDMinActivity->blockSignals(false);
}
void TimeCourseDialog::on_TDShowAverage_toggled(bool checked)
{
    this->plot->setDisplayAverage(checked);
    this->updateDialog(true,true);

}

void TimeCourseDialog::setTimeSeriesGraphEnabled(bool enabled)
{
    this->isEnabled = enabled;
}


void TimeCourseDialog::updateExtents()
{
    double tmin, tmax, amin, amax;
    this->plot->getTimeExtents(tmin,tmax);
    this->plot->getActivityExtents(amin, amax);
    this->ui->TDMinActivity->setMinimum(amin);
    this->ui->TDMinActivity->setMaximum(amax - 1.0);
    this->ui->TDMinTime->setMinimum(tmin);
    this->ui->TDMinTime->setMaximum(tmax - 1.0);
    this->ui->TDMaxActivity->setMinimum(amin + 1.0);
    this->ui->TDMaxActivity->setMaximum(amax);
    this->ui->TDMaxTime->setMinimum(tmin + 1.0);
    this->ui->TDMaxTime->setMaximum(tmax);    
}




void TimeCourseDialog::on_TDKeepLast_valueChanged(int arg1)
{
    this->plot->setMaxTimeLines(arg1);
    this->updateDialog(true,true);
}

void TimeCourseDialog::on_zoomXCheckBox_toggled(bool checked)
{
    this->plot->magnifier->setAxisEnabled(QwtPlot::xBottom,checked);
    this->plot->magnifier->setAxisEnabled(QwtPlot::xTop,checked);
}

void TimeCourseDialog::on_zoomYCheckBox_toggled(bool checked)
{
    this->plot->magnifier->setAxisEnabled(QwtPlot::yLeft,checked);
    this->plot->magnifier->setAxisEnabled(QwtPlot::yRight,checked);
}

void TimeCourseDialog::on_lineWidthSpinBox_valueChanged(int arg1)
{
    this->plot->setTimeLineWidth(arg1);
    this->updateDialog(true,true);
}

void TimeCourseDialog::on_autoFitTimeLinesCheckBox_toggled(bool checked)
{
    this->plot->setAutoScaleEnabled(checked);
    this->updateDialog(true);
}

void TimeCourseDialog::on_resetViewButton_clicked()
{
    this->plot->resetView();
    this->updateDialog(true);
}

void TimeCourseDialog::on_exportImageButton_clicked()
{
    QStringList formats;
    QString formatString("Image Files (");
    
    for ( int i = 0; i < QImageWriter::supportedImageFormats().count(); i++ ) {
        QString str = QString( QImageWriter::supportedImageFormats().at( i ) );
        formats.append( str );
        formatString.append("*."+str);
        if(i<(QImageWriter::supportedImageFormats().count()-1)) formatString.append(" ");
        else formatString.append(")");
    }
    QStringList files;

    QString fileName = QFileDialog::getSaveFileName( this, tr("Save File"),QString::null, formatString );
    if ( !fileName.isEmpty() )
    {
        /*QPixmap *image = new QPixmap(this->plot->width(),this->plot->height());
        //image->setDotsPerMeterX(1000);
        //image->setDotsPerMeterY(1000);
        QwtPlotRenderer *renderer = new QwtPlotRenderer();
        renderer->renderDocument(this->plot, filename, QRectF(this->plot->width(),this->plot->height());
        image->save(filename, "*.jpg");
        std::cout << "File Saved" << std::endl;
        delete image;*/

        const QRect imageRect = this->plot->rect();
        const int dotsPerMeter = 3863;//85 DPI converted to meters
        QImage image( imageRect.size(), QImage::Format_ARGB32 );
        image.setDotsPerMeterX( float(QApplication::desktop()->physicalDpiX())/0.0254);//dotsPerMeter );
        image.setDotsPerMeterY( float(QApplication::desktop()->physicalDpiY())/0.0254);//dotsPerMeter );
        image.fill( QColor( Qt::white ).rgb() );

        QPainter painter( &image );
        
        QwtPlotRenderer * renderer = new QwtPlotRenderer();
        renderer->render( plot, &painter, imageRect );
        painter.end();

        image.save( fileName );
    }
}

void TimeCourseDialog::on_openTimeLineButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName( this, tr("Open Dvars File"),QString::null, tr("Dvars Files (*.txt)") );
    QFile file(fileName);
    //file.open(QIODevice::ReadOnly);
    TimeLine tl;
    tl.timeStep = 1.0;
    tl.label = fileName;
    tl.filename = fileName;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        
        CaretLogWarning("There was an error reading the dvars file.");
        return;
    }
    while(!file.atEnd())
    {
        QByteArray line = file.readLine(100);
        float var = 0.0;
        sscanf(line,"%g ", &var);
        tl.y.push_back(var);        
    }
    tl.x.resize(tl.y.count());
    this->addTimeLine(tl);
    this->updateDialog();
}

void TimeCourseDialog::on_TDHistoryList_itemActivated(QListWidgetItem *item)
{

}
