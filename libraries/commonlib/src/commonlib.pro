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
ICONS        = $$TOP/resources/icons
STYLES       = $$TOP/resources/styles

TEMPLATE     = lib
CONFIG      += dll qt warn_on exceptions debug_and_release c++17
DESTDIR      = $$DIST/lib
TARGET       = commonlib
OBJECTS_DIR  = .obj
MOC_DIR      = .moc
UI_DIR       = .ui
RCC_DIR      = .rcc

QT          += core gui widgets

LIBS += -L$$DIST/lib 

RESOURCES += \
   $$ICONS/basic/basic_icons.qrc \
   $$ICONS/development/development_icons.qrc \
   $$ICONS/custom/custom_icons.qrc \
   $$ICONS/word_processing/word_processing_icons.qrc

INCLUDEPATH += \
   widgets/color_button \
   widgets/define \
   widgets/dialogs \
   widgets/directory_list \
   widgets/find_replace_panel \
   widgets/include_path \
   widgets/line_edit \
   widgets/transcript \
   widgets/tree_widget \
   utility \
   $$PWD

DEFINES += COMMONLIB_LIBRARY

HEADERS += \
   commonlib_global.h  \
   widgets/color_button/color_button.h \
   widgets/define/define.h \
   widgets/define/define_dialog.h \
   widgets/dialogs/summary_dialog.h \
   widgets/directory_list/directory_list.h \
   widgets/find_replace_panel/find_replace_panel.h \
   widgets/include_path/include_path.h \
   widgets/line_edit/line_edit.h \
   widgets/transcript/transcript_base.h \
   widgets/transcript/list_transcript.h \
   widgets/transcript/text_transcript.h \
   widgets/tree_widget/tree_widget.h \
   utility/command_line_parser.h \
   utility/message.h \
   utility/message_preferences.h


SOURCES += \
   widgets/color_button/color_button.cpp \
   widgets/define/define.cpp \
   widgets/define/define_dialog.cpp \
   widgets/dialogs/summary_dialog.cpp \
   widgets/directory_list/directory_list.cpp \
   widgets/find_replace_panel/find_replace_panel.cpp \
   widgets/include_path/include_path.cpp \
   widgets/line_edit/line_edit.cpp \
   widgets/transcript/transcript_base.cpp \
   widgets/transcript/list_transcript.cpp \
   widgets/transcript/text_transcript.cpp \
   widgets/tree_widget/tree_widget.cpp \
   utility/command_line_parser.cpp \
   utility/message.cpp \
   utility/message_preferences.cpp


FORMS += \
   widgets/define/define.ui \
   widgets/define/define_dialog.ui \
   widgets/dialogs/summary_dialog.ui \
   widgets/find_replace_panel/find_replace_panel.ui \
   widgets/include_path/include_path.ui \
   utility/message_preferences.ui

