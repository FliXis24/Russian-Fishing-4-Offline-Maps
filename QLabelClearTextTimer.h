#ifndef QLABELCLEARTEXTTIMER_H
#define QLABELCLEARTEXTTIMER_H

#include <QLabel>
#include <QTimer>
#include <QTime>

// Константа таймера исчезновения текстового уведомления в информаторе
extern int AUTO_CLEAR_TEXT_TIME;

class QLabelClearTextTimer : public QLabel
{
    Q_OBJECT
public:
    explicit QLabelClearTextTimer(QWidget *parent = nullptr);

    // Перегруженный метод для установки текстовых уведомлений с таймером для их очистки
    void setText(const QString &text);

private:
    // Таймер
    QTimer *timer_;
};

#endif // QLABELCLEARTEXTTIMER_H
