//CircleDecomposer.hpp

#include <opencv/cv.h>
#include <QtGui>

class CircleDecomposer : public QGraphicsScene {
    Q_OBJECT
public:
    typedef enum {GRID,COMPACT} LayoutType;

    CircleDecomposer()
	: QGraphicsScene(), filterBg(false), bgPickMode(false),
	  radius(8), spacing(1), layout(GRID), capcount(0)
    {
	QObject::connect(this, SIGNAL(parametersChanged(void)), this, SLOT(invalidateImage(void)));
	QObject::connect(this, SIGNAL(imageUpdated(bool)), this, SLOT(showImage(bool)));
    };
    virtual ~CircleDecomposer() {};

    Q_SLOT void setRadius(int value);
    Q_SLOT void setSpacing(int value);
    Q_SLOT void setLayout(CircleDecomposer::LayoutType layout);
    Q_SLOT void setLayout(int layout);
    Q_SLOT void showImage(bool flag);
    void setSourceImage(cv::Mat &orig);


    Q_SLOT void setBackgroundPickMode(bool enable=true);
    void setBackgroundColor(cv::Scalar &color);
    void filterBackground(bool doFilter);

    Q_SLOT void invalidateImage();
    Q_SIGNAL void backgroundPicked();
    Q_SIGNAL void parametersChanged();
    Q_SIGNAL void imageUpdated(bool);
    Q_SIGNAL void capCountChanged(int);

    void regenerateCappedImage();

protected:
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

private:
    cv::Mat cappedImg;
    cv::Mat originalImg;
    cv::Scalar bgColor;
    bool filterBg;
    bool bgPickMode;
    unsigned int  radius;
    unsigned int spacing;
    LayoutType layout;
    int capcount;

    unsigned long create_compact_average_grid(cv::Mat &img, cv::Mat &dst, int radius, int spacing);
    unsigned long create_average_grid(cv::Mat &img, cv::Mat &dst, int radius, int spacing);
};
