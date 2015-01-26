#include <QtGui>
#include <iostream>

using namespace std;

class CircleItem : public QObject, public QGraphicsEllipseItem {
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    Q_PROPERTY(QRectF rect READ rect WRITE setRect)

public:
    CircleItem(const QRectF & rect, QGraphicsItem * parent = 0 ) : QGraphicsEllipseItem(rect, parent) {
    }

    Q_SLOT void animate() {
	cout << "pos: " << this->boundingRect().x() << "," << this->boundingRect().y() << endl;

    	QPropertyAnimation *anim = new QPropertyAnimation(this,"rect");
    	anim->setDuration(1000);
	anim->setStartValue(this->boundingRect());
	QRectF end(this->boundingRect());
	end.moveTo(end.x()+50, end.y());
	cout << " to: " << end.x() << "," << end.y() << endl;
	anim->setEndValue(end);
    	anim->start(QAbstractAnimation::DeleteWhenStopped);
    }
};
