#include "License.h"
#include "ui_License.h"
#include <QStyle>
#include <QDesktopWidget>
#include <QPushButton>

License::License(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::License)
{
    ui->setupUi(this);

    // Запуск виджета по центру экрана
    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            size(),
            qApp->desktop()->availableGeometry()
        )
    );

    ui->buttonBoxLicense->button(QDialogButtonBox::Yes)->setEnabled(false);

    // Текст лицензионного соглашения
    setTextLicense();

    // Соединяет сигнал от чекбокса со слотом переключения активности кнопки
    connect(ui->checkBoxLicense, &QCheckBox::stateChanged, this, &License::slotEnableButtonYes);
}

License::~License()
{
    delete ui;
}

// Переопределение эвента, который вызывается после отображения окна
void License::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    // Не разрешаем менять размер окна
    setMinimumSize(width(), height());
    setMaximumSize(width(), height());
}

// Переключение активности кнопки
void License::slotEnableButtonYes(int state)
{
    ui->buttonBoxLicense->button(QDialogButtonBox::Yes)->setEnabled(state);
}

void License::setTextLicense()
{
    ui->labelLicense->setText(tr(
        "MIT License\n\n"

        "Copyright (c) 2019 FliXis24\n\n"

        "Permission is hereby granted, free of charge, to any person obtaining a copy\n"
        "of this software and associated documentation files (the \"Software\"), to deal\n"
        "in the Software without restriction, including without limitation the rights\n"
        "to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n"
        "copies of the Software, and to permit persons to whom the Software is\n"
        "furnished to do so, subject to the following conditions:\n\n"

        "The above copyright notice and this permission notice shall be included in all\n"
        "copies or substantial portions of the Software.\n\n"

        "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
        "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
        "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n"
        "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"
        "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"
        "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n"
        "SOFTWARE."
        )
    );
}
