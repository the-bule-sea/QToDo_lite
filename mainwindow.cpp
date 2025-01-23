#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 假设 onCreateBoard 是用于初始化 Whiteboard 的槽函数
    connect(ui->btnCreateBoard, &QPushButton::clicked, this, &MainWindow::onCreateBoard);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onCreateBoard() {
    Whiteboard *whiteboard = new Whiteboard();
    whiteboard->show();
}

void CustomTextEdit::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        emit returnPressed();
        event->accept(); // 标记事件为已处理
        return;
    }
    QTextEdit::keyPressEvent(event);
}

void TaskListWidget::mousePressEvent(QMouseEvent *event) {
    QListWidgetItem *item = itemAt(event->pos());
    if (!item) {
        emit clickedBlankSpace();
    } else {
        editItem(item);
    }
    QListWidget::mousePressEvent(event);
}

Whiteboard::Whiteboard(QWidget *parent) : QWidget(parent) {
    setWindowTitle("桌面白板");
    resize(400, 300);
    setStyleSheet("background-color: white; border: 1px solid black;");

    QVBoxLayout *layout = new QVBoxLayout(this);
    taskList = new TaskListWidget();
    layout->addWidget(taskList);
    setLayout(layout);

    // 初始添加一个可编辑任务项
    addNewTask();

    connect(taskList, &TaskListWidget::clickedBlankSpace, this, &Whiteboard::focusOnLastTask);
    connect(taskList, &QListWidget::itemChanged, this, &Whiteboard::onItemEdited);
}

void Whiteboard::onItemEdited(QListWidgetItem *item) {
    if (!item->text().trimmed().isEmpty()) {
        addNewTask();  // 输入完成后添加新任务
    }
}

void Whiteboard::handleReturnPressed() {
    CustomTextEdit* editor = qobject_cast<CustomTextEdit*>(sender());
    if (editor) {
        QListWidgetItem* item = taskList->currentItem();
        if (item && taskList->itemWidget(item) == editor) {
            addNewTask();
            editor->setText(editor->toPlainText().trimmed());  // 去除尾部空格
            editor->clearFocus();  // 移除焦点，防止光标留在旧的编辑器内
        }
    }
}

void Whiteboard::addNewTask() {
    QListWidgetItem *newItem = new QListWidgetItem("");
    newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);

    taskList->setWordWrap(true);
    taskList->addItem(newItem);
    CustomTextEdit* editor = createMultiLineEditor(newItem);
    taskList->setItemWidget(newItem, editor);

    QTimer::singleShot(0, [this, newItem]() {
        taskList->setCurrentItem(newItem);
        taskList->editItem(newItem);
    });
}

CustomTextEdit* Whiteboard::createMultiLineEditor(QListWidgetItem *item) {
    CustomTextEdit *editor = new CustomTextEdit();
    editor->setPlaceholderText("输入任务...");
    editor->setFrameShape(QFrame::NoFrame);
    editor->setWordWrapMode(QTextOption::WordWrap);
    editor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    connect(editor, &CustomTextEdit::textChanged, [this, editor, item]() {
        item->setSizeHint(editor->document()->size().toSize());
    });

    connect(editor, &CustomTextEdit::returnPressed, this, &Whiteboard::handleReturnPressed);

    return editor;
}

void Whiteboard::focusOnLastTask() {
    if (taskList->count() > 0) {
        QListWidgetItem *lastItem = taskList->item(taskList->count() - 1);
        taskList->setCurrentItem(lastItem);
        taskList->scrollToItem(lastItem);

        QTimer::singleShot(0, [this, lastItem]() {
            taskList->editItem(lastItem);
        });
    }
}
