#include <QApplication>
#include <QIcon>

#include "serial_monitor.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/serial_icon.ico"));
    SerialMonitor monitor;
    monitor.setFixedSize(450, 350);
    monitor.show();
    return app.exec();
}
