#ifndef FOUNDPOINT_H
#define FOUNDPOINT_H

#include <QWidget>
#include <QTimer>
#include <QTime>

// Константы размеров для рисования и геометрии
extern const int CIRCLE_SIZE;
extern const int CIRCLE_FRAME;
extern const int WIDGET_SIZE;
extern const int HALF_CIRCLE_SIZE;
extern const int CIRCLE_INDENT;

// Константа времени отображения виджета найденной точки
extern int DISPLAY_TIME;

class FoundPoint : public QWidget
{
    Q_OBJECT
public:
    explicit FoundPoint(const QPointF &mousePoint, const QPointF &gamePoint, QWidget *parent = nullptr);

    // Возвращает игровые координаты точки
    const QPointF &getGamePoint() const;

protected:
    // Переопределение эвента для отрисовки виджета
    void paintEvent(QPaintEvent *event);

    // Переопределение эвента для запуска таймера
    void showEvent(QShowEvent *event);

private:
    // Координаты мыши точки
    QPointF mousePoint_;

    // Игровые координаты точки
    QPointF gamePoint_;

    // Таймер
    QTimer *timer_;
};

#endif // FOUNDPOINT_H
