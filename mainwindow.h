#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QTextEdit>
#include <QScreen>
#include <QDebug>
#include <QLabel>
#include <QToolButton>
#include <exception>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include "ui_mainwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onCreateBoard();  // 新建白板槽函数

private:
    Ui::MainWindow *ui;
};

// 自定义 QTextEdit 类以处理回车键事件
class CustomTextEdit : public QTextEdit {
    Q_OBJECT

public:
    explicit CustomTextEdit(QWidget *parent = nullptr) : QTextEdit(parent) {}

protected:
    void keyPressEvent(QKeyEvent *event) override;

signals:
    void returnPressed();
};

class TaskListWidget : public QListWidget {
    Q_OBJECT
public:
    using QListWidget::QListWidget;

protected:
    void mousePressEvent(QMouseEvent *event) override;
signals:
    void clickedBlankSpace();
};

class Whiteboard : public QWidget
{
    Q_OBJECT
public:
    Whiteboard(QWidget *parent = nullptr);

private slots:
    void onItemEdited(QListWidgetItem *item);
    void handleReturnPressed();

private:
    void addNewTask(const QString& text = "");
    CustomTextEdit* createMultiLineEditor(QListWidgetItem *item);
    void focusOnLastTask();
    void mousePressEvent(QMouseEvent *event);
    void closeEvent(QCloseEvent *event);
    void saveWhiteboardConfig();
    void loadWhiteboardConfig();

    TaskListWidget *taskList;

    bool isHandlingReturn; // 判断是否需要新建，若为最后一个且没内容，就不再新建
};

#endif // MAINWINDOW_H
