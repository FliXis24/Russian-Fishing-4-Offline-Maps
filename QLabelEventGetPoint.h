#ifndef QLABELEVENTGETPOINT_H
#define QLABELEVENTGETPOINT_H

#include <QLabel>

// Класс для расчета координат с помощью переопределенного метода
class QLabelEventGetPoint : public QLabel {
    Q_OBJECT

public:
    explicit QLabelEventGetPoint(QWidget *parent = nullptr);

    // При наведении на виджет точки
    void mouseMovePoint(const QPoint position);

protected:
    // Переопределение эвента, который вызывается после каждого движения мышью в пределах
    // изображения карты
    void mouseMoveEvent(QMouseEvent *event) override;

    // Переопределение эвента, который вызывается после каждого нажатия мыши в пределах
    // изображения карты
    void mousePressEvent(QMouseEvent *event) override;

    // Переопределение эвента, который вызывается после каждого изменения размера виджета
    void resizeEvent(QResizeEvent *event) override;

private:
    QPointF computeGamePoint(const double MousePointX, const double MousePointY);

    // Переключатель режима отображения(игровые точки/координаты мыши)
    bool modeGamePoint_;

signals:
    // Отсылает игровые точки/координаты мыши
    void signalSendPoint(const QPointF);

    // Отсылает координаты мыши
    void signalSendMousePoint(const QPointF);

public slots:
    // Переключение режима отображения(игровые точки/координаты мыши)
    void slotSwitchMode();
};

#endif // QLABELEVENTGETPOINT_H
