#include <QApplication>
#include "timelapseapp.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    TimelapseApp window;
    window.show();
    return app.exec();
}
