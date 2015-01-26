//cappywindow.cpp

#ifndef _CAPPYWINDOW_CPP
#define _CAPPYWINDOW_CPP

#include <cappywindow.h>
#include <exception>

CappyWindow::CappyWindow(QMainWindow *parent)
    : QMainWindow(parent), original_scene(NULL), capped_scene(NULL)
{
    setupUi(this);

    this->original_scene = new QGraphicsScene();
    this->ui_original_gview->setScene(this->original_scene);

    this->capped_scene = new CircleDecomposer();
    this->ui_cap_gview->setScene(this->capped_scene);


    this->openFileDlg = new QFileDialog(this);
    this->openFileDlg->setAcceptMode(QFileDialog::AcceptOpen);
    this->openFileDlg->setFileMode(QFileDialog::ExistingFile);
    this->openFileDlg->setNameFilter("Image Files (*.bmp *.dib *.jpeg *.jpg *.jpe *.jp2 *.png *.pbm *.pgm *.ppm *.tiff *.tif)");


    QObject::connect(this->ui_radius, SIGNAL(valueChanged(int)), this, SLOT(updateRadius(int)));
    QObject::connect(this->ui_spacing, SIGNAL(valueChanged(int)), this, SLOT(updateSpacing(int)));
    QObject::connect(this->ui_layout, SIGNAL(currentIndexChanged(int)), this, SLOT(updateLayout(int)));

    QObject::connect(this->action_Open, SIGNAL(triggered(bool)), this->openFileDlg, SLOT(show(void)));
    QObject::connect(this->openFileDlg, SIGNAL(fileSelected(QString)), this, SLOT(loadImage(QString)));
    QObject::connect(this->pickbackground, SIGNAL(clicked()), this->capped_scene, SLOT(setBackgroundPickMode()));

    QObject::connect(this->capped_scene, SIGNAL(capCountChanged(int)), this, SLOT(update_status_bar(int)));
    QObject::connect(this->capped_scene, SIGNAL(imageUpdated(bool)), this, SLOT(setParametersEnabled(bool)));
    QObject::connect(this->capped_scene, SIGNAL(backgroundPicked()), this, SLOT(bgPicked()));

    this->original_scene->addText("No file selected [File->Open]");
    this->setParametersEnabled(false);
    this->show();
}

CappyWindow::~CappyWindow() {
    delete this->original_scene;
    delete this->capped_scene;
    delete this->openFileDlg;
}

void CappyWindow::updateRadius(int value) { this->setParametersEnabled(false); this->capped_scene->setRadius(value); }
void CappyWindow::updateSpacing(int value) { this->setParametersEnabled(false); this->capped_scene->setSpacing(value); }
void CappyWindow::updateLayout(int value) { this->setParametersEnabled(false); this->capped_scene->setLayout(value); }
void CappyWindow::bgPicked() { this->setParametersEnabled(true); }

void CappyWindow::setParametersEnabled(bool enabled) {
    this->ui_radius->setEnabled(enabled);
    this->ui_spacing->setEnabled(enabled);
    this->ui_layout->setEnabled(enabled);
    this->pickbackground->setEnabled(enabled);
    this->update();
}

void CappyWindow::update_status_bar(int capcount) {
    QString msg = "Cap count: ";
    msg.append(QString::number(capcount));
    msg.append(". That's about ");
    msg.append(QString::number((12 * capcount) / 128.0));
    msg.append(" gallons. Estimated cost is $");
    msg.append(QString::number(floor(capcount * 1.25)));
    msg.append(" to $");
    msg.append(QString::number(ceil(capcount * 1.5)));

    ((Ui::MainWindow*)this)->statusBar->showMessage(msg);

    this->setParametersEnabled(true);
}


bool CappyWindow::loadImage(QString path) {
    this->setParametersEnabled(false);
    this->original_scene->clear();
    this->capped_scene->clear();

    this->original_scene->addPixmap(QPixmap(path));

    try {
	this->capped_scene->setSourceImage(QImage(path));
	this->setParametersEnabled(true);
    }
    catch (std::exception) {
	this->original_scene->addText("Sorry, eh: Could not read image");
    }
    return true;
}

#endif
