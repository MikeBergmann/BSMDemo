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

#ifndef BSMFILESEND_H
#define BSMFILESEND_H

#include <QDataStream>
#include <QTimer>

class QextSerialPort;
class QTextEdit;
class QProgressDialog;

class BSMFileSend  : public QObject {

  Q_OBJECT

public:
  explicit BSMFileSend(QextSerialPort *i_port, QTextEdit *i_textOut);

  void openFile(const QString &i_path);
  void sendFile(bool i_param);
  void closeFile();

private:
    QextSerialPort  *m_port;
    QTextEdit       *m_textOut;
    QDataStream      stream;
    QProgressDialog *dlg;
    QTimer           m_transferTimeout;
    quint32          m_crc;

    static qint16 CRC16(char *sn, quint16 n, quint16 crc_16);

private slots:
    void transferTimeout();

};

#endif // BSMFILESEND_H
