#=============================================================================
# Treasure Coast Technologies, Inc.          www.TreasureCoastTechnologies.com
#
#   This confidential and proprietary code may be used only as
#   authorized by a licensing agreement from RockSolid Solutions.
#   In the event of publication, the following notice is applicable:
#
#        COPYRIGHT © 2026 to the present, Treasure Coast Technologies, Inc.
#        ALL RIGHTS RESERVED
#
#   The entire notice above must be reproduced on all authorized copies.
#
#=============================================================================
#
# Description :
#
#=============================================================================
TOP          = $$PWD/../../..
DIST         = $$TOP/../distribution
LIBRARIES    = $$TOP/libraries

TEMPLATE     = lib
CONFIG      += dll qt warn_on exceptions debug_and_release c++17
DESTDIR      = $$DIST/lib
TARGET       = artilib
OBJECTS_DIR  = .obj
MOC_DIR      = .moc
UI_DIR       = .ui
RCC_DIR      = .rcc

QT       += core qml

LIBS += -L$$DIST/lib 

INCLUDEPATH += \
   $$PWD 

DEFINES += ARTILIB_LIBRARY

HEADERS += \
   artilib_global.h \
   arti_model.h \
   doc.h \
   engine.h \
   parser.h \
   renderer.h

SOURCES += \
   arti_model.cpp \
   doc.cpp \
   engine.cpp \
   parser.cpp \
   renderer.cpp

# other libraries to reference
include($$LIBRARIES/commonlib/src/commonlib.pri)
include($$LIBRARIES/jsonlib/src/jsonlib.pri)
