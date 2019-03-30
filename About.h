#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>

// Константа названия приложения
extern const QString PROGRAM_NAME;

// Константа версии приложения
extern const QString VERSION;

// Константа гиперссылки на исходный код приложения
extern const QString SOURCE_LINK;

// Константа авторства приложения
extern const QString AUTHOR;

// Константа времени отображения виджета
extern const int AUTO_CLOSE_TIME;

namespace Ui {
class About;
}

class About : public QDialog
{
    Q_OBJECT

public:
    explicit About(QWidget *parent = nullptr);
    ~About();

protected:
    // Переопределение эвента для запуска таймера
    void showEvent(QShowEvent *event);

private:
    Ui::About *ui;

    // Таймер
    QTimer *timer_;
};

#endif // ABOUT_H
