//CircleDecomposer.cpp

#include <ui/CircleDecomposer.hpp>
#include <QtConcurrentRun>

#include <iostream>
using namespace std;


void CircleDecomposer::setSourceImage(QImage orig)
{
    this->originalImg = QImage(orig);
    this->cappedImg = QImage(orig);
    this->invalidateImage();
}

void CircleDecomposer::invalidateImage()
{
    QRectF area = this->sceneRect();
    this->caps.clear();
    this->procRect = this->addRect(area, QPen(), QBrush(QColor(255,255,255, 200)));
    this->procText = this->addText("Processing...");
    this->update();

    //Don't run on the event thread
    QtConcurrent::run(this, &CircleDecomposer::regenerateCappedImage);
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

void CircleDecomposer::showImage(bool flag)
{
    if (this->procRect) this->removeItem(this->procRect);
    if (this->procText) this->removeItem(this->procText);
    
    this->procRect = NULL;
    this->procText = NULL;
    
    this->clear();
    if(flag) {
	for(int i=0; i<this->caps.size(); ++i) {
	    this->addItem(this->caps.at(i));
	}
    }
}

void CircleDecomposer::regenerateCappedImage()
{
    switch(this->layout) {
    case GRID:
    	this->capcount = create_average_grid();
    	break;
    case COMPACT:
    	this->capcount = create_compact_average_grid();
    	break;
    default:
    	cout << "[Error] Invalid layout requested. Defaulting to GRID." << endl;
    	break;
    };

    Q_EMIT imageUpdated(true);
    Q_EMIT capCountChanged(this->capcount);
}

void CircleDecomposer::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(mouseEvent->button() == Qt::LeftButton
       && this->bgPickMode) {
	this->bgPickMode = false;

	QPointF pt = mouseEvent->scenePos();

	QGraphicsItem *item = this->itemAt(pt);
	if (item) {
	    QAbstractGraphicsShapeItem *shape = static_cast<QAbstractGraphicsShapeItem*>(item);
	    this->setBackgroundBrush(shape->brush());
	    Q_EMIT parametersChanged();
	}
	else if(this->sceneRect().contains(pt) && this->caps.size() > 0) {
	    //Fall back algorithm: if this event is 'in' the sceneRect, then
	    // find the closest (cartesian) object and use /its/ brush.

	    item = this->caps[0];
	    uint dist = sqrt(pow(abs(pt.x() - item->x()), 2) + pow(abs(pt.y() - item->y()), 2));
	    
	    for(int i=1; i<this->caps.size(); ++i) {
		uint tdist = sqrt(
				  pow(abs(pt.x() - this->caps[i]->boundingRect().x()), 2) +
				  pow(abs(pt.y() - this->caps[i]->boundingRect().y()), 2)
				  );

		if (tdist < dist) {
		    dist = tdist;
		    item = this->caps[i];
		}
	    }

	    if (item) {
	        QAbstractGraphicsShapeItem *shape = static_cast<QAbstractGraphicsShapeItem*>(item);
	        this->setBackgroundBrush(shape->brush());
		Q_EMIT parametersChanged();
	    }

	}

	this->filterBg = true;
	Q_EMIT backgroundPicked();
    }
    else {
	QGraphicsScene::mouseReleaseEvent(mouseEvent);
    }
}

static QColor getDominantColor(QImage &img, QRectF area)
{
    //TODO: this is an average. It should be: convert rgb to hsv, use hue to
    // bin the colors
    unsigned long r=0, g=0, b=0;

    for(int y=area.y(); y<area.y() + area.height(); ++y) {
    	for(int x=area.x(); x < area.x() + area.width(); ++x) {
    	    QRgb color = img.pixel(x,y);
    	    r += qRed(color);
    	    g += qGreen(color);
    	    b += qBlue(color);
    	}
    }

    unsigned long pixelCount = area.width() * area.height();
    return QColor(r/pixelCount, g/pixelCount, b/pixelCount);
}

unsigned long CircleDecomposer::create_compact_average_grid()
{
    this->caps.clear();

    uint blocksize = 2*radius + spacing;
    uint vert_space = blocksize * sin(M_PI/4.0);
    uint horiz_space = blocksize / cos(M_PI/4.0);
    uint offset = blocksize * cos(M_PI/4.0);
    uint x = 0;
    uint y = 0;
    uint row = 0;
    QColor bgColor = this->backgroundBrush().color();

    while(y + blocksize < (unsigned int)this->originalImg.height()) {
    	while (x + blocksize < (unsigned int)this->originalImg.width()) {
	    QRectF currentArea(x,y,radius*2,radius*2);
    	    QColor avgColor = getDominantColor(this->originalImg, currentArea);
	    if(avgColor != bgColor || !filterBg) {
		CircleItem *item = new CircleItem(currentArea);
		item->setBrush(QBrush(avgColor));
		item->setPen(QPen(avgColor));
		caps.append(item);
	    }

    	    x += horiz_space;
    	}
    	y += vert_space;
    	x = ((++row % 2) * offset);
    }

    return caps.size();
}

unsigned long CircleDecomposer::create_average_grid()
{
    this->caps.clear();
    uint blocksize = 2*radius + spacing;
    QColor bgColor = this->backgroundBrush().color();

    for(uint y=0; y+blocksize<(uint)this->originalImg.height(); y += blocksize) {
	for(uint x=0; x+blocksize<(uint)this->originalImg.width(); x += blocksize) {
	    QRectF currentArea(x,y,radius*2,radius*2);	    
    	    QColor avgColor = getAverage(this->originalImg, currentArea);
	    if(avgColor != bgColor || !filterBg) {
		CircleItem *item = new CircleItem(currentArea);
		item->setBrush(QBrush(avgColor));
		item->setPen(QPen(avgColor));

		caps.append(item);
	    }
    	}
    }

    return caps.size();
}

void CircleDecomposer::setBackgroundPickMode(bool enable)
{
    this->bgPickMode = enable;
}
