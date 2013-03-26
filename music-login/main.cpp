#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QIcon>
#include <QString>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setWindowIcon(QIcon::fromTheme("ubuntuone"));

    // Set Stylesheet
    QFile file(":/qss/stylesheet");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    a.setStyleSheet(styleSheet);

    MainWindow w;
    w.show();

    return a.exec();
}
