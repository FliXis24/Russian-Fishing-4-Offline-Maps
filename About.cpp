#include "About.h"
#include "ui_About.h"
#include <QStyle>
#include <QDesktopWidget>
#include <QTimer>
#include <QTime>

// Константа названия приложения
const QString PROGRAM_NAME("Russian Fishing 4 Offline Maps");

// Константа версии приложения
const QString VERSION("v.Alpha");

// Константа гиперссылки на исходный код приложения
const QString SOURCE_LINK("<a href=\"https://github.com/FliXis24/Russian-Fishing-4-Offline-Maps/\">github.com</a>");

// Константа авторства приложения
const QString AUTHOR("FliXis");

// Константа времени отображения виджета
const int AUTO_CLOSE_TIME = 10000;

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About),
    timer_(new QTimer(this))
{
    ui->setupUi(this);

    // Окно верхнего уровня и поверх других окон
    setWindowFlags(Qt::Tool);

    // Удалить виджет после события закрытия
    setAttribute(Qt::WA_DeleteOnClose);

    ui->textProgramName->setText(PROGRAM_NAME);

    ui->textAuthor->setText(AUTHOR);

    ui->textVersion->setText(VERSION);

    ui->textSourceLink->setText(SOURCE_LINK);

    // Чтобы сделать линк на source
    ui->textSourceLink->setTextFormat(Qt::RichText);
    ui->textSourceLink->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->textSourceLink->setOpenExternalLinks(true);

    // Выставляем минимально возможный размер для виджета
    resize(sizeHint());

    // Запуск виджета по центру экрана
    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            size(),
            qApp->desktop()->availableGeometry()
        )
    );

    // Соединение сигнала окончания таймера с закрытием виджета
    connect(timer_, &QTimer::timeout, this, &About::close);
}

About::~About()
{
    delete ui;
}

// Переопределение эвента для запуска таймера
void About::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    // Не разрешаем менять размер окна
    setMinimumSize(width(), height());
    setMaximumSize(width(), height());

    // Запуск таймера
    timer_->start(AUTO_CLOSE_TIME);
}
