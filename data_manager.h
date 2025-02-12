#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <QString>
#include <QList>
#include <QStandardPaths>

// 结构体
struct TaskData {
    QString text;
};

class DataManager {
public:
    static void saveData(const QList<TaskData>& tasks, const QString& filePath);
    static QList<TaskData> loadData(const QString& filePath);
};

#endif // DATA_MANAGER_H
