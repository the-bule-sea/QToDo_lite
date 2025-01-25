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

Whiteboard::Whiteboard(QWidget *parent) : QWidget(parent) , isHandlingReturn(false){
    setWindowTitle("桌面白板");
    QScreen* screen = QGuiApplication::primaryScreen();
    // 获取当前实际屏幕大小
    QRect rect1 = screen->geometry();
    qDebug() << "rect1" << rect1.size().width() << rect1.size().height();
    qDebug() << rect1.topLeft();
    qDebug() << rect1.bottomRight();
    //获取当前实际可用屏幕大小（去掉下边框）
    QRect rect2 = screen->availableGeometry();
    qDebug() << "rect2" << rect2.size().width() << rect2.size().height();
    qDebug() << rect2.topLeft();
    qDebug() << rect2.bottomRight();

    int whiteboard_width = rect2.size().width()*0.25;
    int whiteboard_height = rect2.size().height()*0.35;
    this->setMinimumHeight(whiteboard_height);
    this->setMinimumWidth(whiteboard_width);
    resize(whiteboard_width, whiteboard_height);
    setStyleSheet("background-color: white; border: 1px solid black;");

    QVBoxLayout *layout = new QVBoxLayout(this);
//    layout->setSpacing(60); // 设置项目间的间距为60像素
//    layout->setContentsMargins(5,5,5,5);
    taskList = new TaskListWidget();

    // 设置边距
    taskList->setSpacing(2);

    // 隐藏框线
    taskList->setFrameShape(QListWidget::NoFrame);

    // 隐藏滚动条
    taskList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    taskList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);


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
    qDebug() << "handleReturnPressed called by" << sender();
    if(isHandlingReturn) return;
    isHandlingReturn = true;
    CustomTextEdit* editor = qobject_cast<CustomTextEdit*>(sender());
    if (editor) {
        QListWidgetItem* item = taskList->currentItem();
        qDebug() << "item" << taskList->currentItem() << "editor" << editor;
        if (item && ((taskList->itemWidget(item))->findChild<CustomTextEdit*>()) == editor) {
            qDebug() << "if (item && taskList->itemWidget(item) == editor)";
            QString text = editor->toPlainText().trimmed();
            if (text.isEmpty()){
                editor->setFocus();
            }
            else{
                qDebug() << "addNewTask()";
                addNewTask();
                editor->setText(editor->toPlainText().trimmed());  // 去除尾部空格
                editor->clearFocus();  // 移除焦点，防止光标留在旧的编辑器内
            }
        }
    }

    isHandlingReturn = false;
}

void Whiteboard::mousePressEvent(QMouseEvent *event) {
    // 如果点击在taskList之外的区域，触发焦点
    if (!taskList->geometry().contains(event->pos())) {
        focusOnLastTask();
    }
    QWidget::mousePressEvent(event);
}


void Whiteboard::addNewTask() {
    QListWidgetItem *newItem = new QListWidgetItem("");
    newItem->setSizeHint(QSize(0, 62));
    newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);
    taskList->setWordWrap(true);
    taskList->addItem(newItem);

    QWidget *taskWidget = new QWidget();
    QHBoxLayout *taskLayout = new QHBoxLayout(taskWidget);

    // 创建大圆点
    QLabel *bulletLabel = new QLabel("●");
    QFont bulletFont;
    bulletFont.setPixelSize(19);  // 设置大圆点的大小
    bulletLabel->setFont(bulletFont);
    bulletLabel->setStyleSheet("margin-top: -10px;");
    bulletLabel->setAlignment(Qt::AlignTop);  // 顶部对齐，避免多行时不居中
    bulletLabel->setFrameShape(QFrame::NoFrame);
    bulletLabel->setStyleSheet("border:none;"); // 无边框

    // 文本输入框
    CustomTextEdit* editor = createMultiLineEditor(newItem);

    // 监听输入变化调整大小
    connect(editor, &CustomTextEdit::textChanged, [editor, newItem]() {
       QSize documentSize = editor->document()->size().toSize();
       editor->setFixedHeight(documentSize.height());  // 确保输入时高度自适应
       newItem->setSizeHint(QSize(editor->width(), documentSize.height() + 20));
    });


    // 将圆点和输入框添加到布局
    taskLayout->addWidget(bulletLabel);
    taskLayout->addWidget(editor,1);

    taskList->setItemWidget(newItem, taskWidget);

    taskList->setCurrentItem(newItem);
    editor->setFocus();

    QTimer::singleShot(0, [this, newItem]() {
        taskList->setCurrentItem(newItem);
        taskList->editItem(newItem);
    });
}

CustomTextEdit* Whiteboard::createMultiLineEditor(QListWidgetItem *item) {
    CustomTextEdit *editor = new CustomTextEdit();
    QFont textfont;
    textfont.setPixelSize(30);
    editor->setFont(textfont);
//    editor->setPlaceholderText(".......");
    editor->setFrameShape(QFrame::NoFrame);
    editor->setWordWrapMode(QTextOption::WordWrap);
    editor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    // 隐藏滚动条
    editor->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    editor->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    editor->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);


    // 隐藏边框
    editor->setStyleSheet("border:none;");

    // test background
//    editor->setStyleSheet("background-color: lightblue;");

//    connect(editor, &CustomTextEdit::textChanged, [this, editor, item]() {
//        item->setSizeHint(editor->document()->size().toSize());
//    });

    connect(editor, &CustomTextEdit::returnPressed, this, &Whiteboard::handleReturnPressed);

    return editor;
}

void Whiteboard::focusOnLastTask() {
    if (taskList->count() > 0) {
        qDebug() << "Whiteboard::focusOnLastTask";
        QListWidgetItem* itemL = taskList->item(taskList->count() - 1);

        // 通过 itemL 查找其关联的 QWidget 并获取 CustomTextEdit
        QWidget* taskWidget = taskList->itemWidget(itemL);
        CustomTextEdit* editor = taskWidget->findChild<CustomTextEdit*>();

        if (editor) {
            editor->setFocus();  // 让输入框获取焦点
        }

        taskList->setCurrentItem(itemL);
        taskList->scrollToItem(itemL);

        QTimer::singleShot(0, [this, itemL]() {
            taskList->editItem(itemL);
            isHandlingReturn = false;
        });
    }
}

