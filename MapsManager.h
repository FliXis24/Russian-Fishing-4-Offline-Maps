#ifndef MAPSMANAGER_H
#define MAPSMANAGER_H

#include <QWidget>
#include "MapSetup.h"
#include <QPointer>
#include <Map.h>
#include <QStringListModel>

namespace Ui {
class MapsManager;
}

class MapsManager : public QWidget
{
    Q_OBJECT

public:
    explicit MapsManager(QWidget *parent = nullptr);
    ~MapsManager();

private:
    Ui::MapsManager *ui;

    // Текущая карта
    Map selectedMap_;

    // Для проверки запущен ли виджет калибровки карты
    QPointer<MapSetup> mapSetupPtr_;

    // Режим редактирования карты или создание новой
    bool modeCreateMap_;

    // Валидность полей и уведомляет пользователя
    void validFields() const;

    // Переключение активности кнопки удаления
    void deleteEnabledSwitch();

    // Переключение активности кнопки создания новой карты или подтверждения
    void buttonConfirmCreateEnabledSwitch(const bool &result) const;

    // Возвращает валидность имени файла изображения карты
    bool isValidFileName(const QString &fileName);

signals:
    // Сигнал для очистки полей отображения координат
    void signalClearLineEditPoints();

    // Сигнал отсылает текст для слота текстовых уведомлений
    void signalSendTextInformator(const QString& text);

    // Сигнал отсылает тип звука в слот звуковых уведомлений
    void signalPlaySound(const QString& type);

private slots:
    // Слот выбора изображения карты
    void slotChooseMapImage();

    // Слот изменения имени текущей карты
    void slotChangedMapName(const QString &text);

    // Слот запуска виджета настройки карты
    void slotMapSetup();

    // Слот создания новой карты
    void slotCreateNewMap();

    // Слот удаления текущей карты
    void slotDeleteCurrentMap();

    // Слот применения изменений в текущей карте
    void slotConfirmEditMap();

    // Слот установки диапазона координат текущей карты
    void slotReceiveRanges(QPair<QPair<double, double>, QPair<double, double>> mapRanges);

    // Слот установки текущей карты
    void slotClickedMap(const QModelIndex &currentIndex, const QModelIndex &previousIndex);

    // Слот помещающий карту в списке на уровень выше
    void slotMoveLevelUp();

    // Слот помещающий карту в списке на уровень ниже
    void slotMoveLevelDown();
};

#endif // MAPSMANAGER_H
