#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ssowizard.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    SSOWizard* wizard = qobject_cast<SSOWizard*>(this->centralWidget());
    QObject::connect(wizard, SIGNAL(aboutToClose()), this, SLOT(close()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
