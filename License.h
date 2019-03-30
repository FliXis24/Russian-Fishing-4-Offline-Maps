#ifndef LICENSE_H
#define LICENSE_H

#include <QDialog>

namespace Ui {
class License;
}

class License : public QDialog
{
    Q_OBJECT

public:
    explicit License(QWidget *parent = nullptr);
    ~License() override;

private:
    Ui::License *ui;

    void setTextLicense();

protected:
    // Переопределение эвента, который вызывается после отображения окна
    void showEvent(QShowEvent *event) override;

private slots:
    // Переключение активности кнопки
    void slotEnableButtonYes(int state);
};

#endif // LICENSE_H
