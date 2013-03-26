#ifndef LOADINGOVERLAY_H
#define LOADINGOVERLAY_H

#include <QFrame>

namespace Ui {
class LoadingOverlay;
}

class LoadingOverlay : public QFrame
{
    Q_OBJECT

public:
    explicit LoadingOverlay(QWidget *parent = 0);
    ~LoadingOverlay();

    virtual bool eventFilter(QObject *, QEvent *);

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void showEvent(QShowEvent *);
    virtual void timerEvent(QTimerEvent *);


private:
    Ui::LoadingOverlay *ui;
    int counter;
    bool orientation;
    int timer;
};

#endif // LOADINGOVERLAY_H
