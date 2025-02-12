#include "settingwindow.h"
#include "ui_settingwindow.h"
#include <QCoreApplication>
#include <QDebug>

#ifdef Q_OS_WIN32
#include <QSettings>
#else
#include <QFile>
#include <QDir>
#include <QTextStream>
#endif

// 应用常量名
const QString APP_NAME = "QToDoLite";

SettingWindow::SettingWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingWindow)
{
    ui->setupUi(this);

    connect(ui->autoStartCheckBox, &QCheckBox::clicked, this, [this](bool checked){
        setAutoStart(checked);
    });
    // 初始化时检查自启动状态并更新CheckBox
#ifdef Q_OS_WIN32
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::Registry32Format);
    bool isAutoStart = settings.contains(APP_NAME);
#else
    QString desktopFilePath = QDir::homePath() + "/.config/autostart/your_app_name.desktop";
    bool isAutoStart = QFile::exists(desktopFilePath);
#endif
    ui->autoStartCheckBox->setChecked(isAutoStart);
}

SettingWindow::~SettingWindow()
{
    delete ui;
}

void SettingWindow::setAutoStart(bool enable)
{
    QString appPath = QCoreApplication::applicationFilePath();

#ifdef Q_OS_WIN32
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::Registry32Format);
    if (enable) {
        settings.setValue(APP_NAME, appPath);
    } else {
        settings.remove(APP_NAME);
    }
#else // Q_OS_LINUX
    QString autostartDir = QDir::homePath() + "/.config/autostart";
    QString desktopFilePath = autostartDir + "/" + APP_NAME + ".desktop";

    QDir dir(autostartDir);
    if (!dir.exists()) {
            if (!dir.mkpath(autostartDir)) {  // Use mkpath to create parent directories if needed
                qWarning() << "Failed to create autostart directory:" << autostartDir;
                return; // Or handle the error as appropriate
            }
        }

    if (enable) {
        QFile file(desktopFilePath);
        if (file.open(QFile::WriteOnly | QFile::Text)) {
            QTextStream stream(&file);
            stream << "[Desktop Entry]" << endl;
            stream << "Type=Application" << endl;
            stream << "Name=" << APP_NAME << endl;
            stream << "Exec=" << appPath << endl;
            stream << "Hidden=false" << endl;
            stream << "Terminal=false" << endl;
            file.close();
        } else {
            qWarning() << "Failed to open desktop file for writing:" << desktopFilePath;
        }
    } else {
        if(!QFile::remove(desktopFilePath)) {
                    qWarning() << "Failed to remove desktop file:" << desktopFilePath;
                }
    }
#endif
}
