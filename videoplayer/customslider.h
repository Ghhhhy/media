#ifndef CUSTOMSLIDER_H
#define CUSTOMSLIDER_H
#include <QSlider>
#include <QObject>

class customslider : public QSlider
{
public:
    customslider(QWidget *parent = nullptr);
    ~customslider();
    void mousePressEvent(QMouseEvent *ev);//重写QSlider的mousePressEvent事件
};

#endif // CUSTOMSLIDER_H
