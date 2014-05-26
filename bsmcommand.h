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

#ifndef BSMCOMMAND_H
#define BSMCOMMAND_H

#include "bsmcommand.h"

#include <QByteArray>
#include <QMap>

#define MAX_FRAME_LEN 517   // Flag + Adress + Control + 512-Byte-Information-Max + CRC + Flag
#define FILE_MAX      272   // BSM Length for all file names including path.

class BSMCommand : public QByteArray {
public:
  typedef enum {
    SAPI_ACAPELLA = 0x00,
    SAPI_BONES    = 0x01,
    SAPI_LANGUAGE = 0x02,
    SAPI_SOUND    = 0x03
  } SAPI;

  BSMCommand();
  BSMCommand(const QByteArray &i_other);

  int checkMessage(QByteArray &o_data) const;
  int makeCommand(SAPI i_sapi, const QByteArray &i_data);
  static unsigned char maximCRC8(const QByteArray &i_data);

private:
  /**
   * crc array from the Maxim ApNote
   * \see http://www.maxim-ic.com/app-notes/index.mvp/id/27
   * \see http://www.dattalo.com/technical/software/pic/crc_8bit.c
   */
  static const unsigned char crc_array[256];
};

#endif // BSMCOMMAND_H
