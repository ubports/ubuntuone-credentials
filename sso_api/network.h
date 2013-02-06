#ifndef _NETWORK_H_
#define _NETWORK_H_ 

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

namespace SSO {

static QString API_BASE = "https://login.staging.ubuntu.com/api/v2/";

class Network : public QObject
{
    Q_OBJECT
public:
    explicit Network(QObject *parent = 0);
    void Post(const QByteArray&);

signals:

public slots:

private slots:
    void OnReply(QNetworkReply*);

private:
    QNetworkAccessManager* _nam;
    QNetworkRequest* _request;

    static const QString _tokensApi, _accountsApi;
};

} /* end SSO namespace */

#endif /* _NETWORK_H_ */
