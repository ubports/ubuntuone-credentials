#include <QApplication>
#include <QtTest/QtTest>
#include "testheader.h"
#include "testloginform.h"
#include "testregisterform.h"
#include "testssowizard.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    int result = 0;

    TestHeader test_header;
    TestLoginForm test_login_form;
    TestRegisterForm test_register_form;
    TestSSOWizard test_sso_wizard;

    result += QTest::qExec(&test_header);
    result += QTest::qExec(&test_login_form);
    result += QTest::qExec(&test_register_form);
    result += QTest::qExec(&test_sso_wizard);

//    return a.exec();
    return result;
}
