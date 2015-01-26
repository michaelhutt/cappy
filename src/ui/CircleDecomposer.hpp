//CircleDecomposer.hpp

#include <QtGui>
#include <ui/CircleItem.hh>

class CircleDecomposer : public QGraphicsScene {
    Q_OBJECT
public:
    typedef enum {GRID,COMPACT} LayoutType;

    CircleDecomposer()
	: QGraphicsScene(), bgPickMode(false), filterBg(false),
	  radius(8), spacing(1), layout(GRID), capcount(0), procRect(NULL), procText(NULL)
    {
	this->setBackgroundBrush(QBrush());
	QObject::connect(this, SIGNAL(parametersChanged(void)), this, SLOT(invalidateImage(void)));
	QObject::connect(this, SIGNAL(imageUpdated(bool)), this, SLOT(showImage(bool)));
    };
    virtual ~CircleDecomposer() {};

    Q_SLOT void setRadius(int value);
    Q_SLOT void setSpacing(int value);
    Q_SLOT void setLayout(CircleDecomposer::LayoutType layout);
    Q_SLOT void setLayout(int layout);
    Q_SLOT void showImage(bool flag);
    void setSourceImage(QImage orig);


    Q_SLOT void setBackgroundPickMode(bool enable=true);
    
    Q_SLOT void invalidateImage();
    Q_SIGNAL void backgroundPicked();
    Q_SIGNAL void parametersChanged();
    Q_SIGNAL void imageUpdated(bool);
    Q_SIGNAL void capCountChanged(int);

    void regenerateCappedImage();

protected:
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

private:
    QImage cappedImg;
    QImage originalImg;
    bool bgPickMode;
    bool filterBg;
    unsigned int  radius;
    unsigned int spacing;
    LayoutType layout;
    int capcount;

    QGraphicsRectItem *procRect;
    QGraphicsTextItem *procText;

    QList<CircleItem*> caps;

    unsigned long create_compact_average_grid();
    unsigned long create_average_grid();
};
