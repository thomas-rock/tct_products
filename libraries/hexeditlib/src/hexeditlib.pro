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
SRC       = $$PWD/../..
LIBRARIES = $$SRC/libraries
DIST      = $$SRC/../distribution

TEMPLATE     = lib
CONFIG      += dll qt warn_on exceptions debug_and_release c++17
DESTDIR      = $$DIST/lib
TARGET       = hexeditlib
OBJECTS_DIR  = .obj
MOC_DIR      = .moc
UI_DIR       = .ui
RCC_DIR      = .rcc

QT       += gui widgets printsupport

DEFINES += HEXEDITLIB_LIBRARY

INCLUDEPATH += \
   $$PWD/dialogs \
   $$PWD

RESOURCES += \
   $$SRC/styles/styles.qrc \
   $$SRC/icons/basic/basic_icons.qrc \
   $$SRC/icons/custom/custom_icons.qrc \
   $$SRC/icons/development/development_icons.qrc \
   $$SRC/icons/word_processing/word_processing_icons.qrc

HEADERS += \
   $$PWD/dialogs/colorButton.h \
   $$PWD/dialogs/preferencesDialog.h \
   $$PWD/dialogs/summaryDialog.h \
   $$PWD/chunks.h \
   $$PWD/commands.h \
   $$PWD/findReplacePanel.h \
   $$PWD/QHexEdit.h \
   $$PWD/editorWidget.h \
   $$PWD/editorSettings_p.h \
   $$PWD/editorTab.h \
   $$PWD/hexEdit.h \
   $$PWD/editorlib_global.h

SOURCES += \
   $$PWD/dialogs/colorButton.cpp \
   $$PWD/dialogs/preferencesDialog.cpp \
   $$PWD/dialogs/summaryDialog.cpp \
   $$PWD/chunks.cpp \
   $$PWD/commands.cpp \
   $$PWD/findReplacePanel.cpp \
   $$PWD/QHexEdit.cpp \
   $$PWD/editorWidget.cpp \
   $$PWD/editorTab.cpp \
   $$PWD/hexEdit.cpp

FORMS += \
   $$PWD/dialogs/preferencesDialog.ui \
   $$PWD/dialogs/summaryDialog.ui \
   $$PWD/findReplacePanel.ui \
   $$PWD/hexEdit.ui


