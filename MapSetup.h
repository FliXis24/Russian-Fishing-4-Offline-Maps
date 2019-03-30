#ifndef MAPSETUP_H
#define MAPSETUP_H

#include <QWidget>
#include <memory>

namespace Ui {
class MapSetup;
}

class MapSetup : public QWidget
{
    Q_OBJECT

public:
    MapSetup(const int& width, const int& height, QWidget *parent = nullptr);
    ~MapSetup();

private:
    // Возвращает текст руководства
    void setTextManual();

    // Проверка полей на валидность
    bool isValidFields();

    Ui::MapSetup *ui;

    // Координаты мыши
    QPointF leftMousePoint_;
    QPointF rightMousePoint_;
    QPointF topMousePoint_;

    // Игровые координаты
    QPointF leftGamePoint_;
    QPointF rightGamePoint_;
    QPointF topGamePoint_;

    // Текущий размер QLable с QPixmap
    double widthControl_;
    double heightControl_;

    // Счетчик кликов
    int clickCounter_ = 0;

signals:
    // Сигнал переключение режима отображения(игровые точки/координаты мыши)
    void signalSwitchMode();

    // Сигнал возвращает диапазон координат для Менеджера карт
    void signalReturnRanges(QPair<QPair<double, double>, QPair<double, double>>);

    // Сигнал отсылает текст для слота текстовых уведомлений
    void signalSendTextInformator(const QString& text);

public slots:
    // Установка координат мыши за три клика
    void slotSetLineEditPoint(QPointF mousePoint);

private slots:
    // Создание Пользовательских настроек текущей карты
    void on_pushButtonOK_clicked();
    // Включение кнопки OK
    void slotEnabledButtonOK(const QString& text);
};

#endif // MAPSETUP_H
