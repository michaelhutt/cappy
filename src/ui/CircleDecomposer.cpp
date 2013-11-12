//CircleDecomposer.cpp

#include <ui/CircleDecomposer.hpp>
#include <iostream>
using namespace std;

#include <opencv/highgui.h>

void CircleDecomposer::setSourceImage(cv::Mat &orig)
{
    orig.copyTo(this->originalImg);
    orig.copyTo(this->cappedImg);
    this->invalidateImage();
}

void CircleDecomposer::invalidateImage()
{
    QRectF area = this->sceneRect();
    this->addRect(area, QPen(), QBrush(QColor(255,255,255, 200)));
    this->addText("Processing...");

    //Don't run on the event thread
    QFuture<void> future = QtConcurrent::run(this, &CircleDecomposer::regenerateCappedImage);
}

void CircleDecomposer::setRadius(int value)
{
    this->radius = value;
    Q_EMIT parametersChanged();
}

void CircleDecomposer::setSpacing(int value)
{
    this->spacing = value;
    Q_EMIT parametersChanged();
}

void CircleDecomposer::setLayout(int layout)
{
    this->setLayout((LayoutType)layout);
}

void CircleDecomposer::setLayout(CircleDecomposer::LayoutType layout) {
    this->layout = layout;
    Q_EMIT parametersChanged();
}

void CircleDecomposer::showImage(bool flag) {
    this->clear();

    if(flag) {
	this->addPixmap(QPixmap::fromImage(QImage(this->cappedImg.data,
						  this->cappedImg.cols,
						  this->cappedImg.rows,
						  this->cappedImg.step,
						  QImage::Format_RGB888
						  )));
    }
}

void CircleDecomposer::regenerateCappedImage()
{
    switch(this->layout) {
    case GRID:
    	this->capcount = create_average_grid(this->originalImg, this->cappedImg, this->radius, this->spacing);
    	break;
    case COMPACT:
    	this->capcount = create_compact_average_grid(this->originalImg, this->cappedImg, this->radius, this->spacing);
    	break;
    default:
    	cout << "[Error] Invalid layout requested. Defaulting to GRID." << endl;
    	break;
    };

    cv::cvtColor(this->cappedImg, this->cappedImg, CV_BGR2RGB);
    
    Q_EMIT imageUpdated(true);
    Q_EMIT capCountChanged(this->capcount);
}

void CircleDecomposer::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(mouseEvent->button() == Qt::LeftButton
       && this->bgPickMode) {
	QPointF pt = mouseEvent->scenePos();

	uchar *data = this->cappedImg.row(pt.y()).col(pt.x()).data;
	cout << "color picked (rgb): " << (int)*data << " " << (int)(*(data+1)) << " " << (int)(*(data + 2)) << endl;

	Q_EMIT backgroundPicked();
    }
    else {
	QGraphicsScene::mouseReleaseEvent(mouseEvent);
    }
}


unsigned long CircleDecomposer::create_compact_average_grid(cv::Mat &img, cv::Mat &dst, int radius, int spacing) {
    cv::Mat mask(img.rows, img.cols, CV_8UC1);
    dst.setTo(cv::Scalar(0));

    unsigned long capcount = 0;

    CvMat imgdata(img);
    CvMat maskdata(mask);

    int vert_space = ((2 * radius) + spacing) * sin(M_PI/4.0);
    int horiz_space = ((2 * radius) + spacing) / cos(M_PI/4.0);
    int offset = ((2 * radius) + spacing) * cos(M_PI/4.0);
    int x = spacing+radius;
    int y = spacing+radius;
    int row = 0;

    while(y + spacing + radius < dst.rows) {
	while (x + spacing + radius < dst.cols) {
	    cv::Point center(x,y);
	    mask.setTo(cv::Scalar(0));
	    cv::circle(mask, center, radius, cv::Scalar(255), CV_FILLED);
	    cv::Scalar avg = cvAvg(&imgdata, &maskdata);
	    cv::circle(dst, center, radius, avg, CV_FILLED);
	    ++capcount;

	    x += horiz_space;
	}
	y += vert_space;
	x = spacing + radius + ((++row % 2) * offset);
    }

    return capcount;
}

unsigned long CircleDecomposer::create_average_grid(cv::Mat &img, cv::Mat &dst, int radius, int spacing) {
    unsigned long cols = int(floor(img.cols / (2 * radius + spacing)));
    unsigned long rows = int(floor(img.rows / (2 * radius + spacing)));
    cv::Mat mask(img.rows, img.cols, CV_8UC1);
    dst.setTo(cv::Scalar(0));

    CvMat imgdata(img);
    CvMat maskdata(mask);

    for (uint r=0; r<rows; r++) {
	for (uint c=0; c<cols; c++) {
	    cv::Point center(c * (2 * radius + spacing) + radius, r * (2 * radius + spacing) + radius);
	    mask.setTo(cv::Scalar(0));
	    cv::circle(mask, center, radius, cv::Scalar(255), CV_FILLED);
	    cv::Scalar avg = cvAvg(&imgdata, &maskdata);
	    cv::circle(dst, center, radius, avg, CV_FILLED);
	}
    }

    return (rows * cols);
}

void CircleDecomposer::setBackgroundPickMode(bool enable)
{
    this->bgPickMode = enable;
}

void CircleDecomposer::setBackgroundColor(cv::Scalar &color)
{
    
}

void CircleDecomposer::filterBackground(bool doFilter)
{

}
