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
    
    matrix->getMapRGBA(0,rgba,pf);
    int nrows = rgba.size()/4;
    if(nrows != dim[1]) return; //add error message
    std::vector<std::vector<QColor>> cMatrix;
    cMatrix.resize(ncols);
    for(int i = 0;i<ncols;i++) cMatrix[i].resize(nrows);
    
    for(int i = 0;i<ncols;i++)
    {
        matrix->getMapRGBA(i,rgba,pf);
        for(int j = 0;j<nrows;j++)
        {
            cMatrix[i][j] = QColor(rgba[j*4],rgba[j*4+1],rgba[j*4+2],rgba[j*4+3]);
        }
    }  
    table->populate(cMatrix);
}
