#include "widget.h"
#include "login.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QPointer<login> l = new login();
    Widget w;
    QObject::connect(l,&login::finished,&w,&Widget::show);
    QObject::connect(l,&login::finished,&w,&Widget::receive);
    QObject::connect(&w,&Widget::trigger,&w,&Widget::started);
    QObject::connect(&w,&Widget::trigger,l,&login::destroy);

    l->show();
    return a.exec();
}
