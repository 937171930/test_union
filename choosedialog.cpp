#include "choosedialog.h"
#include "ui_choosedialog.h"

ChooseDialog::ChooseDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChooseDialog)
{
    ui->setupUi(this);
}

ChooseDialog::~ChooseDialog()
{
    delete ui;
}

void ChooseDialog::on_pushButton_clicked()
{

}

void ChooseDialog::on_pushButton_2_clicked()
{

}
