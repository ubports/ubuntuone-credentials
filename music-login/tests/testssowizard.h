#ifndef TESTSSOWIZARD_H
#define TESTSSOWIZARD_H

#include <QObject>
#include "ssowizard.h"

class TestSSOWizard : public QObject
{
    Q_OBJECT
public:
    explicit TestSSOWizard(QObject *parent = 0);

public slots:
    void receiveAboutToClose();

private slots:
    void testCleanArgument();
    void testShowError();
    void testHideError();
    void testShowRegisterPage();
    void testLoginAndBuy();
    void testRegisterAndBuy();
    void testShowPageLogin();
    void testAccountAuthenticated();
    void testServiceFailed();
    void testLoginFailed();
    void cleanup();

private:
    SSOWizard wizard;
    bool _aboutToCloseEmitted;

};

#endif // TESTSSOWIZARD_H
