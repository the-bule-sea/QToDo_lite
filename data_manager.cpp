#include "data_manager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

// 保存数据到文件
void DataManager::saveData(const QList<TaskData>& tasks, const QString& filePath) {
    QJsonArray taskArray;
    for (const auto& task : tasks) {
        QJsonObject taskObject;
        taskObject["text"] = task.text;
        taskArray.append(taskObject);
    }

    QJsonObject dataObject;
    dataObject["tasks"] = taskArray;

    QJsonDocument document(dataObject);
    QByteArray jsonData = document.toJson();

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(jsonData);
        file.close();
    }
}

// 从文件加载数据
QList<TaskData> DataManager::loadData(const QString& filePath) {
    QList<TaskData> tasks;

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray jsonData = file.readAll();
        file.close();

        QJsonDocument document = QJsonDocument::fromJson(jsonData);
        QJsonObject dataObject = document.object();
        QJsonArray taskArray = dataObject["tasks"].toArray();

        for (const auto& taskValue : taskArray) {
            QJsonObject taskObject = taskValue.toObject();
            TaskData task;
            task.text = taskObject["text"].toString();
            tasks.append(task);
        }
    }

    return tasks;
}
