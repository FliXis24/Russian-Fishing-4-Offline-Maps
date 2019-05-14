#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "CreatePoint.h"
#include "WaterBody.h"
#include "License.h"
#include <QSettings>
#include <cmath>
#include <QMessageBox>

// Константы для выставления минимального размера окна
const int MINIMUM_WIDTH = 640;
const int MINIMUM_HEIGHT = 360;

// Константы цветов текста точек
const QString WHITE("White");
const QString BLACK("Black");
const QString RED("Red");
const QString BLUE("Blue");
const QString CYAN("Cyan");
const QString MAGENTA("Magenta");
const QString YELLOW("Yellow");
const QString LIME("Lime");

// Константы размера шрифта текста точек
const int FONT_SIZE_8(8);
const int FONT_SIZE_9(9);
const int FONT_SIZE_10(10);
const int FONT_SIZE_11(11);
const int FONT_SIZE_12(12);

// Константы размера изображения точек
const int IMAGE_SIZE_16(16);
const int IMAGE_SIZE_20(20);
const int IMAGE_SIZE_24(24);
const int IMAGE_SIZE_32(32);

// Константа уровня звука
const double VOLUME(0.50);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    foundPointPtr_(nullptr),
    mapsManagerPtr_(nullptr),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Установить название главного окна с версией приложения
    setWindowTitle(PROGRAM_NAME + " " + VERSION);

    // Минимальный размер
    setMinimumSize(MINIMUM_WIDTH, MINIMUM_HEIGHT);

    // Валидатор для полей отображения координат. Необязательный минус, затем 0-999
    ui->textEditPointX->setValidator(new QRegExpValidator(QRegExp("(-\\d{2})|(\\d{3})"), this));
    ui->textEditPointY->setValidator(new QRegExpValidator(QRegExp("(-\\d{2})|(\\d{3})"), this));

    // Первоначально выключено
    ui->pushButtonFindGamePoint->setEnabled(false);

    // Соединяет сигнал пункта меню со слотом запуска менеджера карт
    connect(ui->actionMapsManager, &QAction::triggered, this, &MainWindow::slotMapsManager);

    // Соединяет сигнал с игровыми координатами со слотом поля, где они отображаются
    connect(ui->labelImageMap, &QLabelEventGetPoint::signalSendPoint, this, &MainWindow::slotViewPoint);

    // Соединяет сигнал с координатами мыши со слотом создания точки
    connect(ui->labelImageMap, &QLabelEventGetPoint::signalSendMousePoint, this, &MainWindow::slotCreateGamePoint);

    // Соединяет сигнал кнопки поиска точек со своим слотом
    connect(ui->pushButtonFindGamePoint, &QPushButton::clicked, this, &MainWindow::slotFindGamePoint);

    // Соединяет сигналы полей координат со слотами переключения активности кнопки поиска
    connect(ui->textEditPointX, &QLineEdit::textChanged, this, &MainWindow::slotFindGamePointEnabledSwitch);
    connect(ui->textEditPointY, &QLineEdit::textChanged, this, &MainWindow::slotFindGamePointEnabledSwitch);

    // Соединение сигнала переключения режима отображения координат со своим слотом
    connect(this, &MainWindow::signalSwitchMode, ui->labelImageMap, &QLabelEventGetPoint::slotSwitchMode);

    // Соединяет сигнал пункта меню со слотом цвета шрифта точек
    connect(ui->menuPointsFontColor, &QMenu::triggered, this, &MainWindow::slotMenuPointsFontColor);

    // Соединяет сигнал пункта меню со слотом размера шрифта точек
    connect(ui->menuPointsFontSize, &QMenu::triggered, this, &MainWindow::slotMenuPointsFontSize);

    // Соединяет сигнал пункта меню со слотом размера изображения точек
    connect(ui->menuPointsImageSize, &QMenu::triggered, this, &MainWindow::slotMenuPointsImageSize);

    // Если в меню водоемов водоемы отсутствуют, то вызывается функция предупреждения
    connect(ui->menuWaterBodies, &QMenu::aboutToShow, [this](){ if(ui->menuWaterBodies->isEmpty()) menuEmptyMessage(); });

    // Соединение сигнала от меню выбора водоема со слотом установки водоема
    connect(ui->menuWaterBodies, &QMenu::triggered, this, &MainWindow::slotConstructWaterBodies);

    // Соединение сигнала чекбокса звука с функцией записи настроек звука в файл
    connect(ui->actionSounds, &QAction::triggered, [this](const bool &sound){ writeSoundSettings(sound); });

    // Соединение сигнала пункта меню со своим слотом
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::slotAbout);

    // К виджету с изображением карты будут частые обращения из разных частей программы,
    // потому записываем указатель в синглтон
    WaterBody::getInstance().setLabelImageMapPtr(ui->labelImageMap);

    // Выставляем громкость
    effect_.setVolume(VOLUME);

    // Подсказки по сторонам света
    ui->labelTextCompass->setText(tr(
        "top - <font color=red>N</font><b>orth</b>, "
        "bottom - <font color=red>S</font><b>outh</b>, "
        "left - <font color=red>W</font><b>est</b>, "
        "right - <font color=red>E</font><b>ast</b>"
    ));

    // Считываем все настройки из файла
    readAllSettings();
}

MainWindow::~MainWindow()
{
    // Записываем все настройки в файл .ini перед закрытием программы
    writeAllSettings();

    delete ui;

    // В некоторых случаях, к форме после ее удаления еще могут быть обращения
    ui = nullptr;
}

void MainWindow::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    slotSetTextInformator(tr("Catch fish big and small !!!"));
}


// Переопределение эвента, который вызывается после каждого изменения размера виджета
void MainWindow::resizeEvent(QResizeEvent *event)
{
    if(!WaterBody::getInstance().getLabelImageMapPtr())
        return;

    adjustWidgetSize();

    // Если размеры окна изменились, то необходимо пересчитать геометрию виджета найденной
    // точки
    if(foundPointPtr_)
        setNewGeometryFoundPoint();

    QMainWindow::resizeEvent(event);
}

// Устанавливает фиксированный размер ширины QLable в котором находится QPixmap
// с изображением текущей карты
void MainWindow::adjustWidgetSize()
{
    int widthCurrentPixMap = 1;
    int heightCurrentPixMap = 1;

    const QPixmap* pixPtr = this->ui->labelImageMap->pixmap();
    if(pixPtr && !pixPtr->isNull()) {
        widthCurrentPixMap = pixPtr->width();
        heightCurrentPixMap = pixPtr->height();
    }
    const QSize pixmapSize(widthCurrentPixMap, heightCurrentPixMap);

    ui->labelImageMap->setFixedWidth(
        ui->centralWidget->height() * pixmapSize.width() / pixmapSize.height()
    );
}

// Устанавливает новую геометрию виджету поиска точки после изменения размера окна
void MainWindow::setNewGeometryFoundPoint()
{
    QPointF mouseCentrPoint(findCenterPoint(foundPointPtr_->getGamePoint()));

    foundPointPtr_->setGeometry(
        qRound(mouseCentrPoint.x()) - HALF_CIRCLE_SIZE,
        qRound(mouseCentrPoint.y()) - HALF_CIRCLE_SIZE,
        WIDGET_SIZE, WIDGET_SIZE
    );
}

// Слот установки цвета текста точек
void MainWindow::slotMenuPointsFontColor(QAction *action)
{
    WaterBody::getInstance().setPointsFontColor(action->data().toString());

    slotSetTextInformator(tr("Points font color change on ") + action->data().toString());
}

// Слот установки размера текста точек
void MainWindow::slotMenuPointsFontSize(QAction *action)
{
    WaterBody::getInstance().setPointsFontSize(action->data().toInt());

    slotSetTextInformator(tr("Points font size change on ") + action->data().toString());
}

// Слот установки размера изображения точки
void MainWindow::slotMenuPointsImageSize(QAction *action)
{
    WaterBody::getInstance().setPointsImageSize(action->data().toInt());

    slotSetTextInformator(tr("Points image size change on ") + action->data().toString());
}

// Установка водоема
void MainWindow::slotConstructWaterBodies(QAction *action)
{
    // Если запущен виджет отображения найденной точки, то его необходимо закрыть
    if(foundPointPtr_)
        foundPointPtr_->close();

    // Очищаем текущие точки
    WaterBody::getInstance().clearPoint();

    // Установка нужного водоема
    WaterBody::getInstance().setWaterBody(static_cast<std::vector<Map*>::size_type>(action->data().toInt()));

    // Установка изображения карты
    ui->labelImageMap->setPixmap(QPixmap(WaterBody::getInstance().getMapFilePath()));

    writeMapSettings(WaterBody::getInstance().getFileMapName());

    slotSetTextInformator(WaterBody::getInstance().getMapName() + tr(" map launched"));
    slotPlaySound("switch");
}

// Слот отображения точек в полях отображения координат
void MainWindow::slotViewPoint(const QPointF point)
{
    int pointX = qRound(point.x());
    int pointY = qRound(point.y());

    ui->textEditPointX->setText(QString::number(pointX));
    ui->textEditPointY->setText(QString::number(pointY));
}

// Проверяет допустимость введенных или отображаемых координат
bool MainWindow::isValidGamePoints()
{
    if(ui->textEditPointX->text() == "" || ui->textEditPointY->text() == "")
        return false;
    if(ui->textEditPointX->text() == "-" || ui->textEditPointY->text() == "-")
        return false;

    QPair<QPair<double, double>, QPair<double, double>> mapRanges = WaterBody::getInstance().getMapRanges();
    QPair<double, double> rangeX = mapRanges.first;
    QPair<double, double> rangeY = mapRanges.second;

    if(ui->textEditPointX->text().toDouble() < rangeX.first || ui->textEditPointX->text().toDouble() > rangeX.second)
        return false;
    if(ui->textEditPointY->text().toDouble() < rangeY.first || ui->textEditPointY->text().toDouble() > rangeY.second)
        return false;

    return true;
}

// Слот переключения активности кнопки поиска координат
void MainWindow::slotFindGamePointEnabledSwitch()
{
    if(isValidGamePoints() && !mapsManagerPtr_)
        ui->pushButtonFindGamePoint->setEnabled(true);
    else
        ui->pushButtonFindGamePoint->setEnabled(false);
}

// Слот поиска точки
void MainWindow::slotFindGamePoint()
{
    // Если виджет отображения найденной точки уже запущен, то его необходимо закрыть
    if(foundPointPtr_)
        foundPointPtr_->close();

    // Искомые координаты
    const QPointF findPoint(
        ui->textEditPointX->text().toDouble(),
        ui->textEditPointY->text().toDouble()
    );

    // Поиск координат точки
    QPointF mouseCentrPoint(findCenterPoint(findPoint));

    // Создание и отображение виджета поиска точки
    foundPointPtr_ = new FoundPoint(mouseCentrPoint, findPoint, ui->labelImageMap);
    foundPointPtr_->show();

    const QString strGamePoint(
        QString::number(findPoint.x()) +
        ":" +
        QString::number(findPoint.y())
    );
    slotSetTextInformator(tr("Point ") + strGamePoint + tr(" found"));
    slotPlaySound("positive");
}

// Слот создания точки
void MainWindow::slotCreateGamePoint(const QPointF currentMousePoint)
{
    // Проверить не запущен ли виджет менеджера карт
    if(mapsManagerPtr_)
        return;

    // Если запущен виджет отображения найденной точки, то его необходимо закрыть
    if(foundPointPtr_)
        foundPointPtr_->close();

    // Берем координаты из полей
    const QPointF gamePoint(
        ui->textEditPointX->text().toDouble(),
        ui->textEditPointY->text().toDouble()
    );

    QPointF mousePoint(currentMousePoint.x(), currentMousePoint.y());
    if(mousePoint.x() < 0 || mousePoint.y() < 0) {
        // Если координаты мыши невалидны, то высчитываем их из игровых координат
        QPointF mouseCentrPoint(findCenterPoint(gamePoint));
        mousePoint.rx() = mouseCentrPoint.x();
        mousePoint.ry() = mouseCentrPoint.y();
    }
    // Высчитываем проценты координат мыши относительно области левого фрейма окна
    QPair<double, double> mousePercents(
        mousePoint.x() * 100 / ui->labelImageMap->width(),
        mousePoint.y() * 100 / ui->labelImageMap->height()
    );

    // Создаем точку и отображаем ее
    CreatePoint* createPoint = new CreatePoint(gamePoint, mousePoint, mousePercents, this);
    createPoint->show();
}

// Вернуть указатель на менеджер карт
QPointer<MapsManager> MainWindow::getMapsManagerPtr()
{
    return mapsManagerPtr_;
}

// Возвращает координаты мыши для центра игровой точки
QPointF MainWindow::findCenterPoint(const QPointF findPoint)
{
    // Диапазоны координат из синглтона
    QPair<QPair<double, double>, QPair<double, double>> mapRanges = WaterBody::getInstance().getMapRanges();
    const QPair<double, double> rangeX = mapRanges.first;
    const QPair<double, double> rangeY = mapRanges.second;

    // Истинные диапазоны
    const double totalRangeX = rangeX.second - rangeX.first;
    const double totalRangeY = rangeY.second - rangeY.first;

    // Проценты координат относительно начала диапазонов
    const double percentX = 100 * (findPoint.x() - rangeX.first) / totalRangeX;
    const double percentY = 100 * (findPoint.y() - rangeY.first) / totalRangeY;

    // Текущая ширина и высота QLable с QPixmap внутри
    const double width = ui->labelImageMap->width();
    const double height = ui->labelImageMap->height();

    // Координаты мыши
    const int mousePointX = static_cast<int>(percentX * width / 100);
    const int mousePointY = static_cast<int>(height - (percentY * height / 100));

    return QPointF(mousePointX, mousePointY);
}

// Слот запуска окна менеджера карт
void MainWindow::slotMapsManager()
{
    // Проверить не запущен ли виджет менеджера карт
    if(mapsManagerPtr_)
        return;

    // Если запущен виджет отображения найденной точки, то его необходимо закрыть
    if(foundPointPtr_)
        foundPointPtr_->close();

    // Установить максимальный размер виджета для повышения точность и удобства калибровки,
    setWindowState(Qt::WindowMaximized);
    // Зафиксировать размер виджета
    setFixedSize(width(), height());

    // Необходимо сделать неактивной кнопку поиска точек
    ui->pushButtonFindGamePoint->setEnabled(false);

    // Выключаем главное меню
    ui->menuBar->setEnabled(false);

    // Валидатор для полей отображения координат. Необязательный минус, затем 0-999
    // Под координаты мыши
    ui->textEditPointX->setValidator(new QIntValidator(0, 9999, this));
    ui->textEditPointY->setValidator(new QIntValidator(0, 9999, this));

    // Возможность редактирования в этот момент не нужна
    ui->textEditPointX->setReadOnly(true);
    ui->textEditPointY->setReadOnly(true);

    ui->textEditPointX->clear();
    ui->textEditPointY->clear();

    // Остановить все таймеры
    ui->labelInformator->stopTimers();

    // Очищаем точки и текущую карту
    WaterBody::getInstance().clearPoint();
    ui->labelImageMap->setPixmap(QPixmap());

    mapsManagerPtr_ = new MapsManager(this);

    // Переключение режима отображения в пользу координат мыши
    emit signalSwitchMode();

    // Соединяем сигнал передающий текст со слотом текстовых уведомлений
    connect(mapsManagerPtr_, &MapsManager::signalSendTextInformator, this, &MainWindow::slotSetTextInformator);

    // Соединяем сигнал очистки полей координат со своим слотом
    connect(mapsManagerPtr_, &MapsManager::signalClearLineEditPoints, this, &MainWindow::slotClearLineEditPoints);

    // Соединяем сигнал передающий тип звука со слотом звуковых уведомлений
    connect(mapsManagerPtr_, &MapsManager::signalPlaySound, this, &MainWindow::slotPlaySound);

    // Соединяет сигнал закрытия виджета Менеджера карт с лямбдой
    connect(mapsManagerPtr_, &MapsManager::destroyed, this, [this]()
    {
        // Если пользователь нажал на крестик, то обращение к ui будет невалидным, т.к. оно
        // уже не существует в этот момент
        if(!ui)
            return;

        // Вернуть стандартные минимальные и максимальные размеры виджета для отмены фиксации
        setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        setMinimumSize(MINIMUM_WIDTH, MINIMUM_HEIGHT);

        // Возвращаем активность кнопкам поиска и создания точек
        if(ui->labelImageMap->pixmap() && !ui->labelImageMap->pixmap()->isNull())
            ui->pushButtonFindGamePoint->setEnabled(true);

        // Включаем главное меню обратно
        ui->menuBar->setEnabled(true);

        // Валидатор для полей отображения координат. Необязательный минус, затем 0-999
        // Под игровые координаты
        ui->textEditPointX->setValidator(new QRegExpValidator(QRegExp("(-\\d{2})|(\\d{3})"), this));
        ui->textEditPointY->setValidator(new QRegExpValidator(QRegExp("(-\\d{2})|(\\d{3})"), this));

        // Возвращаем возможность редактирования
        ui->textEditPointX->setReadOnly(false);
        ui->textEditPointY->setReadOnly(false);

        ui->textEditPointX->clear();
        ui->textEditPointY->clear();

        // Включить отображение игрового времени
        ui->labelInformator->startTime();

        // Переключение режима отображения в пользу игровых точек
        emit signalSwitchMode();

        // Меню водоемов могло изменится, поэтому пересоздаем его
        reloadActionsMenuWaterBodies();

        // Грузим карту
        readMapSettings();
    });

    mapsManagerPtr_->show();
}

// Считывает настройки дисплея из файла
void MainWindow::readDisplaySettings()
{
    QSettings settings(".//settings.ini", QSettings::IniFormat);

    settings.beginGroup("DISPLAY");
        const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    settings.endGroup();

    restoreGeometry(geometry);
}

// Считывает настройки карт из файла
void MainWindow::readMapSettings()
{
    QSettings settings(".//settings.ini", QSettings::IniFormat);

    settings.beginGroup("MAPS");
        const QString mapName = settings.value("map").toString();
    settings.endGroup();

    if(ui->menuWaterBodies->actions().isEmpty())
        return;

    if(!mapName.isEmpty())
    {
        int mapIndex = WaterBody::getInstance().getMapIndex(mapName);
        if(mapIndex != -1)
            // Грузим карту по индексу
            ui->menuWaterBodies->actions().at(mapIndex)->trigger();
        else
            // Если индекс невалидный, то грузим первую же карту
            ui->menuWaterBodies->actions().at(0)->trigger();
    }
    else
        // Если в настройках пусто, то грузим первую же карту
        ui->menuWaterBodies->actions().at(0)->trigger();
}

void MainWindow::readSoundSettings()
{
    QSettings settings(".//settings.ini", QSettings::IniFormat);

    settings.beginGroup("SOUND");
        const bool sound = settings.value("sound", "true").toBool();
    settings.endGroup();

    if(!sound)
        ui->actionSounds->setChecked(false);
}

// Считывает все настройки из файла
void MainWindow::readAllSettings()
{
    readDisplaySettings();
    reloadActionsMenuWaterBodies();
    WaterBody::getInstance().readPointSettings();
    createActionsMenu();
    readMapSettings();
    readSoundSettings();
}

// Записывает настройки дисплея в файл
void MainWindow::writeDisplaySettings()
{
    QSettings settings(".//settings.ini", QSettings::IniFormat);

    settings.beginGroup("DISPLAY");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();

    settings.sync();
}

// Записывает настройки карт в файл
void MainWindow::writeMapSettings(const QString& mapName)
{
    QSettings settings(".//settings.ini", QSettings::IniFormat);

    settings.beginGroup("MAPS");
    settings.setValue("map", mapName);
    settings.endGroup();

    settings.sync();
}

// Записывает настройки звука в файл
void MainWindow::writeSoundSettings(const bool &sound)
{
    QSettings settings(".//settings.ini", QSettings::IniFormat);

    settings.beginGroup("SOUND");
    settings.setValue("sound", sound);
    settings.endGroup();

    settings.sync();
}

// Записывает все настройки в файл
void MainWindow::writeAllSettings()
{
    writeDisplaySettings();
}

// Пересоздание пунктов меню выбора водоема
void MainWindow::reloadActionsMenuWaterBodies()
{
    // Обязательно сперва очищаем меню
    ui->menuWaterBodies->clear();

    // Читаем настройки карт
    WaterBody::getInstance().readMapsFromFile();

    QStringList mapNames = WaterBody::getInstance().getListMapsName();
    QActionGroup *waterBodiesActionGroup = new QActionGroup(this);
    int mapIndex = 0;
    for(auto it : mapNames)
        waterBodiesActionGroup->addAction(it)->setData(mapIndex++);

    // Обязательно проставляем всем атрибут, иначе меню в linux почему-то не становится
    // групповым
    for(auto it : waterBodiesActionGroup->actions())
        it->setCheckable(true);

    // В групповом меню может быть активно лишь одно значение одновременно
    waterBodiesActionGroup->setExclusive(true);

    // Добавление группы в меню
    ui->menuWaterBodies->addActions(waterBodiesActionGroup->actions());
}

// Слот очистки полей отображения координат
void MainWindow::slotClearLineEditPoints()
{
    ui->textEditPointX->clear();
    ui->textEditPointY->clear();
}

// Вызывает предупреждение, что карты отсутствуют в базе данных карт
void MainWindow::menuEmptyMessage()
{
    emit signalSendTextInformator(tr("Maps not found!"));
    slotPlaySound("warning");

    QMessageBox::warning(this, tr("Maps not found!"),
        tr("You can add new maps to the Maps Manager menu")
    );

    QMessageBox::StandardButton confirmation = QMessageBox::question(
        this, tr("Maps not found!"),
        tr("Go to the Map Manager menu?"),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes
    );

    if(confirmation == QMessageBox::Yes)
        slotMapsManager(); // Запуск менеджера карт
}

// Создание пунктов меню выбора цвета шрифта точек
void MainWindow::createActionsMenuFontColor()
{
    QActionGroup *fontColorActionGroup = new QActionGroup(this);
    fontColorActionGroup->addAction(tr(WHITE.toLatin1()))->setData(WHITE);
    fontColorActionGroup->addAction(tr(BLACK.toLatin1()))->setData(BLACK);
    fontColorActionGroup->addAction(tr(RED.toLatin1()))->setData(RED);
    fontColorActionGroup->addAction(tr(BLUE.toLatin1()))->setData(BLUE);
    fontColorActionGroup->addAction(tr(CYAN.toLatin1()))->setData(CYAN);
    fontColorActionGroup->addAction(tr(MAGENTA.toLatin1()))->setData(MAGENTA);
    fontColorActionGroup->addAction(tr(YELLOW.toLatin1()))->setData(YELLOW);
    fontColorActionGroup->addAction(tr(LIME.toLatin1()))->setData(LIME);

    // Обязательно проставляем всем атрибут, иначе меню в linux почему-то не становится
    // групповым
    for(auto it : fontColorActionGroup->actions())
        it->setCheckable(true);

    fontColorActionGroup->actions().at(0)->setChecked(true);
    // Выставляем активным тот пункт меню, который нам вернет синглтон
    for(auto it : fontColorActionGroup->actions())
        if(it->data() == WaterBody::getInstance().getFontColorAttribute()) {
            it->setChecked(true);
            break;
        }

    // В групповом меню может быть активно лишь одно значение одновременно
    fontColorActionGroup->setExclusive(true);

    // Добавление группы в меню
    ui->menuPointsFontColor->addActions(fontColorActionGroup->actions());
}

// Создание пунктов меню выбора размера шрифта точек
void MainWindow::createActionsMenuFontSize()
{
    QActionGroup *fontSizeActionGroup = new QActionGroup(this);
    fontSizeActionGroup->addAction(QString::number(FONT_SIZE_8))->setData(FONT_SIZE_8);
    fontSizeActionGroup->addAction(QString::number(FONT_SIZE_9))->setData(FONT_SIZE_9);
    fontSizeActionGroup->addAction(QString::number(FONT_SIZE_10))->setData(FONT_SIZE_10);
    fontSizeActionGroup->addAction(QString::number(FONT_SIZE_11))->setData(FONT_SIZE_11);
    fontSizeActionGroup->addAction(QString::number(FONT_SIZE_12))->setData(FONT_SIZE_12);

    // Обязательно проставляем всем атрибут, иначе меню в linux почему-то не становится
    // групповым
    for(auto it : fontSizeActionGroup->actions())
        it->setCheckable(true);

    fontSizeActionGroup->actions().at(0)->setChecked(true);
    // Выставляем активным тот пункт меню, который нам вернет синглтон
    for(auto it : fontSizeActionGroup->actions())
        if(it->data() == WaterBody::getInstance().getFontSizeAttribute()) {
            it->setChecked(true);
            break;
        }

    // В групповом меню может быть активно лишь одно значение одновременно
    fontSizeActionGroup->setExclusive(true);

    // Добавление группы в меню
    ui->menuPointsFontSize->addActions(fontSizeActionGroup->actions());
}

// Создание пунктов меню выбора размера изображения точек
void MainWindow::createActionsMenuImageSize()
{
    QActionGroup *imageSizeActionGroup = new QActionGroup(this);
    imageSizeActionGroup->addAction(QString::number(IMAGE_SIZE_16))->setData(IMAGE_SIZE_16);
    imageSizeActionGroup->addAction(QString::number(IMAGE_SIZE_20))->setData(IMAGE_SIZE_20);
    imageSizeActionGroup->addAction(QString::number(IMAGE_SIZE_24))->setData(IMAGE_SIZE_24);
    imageSizeActionGroup->addAction(QString::number(IMAGE_SIZE_32))->setData(IMAGE_SIZE_32);

    // Обязательно проставляем всем атрибут, иначе меню в linux почему-то не становится
    // групповым
    for(auto it : imageSizeActionGroup->actions())
        it->setCheckable(true);

    imageSizeActionGroup->actions().at(0)->setChecked(true);
    // Выставляем активным тот пункт меню, который нам вернет синглтон
    for(auto it : imageSizeActionGroup->actions())
        if(it->data() == WaterBody::getInstance().getImageSizeAttribute()) {
            it->setChecked(true);
            break;
        }

    // В групповом меню может быть активно лишь одно значение одновременно
    imageSizeActionGroup->setExclusive(true);

    // Добавление группы в меню
    ui->menuPointsImageSize->addActions(imageSizeActionGroup->actions());
}

// Создание пунктов меню выбора цвета и размера шрифта точек, и размера изображения точек
void MainWindow::createActionsMenu()
{
    createActionsMenuFontColor();
    createActionsMenuFontSize();
    createActionsMenuImageSize();
}

// Слот текстовых уведомлений
void MainWindow::slotSetTextInformator(const QString& text)
{
    ui->labelInformator->setTextInformation(text);
}

// Слот звуковых уведомлений
void MainWindow::slotPlaySound(const QString& type)
{
    if(!ui->actionSounds->isChecked())
        return; // Если звук выключен

    // Запускаем проигрывание звуков из папки
    effect_.setSource(QUrl::fromLocalFile("./sound/" + type + ".wav"));
    effect_.play();
}

// Слот запуска окна About
void MainWindow::slotAbout()
{
    aboutPtr = new About(this);

    aboutPtr->show();
}
