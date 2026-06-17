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

TEMPLATE     = app
CONFIG      += qt warn_on exceptions debug_and_release c++17
win32:CONFIG += console
TARGET       = prism
DESTDIR      = $$DIST/bin
OBJECTS_DIR  = .obj
MOC_DIR      = .moc
UI_DIR       = .ui
RCC_DIR      = .rcc

QT       += core gui widgets

INCLUDEPATH += \
   $$PWD

HEADERS += \
   prism.h \
   revision.h \
   settings_p.h \
   bit_renderer.h \
   bus_renderer.h \
   cursor_line_item.h \
   cursor_track_scene.h \
   cursor_track_view.h \
   cursor_track_widget.h \
   fixed_row_delegate.h \
   iwave_importer.h \
   iwave_renderer.h \
   prism.h \
   signal_tree_widget.h \
   time_ruler_scene.h \
   time_ruler_view.h \
   time_ruler_widget.h \
   vcd_importer.h \
   vcd_parser.h \
   wave_display_widget.h \
   wave_graphics_view.h \
   wave_query_ast.h \
   wave_query_engine.h \
   wave_query_parser.h \
   wave_scene.h \
   wave_track_item.h \
   wave_value_utils.h \
   waveform_cursor.h \
   waveform_model.h \
   waveform_view_widget.h

SOURCES += \
   main.cpp   \
   bit_renderer.cpp \
   bus_renderer.cpp \
   cursor_line_item.cpp \
   cursor_track_scene.cpp \
   cursor_track_view.cpp \
   cursor_track_widget.cpp \
   prism.cpp \
   signal_tree_widget.cpp \
   time_ruler_scene.cpp \
   time_ruler_view.cpp \
   time_ruler_widget.cpp \
   vcd_importer.cpp \
   vcd_parser.cpp \
   wave_display_widget.cpp \
   wave_graphics_view.cpp \
   wave_query_engine.cpp \
   wave_query_parser.cpp \
   wave_scene.cpp \
   wave_track_item.cpp \
   waveform_view_widget.cpp

FORMS += \
   prism.ui 

include($$LIBRARIES/commonlib/src/commonlib.pri)

