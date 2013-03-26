#include "footer.h"
#include "ui_footer.h"

Footer::Footer(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::Footer)
{
    ui->setupUi(this);
}

Footer::~Footer()
{
    delete ui;
}
