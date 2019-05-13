#include "WaterBody.h"
#include <QFile>
#include <QDir>
#include <QDataStream>
#include <QTextStream>
#include <QFileInfo>
#include <cmath>
#include <QSettings>

WaterBody * WaterBody::p_instance = nullptr;
WaterBodyDestroyer WaterBody::destroyer;

// Константа дефолтного значения цвета шрифта точек
const QString DEFAULT_POINT_FONT_COLOR = "Yellow";

// Константа дефолтного значения размера шрифта точек
const int DEFAULT_POINT_FONT_SIZE = 9;

// Константа дефолтного значения размера изображения точек
const int DEFAULT_POINT_IMAGE_SIZE = 16;

WaterBodyDestroyer::~WaterBodyDestroyer()
{
    delete p_instance;
}

void WaterBodyDestroyer::initialize( WaterBody* p )
{
    p_instance = p;
}

// Получить указатель на синглтон
WaterBody& WaterBody::getInstance()
{
    if(!p_instance)     {
        p_instance = new WaterBody();
        destroyer.initialize( p_instance);
    }
    return *p_instance;
}

// Установка нужного водоема по индексу
void WaterBody::setWaterBody(const std::vector<Map*>::size_type &mapIndex)
{
    currentMap_ = *mapCollection_.at(mapIndex);

    //readMapsFromFile();
    readFilePoint();
    showPoint();
}

// Получить индекс карты
int WaterBody::getMapIndex(const QString &mapName)
{
    QStringList listMapName = getListFileMapName();
    return listMapName.indexOf(mapName);
}

// Возвращает имя файла водоема
const QString &WaterBody::getFileMapName() const
{
    return currentMap_.fileMapName;
}

// Возвращает путь к файлу водоема
const QString &WaterBody::getMapFilePath() const
{
    return currentMap_.mapFilePath;
}

// Возвращает имя водоема
const QString &WaterBody::getMapName() const
{
    return currentMap_.mapName;
}

// Получить указатель на виджет с изображением карты
QLabelEventGetPoint *WaterBody::getLabelImageMapPtr()
{
    return labelImageMapPtr_;
}

// Установить указатель на виджет с изображением карты
void WaterBody::setLabelImageMapPtr(QLabelEventGetPoint* labelImageMapPtr)
{
    labelImageMapPtr_ = labelImageMapPtr;
}

// Чтение настроек карт
void WaterBody::readMapsFromFile()
{
    QString firstFilePath = ".//bin/maps/maps.dat";
    QString secondFilePath = ".//bin/maps/maps.old.dat";

    // Проверка существования файла, если файла нет, ищем .old-файл
    QFileInfo checkFile(firstFilePath);
    if(!checkFile.exists() || !checkFile.isFile())
    {
        QFileInfo checkFile(secondFilePath);
        if(!checkFile.exists() || !checkFile.isFile())
            return;

        // Если .old-файл найден, то переименовываем его
        QFile::rename(secondFilePath, firstFilePath);
    }

    mapCollection_.clear();

    QFile file(firstFilePath);
    if(file.open(QIODevice::ReadOnly)) {
        QDataStream stream(&file);
        while(!stream.atEnd()) {
            Map *map = new Map;
            stream >> map->mapFilePath >> map->fileMapName >> map->mapName >> map->mapRanges;
            mapCollection_.push_back(map);
        }

        if(stream.status() != QDataStream::Ok) {
            exit(2);
        }
    }
    file.close();
}

// Возвращает диапазоны игровых координат
const QPair<QPair<double, double>, QPair<double, double>> &WaterBody::getMapRanges() const
{
    return currentMap_.mapRanges;
}

// Добавить новую точку в текущие точки
void WaterBody::writeNewPointInStorage(Point* point)
{
    currentMapPoints_[point->getID()] = point;
}

// Удалить текущую точку по индексу
void WaterBody::deletePointInStorage(const int& index)
{
//    vecPointStorage_.erase(std::remove_if(vecPointStorage_.begin(), vecPointStorage_.end(),
//        [index](const Point* point) {
//            return point->getID() == index;
//        }),
//        vecPointStorage_.end()
//    );

    auto ptrPoint = currentMapPoints_.at(index);
    ptrPoint->deleteLater();
    currentMapPoints_.erase(index);
}

// Записать текущие точки в файл
void WaterBody::writePointsInFile() const
{
    // Создает папку, если ее нет
    QDir dir("points");
    if (!dir.exists())
        dir.mkpath(".");

    QString firstFilePath = ".//points/" + getFileMapName() + ".cfg";
    QString secondFilePath = ".//points/" + getFileMapName() + ".old.cfg";

    QFileInfo checkFile(firstFilePath);
    if(checkFile.exists() && checkFile.isFile())
        QFile::rename(firstFilePath, secondFilePath); // Переименовываем старый файл в .old

    QFile file(firstFilePath);
    if(file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream stream(&file);

        const QChar separator = ':';
        //const QChar separatorEnd = ';';

        for(const auto& it : currentMapPoints_)
        {
            stream << it.second->getPriority() << separator
                   << it.second->getMousePercents().first << separator
                   << it.second->getMousePercents().second << separator
                   << it.second->getGamePoint().x() << separator
                   << it.second->getGamePoint().y() << separator
                   << it.second->typeTackle << separator
                   << it.second->from << separator
                   << it.second->fish << separator
                   << it.second->baits << separator
                   << it.second->rigs << separator
                   << it.second->note << separator
                   << it.second->flyCasting << separator
                   << it.second->distance << separator
                   << it.second->depth << separator
                   << it.second->groundbaits << separator
                   << it.second->unwantedFish << separator /*<< separatorEnd*/ << endl;

            file.flush();
        }

        if(stream.status() != QTextStream::Ok) {
            exit(2);
        }
    }
    file.close();

    // Удаляем файл .old
    QFile(secondFilePath).remove();
}

// Чтение файла с точками водоема
void WaterBody::readFilePoint()
{
    QString firstFilePath = ".//points/" + getFileMapName() + ".cfg";
    QString secondFilePath = ".//points/" + getFileMapName() + ".old.cfg";

    // Проверка существования файла, если файла нет, ищем .old-файл
    QFileInfo checkFile(firstFilePath);
    if(!checkFile.exists() || !checkFile.isFile())
    {
        QFileInfo checkFile(secondFilePath);
        if(!checkFile.exists() || !checkFile.isFile())
            return;

        // Если .old-файл найден, то переименовываем его
        QFile::rename(secondFilePath, firstFilePath);
    }

    if(!labelImageMapPtr_)
        exit(1);

    QFile file(firstFilePath);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        while(!file.atEnd())
        {
            QString strPoint = file.readLine();
            QStringList listPoint = strPoint.split(":");

            if(!isValidPercentPoint(listPoint.at(1).toDouble(), listPoint.at(2).toDouble()))
                continue;
            if(!isValidGamePoint(listPoint.at(3).toDouble(), listPoint.at(4).toDouble()))
                continue;

            Point* point = new Point(
                listPoint.at(0).toInt(),
                QPair<double, double>(listPoint.at(1).toDouble(), listPoint.at(2).toDouble()),
                QPointF(qRound(listPoint.at(3).toDouble()), qRound(listPoint.at(4).toDouble())),
                labelImageMapPtr_
            );
            point->typeTackle = listPoint.at(5).toInt();
            point->from = listPoint.at(6).toInt();
            point->fish = listPoint.at(7);
            point->baits = listPoint.at(8);
            point->rigs = listPoint.at(9);
            point->note = listPoint.at(10);
            point->flyCasting = listPoint.at(11);
            point->distance = listPoint.at(12);
            point->depth = listPoint.at(13);
            point->groundbaits = listPoint.at(14);
            point->unwantedFish = listPoint.at(15);

            currentMapPoints_[point->getID()] = point;
        }
    }
    else {
        exit(2);
    }

    file.close();
}

// Показать текущие точки
void WaterBody::showPoint() const
{
    for(const auto& it : currentMapPoints_)
        it.second->show();
}

// Спрятать текущие точки
void WaterBody::hidePoint() const
{
    for(const auto& it : currentMapPoints_)
        it.second->hide();
}

// Очистить текущие точки
void WaterBody::clearPoint()
{
    for(const auto& it : currentMapPoints_)
        it.second->deleteLater();
    currentMapPoints_.clear();
}

// Установить новую геометрию для текущих точек
void WaterBody::setNewGeometry()
{
    if(!labelImageMapPtr_)
        return;

    for(auto &it : currentMapPoints_)
    {
        QPointF mousePoint = computeMousePoint(it.second->getMousePercents());
        it.second->setNewMousePoint(mousePoint);

        setNewGeometry(it.second);
    }
}

// Установить новую геометрию для текущей точки
void WaterBody::setNewGeometry(Point* point)
{
    double widthWidget = point->width();
    double halfWidthWidget = widthWidget / 2;
    double heightLabelImagePoint = point->getLabelImagePoint()->height();
    double halfHeightPix = heightLabelImagePoint / 2;
    point->setGeometry(
        qRound(point->getMousePoint().x() - halfWidthWidget), qRound(point->getMousePoint().y() - halfHeightPix),
        static_cast<int>(widthWidget), point->height()
    );
}

// Высчитать и вернуть координаты мыши для точки
QPointF WaterBody::computeMousePoint(QPair<double, double> mousePercents)
{
    if(!getLabelImageMapPtr())
        exit(1);

    double width = getLabelImageMapPtr()->width();
    double height = getLabelImageMapPtr()->height();

    double mousePointX = width * mousePercents.first / 100;
    double mousePointY = height * mousePercents.second / 100;

    return QPointF(mousePointX, mousePointY);
}

// Проверка на валидность процентных координат точки
bool WaterBody::isValidPercentPoint(const double percentX, const double percentY) const
{
    if(percentX > 100 || percentX < 0 || percentY > 100 || percentY < 0)
        return false;

    return true;
}

// Проверка на валидность игровых координат точки
bool WaterBody::isValidGamePoint(const double pointX, const double pointY) const
{
    QPair<double, double> rangeX = currentMap_.mapRanges.first;
    QPair<double, double> rangeY = currentMap_.mapRanges.second;

    if(pointX < rangeX.first || pointX > rangeX.second || pointY < rangeY.first || pointY > rangeY.second)
        return false;

    return true;
}

// Добавить новую карту в коллекцию
void WaterBody::addNewMapToCollection(Map *newMap)
{
    mapCollection_.push_back(newMap);

    // Сохраняем в файл
    writeMapsInFile();
}

// Удалить карту из коллекции
void WaterBody::deleteMapToCollection(const int &index)
{
    mapCollection_.erase(mapCollection_.begin() + index);

    // Сохраняем в файл
    writeMapsInFile();
}

// Запись настроек текущей карты
void WaterBody::writeMapsInFile() const
{
    // Создает папки, если их нет
    QDir dir("bin/maps");
    if (!dir.exists())
        dir.mkpath(".");

    QString firstFilePath = ".//bin/maps/maps.dat";
    QString secondFilePath = ".//bin/maps/maps.old.dat";

    QFileInfo checkFile(firstFilePath);
    if(checkFile.exists() && checkFile.isFile())
        QFile::rename(firstFilePath, secondFilePath); // Переименовываем старый файл в .old

    QFile file(firstFilePath);
    if(file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QDataStream stream(&file);
        for(auto it : mapCollection_)
            stream << it->mapFilePath << it->fileMapName << it->mapName << it->mapRanges;

        if(stream.status() != QDataStream::Ok) {
            exit(2);
        }
    }
    file.close();

    // Удаляем файл .old
    QFile(secondFilePath).remove();
}

// Вернуть список имен карт
QStringList WaterBody::getListMapsName()
{
    QStringList listMapsName;
    for(auto it : mapCollection_)
        listMapsName.append(it->mapName);

    return listMapsName;
}

// Вернуть список имен файлов карт
QStringList WaterBody::getListFileMapName()
{
    QStringList listMapsName;
    for(auto it : mapCollection_)
        listMapsName.append(it->fileMapName);

    return listMapsName;
}

// Получить карту по индексу из коллекции
void WaterBody::getMapData(const int &index, Map& map) const
{
    map = *mapCollection_.at(static_cast<std::vector<Map*>::size_type>(index));
}

// Обновить настройки для карты в коллекции и в файле карт
void WaterBody::setMapData(const int &index, Map& map)
{
    *mapCollection_.at(static_cast<std::vector<Map*>::size_type>(index)) = map;

    // Сохраняем в файл
    writeMapsInFile();
}

// Поменять местами позиции карт по индексам и записать в файл
void WaterBody::swapMaps(const int &firstIndex, const int &secondIndex)
{
    std::swap(mapCollection_.at(static_cast<std::vector<Map*>::size_type>(firstIndex)), mapCollection_.at(static_cast<std::vector<Map*>::size_type>(secondIndex)));

    // Сохраняем в файл
    writeMapsInFile();
}

// Установить цвет шрифта для точек и записать в файл настроек
void WaterBody::setPointsFontColor(const QString &color)
{
    // Записываем изменение в синглтон
    pointFontColor_ = color;

    for(auto it : currentMapPoints_)
        it.second->getLabelTextGamePoint()->setStyleSheet("color: " + pointFontColor_);

    // Запись в файл настроек
    writePointFontColorSettings(pointFontColor_);
}

// Установить размер шрифта для точек и записать в файл настроек
void WaterBody::setPointsFontSize(const int &fontSize)
{
    // Записываем изменение в синглтон
    pointFontSize_ = fontSize;

    QFont font;
    font.setPointSize(pointFontSize_);

    for(auto it : currentMapPoints_) {
        it.second->getLabelTextGamePoint()->setFont(font);

        // Чтобы виджет точки занимал минимально возможный размер
        it.second->resize(it.second->sizeHint());
    }

    // Если меняется размер шрифта точек, то необходимо внести поправки и в геометрию точек
    setNewGeometry();

    // Запись в файл настроек
    writePointFontSizeSettings(pointFontSize_);
}

// Установить размер изображения точек и записать в файл настроек
void WaterBody::setPointsImageSize(const int &imageSize)
{
    // Записываем изменение в синглтон
    pointImageSize_ = imageSize;

    QString priority;

    for(auto it : currentMapPoints_) {
        if(it.second->getPriority())
            priority = "priority-";
        it.second->getLabelImagePoint()->setPixmap(QPixmap("://image/point-" + priority + QString::number(pointImageSize_)));

        // Чтобы виджет точки занимал минимально возможный размер
        it.second->resize(it.second->sizeHint());

        priority = "";
    }

    // Если меняется размер изображения точек, то необходимо внести поправки и в геометрию
    // точек
    setNewGeometry();

    // Запись в файл настроек
    writePointImageSizeSettings(pointImageSize_);
}

// Вернуть размер и цвет шрифта точек, и размер изображения точек
std::tuple<QString, int, int> WaterBody::getPointAttributes()
{
    return std::make_tuple(pointFontColor_, pointFontSize_, pointImageSize_);
}

// Вернуть цвет шрифта точек
const QString& WaterBody::getFontColorAttribute() const
{
    return pointFontColor_;
}

// Вернуть размер шрифта точек
const int& WaterBody::getFontSizeAttribute() const
{
    return pointFontSize_;
}

// Вернуть размер изображения точек
const int& WaterBody::getImageSizeAttribute() const
{
    return pointImageSize_;
}

// Прочитать настройки цвета и размера шрифта точек, и размераизображения точек
void WaterBody::readPointSettings()
{
    QSettings settings(".//settings.ini", QSettings::IniFormat);

    settings.beginGroup("POINT");
        pointFontColor_ = settings.value("fontColor", DEFAULT_POINT_FONT_COLOR).toString();
        pointFontSize_ = settings.value("fontSize", DEFAULT_POINT_FONT_SIZE).toInt();
        pointImageSize_ = settings.value("imageSize", DEFAULT_POINT_IMAGE_SIZE).toInt();
    settings.endGroup();
}

// Записывает настройки цвета из файла
void WaterBody::writePointFontColorSettings(const QString &color)
{
    QSettings settings(".//settings.ini", QSettings::IniFormat);

    settings.beginGroup("POINT");
    settings.setValue("fontColor", color);
    settings.endGroup();

    settings.sync();
}

// Записывает настройки размера шрифта в поинтах из файла
void WaterBody::writePointFontSizeSettings(const int &fontSize)
{
    QSettings settings(".//settings.ini", QSettings::IniFormat);

    settings.beginGroup("POINT");
    settings.setValue("fontSize", fontSize);
    settings.endGroup();

    settings.sync();
}

// Записывает настройки размера изображения точки из файла
void WaterBody::writePointImageSizeSettings(const int &imageSize)
{
    QSettings settings(".//settings.ini", QSettings::IniFormat);

    settings.beginGroup("POINT");
    settings.setValue("imageSize", imageSize);
    settings.endGroup();

    settings.sync();
}
