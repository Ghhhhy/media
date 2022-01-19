#include "mediaplay.h"
#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    mediaplay w;
    w.setWindowTitle("星空播放器");
    w.setWindowIcon(QIcon(":/src/img/exeicon.ico"));
    w.show();

    return a.exec();
}
