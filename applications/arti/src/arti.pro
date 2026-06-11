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
TOP       = $$PWD/../../..
DIST      = $$TOP/../distribution
LIBRARIES = $$TOP/libraries
ICONS     = $$TOP/resources/icons 
STYLES    = $$TOP/resources/styles 

TEMPLATE     = app
CONFIG      += qt warn_on exceptions debug_and_release c++17
win32:CONFIG += console
TARGET       = arti
DESTDIR      = $$DIST/bin
OBJECTS_DIR  = .obj
MOC_DIR      = .moc
UI_DIR       = .ui
RCC_DIR      = .rcc

QT          += core gui widgets concurrent


INCLUDEPATH += \
   dialogs \
   $$PWD

RESOURCES += \
   ../icons/arti.qrc \
   $$ICONS/basic/basic_icons.qrc \
   $$ICONS/custom/custom_icons.qrc \
   $$ICONS/development/development_icons.qrc \
   $$ICONS/word_processing/word_processing_icons.qrc \
   $$STYLES/styles.qrc

HEADERS += \
   main_page.h \
   arti_cmd.h \
   arti_gui.h \
   dialogs/preferences_dialog.h \
   help_page.h \
   options_parser.h \
   settings_p.h \
   revision.h \
   preview.h \
   template_list.h

SOURCES += \
   main_page.cpp \
   arti_cmd.cpp \
   arti_gui.cpp \
   dialogs/preferences_dialog.cpp \
   help_page.cpp \
   main.cpp \
   options_parser.cpp \
   preview.cpp \
   template_list.cpp

FORMS += \
   main_page.ui \
   arti_gui.ui \
   dialogs/preferences_dialog.ui \
   help_page.ui \
   preview.ui

include($$LIBRARIES/commonlib/src/commonlib.pri)
include($$LIBRARIES/texteditlib/src/texteditlib.pri)
include($$LIBRARIES/jsonlib/src/jsonlib.pri)
include($$LIBRARIES/artilib/src/artilib.pri)
