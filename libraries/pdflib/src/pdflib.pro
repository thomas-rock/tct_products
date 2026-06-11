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
SRC          = $$PWD/../..
LIBRARIES    = $$SRC/libraries
DIST         = $$SRC/../distribution

TEMPLATE     = lib
CONFIG      += dll qt warn_on exceptions debug_and_release c++17
DESTDIR      = $$DIST/lib
TARGET       = pdflib
OBJECTS_DIR  = .obj
MOC_DIR      = .moc
UI_DIR       = .ui
RCC_DIR      = .rcc

QT       += core gui widgets pdfwidgets

INCLUDEPATH += \ 
   $$PWD

RESOURCES += \
   $$SRC/styles/styles.qrc \
   $$SRC/icons/basic/basic_icons.qrc \
   $$SRC/icons/custom/custom_icons.qrc \
   $$SRC/icons/development/development_icons.qrc \
   $$SRC/icons/word_processing/word_processing_icons.qrc

DEFINES += PDFLIB_LIBRARY

HEADERS += \
   $$PWD/pdflib_global.h \
   $$PWD/viewerWidget.h \
   $$PWD/viewerTab.h \
   $$PWD/pdfView.h

SOURCES += \
   $$PWD/viewerWidget.cpp \
   $$PWD/viewerTab.cpp \
   $$PWD/pdfView.cpp

FORMS += \
   $$PWD/pdfView.ui


include($$LIBRARIES/widgetlib/widgetlib.pri)
