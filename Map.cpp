#include "Map.h"

Map::Map()
{

}

Map::Map(const Map& newMap) :
    mapFilePath(newMap.mapFilePath),
    fileMapName(newMap.fileMapName),
    mapName(newMap.mapName),
    mapRanges(newMap.mapRanges)
{

}

Map::Map(const QString& mapFilePath, const QString& fileMapName, const QString& mapName,
const QPair<QPair<double, double>, QPair<double, double> > mapRange) :
    mapFilePath(mapFilePath),
    fileMapName(fileMapName),
    mapName(mapName),
    mapRanges(mapRange)
{

}

// Очистка диапазона координат карты
void Map::clearRanges()
{
    mapRanges.first.first = mapRanges.first.second = mapRanges.second.first = mapRanges.second.second = 0.0;
}
