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

Table::~Table()
{
    delete ui;
}

/*
bool Table::viewportEvent(QEvent *event) {
    if (event->type() == QEvent::ToolTip) {
        /*QHelpEvent *helpEvent = static_cast<QHelpEvent*>(event);
        QModelIndex index = indexAt(helpEvent->pos());
        if (index.isValid()) {
            QSize sizeHint = itemDelegate(index)->sizeHint(viewOptions(), index);
            QRect rItem(0, 0, sizeHint.width(), sizeHint.height());
            QRect rVisual = visualRect(index);
            if (rItem.width() <= rVisual.width())
                return false;
        }
    }
    return this->getTableView()->QTableView::viewportEvent(event);
}*/

QTableView *Table::getTableView()
{
    return ui->tableView;
}

#include <QDesktopWidget>
const QRect Table::adjustTableSize(
    QTableView* tv,
    const QVBoxLayout* lay/*,
    const int& maxRows,
    const int& maxCols*/) {
        int maxCols = tv->model()->columnCount();
        int maxRows = tv->model()->rowCount();
        // get Desktop size
        using std::size_t;
        QDesktopWidget desktop;
        size_t desW = desktop.screen()->width();
        size_t desH = desktop.screen()->height();

        int leftM,rightM,topM,bottomM;
        lay->getContentsMargins(&leftM,&topM,&rightM,&bottomM);

        size_t extraTopHeight = topM + tv->frameWidth();
        size_t extraBottomHeight = bottomM + tv->frameWidth();
        size_t extraLeftWidth = leftM + tv->frameWidth();
        size_t extraRightWidth = rightM + tv->frameWidth();
        size_t w = tv->verticalHeader()->width() + extraLeftWidth + extraRightWidth;
        size_t h = tv->horizontalHeader()->height() + extraTopHeight + extraBottomHeight;
        for(size_t col = 0; col < maxCols; ++col) {
            w += tv->columnWidth(col);
        }
        for(size_t row = 0; row < maxRows; ++row ) {
            h += tv->rowHeight(row);
        }

        std::size_t x,y;
        if((w - extraLeftWidth - extraRightWidth) > desW) {
            x = 0;
            w = desW - extraLeftWidth - extraRightWidth;
        } else
            x = (desW - w)/2;
        if(h - extraTopHeight - extraBottomHeight - QStyle::PM_TitleBarHeight > desH) {
            y = extraTopHeight + QStyle::PM_TitleBarHeight;
            h = desH - (extraTopHeight + QStyle::PM_TitleBarHeight + extraBottomHeight);
        } else
            y = (desH - h)/2;
        return QRect(x,y,w,h);
}
