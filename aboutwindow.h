#ifndef ABOUTWINDOW_H
#define ABOUTWINDOW_H

#include <QWidget>

namespace Ui {
class AboutWindow;
}

class AboutWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AboutWindow(QWidget *parent = nullptr);
    ~AboutWindow();
private slots:
    void openSourceUrl();

private:
    Ui::AboutWindow *ui;
};

#endif // ABOUTWINDOW_H
