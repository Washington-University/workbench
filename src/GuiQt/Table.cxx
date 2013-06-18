#include "Table.h"
#include "ui_Table.h"
#include "QStandardItemModel"
#include "QStyledItemDelegate"
#include "QPainter"
#include "QColor"
#include <vector>

class BackgroundDelegate : public QStyledItemDelegate
{
public:
    explicit BackgroundDelegate(QObject *parent = 0)
        : QStyledItemDelegate(parent)
    {
    }
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const
    {
        // Fill the background before calling the base class paint
        // otherwise selected cells would have a white background
        QVariant background = index.data(Qt::BackgroundRole);
        if (background.canConvert<QBrush>())
            painter->fillRect(option.rect, background.value<QBrush>());

        QStyledItemDelegate::paint(painter, option, index);

        // To draw a border on selected cells
        if(option.state & QStyle::State_Selected)
        {
            /*painter->save();
            QPen pen(QColor(0,0,128,128));
            pen.setWidth(2);
            pen.setDashPattern(Qt::DotLine);
            QPen pen2(Qt::cyan, 2, Qt::DotLine, Qt::SquareCap, Qt::MiterJoin);
            int w = pen.width()/2;
            painter->setPen(pen);
            painter->drawRect(option.rect.adjusted(w,w,-w,-w));
            painter->restore();*/
        }
    }
};

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
    ui->tableView->setItemDelegate(new BackgroundDelegate(this));
    ui->tableView->setStyleSheet("selection-background-color: rgba(128, 128, 128, 40);");

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
            //model->setData(model->index(x,y,QModelIndex()), colors[x][y], Qt::ForegroundRole);
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

