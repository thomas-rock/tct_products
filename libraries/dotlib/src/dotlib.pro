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
TARGET       = dotlib
OBJECTS_DIR  = .obj
MOC_DIR      = .moc
UI_DIR       = .ui
RCC_DIR      = .rcc

QT       += core gui widgets

LIBS += -L$$DIST/lib -L$$DIST/lib/3rdparty/graphviz -lgvc -lcgraph -lcdt

INCLUDEPATH += \ 
   $$SRC/3rdparty/graphviz/include \
   $$PWD

RESOURCES += \
   $$SRC/styles/styles.qrc \
   $$SRC/icons/basic/basic_icons.qrc \
   $$SRC/icons/custom/custom_icons.qrc \
   $$SRC/icons/development/development_icons.qrc \
   $$SRC/icons/word_processing/word_processing_icons.qrc

DEFINES += DOTLIB_LIBRARY

HEADERS += \
   $$PWD/findPanel.h \
   $$PWD/sceneWidget.h \
   $$PWD/viewerWidget.h \
   $$PWD/viewerTab.h \
   $$PWD/dotView.h \
   $$PWD/dotlib_global.h \
   $$PWD/settings_p.h

SOURCES += \
   $$PWD/findPanel.cpp \
   $$PWD/sceneWidget.cpp \
   $$PWD/viewerWidget.cpp \
   $$PWD/viewerTab.cpp \
   $$PWD/dotView.cpp
   
FORMS += \
   $$PWD/dotView.ui \
   $$PWD/findPanel.ui
   
# other libraries to reference
include($$LIBRARIES/widgetlib/widgetlib.pri)

