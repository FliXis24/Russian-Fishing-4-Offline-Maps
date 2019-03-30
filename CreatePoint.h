#ifndef CREATEPOINT_H
#define CREATEPOINT_H

#include <QWidget>
#include "Point.h"

namespace Ui {
class CreatePoint;
}

class CreatePoint : public QWidget
{
    Q_OBJECT

public:
    CreatePoint(const QPointF &gamePoint, const QPointF &mousePoint, const QPair<double, double> mousePercents, QWidget *parent = nullptr);
    CreatePoint(Point *point, QWidget *parent = nullptr);
    ~CreatePoint();

private:
    // Подтверждение сохранения данных после редактирования текущей точки
    void confirmEdit(Point *currentPoint);

    // Игровые координаты точки
    QPointF gameCurrentPoint_;

    // Координаты мыши точки
    QPointF mouseCurrentPoint_;

    // Процентное соотношение координат мыши к области карты
    QPair<double, double> mouseCurrentPercents_;

    Ui::CreatePoint *ui;

signals:
    // Сигнал отсылает текст для слота текстовых уведомлений
    void signalSendTextInformator(const QString& text);

    // Сигнал отсылает тип звука в слот звуковых уведомлений
    void signalPlaySound(const QString& type);

private slots:
    // Слот переключение активности поля Глубина взависимости от индекса Тип Ловли
    void slotDepthEnabledSwitch(int index);

    // Слот удаления текущей точки и запись в файл
    void slotDeletePoint(Point* point);

    // Слот создания новой точки
    void slotConfirmCreate();

};

#endif // CREATEPOINT_H
