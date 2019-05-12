#include "CreatePoint.h"
#include "ui_CreatePoint.h"
#include "WaterBody.h"
#include <QDesktopWidget>
#include <QMessageBox>
#include "MainWindow.h"

CreatePoint::CreatePoint(const QPointF& gamePoint, const QPointF &mousePoint, const QPair<double, double> mousePercents, QWidget *parent) :
    QWidget(parent),
    gameCurrentPoint_(gamePoint), mouseCurrentPoint_(mousePoint),
    mouseCurrentPercents_(mousePercents),
    ui(new Ui::CreatePoint)
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

    // Установить заголовок для виджета
    QString nameGamePoint = QString::number(gamePoint.x()) + ":" + QString::number(gamePoint.y());
    setWindowTitle(WaterBody::getInstance().getMapName() + " " + nameGamePoint);

    // Валидатор для поля дистанции point 0-999
    ui->textEditDistance->setValidator(new QIntValidator(0, 999, this));
    // Валидатор для поля глубины 0-9999
    ui->textEditDepth->setValidator(new QIntValidator(0, 9999, this));

    ui->pushButtonConfirm->setAutoDefault(true);
    ui->pushButtonConfirm->setFocus();

    // Соединяет сигнал от чекбокса Тип снасти со слотом переключения активности поля Глубина
    connect(ui->comboBoxTackle, qOverload<int>(&QComboBox::currentIndexChanged), this, &CreatePoint::slotDepthEnabledSwitch);

    // Соединяет сигнал от кнопки подтверждения со своим слотом
    connect(ui->pushButtonConfirm, &QPushButton::clicked, this, &CreatePoint::slotConfirmCreate);

    // Соединяет сигнал текстовых уведомлений со своим слотом
    if(MainWindow* mainWindow = qobject_cast<MainWindow*>(parent->window()))
        connect(this, &CreatePoint::signalSendTextInformator, mainWindow, &MainWindow::slotSetTextInformator);

    // Соединяет сигнал звуковых уведомлений со своим слотом
    if(MainWindow* mainWindow = qobject_cast<MainWindow*>(parent->window()))
        connect(this, &CreatePoint::signalPlaySound, mainWindow, &MainWindow::slotPlaySound);
}

CreatePoint::CreatePoint(Point* point, QWidget *parent) :
    QWidget(parent),
    gameCurrentPoint_(point->getGamePoint()), mouseCurrentPoint_(point->getMousePoint()),
    mouseCurrentPercents_(point->getMousePercents()),
    ui(new Ui::CreatePoint)
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

    // Установить заголовок для виджета
    QString nameGamePoint = QString::number(gameCurrentPoint_.x()) + ":" + QString::number(gameCurrentPoint_.y());
    setWindowTitle(WaterBody::getInstance().getMapName() + " " + nameGamePoint);

    // Валидатор для поля дистанции point 0-999
    ui->textEditDistance->setValidator(new QIntValidator(0, 999, this));
    // Валидатор для поля глубины 0-9999
    ui->textEditDepth->setValidator(new QIntValidator(0, 9999, this));

    ui->pushButtonConfirm->setText(tr("Edit"));
    ui->pushButtonConfirm->setEnabled(false);

    ui->pushButtonCancel->setAutoDefault(true);
    ui->pushButtonCancel->setFocus();

    ui->pushButtonDeletePoint->setEnabled(true);

    ui->comboBoxTackle->setCurrentIndex(point->typeTackle);
    ui->textEditRigs->setText(point->rigs);
    ui->comboBoxFrom->setCurrentIndex(point->from);
    ui->textEditFish->setText(point->fish);
    ui->textEditBaits->setText(point->baits);
    ui->textEditNote->setText(point->note);
    ui->textEditFlyCasting->setText(point->flyCasting);
    ui->textEditDistance->setText(point->distance);
    ui->textEditDepth->setText(point->depth);
    ui->textEditUnwantedFish->setText(point->unwantedFish);

    slotDepthEnabledSwitch(point->typeTackle);

    // Соединяет сигнал от чекбокса Тип снасти со слотом переключения активности поля Глубина
    connect(ui->comboBoxTackle, qOverload<int>(&QComboBox::currentIndexChanged), this, &CreatePoint::slotDepthEnabledSwitch);

    // Сигнал от кнопки удаления точки
    connect(ui->pushButtonDeletePoint, &QPushButton::clicked, this, [=](){ slotDeletePoint(point); });

    // Сигнал от кнопки подтверждения
    connect(ui->pushButtonConfirm, &QPushButton::clicked, this, [=](){ confirmEdit(point); });

    // Если поля изменились, то отправляется сигнал включающий кнопку подтверждения
    connect(ui->comboBoxTackle, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](){ ui->pushButtonConfirm->setEnabled(true); });
    connect(ui->comboBoxFrom, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](){ ui->pushButtonConfirm->setEnabled(true); });
    connect(ui->textEditRigs, &QLineEdit::textChanged, this, [this](){ ui->pushButtonConfirm->setEnabled(true); });
    connect(ui->textEditFish, &QLineEdit::textChanged, this, [this](){ ui->pushButtonConfirm->setEnabled(true); });
    connect(ui->textEditBaits, &QLineEdit::textChanged, this, [this](){ ui->pushButtonConfirm->setEnabled(true); });
    connect(ui->textEditNote, &QLineEdit::textChanged, this, [this](){ ui->pushButtonConfirm->setEnabled(true); });
    connect(ui->textEditFlyCasting, &QLineEdit::textChanged, this, [this](){ ui->pushButtonConfirm->setEnabled(true); });
    connect(ui->textEditDistance, &QLineEdit::textChanged, this, [this](){ ui->pushButtonConfirm->setEnabled(true); });
    connect(ui->textEditDepth, &QLineEdit::textChanged, this, [this](){ ui->pushButtonConfirm->setEnabled(true); });
    connect(ui->textEditUnwantedFish, &QLineEdit::textChanged, this, [this](){ ui->pushButtonConfirm->setEnabled(true); });

    // Соединяет сигнал текстовых уведомлений со своим слотом
    if(MainWindow *mainWindow = qobject_cast<MainWindow*>(parent->window()))
        connect(this, &CreatePoint::signalSendTextInformator, mainWindow, &MainWindow::slotSetTextInformator);

    // Соединяет сигнал звуковых уведомлений со своим слотом
    if(MainWindow *mainWindow = qobject_cast<MainWindow*>(parent->window()))
        connect(this, &CreatePoint::signalPlaySound, mainWindow, &MainWindow::slotPlaySound);
}

CreatePoint::~CreatePoint()
{
    delete ui;
}

// Слот удаления текущей точки и запись в файл
void CreatePoint::slotDeletePoint(Point* point)
{
    const QString strDeletePoint(
        QString::number(point->getGamePoint().x()) +
        ":" +
        QString::number(point->getGamePoint().y())
    );

    QMessageBox::StandardButton confirmation = QMessageBox::question(
        this, tr("Delete Point"),
        tr("Are you sure you want to delete the ") + strDeletePoint + tr(" point?"),
        QMessageBox::Yes | QMessageBox::No
    );

    if(confirmation == QMessageBox::No)
        return;

    // Удаление текущей точки из коллекции
    WaterBody::getInstance().deletePointInStorage(point->getID());

    // Запись в файл
    WaterBody::getInstance().writePointsInFile();

    emit signalSendTextInformator(tr("Point ") + strDeletePoint + tr(" removed"));
    emit signalPlaySound("negative");

    close();
}

// Слот создания новой точки
void CreatePoint::slotConfirmCreate()
{
    if(!WaterBody::getInstance().getLabelImageMapPtr())
        return;

    Point *point = new Point(
        mouseCurrentPercents_,
        gameCurrentPoint_,
        WaterBody::getInstance().getLabelImageMapPtr(),
        mouseCurrentPoint_
    );
    point->typeTackle = ui->comboBoxTackle->currentIndex();
    point->rigs = ui->textEditRigs->text();
    point->from = ui->comboBoxFrom->currentIndex();
    point->fish = ui->textEditFish->text();
    point->baits = ui->textEditBaits->text();
    point->note = ui->textEditNote->text();
    point->flyCasting = ui->textEditFlyCasting->text();
    point->distance = ui->textEditDistance->text();
    point->depth = ui->textEditDepth->text();
    point->unwantedFish = ui->textEditUnwantedFish->text();

    // Запись новой точки в коллекцию
    WaterBody::getInstance().writeNewPointInStorage(point);

    // Запись в файл
    WaterBody::getInstance().writePointsInFile();

    point->show();

    const QString strGamePoint(
        QString::number(gameCurrentPoint_.x()) +
        ":" +
        QString::number(gameCurrentPoint_.y())
    );
    emit signalSendTextInformator(tr("New point ") + strGamePoint + tr(" created"));
    emit signalPlaySound("positive");

    close();
}

// Подтверждение сохранения данных после редактирования текущей точки
void CreatePoint::confirmEdit(Point* currentPoint)
{
    if(!WaterBody::getInstance().getLabelImageMapPtr())
        return;

    Point *point = new Point(
        mouseCurrentPercents_,
        gameCurrentPoint_,
        WaterBody::getInstance().getLabelImageMapPtr()
    );
    point->typeTackle = ui->comboBoxTackle->currentIndex();
    point->rigs = ui->textEditRigs->text();
    point->from = ui->comboBoxFrom->currentIndex();
    point->fish = ui->textEditFish->text();
    point->baits = ui->textEditBaits->text();
    point->note = ui->textEditNote->text();
    point->flyCasting = ui->textEditFlyCasting->text();
    point->distance = ui->textEditDistance->text();
    point->depth = ui->textEditDepth->text();
    point->unwantedFish = ui->textEditUnwantedFish->text();

    // Запись новой точки в коллекцию
    WaterBody::getInstance().writeNewPointInStorage(point);

    // Удаление старой версии точки из коллекции
    WaterBody::getInstance().deletePointInStorage(currentPoint->getID());

    // Запись в файл
    WaterBody::getInstance().writePointsInFile();

    point->show();

    const QString strGamePoint(
        QString::number(gameCurrentPoint_.x()) +
        ":" +
        QString::number(gameCurrentPoint_.y())
    );
    emit signalSendTextInformator(tr("Point ") + strGamePoint + tr(" edited"));
    emit signalPlaySound("positive");

    close();
}

// Слот переключение активности поля Глубина взависимости от индекса Тип Ловли
void CreatePoint::slotDepthEnabledSwitch(int index)
{
    if(index <= 1)
        ui->textEditDepth->setEnabled(true);
    else {
        ui->textEditDepth->clear();
        ui->textEditDepth->setEnabled(false);
    }
}
