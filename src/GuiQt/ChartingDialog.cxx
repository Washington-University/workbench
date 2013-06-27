#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "CaretMappableDataFile.h"
#include "ChartableInterface.h"
#include "ChartingDialog.h"
#include "EventManager.h"
#include "EventCiftiMappableDataFileColoringUpdated.h"
#include "EventUserInterfaceUpdate.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventListenerInterface.h"
#include "GuiManager.h"
#include "ui_ChartingDialog.h"
#include "TimeCourseControls.h"
#include "PaletteFile.h"
#include "Plot2d.h"
#include "Table.h"
#include "QVBoxLayout"
#include "QPoint"
#include <QList>
#include <QObjectList>

using namespace caret;

ChartingDialog::ChartingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChartingDialog)
{
    ui->setupUi(this);

#ifdef CARET_OS_WINDOWS
    this->setWindowFlags(windowFlags() | Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
#endif
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(customContextMenuRequestedSlot(const QPoint &)));
    //form = new TimeCourseControls();
    table = new Table();
    //plot2d = new Plot2D();
    ui->comboBox->setCurrentIndex(1);
    ui->comboBox->hide();
    QObject::connect(getMatrixTableView()->selectionModel(),
        SIGNAL(currentChanged(const QModelIndex &, const QModelIndex & )),
        this,
        SLOT(currentChanged(const QModelIndex &,const QModelIndex &)));
	QObject::connect(getMatrixTableView()->selectionModel(),SIGNAL(currentChanged(const QModelIndex &,const QModelIndex &)),this, 
		SLOT(currentRowChanged(const QModelIndex &,const QModelIndex &)));
	
	EventManager::get()->addEventListener(this,EventTypeEnum::EVENT_CIFTI_MAPPABLE_DATA_FILE_COLORING_UPDATED);
    showDialogFirstTime = true;

}

ChartingDialog::~ChartingDialog()
{
    delete ui;    
}

void ChartingDialog::currentChanged(const QModelIndex &current,const QModelIndex &)
{
    this->ui->rowTextLabel->setText(this->cmf->getRowName(current.row()));
    this->ui->columnTextLabel->setText(this->cmf->getColumnName(current.column()));
    this->ui->rowNumber->setText(AString::number(current.row()));
    this->ui->columnNumber->setText(AString::number(current.column()));

}

void ChartingDialog::receiveEvent(Event* event)
{
	if(event->getEventType()==EventTypeEnum::EVENT_CIFTI_MAPPABLE_DATA_FILE_COLORING_UPDATED)
	{
		EventCiftiMappableDataFileColoringUpdated *up = dynamic_cast<EventCiftiMappableDataFileColoringUpdated*>(event);
		if(cmf==(up->cmf)) 
		{
			updateMatrix();
		}
	}
}


void ChartingDialog::on_closeButton_clicked()
{
    this->hide();
}

void ChartingDialog::setChartMode(QString type)
{
    QVBoxLayout *layout = (QVBoxLayout *)this->layout();
    layout->removeWidget(this->table);
    //layout->removeWidget(this->form);
    //this->form->hide();
    this->table->hide();
    //this->plot2d->hide();
    if(type == "2D Plot")
    {
        /*layout->insertWidget(1,plot2d,50);
        layout->insertWidget(2,form, 50);
        this->form->show();
        this->plot2d->show();*/
    }
    else if(type == "Matrix View")
    { 
        layout->insertWidget(1,table,100);
        this->ui->toolBarWidget->hide();
        this->ui->toolBoxWidget->hide();
        //layout->removeWidget(this->ui->toolBarWidget);
        //layout->removeWidget(this->ui->toolBoxWidget);
        this->layout()->setSpacing(0);        
        this->layout()->setContentsMargins(0,0,0,0);
        this->table->layout()->setContentsMargins(0,0,0,0);
        QRect rect = table->adjustTableSize(this->getMatrixTableView(),layout);
        
        this->resize(rect.width(),rect.height());
        
        this->table->show();
    }
}

void ChartingDialog::showToolBar(bool show)
{
    if(show)
        this->ui->toolBarWidget->show();
    else
        this->ui->toolBarWidget->hide();
}

void ChartingDialog::showToolBox(bool show)
{
    if(show)
        this->ui->toolBoxWidget->show();
    else
        this->ui->toolBoxWidget->hide();
}

void ChartingDialog::on_comboBox_currentIndexChanged(const QString &arg1)
{
    setChartMode(arg1);
}

#include <QScrollBar>
void ChartingDialog::showDialog()
{
    if(showDialogFirstTime)
    {
        showDialogFirstTime = false;

        QVBoxLayout *layout = (QVBoxLayout *)this->layout();
        QRect rect = table->adjustTableSize(this->getMatrixTableView(),layout);
        this->resize(rect.width(),rect.height()+6);
    }
    this->show();
}


void ChartingDialog::currentRowChanged(const QModelIndex & current, const QModelIndex & previous )
{
	cmf->loadMapData(current.row());
	cmf->updateScalarColoringForMap(0,GuiManager::get()->getBrain()->getPaletteFile());
	std::vector<ChartableInterface *> chartableFiles;
	GuiManager::get()->getBrain()->getAllChartableDataFiles(chartableFiles);
	for(std::vector<ChartableInterface *>::iterator iter = chartableFiles.begin();iter != chartableFiles.end();iter++)
	{
		if((*iter)->getCaretMappableDataFile() == this->cmf) continue;		
		int row = current.row();
		int col = current.column();
		ChartingDialog *chart = GuiManager::get()->getChartingDialog(*iter);
		if(chart->isHidden()) continue;
		if(chart->getMatrixTableView()->model()->rowCount() != this->getMatrixTableView()->model()->rowCount()) continue;
		chart->updateSelectedItem(row,col);
	}
	EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
	EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
	EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}



void ChartingDialog::updateMatrix()
{	
	std::vector<int64_t> dim;
	cmf->getMapDimensions(dim);
	PaletteFile *pf = GuiManager::get()->getBrain()->getPaletteFile();

	int ncols = dim[0];
	std::vector<float> rgba;

	cmf->getMatrixRGBA(rgba,pf);
	int nrows = dim[1];
	if(rgba.size() != nrows*ncols*4) return; //add error message
	std::vector<std::vector<QColor> > cMatrix;
	cMatrix.resize(nrows);
	for(int i = 0;i<nrows;i++) cMatrix[i].resize(ncols);

	for(int i = 0;i<nrows;i++)
	{        
		for(int j = 0;j<ncols;j++)
		{

			cMatrix[i][j] = QColor(rgba[(i*ncols+j)*4]*255,rgba[(i*ncols+j)*4+1]*255,rgba[(i*ncols+j)*4+2]*255,fabs(rgba[(i*ncols+j)*4+3]*255));
		}
	}  
	table->populate(cMatrix);
}

void ChartingDialog::updateSelectedItem(int32_t &row, int32_t &col)
{
	getMatrixTableView()->blockSignals(true);
	getMatrixTableView()->selectRow(row);
	getMatrixTableView()->blockSignals(false);
	this->ui->rowTextLabel->setText(this->cmf->getRowName(row));
	this->ui->columnTextLabel->setText(this->cmf->getColumnName(col));
    this->ui->rowNumber->setText(AString::number(row));
    this->ui->columnNumber->setText(AString::number(col));
	cmf->loadMapData(row);
	cmf->updateScalarColoringForMap(0,GuiManager::get()->getBrain()->getPaletteFile());	
}

void ChartingDialog::openPconnMatrix(CaretMappableDataFile *pconnFile)
{
    CiftiMappableConnectivityMatrixDataFile *matrix = static_cast<CiftiMappableConnectivityMatrixDataFile *>(pconnFile);
    if(matrix == NULL) return;
	this->cmf = matrix;
    updateMatrix();
    AString temp = pconnFile->getFileName();
    if(temp.length() > 80) temp = "..." + temp.right(80);
    this->setWindowTitle(temp);

}

QTableView * 
ChartingDialog::getMatrixTableView() 
{ 
    return this->table->getTableView();
}

#include <QMenu>
#include <QAction>
void ChartingDialog::customContextMenuRequestedSlot(const QPoint &pos)
{
    QMenu menu;
    /*QAction *toggleToolBar = new QAction("Show Toolbar",&menu);
    toggleToolBar->setCheckable(1);
    toggleToolBar->setChecked(!(ui->toolBarWidget->isHidden()));
    connect(toggleToolBar,SIGNAL(toggled(bool)),this,SLOT(showToolBar(bool)));
    menu.addAction(toggleToolBar);*/

    QAction *toggleToolBox = new QAction("Show Toolbox",&menu);
    toggleToolBox->setCheckable(1);
    toggleToolBox->setChecked(!(ui->toolBoxWidget->isHidden()));
    connect(toggleToolBox,SIGNAL(toggled(bool)),this,SLOT(showToolBox(bool)));
    menu.addAction(toggleToolBox);
    menu.exec(QCursor::pos());
}

void caret::ChartingDialog::on_rowSizeSpinBox_valueChanged(int arg1)
{
    this->getMatrixTableView()->verticalHeader()->setDefaultSectionSize(arg1);
    this->getMatrixTableView()->verticalHeader()->hide();
    this->getMatrixTableView()->verticalHeader()->show();
    this->getMatrixTableView()->verticalHeader()->update();
}

void caret::ChartingDialog::on_columnSizeSpinBox_valueChanged(int arg1)
{
    this->getMatrixTableView()->horizontalHeader()->setDefaultSectionSize(arg1);
    this->getMatrixTableView()->horizontalHeader()->hide();
    this->getMatrixTableView()->horizontalHeader()->show();
    this->getMatrixTableView()->horizontalHeader()->update();
}
