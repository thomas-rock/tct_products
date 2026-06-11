##=============================================================================
## RockSolid Logic Corporation                    http://www.RockSolidLogic.com
##
##   This confidential and proprietary code may be used only as
##   authorized by a licensing agreement from RockSolid Logic.
##   In the event of publication, the following notice is applicable:
##
##        COPYRIGHT © 2011 to the present, RockSolid Logic
##        ALL RIGHTS RESERVED
##
##   The entire notice above must be reproduced on all authorized copies.
##
##=============================================================================
## Last checked in by $Author::  $   $Revision: $
##
## $Date:  $
##=============================================================================
## $Id:  $
##=============================================================================
##
## Description :
##
## Notes :
##
##=============================================================================
TOP          = $$PWD/../../..
DIST         = $$TOP/../distribution
LIBRARIES    = $$TOP/libraries
ICONS        = $$TOP/icons
STYLES       = $$TOP/styles 

TEMPLATE     = lib
CONFIG      += dll qt warn_on exceptions debug_and_release c++17
DESTDIR      = $$DIST/lib
TARGET       = jsonlib
OBJECTS_DIR  = .obj
MOC_DIR      = .moc
UI_DIR       = .ui
RCC_DIR      = .rcc

QT          += core 

LIBS += -L$$DIST/lib 

INCLUDEPATH += \
   $$PWD

DEFINES += JSONLIB_LIBRARY

HEADERS += \
   jsonlib_global.h  \
   json5_parser.h 

SOURCES += \
   json5_parser.cpp

include($$LIBRARIES/commonlib/src/commonlib.pri)
