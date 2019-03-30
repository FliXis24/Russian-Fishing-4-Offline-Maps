#ifndef WATERBODY_H
#define WATERBODY_H

#include <QPair>
#include <vector>
#include <map>
#include "Map.h"
#include "Point.h"
#include "QLabelEventGetPoint.h"

// Константа дефолтного значения цвета шрифта точек
extern const QString DEFAULT_POINT_FONT_COLOR;

// Константа дефолтного значения размера шрифта точек
extern const int DEFAULT_POINT_FONT_SIZE;

// Константа дефолтного значения размера изображения точек
extern const int DEFAULT_POINT_IMAGE_SIZE;

class WaterBody;

class WaterBodyDestroyer
{

private:
    WaterBody* p_instance;

public:
    ~WaterBodyDestroyer();
    void initialize( WaterBody* p );
};

class WaterBody
{

private:
    static WaterBody* p_instance;
    static WaterBodyDestroyer destroyer;

    // Чтение файла с точками водоема
    void readFilePoint();

    // Проверка на валидность процентных координат точки
    bool isValidPercentPoint(const double percentX, const double percentY) const;

    // Проверка на валидность игровых координат точки
    bool isValidGamePoint(const double pointX, const double pointY) const;

    // Записывает настройки цвета из файла
    void writePointFontColorSettings(const QString &color);

    // Записывает настройки размера шрифта в поинтах из файла
    void writePointFontSizeSettings(const int &fontSize);

    // Записывает настройки размера изображения точки из файла
    void writePointImageSizeSettings(const int &imageSize);

    // Точки текущей карты
    std::map<int, Point*> currentMapPoints_;

    // Содержит коллекцию карт
    std::vector<Map*> mapCollection_;

    // Указатель на виджет с изображением карты
    QLabelEventGetPoint *labelImageMapPtr_ = nullptr;

    // Текущая карта
    Map currentMap_;

    // Текущие настройки цвета шрифта точек
    QString pointFontColor_;

    // Текущие настройки размера шрифта точек
    int pointFontSize_;

    // Текущие настройки размера изображения точек
    int pointImageSize_;

protected:
    WaterBody() { }
    WaterBody( const WaterBody& );
    WaterBody& operator=( WaterBody& );
   ~WaterBody() { }
    friend class WaterBodyDestroyer;

public:
    // Получить указатель на синглтон
    static WaterBody& getInstance();

    // Установка нужного водоема по индексу
    void setWaterBody(const std::vector<Map*>::size_type &mapIndex);

    // Возвращает имя водоема
    const QString &getMapName() const;

    // Возвращает имя файла водоема
    const QString &getFileMapName() const;

    // Возвращает путь к файлу водоема
    const QString &getMapFilePath() const;

    // Запись настроек текущей карты
    void writeMapsInFile() const;

    // Чтение настроек карт
    void readMapsFromFile();

    // Высчитать и вернуть координаты мыши для точки
    QPointF computeMousePoint(QPair<double, double> mousePercents);

    // Установить указатель на виджет с изображением карты
    void setLabelImageMapPtr(QLabelEventGetPoint *ptr);

    // Получить указатель на виджет с изображением карты
    QLabelEventGetPoint* getLabelImageMapPtr();

    // Возвращает диапазоны игровых координат
    const QPair<QPair<double, double>, QPair<double, double>> &getMapRanges() const;

    // Добавить новую карту в коллекцию
    void addNewMapToCollection(Map *newMap);

    // Удалить карту из коллекции
    void deleteMapToCollection(const int &index);

    // Вернуть список имен карт
    QStringList getListMapsName();

    // Вернуть список имен файлов карт
    QStringList getListFileMapName();

    // Получить карту по индексу из коллекции
    void getMapData(const int &index, Map& map) const;

    // Обновить настройки для карты по индексу в коллекции и в файле карт
    void setMapData(const int &index, Map& map);

    // Установить новую геометрию для текущих точек
    void setNewGeometry();

    // Установить новую геометрию для текущей точки
    void setNewGeometry(Point* point);

    // Добавить новую точку в текущие точки
    void writeNewPointInStorage(Point *point);

    // Удалить текущую точку по индексу
    void deletePointInStorage(const int& index);

    // Записать текущие точки в файл
    void writePointsInFile() const;

    // Показать текущие точки
    void showPoint() const;

    // Спрятать текущие точки
    void hidePoint() const;

    // Очистить текущие точки
    void clearPoint();

    // Поменять местами позиции карт по индексам и записать в файл
    void swapMaps(const int &firstIndex, const int &secondIndex);

    // Получить индекс карты
    int getMapIndex(const QString &mapName);

    // Установить цвет шрифта для точек и записать в файл настроек
    void setPointsFontColor(const QString &color);

    // Установить размер изображения точек и записать в файл настроек
    void setPointsImageSize(const int &imageSize);

    // Установить размер шрифта для точек и записать в файл настроек
    void setPointsFontSize(const int &fontSize);

    // Прочитать настройки цвета и размера шрифта точек, и размераизображения точек
    void readPointSettings();

    // Вернуть цвет шрифта точек
    const QString& getFontColorAttribute() const;

    // Вернуть размер шрифта точек
    const int &getFontSizeAttribute() const;

    // Вернуть размер изображения точек
    const int &getImageSizeAttribute() const;

    // Вернуть размер и цвет шрифта точек, и размер изображения точек
    std::tuple<QString, int, int> getPointAttributes();
};

#endif // WATERBODY_H
