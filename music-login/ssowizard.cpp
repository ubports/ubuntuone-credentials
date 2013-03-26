#include "ssowizard.h"
#include "ui_ssowizard.h"
#include <QStringList>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QResizeEvent>

SSOWizard::SSOWizard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SSOWizard)
{
    ui->setupUi(this);
    this->_overlay = new LoadingOverlay(this);
    this->_overlay->hide();
    this->ui->lblError->setEnabled(false);

    QStringList cmdline_args = QCoreApplication::arguments();

    QString item;
    QString album;
    QString artist;
    QString price;
    QString picture;
    foreach(item, cmdline_args){
        if(item.startsWith("--album=")){
            item.remove(0, 8);
            album = this->cleanArgument(item);
        }else if(item.startsWith("--artist=")){
            item.remove(0, 9);
            artist = this->cleanArgument(item);
        }else if(item.startsWith("--price=")){
            item.remove(0, 8);
            price = this->cleanArgument(item);
        }else if(item.startsWith("--picture=")){
            item.remove(0, 10);
            picture = this->cleanArgument(item);
        }else if(item.startsWith("--url=")){
            item.remove(0, 6);
            this->purchaseUrl = this->cleanArgument(item);
        }
    }

    this->setHeader(artist, album, price, picture);

    // Connect signals
    QObject::connect(&(this->_service), SIGNAL(sessionActivated()), this, SLOT(sessionDetected()));
    this->_service.init_service();
    QObject::connect(&(this->_service), SIGNAL(credentialsFound(QString,QString,QString,QString,QString)),
                     this, SLOT(accountAuthenticated()));
    QObject::connect(&(this->_service), SIGNAL(requestFailed(const ErrorResponse&)),
                     this, SLOT(serviceFailed(const ErrorResponse&)));
    QObject::connect(&(this->_service), SIGNAL(loginFailed(QString)),
                     this, SLOT(loginFailed(const QString&)));
    QObject::connect(this->ui->pageLogin, SIGNAL(newCustomerSelected(QString,QString)),
                     this, SLOT(showRegisterPage(QString,QString)));
    QObject::connect(this->ui->pageLogin, SIGNAL(loginCheckout(QString,QString)),
                     this, SLOT(loginAndBuy(QString,QString)));
    QObject::connect(this->ui->pageRegister, SIGNAL(goBack()),
                     this, SLOT(showPageLogin()));
    QObject::connect(this->ui->pageRegister, SIGNAL(registerCheckout(QString,QString,QString)),
                     this, SLOT(registerAndBuy(QString, QString, QString)));
}

QString SSOWizard::cleanArgument(QString& arg)
{
    if(arg.startsWith("\"")){
        arg.remove(0, 1);
    }
    if(arg.endsWith("\"")){
        arg.chop(1);
    }

    return arg;
}

SSOWizard::~SSOWizard()
{
    delete ui;
}

void SSOWizard::loginFailed(const QString& message)
{
    this->_overlay->hide();
    this->showError(message);
}

void SSOWizard::showRegisterPage(QString email, QString password)
{
    this->ui->pageRegister->setEmail(email);
    this->ui->pageRegister->setPassword(password);

    this->ui->stackedWidget->setCurrentIndex(1);
}

void SSOWizard::loginAndBuy(QString email, QString password)
{
    this->hideError();
    this->_overlay->show();
    this->_service.login(email, password);
}

void SSOWizard::accountAuthenticated()
{
    QDesktopServices::openUrl(this->purchaseUrl);
    this->_overlay->hide();
    emit this->aboutToClose();
}

void SSOWizard::registerAndBuy(QString email, QString password, QString name)
{
    this->hideError();
    this->_overlay->show();
    this->_service.registerUser(email, password, name);
}

void SSOWizard::showPageLogin()
{
    this->ui->stackedWidget->setCurrentIndex(0);
}

void SSOWizard::sessionDetected()
{
    this->ui->pageLogin->setSessionState(true);
    this->ui->pageRegister->setSessionState(true);
}

void SSOWizard::serviceFailed(const ErrorResponse& error)
{
    this->_overlay->hide();
    this->showError(error.message());
}

void SSOWizard::showError(QString message)
{
    this->ui->lblError->setText(message);
    this->ui->lblError->setEnabled(true);
}

void SSOWizard::hideError()
{
    this->ui->lblError->setText("");
    this->ui->lblError->setEnabled(false);
}

void SSOWizard::setHeader(QString artist, QString album, QString price, QString picture_path)
{
    this->ui->header->setAlbum(album);
    this->ui->header->setArtist(artist);
    this->ui->header->setPrice(price);
    this->ui->header->setPicture(picture_path);
}

void SSOWizard::resizeEvent(QResizeEvent * event)
{
    QWidget::resizeEvent(event);
    this->_overlay->resize(event->size());
}
