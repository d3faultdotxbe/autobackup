#include <QCoreApplication>

#include "wasdfcli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    WasdfCli cli;
    Q_UNUSED(cli)

    return a.exec();
}
