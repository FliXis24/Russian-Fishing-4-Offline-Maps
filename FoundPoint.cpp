#include "FoundPoint.h"
#include "WaterBody.h"
#include <QPainter>

// Константы размеров для рисования и геометрии
const int CIRCLE_SIZE = 32;
const int CIRCLE_FRAME = CIRCLE_SIZE / 4;
const int WIDGET_SIZE = CIRCLE_SIZE + CIRCLE_FRAME;
const int HALF_CIRCLE_SIZE = WIDGET_SIZE / 2;
const int CIRCLE_INDENT = CIRCLE_FRAME / 2;

// Константа времени отображения виджета найденной точки
int DISPLAY_TIME = 5000;

FoundPoint::FoundPoint(const QPointF &mousePoint, const QPointF &gamePoint, QWidget *parent) :
    QWidget(parent),
    mousePoint_(mousePoint),
    gamePoint_(gamePoint),
    timer_(new QTimer(this))
{
    // Удалить виджет после события закрытия
    setAttribute(Qt::WA_DeleteOnClose);

    setGeometry(
        qRound(mousePoint_.x()) - HALF_CIRCLE_SIZE,
        qRound(mousePoint_.y()) - HALF_CIRCLE_SIZE,
        WIDGET_SIZE, WIDGET_SIZE
    );

    // Прозрачный для эвентов мыши
    setAttribute(Qt::WA_TransparentForMouseEvents, true);

    // Соединение сигнала окончания таймера с закрытием виджета
    connect(timer_, &QTimer::timeout, this, &FoundPoint::close);
}

// Переопределение эвента для отрисовки виджета
void FoundPoint::paintEvent(QPaintEvent *event)
{
    QPainter paint(this);
    paint.setPen(QPen(Qt::blue, CIRCLE_FRAME, Qt::SolidLine));
    paint.setRenderHint(QPainter::Antialiasing, true);
    paint.drawEllipse(CIRCLE_INDENT, CIRCLE_INDENT, CIRCLE_SIZE, CIRCLE_SIZE);

    QWidget::paintEvent(event);
}

// Переопределение эвента для запуска таймера
void FoundPoint::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    // Запуск таймера
    timer_->start(DISPLAY_TIME);
}

// Возвращает игровые координаты точки
const QPointF &FoundPoint::getGamePoint() const
{
    return gamePoint_;
}
