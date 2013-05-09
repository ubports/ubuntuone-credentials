#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QDir>

class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader(QObject *parent = 0);

    void startDownload(const QString& url);

public slots:
    void onReply(QNetworkReply*);

signals:
    void fileDownloaded(QString&);

private:
    QNetworkAccessManager* _nam;
    QNetworkRequest* _request;
    QDir* _downloadDir;

    void createDownloadDir();

};

#endif // DOWNLOADER_H
