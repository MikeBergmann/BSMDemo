#  Copyright 2013 Mike Bergmann, Bones AG
#
#  This file is part of BSMDemo.
#
#  BSMDemo is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 2 of the License, or
#  (at your option) any later version.
#
#  BSMDemo is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with BSMDemo.  If not, see <http://www.gnu.org/licenses/>.

QMAKE_CXXFLAGS += -funsigned-char

QT       += core gui

TARGET = BSMDemo
TEMPLATE = app

#DEFINES+=FTDI_ONLY

SOURCES += main.cpp\
    mainwindow.cpp \
    PortListener.cpp \
    bsmcommand.cpp \
    bsmfilesend.cpp

HEADERS  += mainwindow.h \
    PortListener.h \
    bsmcommand.h \
    bsmfilesend.h

FORMS    += mainwindow.ui

CONFIG(debug, debug|release):LIBS  += -lqextserialport-1.2d
else:LIBS  += -lqextserialport-1.2
win32:LIBS += -lsetupapi

RESOURCES += \
    BSMDemo.qrc
