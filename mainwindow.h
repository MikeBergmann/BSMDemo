/*
  Copyright 2013 Mike Bergmann, Bones AG

  This file is part of BSMDemo.

  BSMDemo is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  BSMDemo is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with BSMDemo.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qextserialenumerator.h>
#include "PortListener.h"

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

protected:
  void changeEvent(QEvent *e);

private slots:
  void on_pbConnect_clicked();
  void on_pbRefresh_clicked();
  void on_pbSpeak_clicked();

  void on_tbPlay_clicked();

  void on_tbUp_clicked();

  void on_pbSOS_clicked();

private:
  Ui::MainWindow *ui;
  QList<QextPortInfo> m_ports;
  PortListener *m_listener;

  void checkPorts();
};

#endif // MAINWINDOW_H
