#ifndef QLABELCLEARTEXTTIMER_H
#define QLABELCLEARTEXTTIMER_H

#include <QLabel>
#include <QTimer>
#include <QTime>

// Константа времени отображения текстового уведомления в информаторе
extern const int DISPLAY_INFORMATION_TIME;

// Константа частоты обновления игрового времени
extern const int GAME_TIME_UPDATE_RATE;

// Сколько игровых часов в одном реальном часе
extern const int TIME_DIFFERENCE;

class QLabelClearTextTimer : public QLabel
{
    Q_OBJECT
public:
    explicit QLabelClearTextTimer(QWidget *parent = nullptr);

    // Метод для отображения текстовых уведомлений в информаторе, с таймером для их очистки
    void setTextInformation(const QString &text);

    // Запустить отображение игрового времени в информаторе
    void startTime();

    // Остановить таймеры
    void stopTimers();

private:
    // Таймер для вывода информации
    QTimer *timerInfo_;

    // Таймер для вывода игрового времени
    QTimer *timerTime_;

    // Системное время
    QTime currTime_;

    // Очистка текста и остановка таймера
    void clearTextInformation();

    // Обновить игровое время
    void updateTime();
};

#endif // QLABELCLEARTEXTTIMER_H
