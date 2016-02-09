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

    void testInitialization();
    void testPluginType();
    void testPluginMechanisms();
    void testStoredToken_data();
    void testStoredToken();
    void testUserInteraction();

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

#if 0
void PluginTest::testPluginHmacSha1Process_data()
{
    QTest::addColumn<QString>("mechanism");
    QTest::addColumn<QVariantMap>("sessionData");
    QTest::addColumn<int>("replyStatusCode");
    QTest::addColumn<QString>("replyContentType");
    QTest::addColumn<QString>("replyContents");
    QTest::addColumn<int>("errorCode");
    QTest::addColumn<bool>("uiExpected");
    QTest::addColumn<QVariantMap>("response");
    QTest::addColumn<QVariantMap>("stored");

    OAuth1PluginData hmacSha1Data;
    hmacSha1Data.setRequestEndpoint("https://localhost/oauth/request_token");
    hmacSha1Data.setTokenEndpoint("https://localhost/oauth/access_token");
    hmacSha1Data.setAuthorizationEndpoint("https://localhost/oauth/authorize");
    hmacSha1Data.setCallback("https://localhost/connect/login_success.html");
    hmacSha1Data.setConsumerKey("104660106251471");
    hmacSha1Data.setConsumerSecret("fa28f40b5a1f8c1d5628963d880636fbkjkjkj");
    hmacSha1Data.setRealm("MyHost");

    QTest::newRow("invalid mechanism") <<
        "ANONYMOUS" <<
        hmacSha1Data.toMap() <<
        int(200) << "" << "" <<
        int(Error::MechanismNotAvailable) <<
        false << QVariantMap() << QVariantMap();

    // Try without params
    hmacSha1Data.setAuthorizationEndpoint(QString());
    QTest::newRow("without params, HMAC-SHA1") <<
        "HMAC-SHA1" <<
        hmacSha1Data.toMap() <<
        int(200) << "" << "" <<
        int(Error::MissingData) <<
        false << QVariantMap() << QVariantMap();

    // Check for signon UI request for HMAC-SHA1
    hmacSha1Data.setAuthorizationEndpoint("https://localhost/oauth/authorize");
    QTest::newRow("ui-request, HMAC-SHA1") <<
        "HMAC-SHA1" <<
        hmacSha1Data.toMap() <<
        int(200) << "text/plain" <<
        "oauth_token=HiThere&oauth_token_secret=BigSecret" <<
        -1 <<
        true << QVariantMap() << QVariantMap();

    QTest::newRow("ui-request, PLAINTEXT") <<
        "PLAINTEXT" <<
        hmacSha1Data.toMap() <<
        int(200) << "text/plain" <<
        "oauth_token=HiThere&oauth_token_secret=BigSecret" <<
        -1 <<
        true << QVariantMap() << QVariantMap();

    /* Now store some tokens and test the responses */
    hmacSha1Data.m_data.insert("UiPolicy", NoUserInteractionPolicy);
    QVariantMap tokens; // ConsumerKey to Token map
    QVariantMap token;
    token.insert("oauth_token", QLatin1String("hmactokenfromtest"));
    token.insert("oauth_token_secret", QLatin1String("hmacsecretfromtest"));
    token.insert("timestamp", QDateTime::currentDateTime().toTime_t());
    token.insert("Expiry", (uint)50000);
    tokens.insert(QLatin1String("invalidid"), QVariant::fromValue(token));
    hmacSha1Data.m_data.insert(QLatin1String("Tokens"), tokens);

    // Try without cached token for our ConsumerKey
    QTest::newRow("cached tokens, no ConsumerKey") <<
        "HMAC-SHA1" <<
        hmacSha1Data.toMap() <<
        int(200) << "text/plain" <<
        "oauth_token=HiThere&oauth_token_secret=BigSecret" <<
        -1 <<
        true << QVariantMap() << QVariantMap();

    // Ensure that the cached token is returned as required
    tokens.insert(hmacSha1Data.ConsumerKey(), QVariant::fromValue(token));
    hmacSha1Data.m_data.insert(QLatin1String("Tokens"), tokens);
    QVariantMap response;
    response.insert("AccessToken", QLatin1String("hmactokenfromtest"));
    QTest::newRow("cached tokens, with ConsumerKey") <<
        "HMAC-SHA1" <<
        hmacSha1Data.toMap() <<
        int(200) << "" << "" <<
        -1 <<
        false << response << QVariantMap();

    hmacSha1Data.m_data.insert("UiPolicy", RequestPasswordPolicy);
    QTest::newRow("cached tokens, request password policy") <<
        "HMAC-SHA1" <<
        hmacSha1Data.toMap() <<
        int(200) << "text/plain" <<
        "oauth_token=HiThere&oauth_token_secret=BigSecret" <<
        -1 <<
        true << QVariantMap() << QVariantMap();
    hmacSha1Data.m_data.remove("UiPolicy");

    hmacSha1Data.setForceTokenRefresh(true);
    QTest::newRow("cached tokens, force refresh") <<
        "HMAC-SHA1" <<
        hmacSha1Data.toMap() <<
        int(200) << "text/plain" <<
        "oauth_token=HiThere&oauth_token_secret=BigSecret" <<
        -1 <<
        true << QVariantMap() << QVariantMap();
    hmacSha1Data.setForceTokenRefresh(false);

    token.insert("timestamp", QDateTime::currentDateTime().toTime_t() - 50000);
    token.insert("Expiry", (uint)100);
    tokens.insert(hmacSha1Data.ConsumerKey(), QVariant::fromValue(token));
    hmacSha1Data.m_data.insert(QLatin1String("Tokens"), tokens);
    QTest::newRow("cached tokens, expired") <<
        "HMAC-SHA1" <<
        hmacSha1Data.toMap() <<
        int(200) << "text/plain" <<
        "oauth_token=HiThere&oauth_token_secret=BigSecret" <<
        -1 <<
        true << QVariantMap() << QVariantMap();

    /* test the ProvidedTokens semantics */
    OAuth1PluginData providedTokensHmacSha1Data;
    providedTokensHmacSha1Data.setRequestEndpoint("https://localhost/oauth/request_token");
    providedTokensHmacSha1Data.setTokenEndpoint("https://localhost/oauth/access_token");
    providedTokensHmacSha1Data.setAuthorizationEndpoint("https://localhost/oauth/authorize");
    providedTokensHmacSha1Data.setCallback("https://localhost/connect/login_success.html");
    providedTokensHmacSha1Data.setConsumerKey("104660106251471");
    providedTokensHmacSha1Data.setConsumerSecret("fa28f40b5a1f8c1d5628963d880636fbkjkjkj");
    QVariantMap providedTokens;
    providedTokens.insert("AccessToken", "providedhmactokenfromtest");
    providedTokens.insert("TokenSecret", "providedhmacsecretfromtest");
    providedTokens.insert("ScreenName", "providedhmacscreennamefromtest");
    providedTokens.insert("UserId", "providedUserId");

    // try providing tokens to be stored
    providedTokensHmacSha1Data.m_data.insert("ProvidedTokens", providedTokens);
    QVariantMap storedTokensForKey;
    storedTokensForKey.insert("oauth_token", providedTokens.value("AccessToken"));
    storedTokensForKey.insert("oauth_token_secret", providedTokens.value("TokenSecret"));
    QVariantMap storedTokens;
    storedTokens.insert(providedTokensHmacSha1Data.ConsumerKey(), storedTokensForKey);
    QVariantMap stored;
    stored.insert("Tokens", storedTokens);
    QTest::newRow("provided tokens") <<
        "HMAC-SHA1" <<
        providedTokensHmacSha1Data.toMap() <<
        int(200) << "" << "" <<
        -1 <<
        false << providedTokens << stored;

    QTest::newRow("http error 401") <<
        "HMAC-SHA1" <<
        hmacSha1Data.toMap() <<
        int(401) << "text/plain" <<
        "oauth_token=HiThere&oauth_token_secret=BigSecret" <<
        int(Error::OperationFailed) <<
        false << QVariantMap() << QVariantMap();

    QTest::newRow("no content returned") <<
        "HMAC-SHA1" <<
        hmacSha1Data.toMap() <<
        int(200) << "" << "" <<
        int(Error::OperationFailed) <<
        false << QVariantMap() << QVariantMap();

    QTest::newRow("no token returned") <<
        "HMAC-SHA1" <<
        hmacSha1Data.toMap() <<
        int(200) << "text/plain" <<
        "oauth_token=HiThere" <<
        int(Error::OperationFailed) <<
        false << QVariantMap() << QVariantMap();

    /* Test handling of oauth_problem; this is a non standard extension:
     * http://wiki.oauth.net/w/page/12238543/ProblemReporting
     * https://developer.yahoo.com/oauth/guide/oauth-errors.html
     */
    QTest::newRow("problem user_refused") <<
        "HMAC-SHA1" <<
        hmacSha1Data.toMap() <<
        int(400) << "text/plain" <<
        "oauth_problem=user_refused" <<
        int(Error::PermissionDenied) <<
        false << QVariantMap() << QVariantMap();
    QTest::newRow("problem permission_denied") <<
        "HMAC-SHA1" <<
        hmacSha1Data.toMap() <<
        int(400) << "text/plain" <<
        "oauth_problem=permission_denied" <<
        int(Error::PermissionDenied) <<
        false << QVariantMap() << QVariantMap();
    QTest::newRow("problem signature_invalid") <<
        "HMAC-SHA1" <<
        hmacSha1Data.toMap() <<
        int(400) << "text/plain" <<
        "oauth_problem=signature_invalid" <<
        int(Error::OperationFailed) <<
        false << QVariantMap() << QVariantMap();
}

void PluginTest::testPluginHmacSha1Process()
{
    QFETCH(QString, mechanism);
    QFETCH(QVariantMap, sessionData);
    QFETCH(int, replyStatusCode);
    QFETCH(QString, replyContentType);
    QFETCH(QString, replyContents);
    QFETCH(int, errorCode);
    QFETCH(bool, uiExpected);
    QFETCH(QVariantMap, response);
    QFETCH(QVariantMap, stored);

    QSignalSpy result(m_testPlugin, SIGNAL(result(const SignOn::SessionData&)));
    QSignalSpy error(m_testPlugin, SIGNAL(error(const SignOn::Error &)));
    QSignalSpy userActionRequired(m_testPlugin,
                                  SIGNAL(userActionRequired(const SignOn::UiSessionData&)));
    QSignalSpy store(m_testPlugin, SIGNAL(store(const SignOn::SessionData&)));

    TestNetworkAccessManager *nam = new TestNetworkAccessManager;
    m_testPlugin->m_networkAccessManager = nam;
    TestNetworkReply *reply = new TestNetworkReply(this);
    reply->setStatusCode(replyStatusCode);
    if (!replyContentType.isEmpty()) {
        reply->setContentType(replyContentType);
    }
    reply->setContent(replyContents.toUtf8());
    nam->setNextReply(reply);

    m_testPlugin->process(sessionData, mechanism);

    QTRY_COMPARE(result.count(), response.isEmpty() ? 0 : 1);
    /* In the test data sometimes we don't specify the expected stored data,
     * but this doesn't mean that store() shouldn't be emitted. */
    if (!stored.isEmpty()) { QTRY_COMPARE(store.count(), 1); }
    QTRY_COMPARE(userActionRequired.count(), uiExpected ? 1 : 0);
    QTRY_COMPARE(error.count(), errorCode < 0 ? 0 : 1);

    if (errorCode < 0) {
        QCOMPARE(error.count(), 0);

        QVariantMap resp = result.count() > 0 ?
            result.at(0).at(0).value<SessionData>().toMap() : QVariantMap();
        QVariantMap storedData = store.count() > 0 ?
            store.at(0).at(0).value<SessionData>().toMap() : QVariantMap();
        /* We don't check the network request if a response was received,
         * because a response can only be received if a cached token was
         * found -- and that doesn't cause any network request to be made. */
        if (resp.isEmpty()) {
            QCOMPARE(nam->m_lastRequest.url(),
                     sessionData.value("RequestEndpoint").toUrl());
            QVERIFY(nam->m_lastRequestData.isEmpty());

            /* Check the authorization header */
            QString authorizationHeader =
                QString::fromUtf8(nam->m_lastRequest.rawHeader("Authorization"));
            QStringList authorizationHeaderParts =
                authorizationHeader.split(QRegExp(",?\\s+"));
            QCOMPARE(authorizationHeaderParts[0], QString("OAuth"));

            /* The rest of the header should be a mapping, let's parse it */
            bool ok = true;
            QVariantMap authMap =
                parseAuthorizationHeader(authorizationHeaderParts.mid(1), &ok);
            QVERIFY(ok);
            QCOMPARE(authMap.value("oauth_signature_method").toString(), mechanism);
        }

        QVERIFY(mapIsSubset(response, resp));
        QVERIFY(mapIsSubset(stored, storedData));
    } else {
        Error err = error.at(0).at(0).value<Error>();
        QCOMPARE(err.type(), errorCode);
    }
}

void PluginTest::testPluginUseragentUserActionFinished()
{
    SignOn::UiSessionData info;
    PluginData data;
    data.setHost("https://localhost");
    data.setAuthPath("authorize");
    data.setTokenPath("access_token");
    data.setClientId("104660106251471");
    data.setClientSecret("fa28f40b5a1f8c1d5628963d880636fbkjkjkj");
    data.setRedirectUri("http://localhost/connect/login_success.html");
    QStringList scopes = QStringList() << "scope1" << "scope2";
    data.setScope(scopes);

    QSignalSpy resultSpy(m_testPlugin, SIGNAL(result(const SignOn::SessionData&)));
    QSignalSpy error(m_testPlugin, SIGNAL(error(const SignOn::Error &)));
    QSignalSpy userActionRequired(m_testPlugin,
                                  SIGNAL(userActionRequired(const SignOn::UiSessionData&)));
    QSignalSpy store(m_testPlugin, SIGNAL(store(const SignOn::SessionData&)));

    m_testPlugin->process(data, QString("user_agent"));

    QTRY_COMPARE(userActionRequired.count(), 1);
    QString state = parseState(userActionRequired);

    //empty data
    m_testPlugin->userActionFinished(info);
    QTRY_COMPARE(error.count(), 1);
    QCOMPARE(error.at(0).at(0).value<Error>().type(), int(Error::NotAuthorized));
    error.clear();

    //invalid data
    info.setUrlResponse(QString("http://www.facebook.com/connect/login_success.html#access_token=&expires_in=4776"));
    m_testPlugin->userActionFinished(info);
    QTRY_COMPARE(error.count(), 1);
    QCOMPARE(error.at(0).at(0).value<Error>().type(), int(Error::NotAuthorized));
    error.clear();

    //Invalid data
    info.setUrlResponse(QString("http://www.facebook.com/connect/login_success.html"));
    m_testPlugin->userActionFinished(info);
    QTRY_COMPARE(error.count(), 1);
    QCOMPARE(error.at(0).at(0).value<Error>().type(), int(Error::NotAuthorized));
    error.clear();

    // Wrong state
    info.setUrlResponse(QString("http://www.facebook.com/connect/login_success.html"
                                "#access_token=123&expires_in=456&state=%1").
                        arg(state + "Boo"));
    m_testPlugin->userActionFinished(info);
    QTRY_COMPARE(error.count(), 1);
    QCOMPARE(error.at(0).at(0).value<Error>().type(), int(Error::NotAuthorized));
    error.clear();

    //valid data
    info.setUrlResponse(QString("http://www.facebook.com/connect/login_success.html#access_token=testtoken.&expires_in=4776&state=%1").
                        arg(state));
    m_testPlugin->userActionFinished(info);
    QTRY_COMPARE(resultSpy.count(), 1);
    SessionData response = resultSpy.at(0).at(0).value<SessionData>();
    PluginTokenData result = response.data<PluginTokenData>();
    QCOMPARE(result.AccessToken(), QString("testtoken."));
    QCOMPARE(result.ExpiresIn(), 4776);
    QCOMPARE(result.Scope(), QStringList() << "scope1" << "scope2");
    resultSpy.clear();
    QTRY_COMPARE(store.count(), 1);
    SessionData storedData = store.at(0).at(0).value<SessionData>();
    QVariantMap storedTokenData = storedData.data<TokenData>().Tokens();
    QVariantMap storedClientData =
        storedTokenData.value(data.ClientId()).toMap();
    QVERIFY(!storedClientData.isEmpty());
    QCOMPARE(storedClientData["Scopes"].toStringList(), scopes);
    store.clear();

    //valid data, got scopes
    info.setUrlResponse(QString("http://www.facebook.com/connect/login_success.html#access_token=testtoken.&expires_in=4776&state=%1&scope=scope2").
                        arg(state));
    m_testPlugin->userActionFinished(info);
    QTRY_COMPARE(resultSpy.count(), 1);
    response = resultSpy.at(0).at(0).value<SessionData>();
    result = response.data<PluginTokenData>();
    QCOMPARE(result.AccessToken(), QString("testtoken."));
    QCOMPARE(result.ExpiresIn(), 4776);
    QCOMPARE(result.Scope(), QStringList() << "scope2");
    resultSpy.clear();
    store.clear();

    //valid data
    info.setUrlResponse(QString("http://www.facebook.com/connect/login_success.html"
                                "#state=%1&access_token=testtoken.").
                        arg(state));
    m_testPlugin->userActionFinished(info);
    QTRY_COMPARE(resultSpy.count(), 1);
    response = resultSpy.at(0).at(0).value<SessionData>();
    result = response.data<PluginTokenData>();
    QCOMPARE(result.AccessToken(), QString("testtoken."));
    QCOMPARE(result.ExpiresIn(), 0);
    resultSpy.clear();
    /* Check that the expiration time has not been stored, since the expiration
     * time was not given (https://bugs.launchpad.net/bugs/1316021)
     */
    QTRY_COMPARE(store.count(), 1);
    storedData = store.at(0).at(0).value<SessionData>();
    storedTokenData = storedData.data<TokenData>().Tokens();
    storedClientData = storedTokenData.value(data.ClientId()).toMap();
    QVERIFY(!storedClientData.isEmpty());
    QCOMPARE(storedClientData["Token"].toString(), QString("testtoken."));
    QVERIFY(!storedClientData.contains("Expiry"));
    store.clear();

    //Permission denied
    info.setUrlResponse(QString("http://www.facebook.com/connect/login_success.html?error=user_denied"));
    m_testPlugin->userActionFinished(info);
    QTRY_COMPARE(error.count(), 1);
    QCOMPARE(error.at(0).at(0).value<Error>().type(), int(Error::NotAuthorized));
    error.clear();
}

void PluginTest::testPluginWebserverUserActionFinished_data()
{
    QTest::addColumn<QString>("urlResponse");
    QTest::addColumn<int>("errorCode");
    QTest::addColumn<QString>("postUrl");
    QTest::addColumn<QString>("postContents");
    QTest::addColumn<bool>("disableStateParameter");
    QTest::addColumn<int>("replyStatusCode");
    QTest::addColumn<QString>("replyContentType");
    QTest::addColumn<QString>("replyContents");
    QTest::addColumn<QVariantMap>("response");

    QVariantMap response;

    QTest::newRow("empty data") <<
        "" <<
        int(Error::NotAuthorized) <<
        "" << "" << false << 0 << "" << "" << QVariantMap();

    QTest::newRow("no query data") <<
        "http://localhost/resp.html" <<
        int(Error::NotAuthorized) <<
        "" << "" << false << 0 << "" << "" << QVariantMap();

    QTest::newRow("permission denied") <<
        "http://localhost/resp.html?error=user_denied&$state" <<
        int(Error::NotAuthorized) <<
        "" << "" << false << 0 << "" << "" << QVariantMap();

    QTest::newRow("invalid data") <<
        "http://localhost/resp.html?sdsdsds=access.grant." <<
        int(Error::NotAuthorized) <<
        "" << "" << false << 0 << "" << "" << QVariantMap();

    QTest::newRow("reply code, http error 401") <<
        "http://localhost/resp.html?code=c0d3&$state" <<
        int(Error::OperationFailed) <<
        "https://localhost/access_token" <<
        "grant_type=authorization_code&code=c0d3&redirect_uri=http://localhost/resp.html" <<
        false <<
        int(401) <<
        "application/json" <<
        "something else" <<
        QVariantMap();

    QTest::newRow("reply code, empty reply") <<
        "http://localhost/resp.html?code=c0d3&$state" <<
        int(Error::NotAuthorized) <<
        "https://localhost/access_token" <<
        "grant_type=authorization_code&code=c0d3&redirect_uri=http://localhost/resp.html" <<
        false <<
        int(200) <<
        "application/json" <<
        "something else" <<
        QVariantMap();

    QTest::newRow("reply code, no access token") <<
        "http://localhost/resp.html?code=c0d3&$state" <<
        int(Error::NotAuthorized) <<
        "https://localhost/access_token" <<
        "grant_type=authorization_code&code=c0d3&redirect_uri=http://localhost/resp.html" <<
        false <<
        int(200) <<
        "application/json" <<
        "{ \"expires_in\": 3600 }" <<
        QVariantMap();

    QTest::newRow("reply code, no content type") <<
        "http://localhost/resp.html?code=c0d3&$state" <<
        int(Error::OperationFailed) <<
        "https://localhost/access_token" <<
        "grant_type=authorization_code&code=c0d3&redirect_uri=http://localhost/resp.html" <<
        false <<
        int(200) <<
        "" <<
        "something else" <<
        QVariantMap();

    QTest::newRow("reply code, unsupported content type") <<
        "http://localhost/resp.html?code=c0d3&$state" <<
        int(Error::OperationFailed) <<
        "https://localhost/access_token" <<
        "grant_type=authorization_code&code=c0d3&redirect_uri=http://localhost/resp.html" <<
        false <<
        int(200) <<
        "image/jpeg" <<
        "something else" <<
        QVariantMap();

    response.clear();
    response.insert("AccessToken", "t0k3n");
    response.insert("ExpiresIn", int(3600));
    response.insert("RefreshToken", QString());
    QTest::newRow("reply code, valid token, wrong state") <<
        "http://localhost/resp.html?code=c0d3&$wrongstate" <<
        int(Error::NotAuthorized) <<
        "" <<
        "" <<
        false <<
        int(200) <<
        "application/json" <<
        "{ \"access_token\":\"t0k3n\", \"expires_in\": 3600 }" <<
        response;

    response.clear();
    response.insert("AccessToken", "t0k3n");
    response.insert("ExpiresIn", int(3600));
    response.insert("RefreshToken", QString());
    response.insert("Scope", QStringList() << "one" << "two");
    QTest::newRow("reply code, valid token, wrong state ignored") <<
        "http://localhost/resp.html?code=c0d3&$wrongstate" <<
        int(-1) <<
        "https://localhost/access_token" <<
        "grant_type=authorization_code&code=c0d3&redirect_uri=http://localhost/resp.html" <<
        true <<
        int(200) <<
        "application/json" <<
        "{ \"access_token\":\"t0k3n\", \"expires_in\": 3600, "
        "\"scope\": \"one two\" }" <<
        response;

    response.clear();
    response.insert("AccessToken", "t0k3n");
    response.insert("ExpiresIn", int(3600));
    response.insert("RefreshToken", QString());
    response.insert("Scope", QStringList() << "one" << "two" << "three");
    QTest::newRow("reply code, valid token, no scope") <<
        "http://localhost/resp.html?code=c0d3&$state" <<
        int(-1) <<
        "https://localhost/access_token" <<
        "grant_type=authorization_code&code=c0d3&redirect_uri=http://localhost/resp.html" <<
        false <<
        int(200) <<
        "application/json" <<
        "{ \"access_token\":\"t0k3n\", \"expires_in\": 3600 }" <<
        response;

    response.clear();
    response.insert("AccessToken", "t0k3n");
    response.insert("ExpiresIn", int(3600));
    response.insert("RefreshToken", QString());
    response.insert("Scope", QStringList());
    QTest::newRow("reply code, valid token, empty scope") <<
        "http://localhost/resp.html?code=c0d3&$state" <<
        int(-1) <<
        "https://localhost/access_token" <<
        "grant_type=authorization_code&code=c0d3&redirect_uri=http://localhost/resp.html" <<
        false <<
        int(200) <<
        "application/json" <<
        "{ \"access_token\":\"t0k3n\", \"expires_in\": 3600, \"scope\": \"\" }" <<
        response;

    response.clear();
    response.insert("AccessToken", "t0k3n");
    response.insert("ExpiresIn", int(3600));
    response.insert("RefreshToken", QString());
    response.insert("Scope", QStringList() << "one" << "two");
    QTest::newRow("reply code, valid token, other scope") <<
        "http://localhost/resp.html?code=c0d3&$state" <<
        int(-1) <<
        "https://localhost/access_token" <<
        "grant_type=authorization_code&code=c0d3&redirect_uri=http://localhost/resp.html" <<
        false <<
        int(200) <<
        "application/json" <<
        "{ \"access_token\":\"t0k3n\", \"expires_in\": 3600, "
        "\"scope\": \"one two\" }" <<
        response;

    response.clear();
    QTest::newRow("reply code, facebook, no token") <<
        "http://localhost/resp.html?code=c0d3&$state" <<
        int(Error::NotAuthorized) <<
        "https://localhost/access_token" <<
        "grant_type=authorization_code&code=c0d3&redirect_uri=http://localhost/resp.html" <<
        false <<
        int(200) <<
        "text/plain" <<
        "expires=3600" <<
        response;

    response.clear();
    response.insert("AccessToken", "t0k3n");
    response.insert("ExpiresIn", int(3600));
    response.insert("RefreshToken", QString());
    response.insert("Scope", QStringList() << "one" << "two" << "three");
    QTest::newRow("reply code, facebook, valid token") <<
        "http://localhost/resp.html?code=c0d3&$state" <<
        int(-1) <<
        "https://localhost/access_token" <<
        "grant_type=authorization_code&code=c0d3&redirect_uri=http://localhost/resp.html" <<
        false <<
        int(200) <<
        "text/plain" <<
        "access_token=t0k3n&expires=3600" <<
        response;

    response.clear();
    response.insert("AccessToken", "t0k3n");
    response.insert("ExpiresIn", int(3600));
    response.insert("RefreshToken", QString());
    response.insert("Scope", QStringList() << "one" << "two" << "three");
    QTest::newRow("username-password, valid token") <<
        "http://localhost/resp.html?username=us3r&password=s3cr3t" <<
        int(-1) <<
        "https://localhost/access_token" <<
        "grant_type=user_basic&username=us3r&password=s3cr3t" <<
        false <<
        int(200) <<
        "application/json" <<
        "{ \"access_token\":\"t0k3n\", \"expires_in\": 3600 }" <<
        response;

    response.clear();
    response.insert("AccessToken", "t0k3n");
    response.insert("ExpiresIn", int(3600));
    response.insert("RefreshToken", QString());
    response.insert("Scope", QStringList() << "one" << "two" << "three");
    QTest::newRow("assertion, valid token") <<
        "http://localhost/resp.html?assertion_type=http://oauth.net/token/1.0"
        "&assertion=oauth1t0k3n" <<
        int(-1) <<
        "https://localhost/access_token" <<
        "grant_type=assertion&assertion_type=http://oauth.net/token/1.0&assertion=oauth1t0k3n" <<
        false <<
        int(200) <<
        "application/json" <<
        "{ \"access_token\":\"t0k3n\", \"expires_in\": 3600 }" <<
        response;

    response.clear();
    response.insert("AccessToken", "t0k3n");
    response.insert("ExpiresIn", int(3600));
    response.insert("RefreshToken", QString());
    response.insert("Scope", QStringList() << "one" << "two" << "three");
    QTest::newRow("username-password, valid token, wrong content type") <<
        "http://localhost/resp.html?username=us3r&password=s3cr3t" <<
        int(-1) <<
        "https://localhost/access_token" <<
        "grant_type=user_basic&username=us3r&password=s3cr3t" <<
        false <<
        int(200) <<
        "text/plain" <<
        "{ \"access_token\":\"t0k3n\", \"expires_in\": 3600 }" <<
        response;
}

void PluginTest::testPluginWebserverUserActionFinished()
{
    QFETCH(QString, urlResponse);
    QFETCH(int, errorCode);
    QFETCH(QString, postUrl);
    QFETCH(QString, postContents);
    QFETCH(bool, disableStateParameter);
    QFETCH(int, replyStatusCode);
    QFETCH(QString, replyContentType);
    QFETCH(QString, replyContents);
    QFETCH(QVariantMap, response);

    SignOn::UiSessionData info;
    PluginData data;
    data.setHost("localhost");
    data.setAuthPath("authorize");
    data.setTokenPath("access_token");
    data.setClientId("104660106251471");
    data.setClientSecret("fa28f40b5a1f8c1d5628963d880636fbkjkjkj");
    data.setRedirectUri("http://localhost/resp.html");
    data.setScope(QStringList() << "one" << "two" << "three");
    data.setDisableStateParameter(disableStateParameter);

    QSignalSpy result(m_testPlugin, SIGNAL(result(const SignOn::SessionData&)));
    QSignalSpy error(m_testPlugin, SIGNAL(error(const SignOn::Error &)));
    QSignalSpy userActionRequired(m_testPlugin,
                                  SIGNAL(userActionRequired(const SignOn::UiSessionData&)));

    TestNetworkAccessManager *nam = new TestNetworkAccessManager;
    m_testPlugin->m_networkAccessManager = nam;
    TestNetworkReply *reply = new TestNetworkReply(this);
    reply->setStatusCode(replyStatusCode);
    if (!replyContentType.isEmpty()) {
        reply->setContentType(replyContentType);
    }
    reply->setContent(replyContents.toUtf8());
    nam->setNextReply(reply);

    m_testPlugin->process(data, QString("web_server"));
    QTRY_COMPARE(userActionRequired.count(), 1);
    QString state = parseState(userActionRequired);

    if (!urlResponse.isEmpty()) {
        urlResponse.replace("$state", QString("state=") + state);
        urlResponse.replace("$wrongstate", QString("state=12") + state);
        info.setUrlResponse(urlResponse);
    }

    m_testPlugin->userActionFinished(info);

    QTRY_COMPARE(error.count(), errorCode < 0 ? 0 : 1);
    QTRY_COMPARE(result.count(), errorCode < 0 ? 1 : 0);
    if (errorCode >= 0) {
        QCOMPARE(error.at(0).at(0).value<Error>().type(), errorCode);
    } else {
        QCOMPARE(result.at(0).at(0).value<SessionData>().toMap(), response);
    }
    QCOMPARE(nam->m_lastRequest.url(), QUrl(postUrl));
    QCOMPARE(QString::fromUtf8(nam->m_lastRequestData), postContents);

    delete nam;
}

void PluginTest::testUserActionFinishedErrors_data()
{
    QTest::addColumn<int>("uiError");
    QTest::addColumn<int>("expectedErrorCode");

    QTest::newRow("user canceled") <<
        int(QUERY_ERROR_CANCELED) <<
        int(Error::SessionCanceled);

    QTest::newRow("network error") <<
        int(QUERY_ERROR_NETWORK) <<
        int(Error::Network);

    QTest::newRow("SSL error") <<
        int(QUERY_ERROR_SSL) <<
        int(Error::Ssl);

    QTest::newRow("generic") <<
        int(QUERY_ERROR_NOT_AVAILABLE) <<
        int(Error::UserInteraction);
}

void PluginTest::testUserActionFinishedErrors()
{
    QFETCH(int, uiError);
    QFETCH(int, expectedErrorCode);

    SignOn::UiSessionData info;
    PluginData data;
    data.setHost("localhost");
    data.setAuthPath("authorize");
    data.setTokenPath("access_token");
    data.setClientId("104660106251471");
    data.setClientSecret("fa28f40b5a1f8c1d5628963d880636fbkjkjkj");
    data.setRedirectUri("http://localhost/resp.html");

    QSignalSpy error(m_testPlugin, SIGNAL(error(const SignOn::Error &)));
    QSignalSpy userActionRequired(m_testPlugin,
                                  SIGNAL(userActionRequired(const SignOn::UiSessionData&)));

    m_testPlugin->process(data, QString("web_server"));
    QTRY_COMPARE(userActionRequired.count(), 1);

    info.setQueryErrorCode(uiError);
    m_testPlugin->userActionFinished(info);

    QTRY_COMPARE(error.count(), 1);
    QCOMPARE(error.at(0).at(0).value<Error>().type(), expectedErrorCode);
}

void PluginTest::testOauth1UserActionFinished_data()
{
    QTest::addColumn<QString>("mechanism");
    QTest::addColumn<QString>("urlResponse");
    QTest::addColumn<int>("errorCode");
    QTest::addColumn<QVariantMap>("expectedAuthMap");
    QTest::addColumn<int>("replyStatusCode");
    QTest::addColumn<QString>("replyContentType");
    QTest::addColumn<QString>("replyContents");
    QTest::addColumn<QVariantMap>("response");

    QTest::newRow("empty data") <<
        "HMAC-SHA1" <<
        "" <<
        int(Error::NotAuthorized) <<
        QVariantMap() << 0 << "" << "" << QVariantMap();

    QTest::newRow("auth error") <<
        "HMAC-SHA1" <<
        "http://localhost/resp.html?error=permission_denied" <<
        int(Error::NotAuthorized) <<
        QVariantMap() << 0 << "" << "" << QVariantMap();

    QTest::newRow("auth problem") <<
        "HMAC-SHA1" <<
        "http://localhost/resp.html?oauth_problem=permission_denied" <<
        int(Error::PermissionDenied) <<
        QVariantMap() << 0 << "" << "" << QVariantMap();

    QVariantMap authMap;
    authMap.insert("oauth_verifier", QString("v3r1f13r"));
    authMap.insert("oauth_token", QString("HiThere"));
    QTest::newRow("http error 401") <<
        "HMAC-SHA1" <<
        "http://localhost/resp.html?oauth_verifier=v3r1f13r" <<
        int(Error::OperationFailed) <<
        authMap <<
        int(401) <<
        "text/plain" <<
        "something else" <<
        QVariantMap();

    QTest::newRow("empty reply") <<
        "HMAC-SHA1" <<
        "http://localhost/resp.html?oauth_verifier=v3r1f13r" <<
        int(Error::OperationFailed) <<
        authMap <<
        int(200) <<
        "text/plain" <<
        "" <<
        QVariantMap();

    QTest::newRow("missing secret") <<
        "HMAC-SHA1" <<
        "http://localhost/resp.html?oauth_verifier=v3r1f13r" <<
        int(Error::OperationFailed) <<
        authMap <<
        int(200) <<
        "text/plain" <<
        "oauth_token=t0k3n" <<
        QVariantMap();

    QVariantMap response;
    response.insert("AccessToken", QString("t0k3n"));
    response.insert("TokenSecret", QString("t0k3nS3cr3t"));

    QTest::newRow("success") <<
        "HMAC-SHA1" <<
        "http://localhost/resp.html?oauth_verifier=v3r1f13r" <<
        int(-1) <<
        authMap <<
        int(200) <<
        "text/plain" <<
        "oauth_token=t0k3n&oauth_token_secret=t0k3nS3cr3t" <<
        response;

    response.insert("ExtraField", QString("v4lu3"));
    QTest::newRow("success with data") <<
        "HMAC-SHA1" <<
        "http://localhost/resp.html?oauth_verifier=v3r1f13r" <<
        int(-1) <<
        authMap <<
        int(200) <<
        "text/plain" <<
        "oauth_token=t0k3n&oauth_token_secret=t0k3nS3cr3t"
        "&ExtraField=v4lu3" <<
        response;
}

void PluginTest::testOauth1UserActionFinished()
{
    QFETCH(QString, mechanism);
    QFETCH(QString, urlResponse);
    QFETCH(int, errorCode);
    QFETCH(QVariantMap, expectedAuthMap);
    QFETCH(int, replyStatusCode);
    QFETCH(QString, replyContentType);
    QFETCH(QString, replyContents);
    QFETCH(QVariantMap, response);

    SignOn::UiSessionData info;
    OAuth1PluginData data;
    data.setRequestEndpoint("https://localhost/oauth/request_token");
    data.setTokenEndpoint("https://localhost/oauth/access_token");
    data.setAuthorizationEndpoint("https://localhost/oauth/authorize");
    data.setCallback("http://localhost/resp.html");
    data.setConsumerKey("104660106251471");
    data.setConsumerSecret("fa28f40b5a1f8c1d5628963d880636fbkjkjkj");
    data.setRealm("MyHost");

    QSignalSpy result(m_testPlugin, SIGNAL(result(const SignOn::SessionData&)));
    QSignalSpy error(m_testPlugin, SIGNAL(error(const SignOn::Error &)));
    QSignalSpy userActionRequired(m_testPlugin,
                                  SIGNAL(userActionRequired(const SignOn::UiSessionData&)));

    TestNetworkAccessManager *nam = new TestNetworkAccessManager;
    m_testPlugin->m_networkAccessManager = nam;
    TestNetworkReply *reply = new TestNetworkReply(this);
    reply->setStatusCode(200);
    reply->setContentType("text/plain");
    reply->setContent("oauth_token=HiThere&oauth_token_secret=BigSecret");
    nam->setNextReply(reply);

    m_testPlugin->process(data, mechanism);
    QTRY_COMPARE(userActionRequired.count(), 1);

    nam->m_lastRequest = QNetworkRequest();
    nam->m_lastRequestData = QByteArray();

    reply = new TestNetworkReply(this);
    reply->setStatusCode(replyStatusCode);
    if (!replyContentType.isEmpty()) {
        reply->setContentType(replyContentType);
    }
    reply->setContent(replyContents.toUtf8());
    nam->setNextReply(reply);

    if (!urlResponse.isEmpty()) {
        info.setUrlResponse(urlResponse);
    }

    m_testPlugin->userActionFinished(info);
    QTRY_COMPARE(error.count(), errorCode < 0 ? 0 : 1);
    QTRY_COMPARE(result.count(), errorCode < 0 ? 1 : 0);
    QVariantMap resp;
    if (errorCode >= 0) {
        QCOMPARE(error.at(0).at(0).value<Error>().type(), errorCode);
    } else {
        resp = result.at(0).at(0).value<SessionData>().toMap();
        QVERIFY(mapIsSubset(response, resp));
    }

    if (!expectedAuthMap.isEmpty()) {
        QCOMPARE(nam->m_lastRequest.url().toString(), data.TokenEndpoint());
        QVERIFY(nam->m_lastRequestData.isEmpty());

        QString authorizationHeader =
            QString::fromUtf8(nam->m_lastRequest.rawHeader("Authorization"));
        QStringList authorizationHeaderParts =
            authorizationHeader.split(QRegExp(",?\\s+"));
        QCOMPARE(authorizationHeaderParts[0], QString("OAuth"));

        /* The rest of the header should be a mapping, let's parse it */
        bool ok = true;
        QVariantMap authMap =
            parseAuthorizationHeader(authorizationHeaderParts.mid(1), &ok);
        QVERIFY(ok);
        QCOMPARE(authMap.value("oauth_signature_method").toString(), mechanism);
        QVERIFY(mapIsSubset(expectedAuthMap, authMap));
    }

    delete nam;
}

void PluginTest::testErrors_data()
{
    QTest::addColumn<QString>("replyContents");
    QTest::addColumn<int>("expectedErrorCode");

    QTest::newRow("incorrect_client_credentials") <<
        "{ \"error\": \"incorrect_client_credentials\" }" <<
        int(Error::InvalidCredentials);

    QTest::newRow("redirect_uri_mismatch") <<
        "{ \"error\": \"redirect_uri_mismatch\" }" <<
        int(Error::InvalidCredentials);

    QTest::newRow("bad_authorization_code") <<
        "{ \"error\": \"bad_authorization_code\" }" <<
        int(Error::InvalidCredentials);

    QTest::newRow("invalid_client_credentials") <<
        "{ \"error\": \"invalid_client_credentials\" }" <<
        int(Error::InvalidCredentials);

    QTest::newRow("unauthorized_client") <<
        "{ \"error\": \"unauthorized_client\" }" <<
        int(Error::NotAuthorized);

    QTest::newRow("invalid_assertion") <<
        "{ \"error\": \"invalid_assertion\" }" <<
        int(Error::InvalidCredentials);

    QTest::newRow("unknown_format") <<
        "{ \"error\": \"unknown_format\" }" <<
        int(Error::InvalidQuery);

    QTest::newRow("authorization_expired") <<
        "{ \"error\": \"authorization_expired\" }" <<
        int(Error::InvalidCredentials);

    QTest::newRow("multiple_credentials") <<
        "{ \"error\": \"multiple_credentials\" }" <<
        int(Error::InvalidQuery);

    QTest::newRow("invalid_user_credentials") <<
        "{ \"error\": \"invalid_user_credentials\" }" <<
        int(Error::InvalidCredentials);
}

void PluginTest::testErrors()
{
    QFETCH(QString, replyContents);
    QFETCH(int, expectedErrorCode);

    SignOn::UiSessionData info;
    PluginData data;
    data.setHost("localhost");
    data.setAuthPath("authorize");
    data.setTokenPath("access_token");
    data.setClientId("104660106251471");
    data.setClientSecret("fa28f40b5a1f8c1d5628963d880636fbkjkjkj");
    data.setRedirectUri("http://localhost/resp.html");

    QSignalSpy error(m_testPlugin, SIGNAL(error(const SignOn::Error &)));
    QSignalSpy userActionRequired(m_testPlugin,
                                  SIGNAL(userActionRequired(const SignOn::UiSessionData&)));

    TestNetworkAccessManager *nam = new TestNetworkAccessManager;
    m_testPlugin->m_networkAccessManager = nam;
    TestNetworkReply *reply = new TestNetworkReply(this);
    reply->setStatusCode(401);
    reply->setContentType("application/json");
    reply->setContent(replyContents.toUtf8());
    nam->setNextReply(reply);

    m_testPlugin->process(data, QString("web_server"));
    QTRY_COMPARE(userActionRequired.count(), 1);
    QString state = parseState(userActionRequired);

    info.setUrlResponse("http://localhost/resp.html?code=c0d3&state=" + state);
    m_testPlugin->userActionFinished(info);

    QTRY_COMPARE(error.count(), 1);
    QCOMPARE(error.at(0).at(0).value<Error>().type(), expectedErrorCode);

    delete nam;
}

void PluginTest::testRefreshToken_data()
{
    QTest::addColumn<QVariantMap>("sessionData");
    QTest::addColumn<QVariantMap>("expectedResponse");

    PluginData data;
    data.setHost("localhost");
    data.setAuthPath("authorize");
    data.setTokenPath("access_token");
    data.setClientId("104660106251471");
    data.setClientSecret("fa28f40b5a1f8c1d5628963d880636fbkjkjkj");
    data.setRedirectUri("http://localhost/resp.html");

    QVariantMap tokens;
    QVariantMap token;
    token.insert("Token", QLatin1String("tokenfromtest"));
    token.insert("timestamp", QDateTime::currentDateTime().toTime_t() - 10000);
    token.insert("Expiry", 1000);
    token.insert("refresh_token", QString("r3fr3sh"));
    tokens.insert(data.ClientId(), QVariant::fromValue(token));
    data.m_data.insert("Tokens", tokens);

    QVariantMap response;
    response.insert("AccessToken", "n3w-t0k3n");
    response.insert("ExpiresIn", 3600);
    response.insert("RefreshToken", QString());
    response.insert("Scope", QStringList());

    QTest::newRow("expired access token") << data.toMap() << response;

    token.insert("timestamp", QDateTime::currentDateTime().toTime_t());
    token.insert("Expiry", 50000);
    tokens.insert(data.ClientId(), QVariant::fromValue(token));
    data.m_data.insert("Tokens", tokens);
    data.setForceTokenRefresh(true);
    QTest::newRow("valid access token, force refresh") << data.toMap() << response;
}

void PluginTest::testRefreshToken()
{
    QFETCH(QVariantMap, sessionData);
    QFETCH(QVariantMap, expectedResponse);

    SignOn::UiSessionData info;

    QSignalSpy result(m_testPlugin, SIGNAL(result(const SignOn::SessionData&)));
    QSignalSpy error(m_testPlugin, SIGNAL(error(const SignOn::Error &)));

    TestNetworkAccessManager *nam = new TestNetworkAccessManager;
    m_testPlugin->m_networkAccessManager = nam;
    TestNetworkReply *reply = new TestNetworkReply(this);
    reply->setStatusCode(200);
    reply->setContentType("application/json");
    reply->setContent("{ \"access_token\":\"n3w-t0k3n\", \"expires_in\": 3600 }");
    nam->setNextReply(reply);

    m_testPlugin->process(sessionData, QString("web_server"));
    QTRY_COMPARE(result.count(), 1);
    QCOMPARE(error.count(), 0);

    QCOMPARE(nam->m_lastRequest.url(), QUrl("https://localhost/access_token"));
    QCOMPARE(QString::fromUtf8(nam->m_lastRequestData),
             QString("grant_type=refresh_token&refresh_token=r3fr3sh"));

    QCOMPARE(result.at(0).at(0).value<SessionData>().toMap(), expectedResponse);

    delete nam;
}

void PluginTest::testRefreshTokenError_data()
{
    QTest::addColumn<int>("replyErrorCode");
    QTest::addColumn<int>("replyStatusCode");
    QTest::addColumn<QString>("replyContents");
    QTest::addColumn<int>("expectedError");

    QTest::newRow("invalid grant, 400") <<
        int(QNetworkReply::ProtocolInvalidOperationError) <<
        int(400) <<
        "{ \"error\":\"invalid_grant\" }" <<
        int(-1);

    QTest::newRow("invalid grant, 401") <<
        int(QNetworkReply::ContentAccessDenied) <<
        int(401) <<
        "{ \"error\":\"invalid_grant\" }" <<
        int(-1);

    QTest::newRow("invalid grant, 401, no error signal") <<
        int(-1) <<
        int(401) <<
        "{ \"error\":\"invalid_grant\" }" <<
        int(-1);

    QTest::newRow("temporary network failure") <<
        int(QNetworkReply::TemporaryNetworkFailureError) <<
        int(-1) <<
        "" <<
        int(Error::NoConnection);
}

void PluginTest::testRefreshTokenError()
{
    QFETCH(int, replyErrorCode);
    QFETCH(int, replyStatusCode);
    QFETCH(QString, replyContents);
    QFETCH(int, expectedError);

    PluginData data;
    data.setHost("localhost");
    data.setAuthPath("authorize");
    data.setTokenPath("access_token");
    data.setClientId("104660106251471");
    data.setClientSecret("fa28f40b5a1f8c1d5628963d880636fbkjkjkj");
    data.setRedirectUri("http://localhost/resp.html");

    QVariantMap tokens;
    QVariantMap token;
    token.insert("Token", QLatin1String("tokenfromtest"));
    token.insert("timestamp", QDateTime::currentDateTime().toTime_t() - 10000);
    token.insert("Expiry", 1000);
    token.insert("refresh_token", QString("r3fr3sh"));
    tokens.insert(data.ClientId(), QVariant::fromValue(token));
    data.m_data.insert("Tokens", tokens);

    SignOn::UiSessionData info;

    QSignalSpy error(m_testPlugin, SIGNAL(error(const SignOn::Error &)));
    QSignalSpy userActionRequired(m_testPlugin,
                                  SIGNAL(userActionRequired(const SignOn::UiSessionData&)));

    TestNetworkAccessManager *nam = new TestNetworkAccessManager;
    m_testPlugin->m_networkAccessManager = nam;
    TestNetworkReply *reply = new TestNetworkReply(this);
    if (replyErrorCode >= 0) {
        reply->setError(QNetworkReply::NetworkError(replyErrorCode),
                        "Dummy error", 5);
    }
    reply->setStatusCode(replyStatusCode);
    reply->setContentType("application/json");
    reply->setContent(replyContents.toUtf8());
    nam->setNextReply(reply);

    m_testPlugin->process(data, QString("web_server"));

    if (expectedError < 0) {
        QTRY_COMPARE(userActionRequired.count(), 1);
        QCOMPARE(error.count(), 0);
    } else {
        QTRY_COMPARE(error.count(), 1);
        QCOMPARE(error.at(0).at(0).value<Error>().type(), expectedError);
    }

    delete nam;
}

void PluginTest::testClientAuthentication_data()
{
    QTest::addColumn<QString>("clientSecret");
    QTest::addColumn<bool>("forceAuthViaRequestBody");
    QTest::addColumn<QString>("postContents");
    QTest::addColumn<QString>("postAuthorization");

    QTest::newRow("no secret, std auth") <<
        "" << false <<
        "grant_type=authorization_code&code=c0d3"
        "&redirect_uri=http://localhost/resp.html&client_id=104660106251471" <<
        "";
    QTest::newRow("no secret, auth in body") <<
        "" << true <<
        "grant_type=authorization_code&code=c0d3"
        "&redirect_uri=http://localhost/resp.html&client_id=104660106251471" <<
        "";

    QTest::newRow("with secret, std auth") <<
        "s3cr3t" << false <<
        "grant_type=authorization_code&code=c0d3&redirect_uri=http://localhost/resp.html" <<
        "Basic MTA0NjYwMTA2MjUxNDcxOnMzY3IzdA==";
    QTest::newRow("with secret, auth in body") <<
        "s3cr3t" << true <<
        "grant_type=authorization_code&code=c0d3"
        "&redirect_uri=http://localhost/resp.html"
        "&client_id=104660106251471&client_secret=s3cr3t" <<
        "";
}

void PluginTest::testClientAuthentication()
{
    QFETCH(QString, clientSecret);
    QFETCH(bool, forceAuthViaRequestBody);
    QFETCH(QString, postContents);
    QFETCH(QString, postAuthorization);

    SignOn::UiSessionData info;
    PluginData data;
    data.setHost("localhost");
    data.setAuthPath("authorize");
    data.setTokenPath("access_token");
    data.setClientId("104660106251471");
    data.setClientSecret(clientSecret);
    data.setRedirectUri("http://localhost/resp.html");
    data.setForceClientAuthViaRequestBody(forceAuthViaRequestBody);

    QSignalSpy result(m_testPlugin, SIGNAL(result(const SignOn::SessionData&)));
    QSignalSpy error(m_testPlugin, SIGNAL(error(const SignOn::Error &)));
    QSignalSpy userActionRequired(m_testPlugin,
                                  SIGNAL(userActionRequired(const SignOn::UiSessionData&)));

    TestNetworkAccessManager *nam = new TestNetworkAccessManager;
    m_testPlugin->m_networkAccessManager = nam;
    TestNetworkReply *reply = new TestNetworkReply(this);
    reply->setStatusCode(200);
    reply->setContentType("application/json");
    reply->setContent("{ \"access_token\":\"t0k3n\", \"expires_in\": 3600 }");
    nam->setNextReply(reply);

    m_testPlugin->process(data, QString("web_server"));
    QTRY_COMPARE(userActionRequired.count(), 1);
    QString state = parseState(userActionRequired);

    info.setUrlResponse("http://localhost/resp.html?code=c0d3&state=" + state);
    m_testPlugin->userActionFinished(info);

    QTRY_COMPARE(result.count(), 1);
    QCOMPARE(error.count(), 0);
    QCOMPARE(nam->m_lastRequest.url(), QUrl("https://localhost/access_token"));
    QCOMPARE(QString::fromUtf8(nam->m_lastRequestData), postContents);
    QCOMPARE(QString::fromUtf8(nam->m_lastRequest.rawHeader("Authorization")),
             postAuthorization);

    delete nam;
}

#endif

QTEST_MAIN(PluginTest)
#include "tst_plugin.moc"
