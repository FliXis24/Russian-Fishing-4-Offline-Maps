#include "QLabelClearTextTimer.h"

// Константа таймера исчезновения текстового уведомления в информаторе
int AUTO_CLEAR_TEXT_TIME = 5000;

QLabelClearTextTimer::QLabelClearTextTimer(QWidget *parent) :
    QLabel(parent),
    timer_(new QTimer(this))
{
    setStyleSheet("color: green");

    // Соединение сигнала окончания таймера с очисткой текста
    connect(timer_, &QTimer::timeout, this, &QLabelClearTextTimer::clear);
}

// Перегруженный метод для установки текстовых уведомлений с таймером для их очистки
void QLabelClearTextTimer::setText(const QString &text)
{
    QLabel::setText(text);

    // Запуск таймера
    timer_->start(AUTO_CLEAR_TEXT_TIME);
}
