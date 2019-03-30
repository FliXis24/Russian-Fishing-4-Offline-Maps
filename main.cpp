/*************************************
*                                    *
*   Russian-Fishing-4-Offline-Maps   *
*                                    *
*************************************/

#include "MainWindow.h"
#include <QApplication>
#include <QSettings>
#include "License.h"

// Отображает виджет лицензии и возвращает решение пользователя
bool viewLicense();

// Считывает настройки лицензионного соглашения из файла и возвращает статус
bool readLicenseSettings();

// Записывает настройки лицензионного соглашения в файл
void writeLicenseSettings();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    bool licenseState = readLicenseSettings();

    if(!licenseState)
        licenseState = viewLicense();

    // Запускать главное окно только, если лицензионное соглашение принято пользователем
    if(licenseState) {
        writeLicenseSettings();

        MainWindow w;
        w.show();

        return a.exec();
    }

    return 0;
}

// Отображает виджет лицензии и возвращает решение пользователя
bool viewLicense()
{
    License license;
    if(license.exec() != QDialog::Accepted)
        return false;
    else
        return true;
}

// Считывает настройки лицензионного соглашения из файла и возвращает статус
bool readLicenseSettings()
{
    QSettings settings(".//settings.ini", QSettings::IniFormat);

    settings.beginGroup("LICENSE");
    const bool state = settings.value("accept", false).toBool();
    settings.endGroup();

    return state;
}

// Записывает настройки лицензионного соглашения в файл
void writeLicenseSettings()
{
    QSettings settings(".//settings.ini", QSettings::IniFormat);

    settings.beginGroup("LICENSE");
    settings.setValue("accept", true);
    settings.endGroup();

    settings.sync();
}
