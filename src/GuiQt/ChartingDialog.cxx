#include "Brain.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "CaretMappableDataFile.h"
#include "ChartingDialog.h"
#include "GuiManager.h"
#include "ui_ChartingDialog.h"
#include "TimeCourseControls.h"
#include "PaletteFile.h"
#include "Plot2d.h"
#include "Table.h"
#include "QVBoxLayout"

using namespace caret;

ChartingDialog::ChartingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChartingDialog)
{
    ui->setupUi(this);
    //form = new TimeCourseControls();
    table = new Table();
    //plot2d = new Plot2D();
    ui->comboBox->setCurrentIndex(1);
}

ChartingDialog::~ChartingDialog()
{
    delete ui;    
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
        layout->insertWidget(1,table,50);
        this->table->show();
    }
}

void ChartingDialog::on_comboBox_currentIndexChanged(const QString &arg1)
{
    setChartMode(arg1);
}

void ChartingDialog::openPconnMatrix(CaretMappableDataFile *pconnFile)
{
    CiftiMappableConnectivityMatrixDataFile *matrix = static_cast<CiftiMappableConnectivityMatrixDataFile *>(pconnFile);
    if(matrix == NULL) return;
    
    std::vector<int64_t> dim;
    matrix->getMapDimensions(dim);
    PaletteFile *pf = GuiManager::get()->getBrain()->getPaletteFile();
    
    int ncols = dim[0];
    std::vector<float> rgba;
    
    matrix->getMatrixRGBA(rgba,pf);
    int nrows = dim[1];
    if(rgba.size() != nrows*ncols*4) return; //add error message
    std::vector<std::vector<QColor>> cMatrix;
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
