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

#include "PortListener.h"
#include "bsmcommand.h"
#include "qextserialport.h"
#include "bsmfilesend.h"

#include <QtDebug>
#include <QTextEdit>
#include <QTextCodec>

static QMap<quint16, QString> initLanguages() {
  QMap<quint16, QString> map;
  map.insert(0x0100, QObject::tr("Arabic (SA) - Salma. Quality 50."     ));
  map.insert(0x0101, QObject::tr("Arabic (SA) - Youssef. Quality 50."   ));
  map.insert(0x0200, QObject::tr("Dutch (BE) - Sofie. Quality 50."      ));
  map.insert(0x0400, QObject::tr("Danish - Mette. Quality 50."          ));
  map.insert(0x0500, QObject::tr("German - Klaus. Quality 50."          ));
  map.insert(0x0501, QObject::tr("German - Sarah. Quality 50."          ));
  map.insert(0x0600, QObject::tr("English (UK) - Graham. Quality 50."   ));
  map.insert(0x0801, QObject::tr("English (UK) - Lucy. Quality 50."     ));
  map.insert(0x0802, QObject::tr("English (UK) - Peter. Quality 50."    ));
  map.insert(0x0803, QObject::tr("English (UK) - Rachel. Quality 50."   ));
  map.insert(0x0900, QObject::tr("English (USA) - Heather. Quality 50." ));
  map.insert(0x0901, QObject::tr("English (USA) - Laura. Quality 50."   ));
  map.insert(0x0902, QObject::tr("English (USA) - Ryan. Quality 50."    ));
  map.insert(0x0A00, QObject::tr("Finnish - Sanna. Quality 50."         ));
  map.insert(0x0B00, QObject::tr("French - Alice. Quality 50."          ));
  map.insert(0x0B01, QObject::tr("French - Bruno. Quality 50."          ));
  map.insert(0x0B02, QObject::tr("French - Claire. Quality 50."         ));
  map.insert(0x0B03, QObject::tr("French - Julie. Quality 50."          ));
  map.insert(0x0C00, QObject::tr("French (CAN) - Louise. Quality 50."   ));
  map.insert(0x0E00, QObject::tr("Italian - Chiara. Quality 50."        ));
  map.insert(0x0E01, QObject::tr("Italian - Vittorio. Quality 50."      ));
  map.insert(0x0F00, QObject::tr("Dutch (NL) - Femke. Quality 50."      ));
  map.insert(0x0F01, QObject::tr("Dutch (NL) - Max. Quality 50."        ));
  map.insert(0x1000, QObject::tr("Norwegian - Kari. Quality 50."        ));
  map.insert(0x1001, QObject::tr("Norwegian - Olav. Quality 50."        ));
  map.insert(0x1100, QObject::tr("Polish - Ania. Quality 50."           ));
  map.insert(0x1200, QObject::tr("Portuguese - Celia. Quality 50."      ));
  map.insert(0x1400, QObject::tr("Swedish - Elin. Quality 50."          ));
  map.insert(0x1401, QObject::tr("Swedish - Emma. Quality 50."          ));
  map.insert(0x1402, QObject::tr("Swedish - Erik. Quality 50."          ));
  map.insert(0x1500, QObject::tr("Spanish - Maria. Quality 50."         ));
  map.insert(0x1600, QObject::tr("Spanish (USA) - Rosa. Quality 50."    ));
  map.insert(0x1700, QObject::tr("Czech - Eliska. Quality 50."          ));
  return map;
}

static QMap<quint16, QString> initCodepages() {
  QMap<quint16, QString> map;
  map.insert(0x0100, QObject::tr("windows-1256")); // Arabic (SA) - Salma. Quality 50.
  map.insert(0x0101, QObject::tr("windows-1256")); // Arabic (SA) - Youssef. Quality 50.
  map.insert(0x0200, QObject::tr("windows-1252")); // Dutch (BE) - Sofie. Quality 50.
  map.insert(0x0400, QObject::tr("windows-1252")); // Danish - Mette. Quality 50.
  map.insert(0x0500, QObject::tr("windows-1252")); // German - Klaus. Quality 50.
  map.insert(0x0501, QObject::tr("windows-1252")); // German - Sarah. Quality 50.
  map.insert(0x0600, QObject::tr("windows-1252")); // English (UK) - Graham. Quality 50.
  map.insert(0x0801, QObject::tr("windows-1252")); // English (UK) - Lucy. Quality 50.
  map.insert(0x0802, QObject::tr("windows-1252")); // English (UK) - Peter. Quality 50.
  map.insert(0x0803, QObject::tr("windows-1252")); // English (UK) - Rachel. Quality 50.
  map.insert(0x0900, QObject::tr("windows-1252")); // English (USA) - Heather. Quality 50.
  map.insert(0x0901, QObject::tr("windows-1252")); // English (USA) - Laura. Quality 50.
  map.insert(0x0902, QObject::tr("windows-1252")); // English (USA) - Ryan. Quality 50.
  map.insert(0x0A00, QObject::tr("windows-1252")); // Finnish - Sanna. Quality 50.
  map.insert(0x0B00, QObject::tr("windows-1252")); // French - Alice. Quality 50.
  map.insert(0x0B01, QObject::tr("windows-1252")); // French - Bruno. Quality 50.
  map.insert(0x0B02, QObject::tr("windows-1252")); // French - Claire. Quality 50.
  map.insert(0x0B03, QObject::tr("windows-1252")); // French - Julie. Quality 50.
  map.insert(0x0C00, QObject::tr("windows-1252")); // French (CAN) - Louise. Quality 50.
  map.insert(0x0E00, QObject::tr("windows-1252")); // Italian - Chiara. Quality 50.
  map.insert(0x0E01, QObject::tr("windows-1252")); // Italian - Vittorio. Quality 50.
  map.insert(0x0F00, QObject::tr("windows-1252")); // Dutch (NL) - Femke. Quality 50.
  map.insert(0x0F01, QObject::tr("windows-1252")); // Dutch (NL) - Max. Quality 50.
  map.insert(0x1000, QObject::tr("windows-1252")); // Norwegian - Kari. Quality 50.
  map.insert(0x1001, QObject::tr("windows-1252")); // Norwegian - Olav. Quality 50.
  map.insert(0x1100, QObject::tr("windows-1250")); // Polish - Ania. Quality 50.
  map.insert(0x1200, QObject::tr("windows-1252")); // Portuguese - Celia. Quality 50.
  map.insert(0x1400, QObject::tr("windows-1252")); // Swedish - Elin. Quality 50.
  map.insert(0x1401, QObject::tr("windows-1252")); // Swedish - Emma. Quality 50.
  map.insert(0x1402, QObject::tr("windows-1252")); // Swedish - Erik. Quality 50.
  map.insert(0x1500, QObject::tr("windows-1252")); // Spanish - Maria. Quality 50.
  map.insert(0x1600, QObject::tr("windows-1252")); // Spanish (USA) - Rosa. Quality 50.
  map.insert(0x1700, QObject::tr("windows-1250")); // Czech - Eliska. Quality 50.
  return map;
}

const QMap<quint16, QString> PortListener::languages = initLanguages();
const QMap<quint16, QString> PortListener::codepages = initCodepages();

PortListener::PortListener(const QString & portName, QTextEdit *i_textOut, QTextEdit *i_plainTextOut)
: m_port(NULL)
, m_textOut(i_textOut)
, m_plainTextOut(i_plainTextOut)
, m_receivedBytes()
, m_indicationTimeout(this)
, m_volume(-1)
, m_volumeReq(-1)
, m_language(-1)
, m_languageReq(-1)
, m_fileSend(NULL)
{
  m_port = new QextSerialPort(portName, QextSerialPort::EventDriven);
  m_port->setBaudRate(BAUD38400);
  m_port->setFlowControl(FLOW_OFF);
  m_port->setDataBits(DATA_8);
  m_port->setParity(PAR_NONE);
  m_port->setStopBits(STOP_1);

  if(m_port->open(QIODevice::ReadWrite) == true) {
    connect(m_port, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(m_port, SIGNAL(dsrChanged(bool)), this, SLOT(onDsrChanged(bool)));
    m_textOut->append(tr("Listening for data on %1").arg(m_port->portName()));
  } else {
    m_textOut->append(tr("Device failed to open: %1").arg(m_port->errorString()));
  }

  m_indicationTimeout.setSingleShot(true);
  m_indicationTimeout.setInterval(30000);
  connect(&m_indicationTimeout, SIGNAL(timeout()), this, SLOT(indicationTimeout()));

  m_fileSend = new BSMFileSend(m_port, m_textOut);
}

PortListener::~PortListener()
{
  if(m_port->isOpen())
    m_port->close();

  delete m_port;
  delete m_fileSend;
}

void PortListener::sendHexMessage(const QString &i_text)
{
  QByteArray message = QByteArray::fromHex(i_text.toLocal8Bit());
  m_port->write(message);
  m_port->flush();
}

void PortListener::speakText(const QString &i_text)
{
  QString cp = codepages.value(m_language);
  QTextCodec *codec = QTextCodec::codecForName(cp.toLatin1());

  if(!codec) {
    m_textOut->append(tr("Invalid Language set on Board"));
    return;
  }

  QByteArray data = codec->fromUnicode(i_text);

  BSMCommand message;
  data.append(static_cast<char>(0x00));
  message.makeCommand(BSMCommand::SAPI_ACAPELLA, data);
  m_port->write(message);
  m_port->flush();
}

void PortListener::SOS(const QString &i_text)
{
  QByteArray data;

  BSMCommand message;
  data.append(static_cast<char>(0x09));
  data.append(i_text.toAscii());
  data.append(static_cast<char>(0x00));
  message.makeCommand(BSMCommand::SAPI_BONES, data);
  m_port->write(message);
  m_port->flush();
}

void PortListener::onReadyRead()
{
  int oldBytes = m_receivedBytes.size();
  int newBytes = m_port->bytesAvailable();
  m_receivedBytes.resize(oldBytes + newBytes);
  m_port->read(m_receivedBytes.data() + oldBytes, newBytes);
  qDebug() << "bytes read (" << newBytes << "): " << m_receivedBytes.toHex();

  do {
    // Each BSM message must beginn with 0x7e
    QByteArray plainText;
    while(m_receivedBytes.count() && 0x7e != m_receivedBytes[0]) {
      plainText.append(m_receivedBytes[0]);
      m_receivedBytes.remove(0,1);
    }

    if(plainText.count()) {
      qDebug() << "Plain Text Message: " << plainText;
      QColor col = m_plainTextOut->textColor();
      m_plainTextOut->setTextColor(Qt::blue);
      m_plainTextOut->moveCursor(QTextCursor::End);
      m_plainTextOut->insertPlainText(plainText);
      m_plainTextOut->setTextColor(col);
      m_plainTextOut->ensureCursorVisible();
    }

    if(m_receivedBytes.count()<=1)
      return;

    // Subsequent 0x7e can be ignored
    while(0x7e == m_receivedBytes[1])
      m_receivedBytes.remove(1,1);

    for(int i=2; i<m_receivedBytes.size(); ++i) {
      if(0x7e == m_receivedBytes[i]) {
        parseMessage(m_receivedBytes.left(i+1));
        m_receivedBytes = m_receivedBytes.right(m_receivedBytes.size()-(i+1));
        break;
      }
    }
  } while(m_receivedBytes.size()>1 && 0x7e == m_receivedBytes[m_receivedBytes.size()-1]);
}

void PortListener::onDsrChanged(bool status)
{
  if(status)
    qDebug() << "device was turned on";
  else
    qDebug() << "device was turned off";
}

ulong PortListener::getLastError()
{
  return m_port->lastError();
}

bool PortListener::isOpen()
{
  return m_port->isOpen();
}

void PortListener::parseMessage(const BSMCommand &message)
{
  QByteArray data;

  switch(message[1]) { // Look for SAPI
  case BSMCommand::SAPI_ACAPELLA: { // Acapela
      if(message.checkMessage(data)) {
        switch(data[0]) {
        case 0x00:
          m_textOut->append(tr("Speak - failed"));
          return;
        case 0x01:
          m_textOut->append(tr("Speak - confirmation received"));
          m_indicationTimeout.start();
          emit waitOnIndication(true);
          return;
        case 0x02:
          m_textOut->append(tr("Speak - indication received"));
          m_indicationTimeout.stop();
          emit waitOnIndication(false);
          return;
        default:
          ;
        }
      }
    }
    break;
  case BSMCommand::SAPI_BONES: { // Basic Controlling
      if(message.checkMessage(data)) {
        switch(data[0]) {
        case 0x00:
          m_textOut->append(tr("BSM Software Version %1").arg(data.data()+1));
          return;
        case 0x01:
          m_textOut->append(tr("Module Idle"));
          return;
        case 0x06:
          switch(data[1]) {
          case 0x11:
            if(0x01 == data[2]) {
              m_fileSend->sendFile(true);
            } else {
              m_textOut->append(tr("Send File - Error"));
              m_fileSend->sendFile(false);
            }
            return;
          case 0x22:
            if(0x01 == data[2]) {
              m_fileSend->sendFile(true);
            } else {
              m_textOut->append(tr("Send File - Error"));
              m_fileSend->sendFile(false);
            }
            return;
          case 0x33:
            if(0x01 == data[2]) {
              m_textOut->append(tr("Send File - Module checking CRC"));
              m_indicationTimeout.start();
              emit waitOnIndication(true);
            } else {
              m_textOut->append(tr("Send File - Error CRC"));
              m_indicationTimeout.stop();
              emit waitOnIndication(false);
            }
            return;
          case 0x34:
            if(0x01 == data[2]) {
              m_textOut->append(tr("Send File - CRC OK"));
              m_indicationTimeout.stop();
              emit waitOnIndication(false);
            } else {
              m_textOut->append(tr("Send File - Error CRC"));
              m_indicationTimeout.stop();
              emit waitOnIndication(false);
            }
            return;
          }
        case 'B':
        case 'b':
          m_textOut->append(tr("BSM Software Version B%1").arg(data.data()+1));
          return;
        case 0x09:
          m_textOut->append(tr("SOS Command Executed"));
          return;
        case 0xFF:
          m_textOut->append(tr("Module Ready"));
          return;
        default:
          ;
        }
      }
    }
    break;
  case BSMCommand::SAPI_LANGUAGE: { // Language Controlling
      if(message.checkMessage(data)) {
        switch(data[0]) {
        case 0x00:
          if(0x01 == data[1]) {
            m_textOut->append(tr("Set Language - confirmation received"));
            m_indicationTimeout.start();
            emit waitOnIndication(true);
          } else {
            m_textOut->append(tr("Set Language - failed"));
          }
          return;
        case 0x01:
          if(0xff == data[1] && 0xff == data[2]) {
            m_textOut->append(tr("Invalid Language set on Board"));
          } else {
            quint16 code = (static_cast<quint16>(data[1]) << 8) + static_cast<quint16>(data[2]);
            m_language = code;
            emit languageChanged(languages.keys().indexOf(code));
          }
          return;
        case 0x04:
          if(m_languageReq >= 0) {
            m_indicationTimeout.stop();
            m_textOut->append(tr("Set Language - indication received"));
            sendHexMessage("7e020304016d7e"); // Response = ACK
            emit waitOnIndication(false);
            m_language = m_languageReq;
            m_languageReq = -1;
          } else {
            // Language Changed On Board
            m_textOut->append(tr("Set Language - Language changed on Board"));
            getLanguage();
          }
          return;
        }
      }
    }
    break;
  case BSMCommand::SAPI_SOUND: { // Sound Controlling
      if(message.checkMessage(data)) {
        switch(data[0]) {
        case 0x01:
          if(0x01 == data[1]) {
            m_textOut->append(tr("Play WAV - confirmation received"));
            emit waitOnIndication(true);
          } else if(0x02 == data[1]) {
            m_textOut->append(tr("Play WAV - indication received"));
            emit waitOnIndication(false);
          } else {
            m_textOut->append(tr("Play WAV - failed"));
            emit waitOnIndication(false);
          }
          return;
        case 0x09:
          if(0x01 == data[1]) {
            m_textOut->append(tr("Set Volume - confirmation received"));
            m_volume = m_volumeReq;
          } else {
            m_textOut->append(tr("Set Volume - failed"));
            m_volumeReq = -1;
          }
          return;
        case 0x0a:
          m_volume = data[1];
          emit volumeChanged(data[1]-155);
          return;
        default:
          ;
        }
      }
    }
  default:
    ;
  }
  m_textOut->append(message.toHex());
}

void PortListener::setVolume(int i_volume)
{
  i_volume+=155;
  if(i_volume != m_volume) {
    BSMCommand cmd;
    QByteArray data;
    data.append(0x09);
    data.append(i_volume);
    cmd.makeCommand(BSMCommand::SAPI_SOUND, data);
    m_port->write(cmd);
    m_port->flush();
    m_volumeReq = i_volume;
  }
}

void PortListener::getVolume()
{
  BSMCommand cmd;
  QByteArray data;
  data.append(0x0A);
  cmd.makeCommand(BSMCommand::SAPI_SOUND, data);
  m_port->write(cmd);
  m_port->flush();
}

void PortListener::getLanguage()
{
  BSMCommand cmd;
  QByteArray data;
  data.append(0x01);
  cmd.makeCommand(BSMCommand::SAPI_LANGUAGE, data);
  m_port->write(cmd);
  m_port->flush();
}

void PortListener::playAndPause()
{
  BSMCommand cmd;
  QByteArray data;
  data.append(0x01);
  data.append(QString("1.WAV"));
  data.append(static_cast<char>(0x00));
  cmd.makeCommand(BSMCommand::SAPI_SOUND, data);
  m_port->write(cmd);
  m_port->flush();
}

void PortListener::setLanguage(int i_language)
{
  if(i_language<0)
    return;

  int language = languages.keys().at(i_language);

  if(language != m_language) {
    BSMCommand cmd;
    QByteArray data;
    data.append(static_cast<char>(0x00));
    data.append(static_cast<char>((language&0xff00)>>8));
    data.append(static_cast<char>(language&0x00ff));
    cmd.makeCommand(BSMCommand::SAPI_LANGUAGE, data);
    m_port->write(cmd);
    m_port->flush();
    m_languageReq = language;
  }
}

void PortListener::indicationTimeout()
{
  m_textOut->append(tr("Wait on indication - timeout"));
  emit waitOnIndication(false);
}

void PortListener::sendFile(const QString &i_path)
{
  m_fileSend->openFile(i_path);
}
