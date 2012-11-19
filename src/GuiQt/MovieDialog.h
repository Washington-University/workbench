#ifndef MOVIEDIALOG_H
#define MOVIEDIALOG_H

#include <QDialog>
#include "Event.h"
#include "EventListenerInterface.h"
#include <stdint.h>

namespace Ui {
class MovieDialog;
}

using namespace caret;
class MovieDialog : public QDialog, public EventListenerInterface
{
    Q_OBJECT
    
public:
    explicit MovieDialog(QWidget *parent = 0);
    ~MovieDialog();

    void receiveEvent(Event* event);
    
private slots:
    void on_closeButton_clicked();

    void on_animateButton_toggled(bool checked);

    void on_recordButton_toggled(bool checked);

    void on_cropImageCheckBox_toggled(bool checked);

private:
    Ui::MovieDialog *ui;

    void captureFrame(AString filename);

    void processRotateTransformation(const double dx,
        const double dy,
        const double dz);

    int32_t browserWindowIndex;

    int frame_number;
    int rotate_frame_number;
    double dx;
    double dy;
    double dz;               
    bool frameCountEnabled;
    int frameCount;
    bool reverseDirection;


};

#endif // MOVIEDIALOG_H
