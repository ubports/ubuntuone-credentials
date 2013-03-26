#include "loginform.h"
#include "ui_loginform.h"
#include <QRegExp>
#include <QDebug>

LoginForm::LoginForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginForm)
{
    ui->setupUi(this);
    this->_sessionActive = false;
    this->ui->lblEmailError->setVisible(false);
    this->ui->lblPasswordError->setVisible(false);
    this->ui->btnProceed->setDefault(true);

    // Connect Signals
    QObject::connect(this->ui->lineEmail, SIGNAL(textChanged(QString)), this, SLOT(validateForm()));
    QObject::connect(this->ui->linePassword, SIGNAL(textChanged(QString)), this, SLOT(validateForm()));
    QObject::connect(this->ui->lineEmail, SIGNAL(editingFinished()), this, SLOT(showEmailWarning()));
    QObject::connect(this->ui->linePassword, SIGNAL(editingFinished()), this, SLOT(showPasswordWarning()));
}

LoginForm::~LoginForm()
{
    delete ui;
}

void LoginForm::validateForm()
{
    bool value = this->checkEmail() && this->checkPassword() && this->_sessionActive;
    this->ui->btnProceed->setEnabled(value);
}

bool LoginForm::checkEmail()
{
    return !this->ui->lineEmail->text().isEmpty() && this->ui->lineEmail->text().contains("@");
}

bool LoginForm::checkPassword()
{
    bool value = this->ui->linePassword->text().length() > 7;
//    value = value && this->ui->linePassword->text().contains(QRegExp("[A-Z]"));
//    value = value && this->ui->linePassword->text().contains(QRegExp("[0-9]"));

    return value;
}

void LoginForm::on_btnProceed_clicked()
{
    if(this->ui->radioNewCustomer->isChecked()){
        emit this->newCustomerSelected(this->ui->lineEmail->text(), this->ui->linePassword->text());
    }else{
        emit this->loginCheckout(this->ui->lineEmail->text(), this->ui->linePassword->text());
    }
}

void LoginForm::setSessionState(bool value)
{
    this->_sessionActive = value;
}

void LoginForm::on_lineEmail_returnPressed()
{
    this->ui->linePassword->setFocus();
}

void LoginForm::on_linePassword_returnPressed()
{
    this->ui->btnProceed->click();
}

void LoginForm::showEmailWarning()
{
    this->ui->lblEmailError->setVisible(!this->checkEmail());
}

void LoginForm::showPasswordWarning()
{
    this->ui->lblPasswordError->setVisible(!this->checkPassword());
}
