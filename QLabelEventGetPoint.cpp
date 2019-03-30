#include "QLabelEventGetPoint.h"
#include "WaterBody.h"
#include <QMouseEvent>
#include <QPair>
#include <QPointF>

QLabelEventGetPoint::QLabelEventGetPoint(QWidget *parent) : QLabel(parent), modeGamePoint_(true)
{
}

// Переопределение эвента, который вызывается после каждого движения мышью в пределах QLable.
// Расчет координат и их отправка
void QLabelEventGetPoint::mouseMoveEvent(QMouseEvent *event)
{
    mouseMovePoint(event->pos());

    QWidget::mouseMoveEvent(event);
}

// При наведении на виджет точки
void QLabelEventGetPoint::mouseMovePoint(const QPoint position)
{
    if(!pixmap() || pixmap()->isNull())
        return;

    if(!modeGamePoint_) {
        emit signalSendPoint(QPointF {
            static_cast<qreal>(position.x()), static_cast<qreal>(position.y())
        });
    } else
        emit signalSendPoint(computeGamePoint(position.x(), position.y()));
}

// Переопределение эвента, который вызывается после каждого нажатия мыши в пределах
// изображения карты
void QLabelEventGetPoint::mousePressEvent(QMouseEvent *event)
{
    if(!pixmap() || pixmap()->isNull())
        return;

    if (event->button()==Qt::LeftButton) {
        emit signalSendMousePoint(QPointF {
                static_cast<qreal>(event->pos().x()), static_cast<qreal>(event->pos().y())
        });
    }

    QWidget::mousePressEvent(event);
}

// Переопределение эвента, который вызывается после каждого изменения размера виджета
void QLabelEventGetPoint::resizeEvent(QResizeEvent *event)
{   
    WaterBody::getInstance().setNewGeometry();

    QLabel::resizeEvent(event);
}

QPointF QLabelEventGetPoint::computeGamePoint(const double mousePointX, const double mousePointY)
{
    // Текущая ширина и высота QLable
    const double width = this->width();
    const double height = this->height();

    // Диапазоны координат из синглтона
    QPair<QPair<double, double>, QPair<double, double>> mapRanges = WaterBody::getInstance().getMapRanges();
    QPair<double, double> rangeX = mapRanges.first;
    QPair<double, double> rangeY = mapRanges.second;

    // Игровые координаты
    double gamePointX = rangeX.first + (rangeX.second - rangeX.first)
            * (mousePointX / width);
    double gamePointY = rangeY.second - (rangeY.second - rangeY.first)
            * (mousePointY / height);

    return QPointF(gamePointX, gamePointY);
}

// Переключение режима отображения(игровые точки/координаты мыши)
void QLabelEventGetPoint::slotSwitchMode()
{
    modeGamePoint_ = !modeGamePoint_;
}
