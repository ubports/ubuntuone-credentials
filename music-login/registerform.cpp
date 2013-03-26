#include "registerform.h"
#include "ui_registerform.h"

RegisterForm::RegisterForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegisterForm)
{
    ui->setupUi(this);
    this->_sessionActive = false;
    this->ui->lblNameError->setVisible(false);
    this->ui->lblPasswordError->setVisible(false);
    this->ui->lblConfirmError->setVisible(false);
    this->ui->btnCheckout->setDefault(true);

    QObject::connect(this->ui->lineEmail, SIGNAL(textChanged(QString)),
                     this, SLOT(validateForm()));
    QObject::connect(this->ui->lineName, SIGNAL(textChanged(QString)),
                     this, SLOT(validateForm()));
    QObject::connect(this->ui->linePassword, SIGNAL(textChanged(QString)),
                     this, SLOT(validateForm()));
    QObject::connect(this->ui->lineConfirm, SIGNAL(textChanged(QString)),
                     this, SLOT(validateForm()));
    QObject::connect(this->ui->lineName, SIGNAL(editingFinished()),
                     this, SLOT(showNameWarning()));
    QObject::connect(this->ui->linePassword, SIGNAL(editingFinished()),
                     this, SLOT(showPasswordWarning()));
    QObject::connect(this->ui->lineConfirm, SIGNAL(editingFinished()),
                     this, SLOT(showConfirmWarning()));
}

RegisterForm::~RegisterForm()
{
    delete ui;
}

void RegisterForm::setEmail(QString email)
{
    this->ui->lineEmail->setText(email);
}

void RegisterForm::setPassword(QString password)
{
    this->ui->linePassword->setText(password);
}

void RegisterForm::on_btnBack_clicked()
{
    emit this->goBack();
}

void RegisterForm::validateForm()
{
    bool value = (this->checkEmail() && this->checkName() &&
                  this->checkPassword() && this->checkConfirmPassword() &&
                  this->ui->checkBox->isChecked() && this->_sessionActive);
    this->ui->btnCheckout->setEnabled(value);
}

bool RegisterForm::checkEmail()
{
    return !this->ui->lineEmail->text().isEmpty() && this->ui->lineEmail->text().contains("@");
}

bool RegisterForm::checkPassword()
{
    bool value = this->ui->linePassword->text().length() > 7;
//    value = value && this->ui->linePassword->text().contains(QRegExp("[A-Z]"));
//    value = value && this->ui->linePassword->text().contains(QRegExp("[0-9]"));

    return value;
}

bool RegisterForm::checkName()
{
    return !this->ui->lineName->text().isEmpty();
}

bool RegisterForm::checkConfirmPassword()
{
    return this->ui->linePassword->text() == this->ui->lineConfirm->text();
}

void RegisterForm::on_btnCheckout_clicked()
{
    QString name = this->ui->lineName->text();
    QString email = this->ui->lineEmail->text();
    QString password = this->ui->linePassword->text();
    emit this->registerCheckout(email, password, name);
}

void RegisterForm::setSessionState(bool value)
{
    this->_sessionActive = value;
}

void RegisterForm::on_lineEmail_returnPressed()
{
    this->ui->lineName->setFocus();
}

void RegisterForm::on_lineName_returnPressed()
{
    this->ui->linePassword->setFocus();
}

void RegisterForm::on_linePassword_returnPressed()
{
    this->ui->lineConfirm->setFocus();
}

void RegisterForm::on_lineConfirm_returnPressed()
{
    this->ui->btnCheckout->click();
}

void RegisterForm::showNameWarning()
{
    this->ui->lblNameError->setVisible(!this->checkName());
}

void RegisterForm::showPasswordWarning()
{
    this->ui->lblPasswordError->setVisible(!this->checkPassword());
}

void RegisterForm::showConfirmWarning()
{
    this->ui->lblConfirmError->setVisible(!this->checkConfirmPassword());
}
