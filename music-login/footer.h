#ifndef FOOTER_H
#define FOOTER_H

#include <QFrame>

namespace Ui {
class Footer;
}

class Footer : public QFrame
{
    Q_OBJECT
    
public:
    explicit Footer(QWidget *parent = 0);
    ~Footer();
    
private:
    Ui::Footer *ui;
};

#endif // FOOTER_H
