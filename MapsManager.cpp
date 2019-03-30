#include "MapsManager.h"
#include "ui_MapsManager.h"
#include <QDesktopWidget>
#include <QStyle>
#include <QFileDialog>
#include <QMessageBox>
#include "WaterBody.h"
#include "QLabelEventGetPoint.h"
#include "MainWindow.h"

MapsManager::MapsManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MapsManager),
    modeCreateMap_(false)
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

    // Соединение сигнала от кнопки выбора изображения карты со своим слотом
    connect(ui->pushButtonChooseMapImage, &QPushButton::clicked, this, &MapsManager::slotChooseMapImage);

    // Соединение сигнала от кнопки настройки карты со своим слотом
    connect(ui->pushButtonMapSetup, &QPushButton::clicked, this, &MapsManager::slotMapSetup);

    // Соединение сигнала изменения текста со слотом имени карты
    connect(ui->textEditMapName, &QLineEdit::textChanged, this, &MapsManager::slotChangedMapName);

    // Соединяем сигнал от кнопки создания новой карты со своим слотом
    connect(ui->pushButtonCreateNewMap, &QPushButton::clicked, this, &MapsManager::slotCreateNewMap);

    // Соединяем сигнал от кнопки подтверждения со своим слотом
    connect(ui->pushButtonConfirmEditMap, &QPushButton::clicked, this, &MapsManager::slotConfirmEditMap);

    // Соединяем сигнал от кнопки удаления со своим слотом
    connect(ui->pushButtonDeleteMap, &QPushButton::clicked, this, &MapsManager::slotDeleteCurrentMap);

    // Заполнение списком карт
    ui->listWidgetMaps->addItems(WaterBody::getInstance().getListMapsName());

    // Соединение сигнала от модели со слотом установки текущей карты
    connect(ui->listWidgetMaps->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &MapsManager::slotClickedMap);

    // Выставление текущей карты
    ui->listWidgetMaps->setCurrentRow(ui->listWidgetMaps->count() - 1);

    // Соединение сигнала от кнопки Up со своим слотом
    connect(ui->pushButtonMoveLevelUp, &QPushButton::clicked, this, &MapsManager::slotMoveLevelUp);

    // Соединение сигнала от кнопки Down со своим слотом
    connect(ui->pushButtonMoveLevelDown, &QPushButton::clicked, this, &MapsManager::slotMoveLevelDown);

    // Проверяем валидность полей
    validFields();

    // Если карт нет, то кнопка удаления неактивна
    deleteEnabledSwitch();
}

MapsManager::~MapsManager()
{
    delete ui;

    // В некоторых случаях, к форме после ее удаления еще могут быть обращения
    ui = nullptr;
}

// Валидность полей и уведомление пользователя
void MapsManager::validFields() const
{
    bool result = true;
    ui->textBrowserRemarks->clear();
    ui->textBrowserRemarks->setStyleSheet("color: red");

    if(selectedMap_.mapFilePath.isEmpty()) {
        result = false;
        ui->textBrowserRemarks->append(tr("No map image selected"));
        ui->pushButtonMapSetup->setEnabled(false);
    } else {
        ui->pushButtonMapSetup->setEnabled(true);
    }

    if(selectedMap_.mapName.isEmpty()) {
        result = false;
        ui->textBrowserRemarks->append(tr("No map name selected or name already taken"));
    }

    QPair<double, double> mapRangeX = selectedMap_.mapRanges.first;
    QPair<double, double> mapRangeY = selectedMap_.mapRanges.second;
    if(mapRangeX.first == 0.0 && mapRangeX.second == 0.0 && mapRangeY.first == 0.0 && mapRangeY.second == 0.0) {
        result = false;
        ui->textBrowserRemarks->append(tr("Map not setup"));
    }

    // Включаем кнопку, если валидность пройдена
    buttonConfirmCreateEnabledSwitch(result);
}

// Переключение активности кнопки создания новой карты или подтверждения
void MapsManager::buttonConfirmCreateEnabledSwitch(const bool &result) const
{
    if(result) {
        ui->textBrowserRemarks->setStyleSheet("color: green");
        ui->textBrowserRemarks->append(tr("Everything is ready to add a new map"));

        if(modeCreateMap_) {
            ui->pushButtonCreateNewMap->setEnabled(true);
            ui->pushButtonConfirmEditMap->setEnabled(false);
        }
        else {
            ui->pushButtonConfirmEditMap->setEnabled(true);
            ui->pushButtonCreateNewMap->setEnabled(false);
        }
    } else {
        ui->pushButtonCreateNewMap->setEnabled(false);
        ui->pushButtonConfirmEditMap->setEnabled(false);
    }
}

// Переключение активности кнопки удаления
void MapsManager::deleteEnabledSwitch()
{
    if(ui->listWidgetMaps->count())
        ui->pushButtonDeleteMap->setEnabled(true);
    else
        ui->pushButtonDeleteMap->setEnabled(false);
}

// Слот изменения имени текущей карты
void MapsManager::slotChangedMapName(const QString &text)
{
    selectedMap_.mapName = text;

    // Проверяем валидность полей
    validFields();
}

// Слот выбора изображения карты
void MapsManager::slotChooseMapImage()
{
    // Создает папку, если ее нет
    QDir dir("maps");
    if (!dir.exists())
        dir.mkpath(".");

    // Открываем диалог выбора изображения карты
    QString mapFilePath = QFileDialog::getOpenFileName(
        this, tr("Choose Image Map"), ".//maps/", tr("Image Files (*.png *.jpg *.bmp)")
    );

    // Если пользователь ничего не выбрад
    if(mapFilePath.isEmpty())
        return;

    QFileInfo file(mapFilePath);
    QString fileNameWithExtension = file.fileName(); // Имя файла с расширением
    QString fileName = file.baseName(); // Имя файла без расширения

    // Если имя файла не валидно
    if(!isValidFileName(fileName)) {
        selectedMap_.mapFilePath.clear();

        // Проверяем валидность полей
        validFields();

        return;
    }

    // Если файл выбран не из нашей директории
    QFileInfo checkFile(".//maps/" + fileNameWithExtension);
    if(mapFilePath != checkFile.absoluteFilePath())
    {
        if(checkFile.exists() && checkFile.isFile()) {
            // Если в папке уже есть такой файл с таким названием,
            // то его надо удалить перед копированием
            QFile file(".//maps/" + fileNameWithExtension);
            file.remove();
            file.close();
        }
    }
    // Копируем выбранный пользователем файл в директорию
    QFile::copy(mapFilePath, ".//maps/" + fileNameWithExtension);

    selectedMap_.mapFilePath = ".//maps/" + fileNameWithExtension;
    selectedMap_.fileMapName = fileName;

    // Если изображение карты выбрано, то необходимо очистить все поля от старых значений
    selectedMap_.clearRanges();
    selectedMap_.mapName.clear();
    ui->textEditMapName->clear();

    // Переключение в режим создания новой карты
    modeCreateMap_ = true;

    // Установка миниатюры изображения карты
    ui->labelMapImagePreview->setPixmap(QPixmap(mapFilePath));

    // Проверяем валидность полей
    validFields();
}

// Слот установки диапазона координат текущей карты
void MapsManager::slotReceiveRanges(QPair<QPair<double, double>, QPair<double, double>> mapRanges)
{
    // Диапазон приходит из виджета настройки карт

    selectedMap_.mapRanges = mapRanges;

    // Проверяем валидность полей
    validFields();
}

// Слот запуска виджета настройки карты
void MapsManager::slotMapSetup()
{
    // Не создавать копии виджета для калибровки текущей карты
    if(mapSetupPtr_)
        return;

    if(!WaterBody::getInstance().getLabelImageMapPtr())
        return;

    // Скрываем окно Менеджера карт
    hide();

    mapSetupPtr_ = new MapSetup(
        WaterBody::getInstance().getLabelImageMapPtr()->width(),
        WaterBody::getInstance().getLabelImageMapPtr()->height(),
        this
    );

    // Соединяет сигнал текстовых уведомлений со своим слотом
    if(MainWindow *mainWindow = qobject_cast<MainWindow*>(parent()))
        connect(mapSetupPtr_, &MapSetup::signalSendTextInformator, mainWindow, &MainWindow::slotSetTextInformator);

    // Соединяет сигнал возвращение диапазона координат карты из окна настройки карт со своим слотом
    connect(mapSetupPtr_, &MapSetup::signalReturnRanges, this, &MapsManager::slotReceiveRanges);

    // Соединение сигнала отсылки координат мыши со слотом установки координат
    connect(WaterBody::getInstance().getLabelImageMapPtr(), &QLabelEventGetPoint::signalSendMousePoint, mapSetupPtr_, &MapSetup::slotSetLineEditPoint);

    // Соединяет сигнал закрытия виджета настройки карты с лямбдой
    connect(mapSetupPtr_, &MapSetup::destroyed, this, [this]()
    {
        // Если пользователь нажал на крестик, то обращение к ui будет невалидным, т.к. оно
        // уже не существует в этот момент
        if(!ui)
            return;

        // Обратно показываем окно Менеджера карт
        show();

        // Очищаем область карты
        WaterBody::getInstance().getLabelImageMapPtr()->setPixmap(QPixmap());

        // Очищаем поля отображения координат
        emit signalClearLineEditPoints();
    });

    // Иначе окно настройки карт будет свернуто
    mapSetupPtr_->setWindowFlags( windowFlags() | Qt::WindowStaysOnTopHint );
    mapSetupPtr_->show();
    mapSetupPtr_->activateWindow();

    // Устанавливаем изображение текущей карты
    WaterBody::getInstance().getLabelImageMapPtr()->setPixmap(QPixmap(selectedMap_.mapFilePath));
}

// Слот создания новой карты
void MapsManager::slotCreateNewMap()
{
    Map *newMap = new Map(selectedMap_);

    // Добавляем карту в коллекцию
    WaterBody::getInstance().addNewMapToCollection(newMap);

    // Добавляем карту в список карт Менеджера карт и устанавливаем ее текущей
    ui->listWidgetMaps->addItem(selectedMap_.mapName);
    ui->listWidgetMaps->setCurrentRow(ui->listWidgetMaps->count() - 1);

    // Включаем кнопку удаления
    deleteEnabledSwitch();

    // Переключаем в режим редактирования текущей карты
    modeCreateMap_ = false;

    // Проверяем валидность полей
    validFields();

    emit signalSendTextInformator(tr("New map ") + selectedMap_.mapName + tr(" created"));
    emit signalPlaySound("positive");
}

// Слот установки текущей карты
void MapsManager::slotClickedMap(const QModelIndex &currentIndex, const QModelIndex &previousIndex)
{
    Q_UNUSED(previousIndex);

    // Если в списке Менеджера карт есть хоть одна карта, то грузим ее миниатюру, имя и данные
    if(currentIndex.row() != -1 ) {
        WaterBody::getInstance().getMapData(currentIndex.row(), selectedMap_);
        ui->textEditMapName->setText(selectedMap_.mapName);
        ui->labelMapImagePreview->setPixmap(QPixmap(selectedMap_.mapFilePath));
    }
    else {
        selectedMap_ = Map();
        ui->textEditMapName->clear();
        ui->labelMapImagePreview->setText("No map image selected");

        // Проверяем валидность полей
        validFields();
    }
}

// Слот применения изменений в текущей карте
void MapsManager::slotConfirmEditMap()
{
    // На случай, если имя карты изменилось
    ui->listWidgetMaps->currentItem()->setText(selectedMap_.mapName);

    // Сохраняем изменения через индекс карты
    int currentIndex = ui->listWidgetMaps->currentRow();
    WaterBody::getInstance().setMapData(currentIndex, selectedMap_);

    emit signalSendTextInformator(tr("Map ") + selectedMap_.mapName + tr(" edited"));
    emit signalPlaySound("positive");
}

// Слот удаления текущей карты
void MapsManager::slotDeleteCurrentMap()
{
    // Удаляем карту из коллекции и сохраняем в файл
    WaterBody::getInstance().deleteMapToCollection(ui->listWidgetMaps->currentRow());

    emit signalSendTextInformator(tr("Map ") + selectedMap_.mapName + tr(" removed"));

    // Удаляем карту из списка меню Менеджера карт
    delete ui->listWidgetMaps->takeItem(ui->listWidgetMaps->currentRow());

    // Ставим текущей предыдущую карту
    ui->listWidgetMaps->setCurrentRow(ui->listWidgetMaps->count() - 1);

    // Включаем кнопку удаления, если текущая карта выставилась
    deleteEnabledSwitch();

    emit signalPlaySound("negative");
}

// Слот помещающий карту в списке на уровень выше
void MapsManager::slotMoveLevelUp()
{
    int currentRow = ui->listWidgetMaps->currentRow();
    if(currentRow > 0)
    {
        int upRow = currentRow - 1;

        QListWidgetItem tmp = *(ui->listWidgetMaps->currentItem());

        *(ui->listWidgetMaps->item(currentRow)) = *(ui->listWidgetMaps->item(upRow));
        *(ui->listWidgetMaps->item(upRow)) = tmp;

        // Меняем карты местами и записываем в файл
        WaterBody::getInstance().swapMaps(upRow, currentRow);

        ui->listWidgetMaps->setCurrentItem(ui->listWidgetMaps->item(upRow));

        // Переключаем в режим редактирования текущей карты
        modeCreateMap_ = false;

        // Проверяем валидность полей
        validFields();

        emit signalSendTextInformator(selectedMap_.mapName + tr(" map moved up one position"));
        emit signalPlaySound("leaf");
    }
}

// Слот помещающий карту в списке на уровень ниже
void MapsManager::slotMoveLevelDown()
{
    int currentRow = ui->listWidgetMaps->currentRow();
    if(currentRow < ui->listWidgetMaps->count() - 1)
    {
        int downRow = currentRow + 1;

        QListWidgetItem tmp = *(ui->listWidgetMaps->currentItem());

        *(ui->listWidgetMaps->item(currentRow)) = *(ui->listWidgetMaps->item(downRow));
        *(ui->listWidgetMaps->item(downRow)) = tmp;

        // Меняем карты местами и записываем в файл
        WaterBody::getInstance().swapMaps(currentRow, downRow);

        ui->listWidgetMaps->setCurrentItem(ui->listWidgetMaps->item(downRow));

        // Переключаем в режим редактирования текущей карты
        modeCreateMap_ = false;

        // Проверяем валидность полей
        validFields();

        emit signalSendTextInformator(selectedMap_.mapName + tr(" map moved down one position"));
        emit signalPlaySound("leaf");
    }
}

// Возвращает валидность имени файла изображения карты
bool MapsManager::isValidFileName(const QString &fileName)
{
    // Только числа и латинские символы

    bool valid = true;

    if(fileName.contains(QRegularExpression("[^a-zA-Z0-9_]"))) {
        valid = false;
        QMessageBox::information(this, tr("File does not fit"),
            tr("The file name must consist only of Latin letters or numbers, may include underscores")
        );
    }

    // Проверка, используется ли файл для другой карты
    QStringList listFileMapName = WaterBody::getInstance().getListFileMapName();
    for(auto it : listFileMapName) {
        if(fileName == it) {
            valid = false;
            QMessageBox::information(this, tr("File already in use"),
                tr("This file is already in use, please select another one")
            );
        }
    }

    return valid;
}
