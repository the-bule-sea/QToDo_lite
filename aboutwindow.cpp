#include "aboutwindow.h"
#include "ui_aboutwindow.h"
#include <QPixmap>
#include <QDesktopServices>
#include <QUrl>

AboutWindow::AboutWindow(QWidget *parent):
    QWidget(parent),
    ui(new Ui::AboutWindow)
{
    ui->setupUi(this);

    // 介绍页
    QPixmap pixmap("://todo_ico_v2.svg");
    ui->imageLabel->setPixmap(pixmap.scaled(100,100, Qt::KeepAspectRatio));
    ui->imageLabel->setStyleSheet("QLabel{ margin-top:30px }");
    ui->infoLabel->setText("QToDo-lite");
    ui->sourceButton->setStyleSheet("QPushButton {"
                                    "    background-color: #4285F4; "
                                    "    color: white; "
                                    "    border: none; "
                                    "    padding: 10px; "
                                    "    border-radius: 5px; "
                                    "}"
                                    "QPushButton:hover {"
                                    "   background-color: rgba(66, 133, 244, 50%);"
                                    "    border-radius: 5px;"
                                    "}"
                                    );
    connect(ui->sourceButton, &QPushButton::clicked, this, &AboutWindow::openSourceUrl);
}

AboutWindow::~AboutWindow()
{
    delete ui;
}

void AboutWindow::openSourceUrl()
{
    QUrl url("https://github.com/the-bule-sea/QToDo_lite");
    QDesktopServices::openUrl(url);
}
