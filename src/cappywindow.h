#include "ui_main.h"
#include <QtGui>
#include <QtConcurrentRun>

#include "ui/CircleDecomposer.hpp"

#include <opencv/cv.h>
#include <opencv/highgui.h>

//DEBUG
#include <iostream>
using namespace std;

class CappyWindow : public QMainWindow, private Ui::MainWindow {
    Q_OBJECT
 public:
    CappyWindow(QMainWindow *parent = 0);
    ~CappyWindow();

    Q_SLOT bool loadImage(QString path);
    Q_SLOT void update_status_bar(int capcount);

 private:
    QGraphicsScene *original_scene;
    CircleDecomposer *capped_scene;
    cv::Mat origmat;

    void cap_it();

    QFileDialog *openFileDlg;

    Q_SLOT void updateRadius(int);
    Q_SLOT void updateSpacing(int);
    Q_SLOT void updateLayout(int);
    Q_SLOT void setParametersEnabled(bool);
    Q_SLOT void bgPicked();

    static unsigned long create_compact_average_grid(cv::Mat &img, cv::Mat &dst, int radius, int spacing);
    static unsigned long create_average_grid(cv::Mat &img, cv::Mat &dst, int radius, int spacing);
};
