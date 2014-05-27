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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "PortListener.h"
#include "qextserialport.h"

#include <QDebug>
#include <QFileDialog>
#include <QSplitter>
#include <QThread>

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
, ui(new Ui::MainWindow)
, m_ports()
, m_listener(NULL)
{
  ui->setupUi(this);
  ui->groupBox->setEnabled(false);
  ui->pbConnect->setText(tr("&Connect"));
  ui->pbRefresh->setText(tr("&Refresh"));
  ui->pbSpeak->setText(tr("&Speak"));
  checkPorts();

  QSplitter *splitter = new QSplitter(this);
  splitter->setOrientation(Qt::Vertical);

  ui->verticalLayout_2->addWidget(splitter);

  ui->teInfo->setParent(splitter);
  ui->tePlainText->setParent(splitter);

  statusBar()->showMessage(QString("Version ")+qApp->applicationVersion());
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
  QMainWindow::changeEvent(e);
  switch(e->type()) {
  case QEvent::LanguageChange:
    ui->retranslateUi(this);
    break;
  default:
    break;
  }
}

void MainWindow::on_pbConnect_clicked()
{
  if(ui->cbSerial->count() <= 0)
    return;

  if(!m_listener) {
    m_listener = new PortListener(m_ports.at(ui->cbSerial->currentIndex()).portName, ui->teInfo, ui->tePlainText);  // signals get hooked up internally

    connect(m_listener,SIGNAL(volumeChanged(int)),ui->sbVolume,SLOT(setValue(int)));
    connect(ui->sbVolume,SIGNAL(valueChanged(int)),m_listener,SLOT(setVolume(int)));
    m_listener->getVolume();

    ui->cbVoice->insertItems(0,m_listener->languages.values());
    connect(m_listener,SIGNAL(languageChanged(int)),ui->cbVoice,SLOT(setCurrentIndex(int)));
    connect(ui->cbVoice,SIGNAL(currentIndexChanged(int)),m_listener,SLOT(setLanguage(int)));
    m_listener->getLanguage();

    connect(m_listener, SIGNAL(waitOnIndication(bool)),ui->groupBox,SLOT(setDisabled(bool)));

    m_listener->sendHexMessage("7e0103002c7e"); // Get Software Version
    ui->groupBox->setEnabled(true);


  } else {
    ui->groupBox->setEnabled(false);
    ui->pbConnect->setText(tr("&Connect"));
    delete m_listener;
    m_listener = NULL;
  }

  if(m_listener) {
    if(m_listener->isOpen() && m_listener->getLastError() == E_NO_ERROR) {
      ui->pbConnect->setText(tr("&Disconnect"));
    } else {
      ui->groupBox->setEnabled(false);
      ui->pbConnect->setText(tr("&Connect"));
      delete m_listener;
    }
  }
}

void MainWindow::checkPorts()
{
  m_ports.clear();
  ui->cbSerial->clear();

  m_ports = QextSerialEnumerator::getPorts();
  qDebug() << "List of ports:";
  for(int i = 0; i < m_ports.size(); i++) {

#ifdef FTDI_ONLY
    if(m_ports.at(i).vendorID == 0x403 && m_ports.at(i).productID == 0x6001) {
      qDebug() << "Found FTDI:" << m_ports.at(i).portName;
      ui->cbSerial->addItem(m_ports.at(i).friendName);
    }
#else
    qDebug() << "Found:" << m_ports.at(i).portName;
    ui->cbSerial->addItem(m_ports.at(i).friendName);
#endif

    qDebug() << "port name:" << m_ports.at(i).portName;
    qDebug() << "friendly name:" << m_ports.at(i).friendName;
    qDebug() << "physical name:" << m_ports.at(i).physName;
    qDebug() << "enumerator name:" << m_ports.at(i).enumName;
    qDebug() << "vendor ID:" << QString::number(m_ports.at(i).vendorID, 16);
    qDebug() << "product ID:" << QString::number(m_ports.at(i).productID, 16);
    qDebug() << "===================================";
  }
}

void MainWindow::on_pbRefresh_clicked()
{
  checkPorts();
}

void MainWindow::on_pbSpeak_clicked()
{
  m_listener->speakText(ui->leSpeak->text());
}

void MainWindow::on_tbPlay_clicked()
{
  m_listener->playAndPause();
}

void MainWindow::on_tbUp_clicked()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Upload WAV File"), QString(), "*.WAV");
  if(!fileName.isEmpty())
    m_listener->sendFile(fileName);
}

void MainWindow::on_pbSOS_clicked()
{
  m_listener->SOS(ui->leSOS->text());
}
