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

#include "bsmfilesend.h"
#include "bsmcommand.h"
#include "qextserialport.h"

#include <QFile>
#include <QtEndian>
#include <QFileInfo>
#include <QProgressDialog>
#include <QVariant>
#include <QDebug>
#include <QTextEdit>

BSMFileSend::BSMFileSend(QextSerialPort *i_port, QTextEdit *i_textOut)
: QObject()
, m_port(i_port)
, m_textOut(i_textOut)
, stream()
, dlg(NULL)
, m_transferTimeout()
, m_crc(0)
{
  Q_ASSERT(i_port);
  Q_ASSERT(i_textOut);

  m_transferTimeout.setSingleShot(true);
  m_transferTimeout.setInterval(5000);
  connect(&m_transferTimeout, SIGNAL(timeout()), this, SLOT(transferTimeout()));
}

void BSMFileSend::openFile(const QString &i_path)
{
  BSMCommand cmd;
  QByteArray data;

  if(!stream.device()) {
    QFile *file = new QFile(i_path);
    if(!file->exists()) {
      qDebug() << "PortListener::sendFile - Unable to find file";
      delete file;
      return;
    }

    if(!file->open(QIODevice::ReadOnly)) {
      qDebug() << "PortListener::sendFile - Unable to open file";
      delete file;
      return;
    }

    stream.setDevice(file);

    // Initial BSM Command
    data.append(0x06);
    data.append(0x11);
    data.append(QByteArray(8,0));
    data.append(QString("B:/PROMPTS/1.WAV"));
    data.append(static_cast<char>(0x00));

    // Create Progress Dialog
    dlg = new QProgressDialog(tr("Transfer"),tr("Cancel Transfer"),0, stream.device()->bytesAvailable());
    dlg->setMinimumDuration(0);
    dlg->setModal(true);

    m_crc = 0;

    m_transferTimeout.start();
    cmd.makeCommand(BSMCommand::SAPI_BONES, data);
    m_port->write(cmd);
  }
}

void BSMFileSend::sendFile(bool i_param)
{
  BSMCommand cmd;
  QByteArray data;
  bool fileClosed = false;

  if(!stream.device())
    return;

  if(i_param == true) {
    if(stream.atEnd()) {
      closeFile();
      fileClosed = true;
    } else {
      QByteArray data2(4,0);
      QByteArray data3(128,0);
      qint32 val;

      data.append(0x06);
      data.append(0x22);

      // Add current pos
      val = qbswap<qint32>(static_cast<qint32>(stream.device()->pos()));
      data.append(QByteArray::fromRawData(reinterpret_cast<char*>(&val),4));

      //Add the available bytes
      val = qbswap<qint32>(static_cast<qint32>(stream.device()->bytesAvailable()));
      data.append(QByteArray::fromRawData(reinterpret_cast<char*>(&val),4));

      // Read the next data block
      int count = stream.readRawData(data3.data(),128);
      if(count < 128)
        data3.resize(count);
      data.append(data3);

      m_crc = CRC16(data3.data(), count, m_crc);

      cmd.makeCommand(BSMCommand::SAPI_BONES, data);
      m_port->write(cmd);

      dlg->setValue(stream.device()->pos());
    }

    m_transferTimeout.start();

    if(i_param == false || dlg->wasCanceled() || fileClosed) {
      stream.device()->close();
      delete stream.device();
      stream.setDevice(NULL);
      if(!dlg->wasCanceled())
        dlg->setValue(dlg->maximum());
      delete dlg;
      m_transferTimeout.stop();
    }
  }
}

void BSMFileSend::transferTimeout()
{
  m_textOut->append(tr("Wait on file transfer - timeout"));
  sendFile(false);
}

qint16 BSMFileSend::CRC16(char *sn, quint16 n, quint16 crc_16) {
  quint8   j;
  quint16  i;

  for(i=0;i<n;i++) {
    crc_16^=*sn++;
    for(j=0;j<8;j++) {
      if(crc_16 & 0x0001) {
        crc_16>>=1;
        crc_16^=0x8408;
      } else {
        crc_16>>=1;
      }
    }
  }
  return crc_16;
}

void BSMFileSend::closeFile() {
  BSMCommand cmd;
  QByteArray data;
  data.append(0x06);
  data.append(0x33);

  qint32 val;
  val = qbswap<qint32>(static_cast<qint32>(stream.device()->pos()-1));
  data.append(QByteArray::fromRawData(reinterpret_cast<char*>(&val),4));

  data.append(QByteArray(4,0));
  data.append(static_cast<char>((m_crc&0xff00)>>8));
  data.append(static_cast<char>(m_crc&0x00ff));
  cmd.makeCommand(BSMCommand::SAPI_BONES, data);
  qDebug() << "CRC " << data.toHex();
  m_port->write(cmd);
}
