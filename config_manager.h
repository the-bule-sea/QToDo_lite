#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <QString>
#include <QPoint>
#include <QSize>

struct WindowGeometry{
    QPoint topLeft;
    QSize size;
};

class ConfigManager{
public:
    static void saveWindowGeometry(const WindowGeometry& geometry, const QString& filePath);
        static WindowGeometry loadWindowGeometry(const QString& filePath);
};

#endif // CONFIG_MANAGER_H
