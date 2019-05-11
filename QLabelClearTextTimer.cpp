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
    timerInfo_(new QTimer(this)),
    timerTime_(new QTimer(this))
{
    // Соединение сигнала окончания таймера информации со слотом, где запускается таймер
    // игрового времени
    connect(timerInfo_, &QTimer::timeout, this, &QLabelClearTextTimer::clearTextInformation);

    // Соединение сигнала окончания таймера игрового времени со слотом обновления игрового
    // времени
    connect(timerTime_, &QTimer::timeout, this, &QLabelClearTextTimer::updateTime);

    // Запустить отображение игрового времени
    startTime();
}

// Метод для установки текстовых уведомлений с таймером для их очистки
void QLabelClearTextTimer::setTextInformation(const QString &text)
{
    // Останавливаем таймер игрового времени
    if(timerTime_->isActive())
        timerTime_->stop();

    setStyleSheet("color: green");

    setText(text);

    // Запуск таймера вывода информации
    timerInfo_->start(DISPLAY_INFORMATION_TIME);
}

// Очистка текста и остановка таймера
void QLabelClearTextTimer::clearTextInformation()
{
    // остановка таймера
    timerInfo_->stop();

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
    timerTime_->start(GAME_TIME_UPDATE_RATE);
}

// Обновить игровое время
void QLabelClearTextTimer::updateTime()
{
    startTime();
}

// Остановить таймеры
void QLabelClearTextTimer::stopTimers()
{
    timerInfo_->stop();
    timerTime_->stop();

    clear();
}
