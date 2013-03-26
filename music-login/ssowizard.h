#ifndef SSOWIZARD_H
#define SSOWIZARD_H

#include <QWidget>
#include <ssoservice.h>
#include "loadingoverlay.h"

using namespace SSO;

namespace Ui {
class SSOWizard;
}

#ifdef TESTS
class TestSSOWizard;
#endif

class SSOWizard : public QWidget
{
    Q_OBJECT

public:
    explicit SSOWizard(QWidget *parent = 0);
    ~SSOWizard();

signals:
    void aboutToClose();

protected:
    virtual void resizeEvent(QResizeEvent *);

private slots:
    void showRegisterPage(QString email, QString password);
    void loginAndBuy(QString email, QString password);
    void registerAndBuy(QString email, QString password, QString name);
    void showPageLogin();

    void accountAuthenticated();
    void sessionDetected();
    void serviceFailed(const ErrorResponse&);
    void loginFailed(const QString& message);

private:
    Ui::SSOWizard *ui;
    QString purchaseUrl;
    SSOService _service;
    LoadingOverlay* _overlay;

    QString cleanArgument(QString& arg);
    void showError(QString message);
    void hideError();
    void setHeader(QString artist, QString album, QString price, QString picture_path);

#ifdef TESTS
    friend class TestSSOWizard;
#endif
};

#endif // SSOWIZARD_H
