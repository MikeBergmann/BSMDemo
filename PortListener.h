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

#ifndef PORTLISTENER_H_
#define PORTLISTENER_H_

#include "bsmcommand.h"

#include <QTimer>

class QextSerialPort;
class QTextEdit;
class BSMFileSend;

class PortListener : public QObject {

  Q_OBJECT

public:
  PortListener(const QString &portName, QTextEdit *i_textOut, QTextEdit *i_plainTextOut);
  ~PortListener();
  ulong getLastError();
  bool isOpen();
  void sendHexMessage(const QString &i_text);
  void speakText(const QString &i_text);
  void SOS(const QString &i_text);
  void parseMessage(const BSMCommand &i_text);
  void getVolume();
  void getLanguage();
  void sendFile(const QString &i_path);
  void playAndPause();

  static const QMap<quint16, QString> languages;
  static const QMap<quint16, QString> codepages;

private:
  QextSerialPort *m_port;
  QTextEdit      *m_textOut;
  QTextEdit      *m_plainTextOut;
  QByteArray      m_receivedBytes;
  QTimer          m_indicationTimeout;
  int             m_volume;
  int             m_volumeReq;
  int             m_language;
  int             m_languageReq;
  BSMFileSend    *m_fileSend;

signals:
  void volumeChanged(int i_volume);
  void languageChanged(int i_index);
  void waitOnIndication(bool i_wait);

public slots:
  void setVolume(int i_volume);
  void setLanguage(int i_language);

private slots:
  void onReadyRead();
  void onDsrChanged(bool status);
  void indicationTimeout();
};

#endif /*PORTLISTENER_H_*/
