#include "QLabelClearTextTimer.h"
#include "MainWindow.h"

// Константа времени отображения текстового уведомления в информаторе
const int DISPLAY_INFORMATION_TIME = 5000;

// Константа частоты обновления игрового времени
const int GAME_TIME_UPDATE_RATE = 1000;

// Сколько игровых часов в одном реальном часе
const int TIME_DIFFERENCE = 24;

QLabelClearTextTimer::QLabelClearTextTimer(QWidget *parent) :
    QLabel(parent),
    timer_(new QTimer(this))
{
    // Соединение сигнала окончания таймера со слотом, где запускается таймер игрового
    // времени
    connect(timer_, &QTimer::timeout, this, &QLabelClearTextTimer::clearTextInformation);

    // Запустить отображение игрового времени
    startTime();
}

// Метод для установки текстовых уведомлений с таймером для их очистки
void QLabelClearTextTimer::setTextInformation(const QString &text)
{
    // Останавливаем таймер, если он был запущен из-за вызова setTime()
    if(timer_->isActive())
        timer_->stop();

    setStyleSheet("color: green");

    setText(text);

    // Запуск таймера
    timer_->start(DISPLAY_INFORMATION_TIME);
}

// Очистка текста и остановка таймера
void QLabelClearTextTimer::clearTextInformation()
{
    // остановка таймера
    timer_->stop();

    clear();

    // Вернуть дефолтный цвет шрифта в информаторе
    setStyleSheet("color: black");

    // Вернуть отображение игрового времени в информаторе, если не запущен менеджер карт
    if(MainWindow *mainWindow = qobject_cast<MainWindow*>(parentWidget()->window()))
        if(!mainWindow->getMapsManagerPtr())
            startTime();
}

// Запустить отображение игрового времени в информаторе
void QLabelClearTextTimer::startTime()
{
    // Получить текущее системное время
    currTime_ = QTime::currentTime();

    // Перевести текущее системное время в количество игровых секунд
    int totalGameSecond = (currTime_.minute() * 60 + currTime_.second()) * TIME_DIFFERENCE;

    // Создать объект игрового времени
    QTime gameTime(0,0,0);
    gameTime = gameTime.addSecs(totalGameSecond);

    // Отобразить игровое время
    QString timeText = tr("Game Time: ");
    setText(timeText + gameTime.toString("hh:mm"));

    // Запуск таймера
    timer_->start(GAME_TIME_UPDATE_RATE);
}

// Остановить отображение игрового времени в информаторе
void QLabelClearTextTimer::stopTime()
{
    // Останавливаем таймер
    if(timer_->isActive())
        timer_->stop();

    clear();
}
