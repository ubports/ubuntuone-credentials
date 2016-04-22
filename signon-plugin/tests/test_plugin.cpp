/*
 * Copyright 2016 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of version 3 of the GNU Lesser General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPointer>
#include <QRegExp>
#include <QSignalSpy>
#include <QTimer>
#include <QtTest/QtTest>

#include <SignOn/uisessiondata_priv.h>

#include "ubuntuone-plugin.h"

using namespace SignOn;

namespace QTest {
template<>
char *toString(const QVariantMap &map)
{
    QJsonDocument doc(QJsonObject::fromVariantMap(map));
    return qstrdup(doc.toJson(QJsonDocument::Compact).data());
}
} // QTest namespace

class TestNetworkReply: public QNetworkReply
{
    Q_OBJECT

public:
    TestNetworkReply(QObject *parent = 0):
        QNetworkReply(parent),
        m_offset(0)
    {}

    void setError(NetworkError errorCode, const QString &errorString,
                  int delay = -1) {
        QNetworkReply::setError(errorCode, errorString);
        if (delay > 0) {
            QTimer::singleShot(delay, this, SLOT(fail()));
        }
    }

    void setRawHeader(const QByteArray &headerName, const QByteArray &value) {
        QNetworkReply::setRawHeader(headerName, value);
    }

    void setContentType(const QString &contentType) {
        setRawHeader("Content-Type", contentType.toUtf8());
    }

    void setStatusCode(int statusCode) {
        setAttribute(QNetworkRequest::HttpStatusCodeAttribute, statusCode);
    }

    void setContent(const QByteArray &content) {
        m_content = content;
        m_offset = 0;

        open(ReadOnly | Unbuffered);
        setHeader(QNetworkRequest::ContentLengthHeader, QVariant(content.size()));
    }

    void start() {
        QTimer::singleShot(0, this, SIGNAL(readyRead()));
        QTimer::singleShot(10, this, SLOT(finish()));
    }

public Q_SLOTS:
    void finish() { setFinished(true); Q_EMIT finished(); }
    void fail() { Q_EMIT error(error()); }

protected:
    void abort() Q_DECL_OVERRIDE {}
    qint64 bytesAvailable() const Q_DECL_OVERRIDE {
        return m_content.size() - m_offset + QIODevice::bytesAvailable();
    }

    bool isSequential() const Q_DECL_OVERRIDE { return true; }
    qint64 readData(char *data, qint64 maxSize) Q_DECL_OVERRIDE {
        if (m_offset >= m_content.size())
            return -1;
        qint64 number = qMin(maxSize, m_content.size() - m_offset);
        memcpy(data, m_content.constData() + m_offset, number);
        m_offset += number;
        return number;
    }

private:
    QByteArray m_content;
    qint64 m_offset;
};

class TestNetworkAccessManager: public QNetworkAccessManager
{
    Q_OBJECT

public:
    TestNetworkAccessManager(): QNetworkAccessManager() {}

    void setNextReply(TestNetworkReply *reply) { m_nextReply = reply; }

protected:
    QNetworkReply *createRequest(Operation op, const QNetworkRequest &request,
                                 QIODevice *outgoingData = 0) Q_DECL_OVERRIDE {
        Q_UNUSED(op);
        m_lastRequest = request;
        m_lastRequestData = outgoingData->readAll();
        m_nextReply->start();
        return m_nextReply;
    }

public:
    QPointer<TestNetworkReply> m_nextReply;
    QNetworkRequest m_lastRequest;
    QByteArray m_lastRequestData;
};

class PluginTest: public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testLoading();
    void testInitialization();
    void testPluginType();
    void testPluginMechanisms();
    void testStoredToken_data();
    void testStoredToken();
    void testUserInteraction();
    void testTokenCreation_data();
    void testTokenCreation();

    void init();
    void cleanup();

private:
    UbuntuOne::SignOnPlugin *m_testPlugin;
};

void PluginTest::initTestCase()
{
    qRegisterMetaType<SignOn::SessionData>();
    qRegisterMetaType<SignOn::UiSessionData>();
    qRegisterMetaType<SignOn::Error>();
}

void PluginTest::cleanupTestCase()
{
}

// prepare each test by creating new plugin
void PluginTest::init()
{
    m_testPlugin = new UbuntuOne::SignOnPlugin();
}

// finish each test by deleting plugin
void PluginTest::cleanup()
{
    delete m_testPlugin;
    m_testPlugin = 0;
}

void PluginTest::testLoading()
{
    QLibrary module(PLUGIN_PATH);
    if (!module.load()) {
        qDebug() << "Failed to load module:" << module.errorString();
    }

    QVERIFY(module.isLoaded());
    typedef AuthPluginInterface *(*AuthPluginInstanceF)();
    auto instance =
        (AuthPluginInstanceF)module.resolve("auth_plugin_instance");
    QVERIFY(instance);

    auto plugin = qobject_cast<AuthPluginInterface *>(instance());
    QVERIFY(plugin);
}

void PluginTest::testInitialization()
{
    QVERIFY(m_testPlugin);
}

void PluginTest::testPluginType()
{
    QCOMPARE(m_testPlugin->type(), QString("ubuntuone"));
}

void PluginTest::testPluginMechanisms()
{
    QStringList mechs = m_testPlugin->mechanisms();
    QCOMPARE(mechs.count(), 1);
    QCOMPARE(mechs[0], QString("ubuntuone"));
}

void PluginTest::testStoredToken_data()
{
    QTest::addColumn<QVariantMap>("sessionData");
    QTest::addColumn<int>("networkError");
    QTest::addColumn<int>("httpStatus");
    QTest::addColumn<QString>("replyContents");
    QTest::addColumn<int>("expectedErrorCode");
    QTest::addColumn<bool>("uiExpected");
    QTest::addColumn<QVariantMap>("expectedResponse");
    QTest::addColumn<QVariantMap>("expectedStore");

    UbuntuOne::PluginData sessionData;
    UbuntuOne::PluginData response;
    UbuntuOne::PluginData stored;

    QTest::newRow("empty") <<
        sessionData.toMap() <<
        -1 << -1 << QString() <<
        int(Error::MissingData) <<
        false << QVariantMap() << QVariantMap();

    sessionData.setTokenName("helloworld");
    sessionData.setSecret("consumer_key=aAa&consumer_secret=bBb&name=helloworld&token=cCc&token_secret=dDd");
    response.setConsumerKey("aAa");
    response.setConsumerSecret("bBb");
    response.setTokenKey("cCc");
    response.setTokenSecret("dDd");
    QVariantMap storedData;
    storedData[sessionData.TokenName()] = response.toMap();
    stored.setStoredData(storedData);
    response.setTokenName(sessionData.TokenName());
    QTest::newRow("in secret, valid") <<
        sessionData.toMap() <<
        -1 <<
        200 << QString("{\n"
                       "  \"is_valid\": true,\n"
                       "  \"identifier\": \"64we8bn\",\n"
                       "  \"account_verified\": true\n"
                       "}") <<
        -1 <<
        false << response.toMap() << stored.toMap();
    sessionData = UbuntuOne::PluginData();
    response = UbuntuOne::PluginData();
    stored = UbuntuOne::PluginData();
    storedData.clear();

    sessionData.setTokenName("helloworld");
    sessionData.setSecret("consumer_key=aAa&consumer_secret=bBb&name=helloworld&token=cCc&token_secret=dDd");
    response.setConsumerKey("aAa");
    response.setConsumerSecret("bBb");
    response.setTokenKey("cCc");
    response.setTokenSecret("dDd");
    storedData[sessionData.TokenName()] = response.toMap();
    stored.setStoredData(storedData);
    response = UbuntuOne::PluginData();
    QTest::newRow("in secret, invalid") <<
        sessionData.toMap() <<
        -1 <<
        200 << QString("{\n"
                       "  \"is_valid\": false,\n"
                       "  \"identifier\": \"64we8bn\",\n"
                       "  \"account_verified\": true\n"
                       "}") <<
        -1 <<
        true << response.toMap() << stored.toMap();
    sessionData = UbuntuOne::PluginData();
    response = UbuntuOne::PluginData();
    stored = UbuntuOne::PluginData();
    storedData.clear();

    sessionData.setTokenName("helloworld");
    sessionData.setSecret("consumer_key=aAa&consumer_secret=bBb&name=helloworld&token=cCc&token_secret=dDd");
    response.setConsumerKey("aAa");
    response.setConsumerSecret("bBb");
    response.setTokenKey("cCc");
    response.setTokenSecret("dDd");
    storedData[sessionData.TokenName()] = response.toMap();
    stored.setStoredData(storedData);
    response = UbuntuOne::PluginData();
    QTest::newRow("in secret, network error") <<
        sessionData.toMap() <<
        int(QNetworkReply::SslHandshakeFailedError) <<
        -1 << QString() <<
        int(SignOn::Error::Ssl) <<
        true << response.toMap() << stored.toMap();
    sessionData = UbuntuOne::PluginData();
    response = UbuntuOne::PluginData();
    stored = UbuntuOne::PluginData();
    storedData.clear();
}

void PluginTest::testStoredToken()
{
    QFETCH(QVariantMap, sessionData);
    QFETCH(int, httpStatus);
    QFETCH(int, networkError);
    QFETCH(QString, replyContents);
    QFETCH(int, expectedErrorCode);
    QFETCH(bool, uiExpected);
    QFETCH(QVariantMap, expectedResponse);
    QFETCH(QVariantMap, expectedStore);

    QSignalSpy result(m_testPlugin, SIGNAL(result(const SignOn::SessionData&)));
    QSignalSpy error(m_testPlugin, SIGNAL(error(const SignOn::Error &)));
    QSignalSpy userActionRequired(m_testPlugin,
                                  SIGNAL(userActionRequired(const SignOn::UiSessionData&)));
    QSignalSpy store(m_testPlugin, SIGNAL(store(const SignOn::SessionData&)));

    /* Prepare network reply */
    TestNetworkAccessManager *nam = new TestNetworkAccessManager;
    m_testPlugin->m_networkAccessManager = nam;
    TestNetworkReply *reply = new TestNetworkReply(this);
    if (httpStatus > 0) {
        reply->setStatusCode(httpStatus);
    } else {
        reply->setError(QNetworkReply::NetworkError(networkError),
                        "Network error");
    }
    reply->setContent(replyContents.toUtf8());
    nam->setNextReply(reply);


    m_testPlugin->process(sessionData, "ubuntuone");
    if (expectedErrorCode < 0) {
        QCOMPARE(error.count(), 0);
        QTRY_COMPARE(userActionRequired.count(), uiExpected ? 1 : 0);
        if (!expectedResponse.isEmpty()) {
            QTRY_COMPARE(result.count(), 1);
            QVariantMap resp = result.at(0).at(0).value<SessionData>().toMap();
            QCOMPARE(resp, expectedResponse);
        } else {
            QCOMPARE(result.count(), 0);
        }

        if (!expectedStore.isEmpty()) {
            QCOMPARE(store.count(), 1);
            QVariantMap storedData =
                store.at(0).at(0).value<SessionData>().toMap();
            QCOMPARE(storedData, expectedStore);
        } else {
            QCOMPARE(store.count(), 0);
        }
    } else {
        QTRY_COMPARE(error.count(), 1);
        Error err = error.at(0).at(0).value<Error>();
        QCOMPARE(err.type(), expectedErrorCode);
    }
}

void PluginTest::testUserInteraction()
{
    QSignalSpy result(m_testPlugin, SIGNAL(result(const SignOn::SessionData&)));
    QSignalSpy error(m_testPlugin, SIGNAL(error(const SignOn::Error &)));
    QSignalSpy userActionRequired(m_testPlugin,
                                  SIGNAL(userActionRequired(const SignOn::UiSessionData&)));
    QSignalSpy store(m_testPlugin, SIGNAL(store(const SignOn::SessionData&)));

    TestNetworkAccessManager *nam = new TestNetworkAccessManager;
    m_testPlugin->m_networkAccessManager = nam;

    UbuntuOne::PluginData sessionData;
    sessionData.setTokenName("helloworld");
    sessionData.setUserName("tom@example.com");
    m_testPlugin->process(sessionData, "ubuntuone");

    QTRY_COMPARE(userActionRequired.count(), 1);
    QVariantMap data =
        userActionRequired.at(0).at(0).value<UiSessionData>().toMap();
    QVariantMap expectedUserInteraction;
    expectedUserInteraction[SSOUI_KEY_QUERYUSERNAME] = true;
    expectedUserInteraction[SSOUI_KEY_USERNAME] = "tom@example.com";
    expectedUserInteraction[SSOUI_KEY_QUERYPASSWORD] = true;
    QCOMPARE(data, expectedUserInteraction);
    userActionRequired.clear();

    /* Prepare network reply */
    TestNetworkReply *reply = new TestNetworkReply(this);
    reply->setStatusCode(401);
    reply->setContent("{\n"
                      "  \"code\": \"TWOFACTOR_REQUIRED\",\n"
                      "  \"message\": \"This account requires 2-factor authentication.\",\n"
                      "  \"extra\": {}\n"
                      "}");
    nam->setNextReply(reply);

    QVariantMap userReply;
    userReply[SSOUI_KEY_USERNAME] = "tom@example.com";
    userReply[SSOUI_KEY_PASSWORD] = "s3cr3t";
    m_testPlugin->userActionFinished(userReply);

    /* Again the plugin should request user interaction, as OTP is required */
    QTRY_COMPARE(userActionRequired.count(), 1);
    data = userActionRequired.at(0).at(0).value<UiSessionData>().toMap();
    expectedUserInteraction.clear();
    expectedUserInteraction[SSOUI_KEY_USERNAME] = "tom@example.com";
    expectedUserInteraction[SSOUI_KEY_PASSWORD] = "s3cr3t";
    expectedUserInteraction[SSOUI_KEY_QUERY2FA] = true;
    /* We want the map to contain the SSOUI_KEY_2FA_TEXT, but we don't care
     * about the value */
    QVERIFY(data.contains(SSOUI_KEY_2FA_TEXT));
    data.remove(SSOUI_KEY_2FA_TEXT);
    QCOMPARE(data, expectedUserInteraction);
}

void PluginTest::testTokenCreation_data()
{
    QTest::addColumn<QVariantMap>("sessionData");
    QTest::addColumn<int>("networkError");
    QTest::addColumn<int>("httpStatus");
    QTest::addColumn<QString>("replyContents");
    QTest::addColumn<int>("expectedErrorCode");
    QTest::addColumn<QVariantMap>("expectedResponse");
    QTest::addColumn<QVariantMap>("expectedStore");
    QTest::addColumn<QVariantMap>("expectedUserInteraction");

    UbuntuOne::PluginData sessionData;
    UbuntuOne::PluginData response;
    UbuntuOne::PluginData stored;
    QVariantMap userInteraction;

    // Successful creation, with password only
    sessionData.setTokenName("helloworld");
    sessionData.setUserName("jim@example.com");
    sessionData.setSecret("s3cr3t");
    response.setConsumerKey("aAa");
    response.setConsumerSecret("bBb");
    response.setTokenKey("cCc");
    response.setTokenSecret("dDd");
    QVariantMap storedData;
    storedData[sessionData.TokenName()] = response.toMap();
    stored.setStoredData(storedData);
    response.setTokenName(sessionData.TokenName());
    QTest::newRow("no OTP needed, 201") <<
        sessionData.toMap() <<
        -1 <<
        201 << QString("{\n"
                       "  \"href\": \"https://login.ubuntu.com/api/v2/tokens/oauth/the-key\",\n"
                       "  \"token_key\": \"cCc\",\n"
                       "  \"token_secret\": \"dDd\",\n"
                       "  \"token_name\": \"helloworld\",\n"
                       "  \"consumer_key\": \"aAa\",\n"
                       "  \"consumer_secret\": \"bBb\",\n"
                       "  \"date_created\": \"2013-01-11 12:43:23\",\n"
                       "  \"date_updated\": \"2013-01-11 12:43:23\"\n"
                       "}") <<
        -1 <<
        response.toMap() << stored.toMap() << userInteraction;
    sessionData = UbuntuOne::PluginData();
    response = UbuntuOne::PluginData();
    stored = UbuntuOne::PluginData();
    storedData.clear();

    // Wrong password
    sessionData.setTokenName("helloworld");
    sessionData.setUserName("jim@example.com");
    sessionData.setSecret("s3cr3t");
    userInteraction[SSOUI_KEY_QUERYUSERNAME] = true;
    userInteraction[SSOUI_KEY_USERNAME] = "jim@example.com";
    userInteraction[SSOUI_KEY_QUERYPASSWORD] = true;
    QTest::newRow("wrong password") <<
        sessionData.toMap() <<
        -1 <<
        401 << QString("{\n"
                       "  \"code\": \"INVALID_CREDENTIALS\",\n"
                       "  \"message\": \"Wrong password!\",\n"
                       "  \"extra\": {}\n"
                       "}") <<
        -1 <<
        response.toMap() << stored.toMap() << userInteraction;
    sessionData = UbuntuOne::PluginData();
    userInteraction.clear();

    // Network error while creating token
    sessionData.setTokenName("helloworld");
    sessionData.setUserName("jim@example.com");
    sessionData.setSecret("s3cr3t");
    QTest::newRow("network error") <<
        sessionData.toMap() <<
        int(QNetworkReply::SslHandshakeFailedError) <<
        -1 << QString() <<
        int(SignOn::Error::Ssl) <<
        response.toMap() << stored.toMap() << userInteraction;
    sessionData = UbuntuOne::PluginData();

    // Account needs reset
    sessionData.setTokenName("helloworld");
    sessionData.setUserName("jim@example.com");
    sessionData.setSecret("s3cr3t");
    userInteraction[SSOUI_KEY_OPENURL] = "http://www.example.com/reset";
    QTest::newRow("reset needed") <<
        sessionData.toMap() <<
        -1 <<
        403 << QString("{\n"
                       "  \"code\": \"PASSWORD_POLICY_ERROR\",\n"
                       "  \"message\": \"Password too short\",\n"
                       "  \"extra\": {\n"
                       "    \"location\": \"http://www.example.com/reset\"\n"
                       "  }\n"
                       "}") <<
        -1 <<
        response.toMap() << stored.toMap() << userInteraction;
    sessionData = UbuntuOne::PluginData();
    userInteraction.clear();
}

void PluginTest::testTokenCreation()
{
    QFETCH(QVariantMap, sessionData);
    QFETCH(int, httpStatus);
    QFETCH(int, networkError);
    QFETCH(QString, replyContents);
    QFETCH(int, expectedErrorCode);
    QFETCH(QVariantMap, expectedResponse);
    QFETCH(QVariantMap, expectedStore);
    QFETCH(QVariantMap, expectedUserInteraction);

    QSignalSpy result(m_testPlugin, SIGNAL(result(const SignOn::SessionData&)));
    QSignalSpy error(m_testPlugin, SIGNAL(error(const SignOn::Error &)));
    QSignalSpy userActionRequired(m_testPlugin,
                                  SIGNAL(userActionRequired(const SignOn::UiSessionData&)));
    QSignalSpy store(m_testPlugin, SIGNAL(store(const SignOn::SessionData&)));

    /* Prepare network reply */
    TestNetworkAccessManager *nam = new TestNetworkAccessManager;
    m_testPlugin->m_networkAccessManager = nam;
    TestNetworkReply *reply = new TestNetworkReply(this);
    if (httpStatus > 0) {
        reply->setStatusCode(httpStatus);
    } else {
        reply->setError(QNetworkReply::NetworkError(networkError),
                        "Network error");
    }
    reply->setContent(replyContents.toUtf8());
    nam->setNextReply(reply);


    m_testPlugin->process(sessionData, "ubuntuone");
    if (expectedErrorCode < 0) {
        if (!expectedUserInteraction.isEmpty()) {
            QTRY_COMPARE(userActionRequired.count(), 1);
            QVariantMap data =
                userActionRequired.at(0).at(0).value<UiSessionData>().toMap();
            QCOMPARE(data, expectedUserInteraction);
        } else {
            QCOMPARE(userActionRequired.count(), 0);
        }

        if (!expectedResponse.isEmpty()) {
            QTRY_COMPARE(result.count(), 1);
            QVariantMap resp = result.at(0).at(0).value<SessionData>().toMap();
            QCOMPARE(resp, expectedResponse);
        } else {
            QCOMPARE(result.count(), 0);
        }

        if (!expectedStore.isEmpty()) {
            QCOMPARE(store.count(), 1);
            QVariantMap storedData =
                store.at(0).at(0).value<SessionData>().toMap();
            QCOMPARE(storedData, expectedStore);
        } else {
            QCOMPARE(store.count(), 0);
        }

        QCOMPARE(error.count(), 0);
    } else {
        QTRY_COMPARE(error.count(), 1);
        Error err = error.at(0).at(0).value<Error>();
        QCOMPARE(err.type(), expectedErrorCode);
    }
}

QTEST_MAIN(PluginTest)
#include "test_plugin.moc"
