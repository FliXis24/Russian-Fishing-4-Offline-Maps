#ifndef POINT_H
#define POINT_H

#include <QWidget>
#include <QPointF>
#include <QPair>
#include <QString>
#include <QLabel>

namespace Ui {
class Point;
}

class Point : public QWidget
{
    Q_OBJECT

public:
    Point(
        int highPriority,
        QPair<double, double> mousePercents,
        QPointF gamePoint,
        QWidget *parent = nullptr,
        QPointF mousePoint = QPointF(-1, -1)
    );

    ~Point() override;

    // Установить координаты мыши для точки
    void setNewMousePoint(QPointF &mousePoint);

    // Возвращает процентное соотношение координат мыши относительно области карты для
    // текущей точки
    const QPair<double, double> &getMousePercents() const;

    // Возвращает координаты мыши текущей точки
    const QPointF &getMousePoint() const;

    // Возвращает игровые координаты мыши точки
    const QPointF &getGamePoint() const;

    // Вернуть уникальный идентификатор точки
    const int &getID() const;

    // Вернуть приоритет
    int &getPriority();

    // Возвращает указатель на виджет изображения точки у точки
    QLabel* getLabelImagePoint();

    // Возвращает указатель на виджет с текстовыми координатами точки у точки
    QLabel* getLabelTextGamePoint();

    int typeTackle;       // Тип снасти
    int from;             // Лодка/Берег
    QString fish;         // Рыба
    QString baits;        // Наживка/Приманка
    QString rigs;         // Тип оснастки
    QString note;         // Заметка
    QString flyCasting;   // Направление заброса
    QString distance;     // Дальность заброса
    QString depth;        // Глубина
    QString groundbaits;  // Прикормка
    QString unwantedFish; // Нежелательная рыба

protected:
    // Переопределение эвента, который вызывается после отображения окна
    void showEvent(QShowEvent *event) override;

    // Переопределение эвента, который вызывается при движении мышью через область виджета
    // точки
    void mouseMoveEvent(QMouseEvent *event) override;

    // Переопределение эвента, который вызывается после каждого нажатия мыши в пределах
    // виджета точки
    void mousePressEvent(QMouseEvent *event) override;

private:
    Ui::Point *ui;

    // Уникальный идентификатор точки
    int id_;

    // Высокий приоритет
    int highPriority_;

    // Процентное соотношение координат мыши относительно области окна карты
    const QPair<double, double> mousePercents_;

    // Игровые координаты
    const QPointF gamePoint_;

    // Координаты мыши
    QPointF mousePoint_;

    // Установить уникальный идентификатор для точки
    int setIndexNumber();

    // Установка настроек цвета и размера шрифта, и размера изображения точки
    void setPointAttributes();

signals:
    // Сигнал редактирования точки
    void signalEditPoint(Point* currentPoint);

    // Сигнал отсылает текст для слота текстовых уведомлений
    void signalSendTextInformator(const QString& text);

    // Сигнал отсылает тип звука в слот звуковых уведомлений
    void signalPlaySound(const QString& type);

private slots:
    // Слот редактирования текущей точки
    void slotEditPoint(Point* currentPoint);

    // Слот удаления текущей точки
    void slotDeletePoint();
};

#endif // POINT_H
