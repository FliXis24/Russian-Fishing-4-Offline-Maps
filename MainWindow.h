#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "MapSetup.h"
#include "MapsManager.h"
#include "Point.h"
#include <QPointer>
#include "FoundPoint.h"
#include "QLabelEventGetPoint.h"
#include <QSoundEffect>
#include "About.h"

// Константы для выставления минимального размера окна
extern const int MINIMUM_WIDTH;
extern const int MINIMUM_HEIGHT;

// Константы цветов текста точек
extern const QString WHITE;
extern const QString BLACK;
extern const QString RED;
extern const QString BLUE;
extern const QString CYAN;
extern const QString MAGENTA;
extern const QString YELLOW;
extern const QString LIME;

// Константы размера шрифта текста точек
extern const int FONT_SIZE_8;
extern const int FONT_SIZE_9;
extern const int FONT_SIZE_10;
extern const int FONT_SIZE_11;
extern const int FONT_SIZE_12;

// Константы размера изображения точек
extern const int IMAGE_SIZE_16;
extern const int IMAGE_SIZE_20;
extern const int IMAGE_SIZE_24;
extern const int IMAGE_SIZE_32;

// Константа уровня звука
extern const double VOLUME;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    // Вернуть указатель на менеджер карт
    QPointer<MapsManager> getMapsManagerPtr();

private:
    // Устанавливает фиксированный размер ширины QLable в котором находится QPixmap
    // с изображением текущей карты
    void adjustWidgetSize();

    // Устанавливает новую геометрию виджету поиска точки после изменения размера окна
    void setNewGeometryFoundPoint();

    // Считывает настройки дисплея из файла
    void readDisplaySettings();

    // Считывает настройки карт из файла
    void readMapSettings();

    // Считывает настройки точки из файла
    void readPointSettings();

    // Считывает настройки звука из файла
    void readSoundSettings();

    // Считывает все настройки из файла
    void readAllSettings();

    // Записывает настройки дисплея в файл
    void writeDisplaySettings();

    // Записывает настройки карт в файл
    void writeMapSettings(const QString &mapName);

    // Записывает настройки звука в файл
    void writeSoundSettings(const bool &sound);

    // Записывает все настройки в файл
    void writeAllSettings();

    // Проверяет допустимость введенных или отображаемых координат
    bool isValidGamePoints();

    // Вызывает предупреждение, что карты отсутствуют в базе данных карт
    void menuEmptyMessage();

    // Создание пунктов меню выбора цвета шрифта точек
    void createActionsMenuFontColor();

    // Создание пунктов меню выбора размера шрифта точек
    void createActionsMenuFontSize();

    // Создание пунктов меню выбора размера изображения точек
    void createActionsMenuImageSize();

    // Создание пунктов меню выбора цвета и размера шрифта точек, и размера изображения точек
    void createActionsMenu();

    // Пересоздание пунктов меню выбора водоема
    void reloadActionsMenuWaterBodies();

    // Возвращает координаты мыши для центра игровой точки
    QPointF findCenterPoint(const QPointF findPoint);

    // Для проверки запущен ли виджет поиска точки
    QPointer<FoundPoint> foundPointPtr_;

    // Для проверки запущен ли виджет менеджера карт
    QPointer<MapsManager> mapsManagerPtr_;

    // Для проверки запущен ли виджет About
    QPointer<About> aboutPtr;

    // Для звуковых уведомлений
    QSoundEffect effect_;

    Ui::MainWindow *ui;

protected:
    // Переопределение эвента, который вызывается после отображения окна
    void showEvent(QShowEvent *event) override;

    // Переопределение эвента, который вызывается после каждого изменения размера виджета
    void resizeEvent(QResizeEvent *event) override;

signals:
    // Сигнал переключения режима отображения: (координаты мыши/игровые координаты)
    void signalSwitchMode();

    // Сигнал отсылает текст для слота текстовых уведомлений
    void signalSendTextInformator(const QString& text);

    // Сигнал отсылает тип звука в слот звуковых уведомлений
    void signalPlaySound(const QString& type);

private slots:
    // Слот установки цвета текста точек
    void slotMenuPointsFontColor(QAction *action);

    // Слот установки размера текста точек
    void slotMenuPointsFontSize(QAction *action);

    // Слот установки размера изображения точки
    void slotMenuPointsImageSize(QAction *action);

    // Установка водоема
    void slotConstructWaterBodies(QAction *action);

    // Слот отображения точек в полях отображения координат
    void slotViewPoint(const QPointF point);

    // Слот поиска точки
    void slotFindGamePoint();

    // Слот создания точки
    void slotCreateGamePoint(const QPointF currentMousePoint = QPointF(-1, -1));

    // Слот запуска окна менеджера карт
    void slotMapsManager();

    // Слот переключения активности кнопки поиска координат
    void slotFindGamePointEnabledSwitch();

    // Слот очистки полей отображения координат
    void slotClearLineEditPoints();

    // Слот запуска окна About
    void slotAbout();

public slots:
    // Слот текстовых уведомлений
    void slotSetTextInformator(const QString &text);

    // Слот звуковых уведомлений
    void slotPlaySound(const QString& type);
};

#endif // MAINWINDOW_H
