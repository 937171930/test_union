#include "infodialog.h"
#include "ui_infodialog.h"

#include <QToolButton>
#include <QPixmap>
#include <QStyle>
#include <QMouseEvent>

extern MainWindow *w;

InfoDialog::InfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InfoDialog)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);    //无边框模式

    ui->setupUi(this);
}

InfoDialog::~InfoDialog()
{
    delete ui;
}

void InfoDialog::Display(QString str){
    ui->textEdit->append(str);
}

/*******关闭按钮信号槽函数*******/
void InfoDialog::windowclosed(){
    this->close();
    w->ui->action_3->setEnabled(true);
}

/*******UI界面添加关闭按钮*******/
void InfoDialog::addCloseButton(){
    int wide = width();                                                         //获取界面的宽度
    QToolButton *closeButton= new QToolButton(this);    //构建关闭按钮
    QPixmap closePix = style()->standardPixmap(QStyle::SP_TitleBarCloseButton); //获取关闭按钮图标
    closeButton->setIcon(closePix);                         //设置关闭按钮图标
    closeButton->setGeometry(wide-21,3,18,18);              //设置关闭按钮在界面的位置
    closeButton->setToolTip(tr("关闭"));                     //设置鼠标移至按钮上的提示信息
    closeButton->setStyleSheet("background-color:red;");    //设置关闭按钮的样式
    connect(closeButton, SIGNAL(clicked()), this, SLOT(windowclosed()) );
}

void InfoDialog::mousePressEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        mousePressed = true;
        mousePoint = event->pos();
    }
}

void InfoDialog::mouseReleaseEvent(QMouseEvent *event){
    mousePressed    = false;
}

void InfoDialog::mouseMoveEvent(QMouseEvent *event){
    if(mousePressed && event->buttons()){
        this->move(event->globalPos() - mousePoint);
        event->accept();
    }
}
