#ifndef HEADER_H
#define HEADER_H

#include <QFrame>

namespace Ui {
class Header;
}

#ifdef TESTS
class TestHeader;
#endif

class Header : public QFrame
{
    Q_OBJECT

public:
    explicit Header(QWidget *parent = 0);
    ~Header();
    void setAlbum(QString album);
    void setArtist(QString artist);
    void setPrice(QString price);
    void setPicture(QString picture_path);

private:
    Ui::Header *ui;

#ifdef TESTS
    friend class TestHeader;
#endif
};

#endif // HEADER_H
