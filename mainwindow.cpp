#include "mainwindow.h"
#include "data_manager.h"
#include "config_manager.h"
#include <QDir>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    this->setWindowIcon(QIcon("://todo_ico_v2.svg"));

    ui->setupUi(this);
    ui->stackedWidget->addWidget(&settingWindow);
    ui->stackedWidget->addWidget(&aboutWindow);

    btnGroup.addButton(ui->btnSettings, 0);
    btnGroup.addButton(ui->btnAbout,1);

    connect(&btnGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), ui->stackedWidget, &QStackedWidget::setCurrentIndex);

    // 默认选中界面
    btnGroup.button(0)->setChecked(true);
    ui->stackedWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
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
//    this->setWindowFlags(Qt::CustomizeWindowHint |Qt::WindowTitleHint);
    this->setWindowFlags(Qt::Tool |Qt::CustomizeWindowHint |Qt::WindowTitleHint);

    // 创建托盘
    createActions();
    createTrayIcon();

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

    // 获取应用程序数据目录
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString filePath = dataPath + "/data.json";
    qDebug() << "应用程序数据目录" << filePath;
    // 加载数据
    QList<TaskData> tasks = DataManager::loadData(filePath);
    for (const auto& task : tasks){
        if(!task.text.trimmed().isEmpty())
        addNewTask(task.text);
    }
    // 初始添加一个可编辑任务项
    addNewTask();

    // 加载配置文件
    loadWhiteboardConfig();

    connect(taskList, &TaskListWidget::clickedBlankSpace, this, &Whiteboard::focusOnLastTask);
    connect(taskList, &QListWidget::itemChanged, this, &Whiteboard::onItemEdited);
}

void Whiteboard::loadWhiteboardConfig()
{
    // 获取应用配置目录
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QString filePath = configPath + "/whiteboard_config.ini";
    WindowGeometry geometry = ConfigManager::loadWindowGeometry(filePath);
    qDebug() << "loadWhiteboardConfig()" << "白板位置" << geometry.topLeft;
    // 获取标题栏高度
    int titleBarHeight = style()->pixelMetric(QStyle::PM_TitleBarHeight);

    // 调整 y 坐标，减去标题栏高度
    int adjustedY = geometry.topLeft.y() + titleBarHeight;
    qDebug() << "标题栏高度" << titleBarHeight;
    setGeometry(geometry.topLeft.x(), adjustedY, geometry.size.width(), geometry.size.height());
}

void Whiteboard::createActions()
{
    settingAction = new QAction(tr("&Setting"), this);
    connect(settingAction, &QAction::triggered, this, &Whiteboard::createNewMainWindow);
    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, this, &Whiteboard::handleQuitAction);
}

void Whiteboard::createTrayIcon()
{
    QMenu* trayMenu = new QMenu(this);
    trayMenu->addAction(settingAction);
    trayMenu->addSeparator();
    trayMenu->addAction(quitAction);
    
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayMenu);
    trayIcon->setIcon(QIcon("://todo_ico_v2.svg"));
    trayIcon->setToolTip("QToDoLite");
    trayMenu->setStyleSheet("QMenu {"
                             "    background-color: #ffffff; /* Light gray background */"
                             "    border: 2px solid #c0c0c0;      /* Gray border */"
                             "    padding: 4px;"
                             "}"
                             "QMenu::item {"
                             "    background-color: #ffffff; /* 设置菜单项背景颜色 */"
                             "    color: #333333; "
                             "    padding: 5px 10px;          /* Padding around text */"
                             "}"
                             "QMenu::item:selected {"
                             "    border: 0px solid transparent;"
                             "    background-color: #3388FF; /* Slightly bule on hover */"
                             "    color: #ffffff;"
                             "}"
                             "QMenu::item:selected {"
                             "    border-radius: 10px; "
                             "}"
                            );
    trayIcon->show();
}

void Whiteboard::createNewMainWindow()
{
    MainWindow *mainWindow = new MainWindow();
    mainWindow->setWindowTitle("QToDo-lite");
    mainWindow->setAttribute(Qt::WA_QuitOnClose, false); // 设置窗口属性
    mainWindow->show();
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

void Whiteboard::closeEvent(QCloseEvent *event){
    QList<TaskData> tasks;
    for(int i = 0; i < taskList->count(); ++i){
        QListWidgetItem* item = taskList->item(i);
        QWidget *widget = taskList->itemWidget(item);
        if (widget) {
            CustomTextEdit *editor = widget->findChild<CustomTextEdit*>();
            if (editor) {
                tasks.append({editor->toPlainText()});
            }
        }
    }

    // 获取应用程序数据目录
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataPath);
    if (!dir.exists()){
        qDebug() << "创建应用程序数据目录" << dataPath;
        dir.mkpath(".");
    }
    QString filePath = dataPath + "/data.json";
    qDebug() << "closeEvent";
    DataManager::saveData(tasks, filePath);

    // 白板窗口配置(窗口大小与位置)
    saveWhiteboardConfig();
    QWidget::closeEvent(event);
}

void Whiteboard::handleQuitAction()
{
    // 在退出事件循环之前执行 closeEvent
    close(); // 先触发 closeEvent

    // 然后再退出应用程序
    QApplication::quit();
}

void Whiteboard::saveWhiteboardConfig(){
    WindowGeometry geometry;
    geometry.topLeft = pos();
    geometry.size = size();
    qDebug() << "白板位置" << geometry.topLeft << "白板大小" << geometry.size ;
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir dir(configPath);
    if (!dir.exists()){
        dir.mkpath(".");
    }
    QString filePath = configPath + "/whiteboard_config.ini";
    ConfigManager::saveWindowGeometry(geometry, filePath);
}


void Whiteboard::addNewTask(const QString& text) {
    QListWidgetItem *newItem = new QListWidgetItem("");
    newItem->setSizeHint(QSize(0, 62));
    newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);
    taskList->setWordWrap(true);
    taskList->addItem(newItem);

    QWidget *taskWidget = new QWidget();
    QHBoxLayout *taskLayout = new QHBoxLayout(taskWidget);

    // 创建大圆点
    QToolButton *bulletLabel = new QToolButton;
//    bulletLabel->setText("*");
//    QFont bulletFont;
//    bulletFont.setPixelSize(19);  // 设置大圆点的大小
//    bulletLabel->setFont(bulletFont);
//    bulletLabel->setStyleSheet("margin-top: +30px;");
//    bulletLabel->setAlignment(Qt::AlignTop);  // 顶部对齐，避免多行时不居中
//    bulletLabel->setFrameShape(QFrame::NoFrame);
//    bulletLabel->setStyleSheet("border:none;"); // 无边框
    bulletLabel->setStyleSheet("QToolButton{background-color:#c2c2c2;border-radius:9px}\nQToolButton:hover {background-color:#cb4042;border-radius:9px;}\nQToolButton:pressed {background-color:#ab3b3a;border-radius:9px;}");

    bulletLabel->setFixedSize(22, 22);

    // 将按钮放入一个布局管理器中，并设置对齐方式
    QVBoxLayout *buttonLayout = new QVBoxLayout();
    buttonLayout->addWidget(bulletLabel);
    buttonLayout->setAlignment(bulletLabel, Qt::AlignTop);
    buttonLayout->setContentsMargins(0,6,0,0);

    // 文本输入框
    CustomTextEdit* editor = createMultiLineEditor(newItem);
    if(!text.isEmpty()){
        editor->setText(text);
        editor->adjustSize();
        qDebug() << "text: " << text;
        editor->clearFocus();

        // 动态调整
        QSize documentSize = editor->document()->size().toSize();
        int contentHeight = documentSize.height();

        if (contentHeight == 0){
            contentHeight = editor->fontMetrics().height();
        }

        int paddingHeight = editor->contentsMargins().top() + editor->contentsMargins().bottom();
        int actualHeight = contentHeight + paddingHeight; //  这里不需要再加行高，因为documentSize已经包含了
        editor->setFixedHeight(actualHeight+15);
        newItem->setSizeHint(QSize(editor->width(), actualHeight + 40)); // 根据内容调整item高度
        qDebug() << "初始化动态调整actualHeight" << actualHeight;
    }

    // 监听输入变化调整大小
    connect(editor, &CustomTextEdit::textChanged, [editor, newItem]() {
       qDebug() << "加载数据输入框变化" ;
       QSize documentSize = editor->document()->size().toSize();
       editor->setFixedHeight(documentSize.height());  // 确保输入时高度自适应
       newItem->setSizeHint(QSize(editor->width(), documentSize.height() + 25));
       qDebug() << "监听大小" << "editor" << documentSize.height();
       qDebug() << "监听大小" << "editor->width()" << editor->width();
    });

    // 将圆点和输入框添加到布局
    taskLayout->addLayout(buttonLayout);
//    taskLayout->addWidget(bulletLabel);
    taskLayout->addWidget(editor,1);

    taskList->setItemWidget(newItem, taskWidget);

    connect(bulletLabel, &QToolButton::clicked, this, [this, newItem]()
    {
        int row = taskList->row(newItem);
        if (row > 0 || (row == 0&& taskList->count() > 1)) {
            // 从列表中取出该项（同时移除与之关联的部件）
            QListWidgetItem *removedItem = taskList->takeItem(row);
            // debug row
            qDebug() << "row: " << row ;
            // 删除该项，释放内存（注意：itemWidget() 关联的 QWidget 会一并被删除）
            delete removedItem;
        }
        else if(row == 0){
            QListWidgetItem *item = taskList->item(row); // 使用 item() 而不是 takeItem()
                    if (item) {
                        QWidget *widget = taskList->itemWidget(item);
                        if (widget) {
                            CustomTextEdit *editor = widget->findChild<CustomTextEdit*>();
                            if (editor) {
                                editor->setText(""); // 清空文本内容
                            }
                        }
                    }
        }
    });

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

