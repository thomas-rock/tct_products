##=============================================================================
## RockSolid Logic Corporation                    http://www.RockSolidLogic.com
##
##   This confidential and proprietary code may be used only as
##   authorized by a licensing agreement from RockSolid Logic.
##   In the event of publication, the following notice is applicable:
##
##        COPYRIGHT © 2020 to the present, RockSolid Logic
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
TOP       = $$PWD/../../..
DIST      = $$TOP/../distribution
LIBRARIES = $$TOP/libraries
ICONS     = $$TOP/resources/icons 
STYLES    = $$TOP/resources/styles 

TEMPLATE     = app
CONFIG      += qt warn_on exceptions debug_and_release c++17
win32:CONFIG += console
TARGET       = slate
DESTDIR      = $$DIST/bin
OBJECTS_DIR  = .obj
MOC_DIR      = .moc
UI_DIR       = .ui
RCC_DIR      = .rcc

QT          += core gui widgets concurrent

LIBS += -L$$DIST/lib

INCLUDEPATH += \
   $$PWD

RESOURCES += \
   $$ICONS/basic/basic_icons.qrc \
   $$ICONS/custom/custom_icons.qrc \
   $$ICONS/development/development_icons.qrc \
   $$ICONS/word_processing/word_processing_icons.qrc \
   $$STYLES/styles.qrc

HEADERS += \
   settings_p.h \
   revision.h \
   help_page.h \
   slate.h

SOURCES += \
   help_page.cpp \
   slate.cpp \
   main.cpp

FORMS += \
   help_page.ui \
   slate.ui

include($$LIBRARIES/texteditlib/src/texteditlib.pri)
#include($$LIBRARIES/hexeditlib/hexeditlib.pri)
# include($$LIBRARIES/pdflib/pdflib.pri)
# include($$LIBRARIES/rtfeditlib/rtfeditlib.pri)
# include($$LIBRARIES/imagelib/imagelib.pri)
# include($$LIBRARIES/dotlib/dotlib.pri)
# include($$LIBRARIES/chartlib/chartlib.pri)
