#include "config_manager.h"
#include <QFile>
#include <QDataStream>
#include <QDebug>

void ConfigManager::saveWindowGeometry(const WindowGeometry& geometry, const QString& filePath) {
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream stream(&file);
        stream << geometry.topLeft << geometry.size;
        file.close();
        qDebug() << "Window geometry saved to" << filePath;
    } else {
        qDebug() << "Failed to open file for saving:" << filePath;
    }
}

WindowGeometry ConfigManager::loadWindowGeometry(const QString& filePath) {
    WindowGeometry geometry;
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream stream(&file);
        stream >> geometry.topLeft >> geometry.size;
        file.close();
        qDebug() << "Window geometry loaded from" << filePath;
    } else {
        qDebug() << "Failed to open file for loading:" << filePath;
        // 返回默认值，例如窗口位置(0, 0)，大小(800, 600)
        geometry.topLeft = QPoint(0, 0);
        geometry.size = QSize(800, 600);
    }
    return geometry;
}
