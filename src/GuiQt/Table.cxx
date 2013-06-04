#include "Table.h"
#include "ui_table.h"
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
    int xSize = 14;
    int ySize = 6;
    model = new QStandardItemModel(xSize,ySize,this);
    model->setData(model->index(0,0,QModelIndex()), QColor(0,0,0), Qt::BackgroundColorRole);

    ui->tableView->setModel(model);

    std::vector<std::vector<QColor>> colors;
    createColors(xSize,ySize,colors);
    populate(colors);

}

void Table::populate(std::vector< std::vector <QColor>> &colors)
{
    for(int x = 0;x<colors.size();x++)
    {
        for(int y = 0;y<colors[x].size();y++)
        {
            model->setData(model->index(x,y,QModelIndex()), colors[x][y], Qt::BackgroundColorRole);
        }
    }

}

void Table::populate(int xSize, int ySize)
{
    int x_step = 255/(xSize-1);
    int y_step = 255/(ySize-1);

    for(int x = 0;x<xSize;x++)
    {
        for(int y = 0;y<ySize;y++)
        {
            model->setData(model->index(x,y,QModelIndex()), QColor(x*x_step,(x*x_step+y*y_step)/2,y*y_step), Qt::BackgroundColorRole);
        }
    }
}

void Table::createColors(int xSize, int ySize, std::vector< std::vector <QColor>> &colors)
{
    colors.resize(xSize);
    for(int i = 0;i<xSize;i++) colors[i].resize(ySize);
    int x_step = 255/(xSize-1);
    int y_step = 255/(ySize-1);

    for(int x = 0;x<xSize;x++)
    {
        for(int y = 0;y<ySize;y++)
        {
            colors[x][y] = QColor(x*x_step,(x*x_step+y*y_step)/2,y*y_step);
        }
    }
}


Table::~Table()
{
    delete ui;
}
