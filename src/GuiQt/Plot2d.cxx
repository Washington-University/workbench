#include "Plot2d.h"
#include "ui_plot2d.h"
#include <qwt.h>
#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
Plot2D::Plot2D(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Plot2D)
{
    ui->setupUi(this);
    QwtPlot *plot = new QwtPlot(this);
    QVBoxLayout *layout = (QVBoxLayout *)this->layout();
    layout->insertWidget(0,plot,100);

    plot->setAutoFillBackground( true );
    plot->setPalette( QPalette( QColor( 165, 193, 228 ) ) );

    plot->setTitle("2D Plot");

    // canvas
    plot->canvas()->setLineWidth( 1 );
    plot->canvas()->setFrameStyle( QFrame::Box | QFrame::Plain );
    plot->canvas()->setBorderRadius( 1 );//15

    QPalette canvasPalette( Qt::white );
    canvasPalette.setColor( QPalette::Foreground, QColor( 133, 190, 232 ) );
    plot->canvas()->setPalette( canvasPalette );

}

Plot2D::~Plot2D()
{
    delete ui;
}
