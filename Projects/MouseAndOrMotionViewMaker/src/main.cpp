#include <QApplication>

#include "mouseandormotionviewmakergui.h"

//would be trivial to save the image instead of (or in addition to) presenting it, but since i don't need/want that functionality, i'm not going to code it...
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MouseAndOrMotionViewMakerGui gui;
    Q_UNUSED(gui)

    return a.exec();
}
