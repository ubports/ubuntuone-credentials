#ifndef TESTHEADER_H
#define TESTHEADER_H

#include <QObject>
#include <header.h>

class TestHeader : public QObject
{
    Q_OBJECT
public:
    explicit TestHeader(QObject *parent = 0);

private slots:
    void testSetAlbum();
    void testSetArtist();
    void testSetPrice();
    void testSetPicture();

private:
    Header header;

};

#endif // TESTHEADER_H
