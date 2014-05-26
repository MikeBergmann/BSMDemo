#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  a.setApplicationName("BSMDemo");
  a.setApplicationVersion(QString(__DATE__) + " " + __TIME__);

  MainWindow w;
  w.show();

  return a.exec();
}
