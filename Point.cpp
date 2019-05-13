#include "Point.h"
#include "ui_Point.h"
#include <QPainter>
#include <QMouseEvent>
#include <WaterBody.h>
#include <QMessageBox>
#include "CreatePoint.h"
#include "MainWindow.h"

Point::Point(
        int highPriority,
        QPair<double, double> mousePercents,
        QPointF gamePoint,
        QWidget *parent,
        QPointF mousePoint
    ) :
    QWidget(parent),
    ui(new Ui::Point),
    highPriority_(highPriority),
    mousePercents_(mousePercents),
    gamePoint_(gamePoint),
    mousePoint_(mousePoint)
{
    ui->setupUi(this);

    // Присваивание точке уникального идентификатора
    id_ = setIndexNumber();

    // Отображение координат точки
    int pointX = static_cast<int>(gamePoint_.x());
    int pointY = static_cast<int>(gamePoint_.y());
    QString textGamePoint(QString::number(pointX) + ":" + QString::number(pointY));
    ui->textGamePoint->setText(textGamePoint);

    // Установить настройки цвета и размера шрифта, и размера изображения точки
    setPointAttributes();

    // Выставляем минимально возможный размер для виджета
    resize(sizeHint());

    // Высчитать координаты мыши для точки
    mousePoint_ = WaterBody::getInstance().computeMousePoint(mousePercents_);

    // При наведении на виджет тип курсора изменяется
    setCursor(Qt::PointingHandCursor);

    // Соединяет сигнал клика по виджету точки со слотом редактирования этой точки
    connect(this, &Point::signalEditPoint, this, &Point::slotEditPoint);

    // Соединяет сигнал текстовых уведомлений со своим слотом
    if(MainWindow *mainWindow = qobject_cast<MainWindow*>(window()))
        connect(this, &Point::signalSendTextInformator, mainWindow, &MainWindow::slotSetTextInformator);

    // Соединяет сигнал звуковых уведомлений со своим слотом
    if(MainWindow *mainWindow = qobject_cast<MainWindow*>(window()))
        connect(this, &Point::signalPlaySound, mainWindow, &MainWindow::slotPlaySound);
}

Point::~Point()
{
    delete ui;
}

// Переопределение эвента, который вызывается после отображения окна
void Point::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    WaterBody::getInstance().setNewGeometry(this);
}

// Переопределение эвента, который вызывается при движении мышью через область виджета точки
void Point::mouseMoveEvent(QMouseEvent *event)
{
    qobject_cast<QLabelEventGetPoint*>(parentWidget())->mouseMovePoint(event->pos());

    QWidget::mouseMoveEvent(event);
}

// Переопределение эвента, который вызывается после каждого нажатия мыши в пределах
// виджета точки
void Point::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
        emit signalEditPoint(this);

    if(event->button() == Qt::RightButton)
        slotDeletePoint();
}

// Слот удаления текущей точки
void Point::slotDeletePoint()
{
    const QString strDeletePoint(
        QString::number(getGamePoint().x()) +
        ":" +
        QString::number(getGamePoint().y())
    );

    QMessageBox::StandardButton confirmation = QMessageBox::question(
        this, tr("Delete Point"),
        tr("Are you sure you want to delete the ") + strDeletePoint + tr(" point?"),
        QMessageBox::Yes | QMessageBox::No
    );

    if(confirmation == QMessageBox::No)
        return;

    WaterBody::getInstance().deletePointInStorage(getID());
    WaterBody::getInstance().writePointsInFile();

    emit signalSendTextInformator(tr("Point ") + strDeletePoint + tr(" removed"));
    emit signalPlaySound("negative");

    close();
}

// Установить координаты мыши для точки
void Point::setNewMousePoint(QPointF& mousePoint)
{
    mousePoint_ = mousePoint;
}

// Слот редактирования текущей точки
void Point::slotEditPoint(Point* currentPoint)
{
    CreatePoint* createPoint = new CreatePoint(currentPoint, this);
    createPoint->show();
}

// Установить уникальный идентификатор для точки
int Point::setIndexNumber()
{
    static int number = 0;

    return number++;
}

// Возвращает процентное соотношение координат мыши относительно области карты для текущей
// точки
const QPair<double, double> &Point::getMousePercents() const
{
    return mousePercents_;
}

// Возвращает координаты мыши текущей точки
const QPointF &Point::getMousePoint() const
{
    return mousePoint_;
}

// Возвращает игровые координаты мыши точки
const QPointF &Point::getGamePoint() const
{
    return gamePoint_;
}

// Вернуть уникальный идентификатор точки
const int &Point::getID() const
{
    return id_;
}

// Возвращает указатель на виджет изображения точки у точки
QLabel *Point::getLabelImagePoint()
{
    return ui->imagePoint;
}

// Возвращает указатель на виджет с текстовыми координатами точки у точки
QLabel *Point::getLabelTextGamePoint()
{
    return ui->textGamePoint;
}

// Вернуть приоритет
int &Point::getPriority()
{
    return highPriority_;
}

// Установка настроек цвета и размера шрифта, и размера изображения точки
void Point::setPointAttributes()
{
    QString pointFontColor;
    int pointFontSize;
    int pointImageSize;
    std::tie(pointFontColor, pointFontSize, pointImageSize) = WaterBody::getInstance().getPointAttributes();

    // Цвет шрифта точки
    ui->textGamePoint->setStyleSheet("color: " + pointFontColor);

    // Размер шрифта точки
    QFont font;
    font.setPointSize(pointFontSize);
    ui->textGamePoint->setFont(font);

    // Размер изображения точки и тип приоритета
    QString priority;
    if(highPriority_)
        priority = "priority-";
    ui->imagePoint->setPixmap(QPixmap("://image/point-" + priority + QString::number(pointImageSize)));
}
