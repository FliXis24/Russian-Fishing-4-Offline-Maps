#include "MapSetup.h"
#include "ui_MapSetup.h"
#include "WaterBody.h"
#include <QStyle>
#include <QDesktopWidget>
#include <QDir>
#include <QIntValidator>
#include <QMessageBox>

MapSetup::MapSetup(const int& width, const int& height, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MapSetup),
    widthControl_(width), heightControl_(height)
{
    ui->setupUi(this);

    // Окно верхнего уровня и поверх других окон
    setWindowFlags(Qt::Tool);

    // Удалить виджет после события закрытия
    setAttribute(Qt::WA_DeleteOnClose);

    // Запуск виджета по центру экрана
    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            size(),
            qApp->desktop()->availableGeometry()
        )
    );

    // Установить текст руководства
    setTextManual();

    // Валидатор для game point 0-999
    ui->leftGamePointX->setValidator(new QIntValidator(0, 999, this));
    ui->leftGamePointY->setValidator(new QIntValidator(0, 999, this));
    ui->rightGamePointX->setValidator(new QIntValidator(0, 999, this));
    ui->rightGamePointY->setValidator(new QIntValidator(0, 999, this));
    ui->topGamePointX->setValidator(new QIntValidator(0, 999, this));
    ui->topGamePointY->setValidator(new QIntValidator(0, 999, this));

    // Валидатор для mouse point 0-9999
    ui->leftMousePointX->setValidator(new QIntValidator(0, 9999, this));
    ui->leftMousePointY->setValidator(new QIntValidator(0, 9999, this));
    ui->rightMousePointX->setValidator(new QIntValidator(0, 9999, this));
    ui->rightMousePointY->setValidator(new QIntValidator(0, 9999, this));
    ui->topMousePointX->setValidator(new QIntValidator(0, 9999, this));
    ui->topMousePointY->setValidator(new QIntValidator(0, 9999, this));

    // Соединяет сигналы редактирования текста в полях со слотом активации кнопки OK
    connect(ui->leftGamePointX, &QLineEdit::textChanged, this, &MapSetup::slotEnabledButtonOK);
    connect(ui->leftGamePointY, &QLineEdit::textChanged, this, &MapSetup::slotEnabledButtonOK);
    connect(ui->rightGamePointX, &QLineEdit::textChanged, this, &MapSetup::slotEnabledButtonOK);
    connect(ui->rightGamePointY, &QLineEdit::textChanged, this, &MapSetup::slotEnabledButtonOK);
    connect(ui->topGamePointX, &QLineEdit::textChanged, this, &MapSetup::slotEnabledButtonOK);
    connect(ui->topGamePointY, &QLineEdit::textChanged, this, &MapSetup::slotEnabledButtonOK);

    connect(ui->leftMousePointX, &QLineEdit::textChanged, this, &MapSetup::slotEnabledButtonOK);
    connect(ui->leftMousePointY, &QLineEdit::textChanged, this, &MapSetup::slotEnabledButtonOK);
    connect(ui->rightMousePointX, &QLineEdit::textChanged, this, &MapSetup::slotEnabledButtonOK);
    connect(ui->rightMousePointY, &QLineEdit::textChanged, this, &MapSetup::slotEnabledButtonOK);
    connect(ui->topMousePointX, &QLineEdit::textChanged, this, &MapSetup::slotEnabledButtonOK);
    connect(ui->topMousePointY, &QLineEdit::textChanged, this, &MapSetup::slotEnabledButtonOK);
}

MapSetup::~MapSetup()
{
    delete ui;
}

// Создание Пользовательских настроек текущей карты
void MapSetup::on_pushButtonOK_clicked()
{
    // Контрольные координаты
    leftMousePoint_ = QPointF(ui->leftMousePointX->text().toDouble(), ui->leftMousePointY->text().toDouble());
    rightMousePoint_ = QPointF(ui->rightMousePointX->text().toDouble(), ui->rightMousePointY->text().toDouble());
    topMousePoint_ = QPointF(ui->topMousePointX->text().toDouble(), ui->topMousePointY->text().toDouble());

    // Контрольные точки
    leftGamePoint_ = QPointF(ui->leftGamePointX->text().toDouble(), ui->leftGamePointY->text().toDouble());
    rightGamePoint_ = QPointF(ui->rightGamePointX->text().toDouble(), ui->rightGamePointY->text().toDouble());
    topGamePoint_ = QPointF(ui->topGamePointX->text().toDouble(), ui->topGamePointY->text().toDouble());

    // Получение кол-ва точек в отрезках между контрольными точками
    const double countWidthSquarePoints = (rightGamePoint_.x()
            - leftGamePoint_.x());
    const double countHeightSquarePoints = (topGamePoint_.y()
            - rightGamePoint_.y());

    // Получение длины одной точки
    double widthPoint = (rightMousePoint_.x() - leftMousePoint_.x())
            / countWidthSquarePoints;
    double heightPoint = (rightMousePoint_.y() - topMousePoint_.y())
            / countHeightSquarePoints;

    // Получение длины точек вне зон контрольных точек
    const double widthBeforeLeftMousePoint = leftMousePoint_.x() / widthPoint;
    const double widthAfterRightMousePoint = (widthControl_ - rightMousePoint_.x()) / widthPoint;
    const double heightBeforeTopMousePoint = topMousePoint_.y() / heightPoint;
    const double heightAfterRightMousePoint = (heightControl_ - rightMousePoint_.y()) / heightPoint;

    // Начальный диапазон на основе контрольных точек
    QPair<double, double> RangeX {leftGamePoint_.x(), rightGamePoint_.x()};
    QPair<double, double> RangeY {rightGamePoint_.y(), topGamePoint_.y()};

    // Правка диапазона с учетом точек вне зоны контрольных точек
    RangeX.first -= widthBeforeLeftMousePoint;
    RangeX.second += widthAfterRightMousePoint;
    RangeY.first -= heightAfterRightMousePoint;
    RangeY.second += heightBeforeTopMousePoint;

    // Вернуть диапазон координат для Менеджера карт
    emit signalReturnRanges(qMakePair(RangeX, RangeY));

    close();
}

// Установка координат мыши за три клика
void MapSetup::slotSetLineEditPoint(QPointF mousePoint)
{
    // Первый клик мыши
    if(clickCounter_ == 0) {
        ui->leftMousePointX->setText(QString::number(mousePoint.x()));
        ui->leftMousePointY->setText(QString::number(mousePoint.y()));
        ++clickCounter_;
        emit signalSendTextInformator(tr("First click"));
    // Второй клик мыши
    } else if(clickCounter_ == 1) {
        ui->rightMousePointX->setText(QString::number(mousePoint.x()));
        ui->rightMousePointY->setText(QString::number(mousePoint.y()));
        ++clickCounter_;
        emit signalSendTextInformator(tr("Second click"));
    // Третий клик мыши
    } else if(clickCounter_ == 2) {
        ui->topMousePointX->setText(QString::number(mousePoint.x()));
        ui->topMousePointY->setText(QString::number(mousePoint.y()));
        clickCounter_ = 0;
        emit signalSendTextInformator(tr("Third click"));
    }
}

// Включение кнопки OK
void MapSetup::slotEnabledButtonOK(const QString& text)
{
     ui->pushButtonOK->setEnabled(!text.isEmpty() && isValidFields());
}

// Проверка полей на валидность
bool MapSetup::isValidFields()
{
    // Поля игровых точек не могут быть пустыми
    if(ui->leftGamePointX->text().isEmpty())
        return false;
    if(ui->leftGamePointY->text().isEmpty())
        return false;
    if(ui->rightGamePointX->text().isEmpty())
        return false;
    if(ui->rightGamePointY->text().isEmpty())
        return false;
    if(ui->topGamePointX->text().isEmpty())
        return false;
    if(ui->topGamePointY->text().isEmpty())
        return false;

    // Поля координат мыши не могут быть пустыми
    if(ui->leftMousePointX->text().isEmpty())
        return false;
    if(ui->leftMousePointY->text().isEmpty())
        return false;
    if(ui->rightMousePointX->text().isEmpty())
        return false;
    if(ui->rightMousePointY->text().isEmpty())
        return false;
    if(ui->topMousePointX->text().isEmpty())
        return false;
    if(ui->topMousePointY->text().isEmpty())
        return false;

    // Координаты мыши левой и правой точки не могут различаться по высоте
    if(ui->leftMousePointY->text().toInt() != ui->rightMousePointY->text().toInt())
        return false;
    // Координаты мыши правой и верхней точки не могут различаться по ширине
    if(ui->rightMousePointX->text().toInt() != ui->topMousePointX->text().toInt())
        return false;

    // Координаты мыши левой точки не могут быть больше или равны координатам мыши
    // правой точки по ширине
    if(ui->leftMousePointX->text().toInt() >= ui->rightMousePointX->text().toInt())
        return false;
    // Координаты мыши верхней точки не могут быть больше или равны координатам мыши
    // правой точки по высоте
    if(ui->topMousePointY->text().toInt() >= ui->rightMousePointY->text().toInt())
        return false;

    // Игровые координаты левой и правой точки не могут различаться по высоте
    if(ui->leftGamePointY->text().toInt() != ui->rightGamePointY->text().toInt())
        return false;
    // Игровые координаты правой и верхней точки не могут различаться по ширине
    if(ui->rightGamePointX->text().toInt() != ui->topGamePointX->text().toInt())
        return false;

    // Игровые координаты левой точки не могут быть больше или равны игровым координатам
    // правой точки по ширине
    if(ui->leftGamePointX->text().toInt() >= ui->rightGamePointX->text().toInt())
        return false;
    // Игровые координаты правой точки не могут быть больше или равны игровым координатам
    // правой точки по высоте
    if(ui->rightGamePointY->text().toInt() >= ui->topGamePointY->text().toInt())
        return false;

    return true;
}

// Возвращает текст руководства
void MapSetup::setTextManual()
{
    ui->textBrowserManual->setText(tr(
        "You need to read this manual carefully!\n"

        "Using the image as an example, you need to click as accurately as "
        "possible three times in the right areas of the map.\n"

        "After each click, you must manually fill in the game coordinate "
        "fields for each of the points. First, the width (shown on the left "
        "and right sides of the card), then the height (shown on the top "
        "and bottom sides of the card).\n"

        "If you clicked the wrong way, you can click the remaining points "
        "and start the clicks again or write the values ​​into the mouse "
        "coordinate fields manually. The coordinates of the mouse are "
        "displayed in the lower part of the window when you hover the mouse "
        "over the desired area of ​​the map.\n"

        "After entering the data you need to check whether the fields are "
        "filled in correctly:\n"

        "Fields of game coordinates and mouse coordinates can not be empty.\n"

        "The coordinates of the mouse left and right points can not differ "
        "in height.\n"

        "The coordinates of the mouse right and top points can not differ "
        "in width.\n"

        "The coordinates of the mouse of the left point can not be greater "
        "than or equal to the coordinates of the mouse of the right point "
        "in width.\n"

        "The coordinates of the upper point mouse can not be greater than "
        "or equal to the coordinates of the mouse right point in height.\n"

        "Game coordinates of the left and right points can not differ in "
        "height.\n"

        "Game coordinates of the right and top points can not differ in "
        "width.\n"

        "Game coordinates of the left point can not be greater than or "
        "equal to the game coordinates of the right point in width.\n"

        "The game coordinates of the right point cannot be greater than or "
        "equal to the game coordinates of the right point in height.\n"

        "The confirmation button is activated only when all fields are "
        "correctly filled."
        )
    );
}
