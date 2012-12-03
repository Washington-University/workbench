#ifndef MOVIE_DIALOG_H
#define MOVIE_DIALOG_H

#include <QDialog>
#include <VolumeSliceViewPlaneEnum.h>
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

	void processUpdateVolumeSlice();
    int32_t getSliceDelta(const std::vector<int64_t> &dim, const caret::VolumeSliceViewPlaneEnum::Enum &vpe, const int32_t &sliceIndex);

private slots:
    void on_closeButton_clicked();

    void on_animateButton_toggled(bool checked);

    void on_recordButton_toggled(bool checked);

    void on_cropImageCheckBox_toggled(bool checked);

    void on_workbenchWindowSpinBox_valueChanged(int arg1);

private:
    Ui::MovieDialog *ui;

    void captureFrame(AString filename);

    void processRotateTransformation(const double dx,
        const double dy,
        const double dz);

    int32_t m_browserWindowIndex;

    int frame_number;
    int rotate_frame_number;
    double dx;
    double dy;
    double dz;               
    bool frameCountEnabled;
    int frameCount;
    bool reverseDirection;
	int32_t dP;//change in Parasagital slice
	int32_t dC;//change in Coronal slice
	int32_t dA;//change in Axial slice

    bool m_useCustomSize;
    int32_t imageX;
    int32_t imageY;
    int32_t croppedImageX;
    int32_t croppedImageY;

	bool m_animationStarted;
	int32_t m_volumeSliceIncrement;
	bool m_reverseVolumeSliceDirection;
	int64_t m_AStart;
	int64_t m_AEnd;
	int64_t m_CStart;
	int64_t m_CEnd;
	int64_t m_PStart;
	int64_t m_PEnd;   
	
};
#endif // MOVIE_DIALOG_H
