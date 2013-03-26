/*
 * Copyright (C) 2013 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */

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
