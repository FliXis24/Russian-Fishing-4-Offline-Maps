#ifndef MAP_H
#define MAP_H

#include <QString>
#include <qpair.h>

class Map
{
public:
    Map();
    Map(const Map& newMap);
    Map(const QString& mapFilePath, const QString& fileMapName, const QString& mapName, const QPair<QPair<double, double>, QPair<double, double>>  mapRange);

    // Очистка диапазона координат карты
    void clearRanges();

    // Путь к файлу изображения карты
    QString mapFilePath;

    // Имя файла с изображением карты
    QString fileMapName;

    // Имя карты
    QString mapName;

    // Диапазон координат карты
    QPair<QPair<double, double>, QPair<double, double>> mapRanges;
};

#endif // MAP_H
