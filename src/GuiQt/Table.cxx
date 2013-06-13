#include "Table.h"
#include "ui_Table.h"
#include "QStandardItemModel"
#include "QColor"
#include <vector>
Table::Table(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Table)
{

    ui->setupUi(this);

    createModel();
}

void Table::createModel()
{
    model = new QStandardItemModel(0,0,this);
    model->setData(model->index(0,0,QModelIndex()), QColor(0,0,0), Qt::BackgroundColorRole);    

    ui->tableView->verticalHeader()->setDefaultSectionSize(20);
    ui->tableView->horizontalHeader()->setDefaultSectionSize(20);
    ui->tableView->setModel(model);

    std::vector<std::vector<QColor> > colors;
    //createColors(ncols,nrows,colors);
    //populate(colors);

}

void Table::populate(std::vector< std::vector <QColor> > &colors)
{
    model->setRowCount(colors.size());
    model->setColumnCount(colors[0].size());
    for(int x = 0;x<colors.size();x++)
    {
        for(int y = 0;y<colors[x].size();y++)
        {
            model->setData(model->index(x,y,QModelIndex()), colors[x][y], Qt::BackgroundColorRole);
        }
    }

}

void Table::populate(int nCols, int nRows)
{
    int x_step = 255/(nCols-1);
    int y_step = 255/(nRows-1);

    for(int x = 0;x<nCols;x++)
    {
        for(int y = 0;y<nRows;y++)
        {
            model->setData(model->index(x,y,QModelIndex()), QColor(x*x_step,(x*x_step+y*y_step)/2,y*y_step), Qt::BackgroundColorRole);
        }
    }    
}

void Table::createColors(int nCols, int nRows, std::vector< std::vector <QColor> > &colors)
{
    colors.resize(nCols);
    for(int i = 0;i<nCols;i++) colors[i].resize(nRows);
    int x_step = 255/(nCols-1);
    int y_step = 255/(nRows-1);

    for(int x = 0;x<nCols;x++)
    {
        for(int y = 0;y<nRows;y++)
        {
            colors[x][y] = QColor(x*x_step,(x*x_step+y*y_step)/2,y*y_step);
        }
    }
}

Table::~Table()
{
    delete ui;
}

QTableView *Table::getTableView()
{
    return ui->tableView;
}
