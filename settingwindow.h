#ifndef SETTINGWINDOW_H
#define SETTINGWINDOW_H

#include <QWidget>

namespace Ui
{
    class SettingWindow;
} // namespace Ui

class SettingWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SettingWindow(QWidget *parent = nullptr);
    ~SettingWindow();

private:
    Ui::SettingWindow *ui;

    void setAutoStart(bool enable);
};
#endif // SETTINGWINDOW_H
