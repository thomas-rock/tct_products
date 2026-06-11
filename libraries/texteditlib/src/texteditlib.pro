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
TOP       = $$PWD/../../..
DIST      = $$TOP/../distribution
LIBRARIES = $$TOP/libraries
FONTS     = $$TOP/resources/fonts
ICONS     = $$TOP/resources/icons
STYLES    = $$TOP/resources/styles

TEMPLATE     = lib
CONFIG      += dll qt warn_on exceptions debug_and_release c++17
DESTDIR      = $$DIST/lib
TARGET       = texteditlib
OBJECTS_DIR  = .obj
MOC_DIR      = .moc
UI_DIR       = .ui
RCC_DIR      = .rcc

QT       += gui widgets printsupport

LIBS += -L$$DIST/lib/3rdparty/qscintilla -lqscintilla2_qt6d

DEFINES += TEXTEDITLIB_LIBRARY
DEFINES += QSCINTILLA_DLL

RESOURCES += \
   $$FONTS/textedit_fonts.qrc \
   $$ICONS/basic/basic_icons.qrc \
   $$ICONS/custom/custom_icons.qrc \
   $$ICONS/development/development_icons.qrc \
   $$ICONS/word_processing/word_processing_icons.qrc \
   ../icons/textedit_icons.qrc
   $$STYLES/styles.qrc \

INCLUDEPATH += \
   $$TOP/3rdparty/QScintilla_src-2.14.1/src \
   $$PWD/dialogs \
   $$PWD

HEADERS += \
   dialogs/color_button.h \
   dialogs/goto_dialog.h \
   dialogs/preferences_dialog.h \
   lexers/lexer_arti.h \
   lexers/lexer_bnf.h \
   lexers/lexer_ini.h \
   lexers/lexer_text.h \
   lexer_manager.h \
   editor_widget.h \
   editor_tab.h \
   editor_settings_p.h \
   text_edit.h \
   texteditlib_global.h

SOURCES += \
   dialogs/color_button.cpp \
   dialogs/goto_dialog.cpp \
   dialogs/preferences_dialog.cpp \
   dialogs/preferences_general.cpp \
   dialogs/preferences_styles.cpp \
   lexers/lexer_arti.cpp \
   lexers/lexer_bnf.cpp \
   lexers/lexer_ini.cpp \
   lexers/lexer_text.cpp \
   lexer_manager.cpp \
   editor_widget.cpp \
   editor_tab.cpp \
   text_edit.cpp

FORMS += \
   dialogs/goto_dialog.ui \
   dialogs/preferences_dialog.ui \
   text_edit.ui

include($$LIBRARIES/commonlib/src/commonlib.pri)
include($$LIBRARIES/jsonlib/src/jsonlib.pri)
include($$LIBRARIES/artilib/src/artilib.pri)