//cappy.cc

#include <stdlib.h>
#include <iostream>
#include "cappywindow.h"
#include <QtGui>

#include <opencv/cv.h>
#include <opencv/highgui.h>

using namespace std;

int main(int argc, char **argv) {
    cout << "Running..." << endl;
    QApplication app(argc, argv);
    CappyWindow *win = new CappyWindow();

    win->show();
    return app.exec();
}
