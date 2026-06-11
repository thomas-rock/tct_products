//=============================================================================
// X-Tek Corporation                                   http://www.x-tekcorp.com
//
//   This confidential and proprietary code may be used only as
//   authorized by a licensing agreement from X-Tek Corporation.
//   In the event of publication, the following notice is applicable:
//
//        COPYRIGHT © 2011 to the present, X-Tek Corporation
//        ALL RIGHTS RESERVED
//
//   The entire notice above must be reproduced on all authorized copies.
//
//=============================================================================
// Last checked in by $Author: thomas@x-tekcorp.com $   $Revision: 1255 $
//
// $Date: 2016-05-27 08:50:47 -0400 (Fri, 27 May 2016) $
//=============================================================================
// $Id: xTextEditPreferences_general.cpp 1255 2016-05-27 12:50:47Z thomas@x-tekcorp.com $
//=============================================================================
//
// Description :
//
// Notes :
//
//=============================================================================

#include "preferencesDialog.h"
#include "ui_preferencesDialog.h"
#include "editorSettings_p.h"

using namespace texteditLib;

void preferencesDialog::defaultGeneral ()
{
   _ui->_tabPolicyCombo->setCurrentIndex((TAB_POLICY_DEFAULT) ? 0 : 1);    // true = tabs, false = spaces
   _ui->_tabSizeSpin->setValue(TAB_SIZE_DEFAULT);
   _ui->_autoIndentCheck->setChecked(AUTO_INDENT_DEFAULT);
   _ui->_unindentCheck->setChecked(BACK_UNINDENT_DEFAULT);
   _ui->_autoPairCheck->setChecked(AUTO_PAIR_DEFAULT);
   _ui->_autoEncloseCheck->setChecked(AUTO_ENCLOSE_DEFAULT);
   _ui->_lineWrapCheck->setChecked(LINE_WRAP_DEFAULT);
   _ui->_rightEdgeCheck->setChecked(RIGHT_EDGE_DEFAULT != -1);
   _ui->_rightEdgeSpin->setValue((RIGHT_EDGE_DEFAULT == -1) ? 80 : RIGHT_EDGE_DEFAULT);

   _ui->_lineNumbersCheck->setChecked(LINE_NUMBERS_DEFAULT);
   _ui->_markersCheck->setChecked(MARKERS_DEFAULT);
   _ui->_changeBarsCheck->setChecked(CHANGE_BARS_DEFAULT);
   _ui->_indentationCheck->setChecked(INDENTATION_DEFAULT);
   _ui->_whitespaceCheck->setChecked(WHITESPACE_DEFAULT);
   _ui->_eolCheck->setChecked(END_OF_LINE_DEFAULT);
   _ui->_highlightCheck->setChecked(HIGHLIGHT_LINE_DEFAULT);
   _ui->_matchCheck->setChecked(MATCH_BRACES_DEFAULT);
   _ui->_markerCombo->setCurrentIndex(MARKER_ICON_DEFAULT);

   _ui->_bgButton->setCurrentColor(BACKGROUND_COLOR_DEFAULT);
//   _ui->_selectionButton->setCurrentColor(SELECTION_COLOR_DEFAULT);
   _ui->_highlightButton->setCurrentColor(HIGHLIGHT_COLOR_DEFAULT);
   _ui->_rightEdgeButton->setCurrentColor(RIGHT_EDGE_COLOR_DEFAULT);
   _ui->_marginFgButton->setCurrentColor(MARGIN_FG_COLOR_DEFAULT);
   _ui->_marginBgButton->setCurrentColor(MARGIN_BG_COLOR_DEFAULT);
   _ui->_indentationFgButton->setCurrentColor(INDENTATION_FG_COLOR_DEFAULT);
   _ui->_indentationBgButton->setCurrentColor(INDENTATION_BG_COLOR_DEFAULT);
   _ui->_matchedFgButton->setCurrentColor(MATCHED_FG_COLOR_DEFAULT);
   _ui->_matchedBgButton->setCurrentColor(MATCHED_BG_COLOR_DEFAULT);
   _ui->_unmatchedFgButton->setCurrentColor(UNMATCHED_FG_COLOR_DEFAULT);
   _ui->_unmatchedBgButton->setCurrentColor(UNMATCHED_BG_COLOR_DEFAULT);
   _ui->_whitespaceFgButton->setCurrentColor(WHITESPACE_FG_COLOR_DEFAULT);
   _ui->_whitespaceBgButton->setCurrentColor(WHITESPACE_BG_COLOR_DEFAULT);
}
void preferencesDialog::loadGeneral ()
{
   QSettings settings;
   settings.beginGroup(PREFERENCE_SETTINGS);

   _ui->_tabPolicyCombo->setCurrentIndex((settings.value(TAB_POLICY, TAB_POLICY_DEFAULT).toBool()) ? 0 : 1);      // true = tabs, false = spaces
   _ui->_tabSizeSpin->setValue(settings.value(TAB_SIZE, TAB_SIZE_DEFAULT).toInt());
   _ui->_autoIndentCheck->setChecked(settings.value(AUTO_INDENT, AUTO_INDENT_DEFAULT).toBool());
   _ui->_unindentCheck->setChecked(settings.value(BACK_UNINDENT, BACK_UNINDENT_DEFAULT).toBool());
   _ui->_autoPairCheck->setChecked(settings.value(AUTO_PAIR, AUTO_PAIR_DEFAULT).toBool());
   _ui->_autoEncloseCheck->setChecked(settings.value(AUTO_ENCLOSE, AUTO_ENCLOSE_DEFAULT).toBool());
   _ui->_lineWrapCheck->setChecked(settings.value(LINE_WRAP, LINE_WRAP_DEFAULT).toBool());
   int rightEdge = settings.value(RIGHT_EDGE, RIGHT_EDGE_DEFAULT).toInt();
   _ui->_rightEdgeCheck->setChecked(rightEdge != -1);
   if (rightEdge != -1)
      _ui->_rightEdgeSpin->setValue(rightEdge);

   _ui->_lineNumbersCheck->setChecked(settings.value(LINE_NUMBERS, LINE_NUMBERS_DEFAULT).toBool());
   _ui->_changeBarsCheck->setChecked(settings.value(CHANGE_BARS, CHANGE_BARS_DEFAULT).toBool());
   _ui->_markersCheck->setChecked(settings.value(MARKERS, MARKERS_DEFAULT).toBool());
   _ui->_indentationCheck->setChecked(settings.value(INDENTATION, INDENTATION_DEFAULT).toBool());
   _ui->_whitespaceCheck->setChecked(settings.value(WHITESPACE, WHITESPACE_DEFAULT).toBool());
   _ui->_eolCheck->setChecked(settings.value(END_OF_LINE, END_OF_LINE_DEFAULT).toBool());
   _ui->_highlightCheck->setChecked(settings.value(HIGHLIGHT_LINE, HIGHLIGHT_LINE_DEFAULT).toBool());
   _ui->_matchCheck->setChecked(settings.value(MATCH_BRACES, MATCH_BRACES_DEFAULT).toBool());
   _ui->_markerCombo->setCurrentIndex(settings.value(MARKER_ICON, MARKER_ICON_DEFAULT).toInt());

   _ui->_bgButton->setCurrentColor(settings.value(BACKGROUND_COLOR, BACKGROUND_COLOR_DEFAULT).toString());
//   _ui->_selectionButton->setCurrentColor(settings.value(SELECTION_COLOR, SELECTION_COLOR_DEFAULT).toString());
   _ui->_highlightButton->setCurrentColor(settings.value(HIGHLIGHT_COLOR, HIGHLIGHT_COLOR_DEFAULT).toString());
   _ui->_rightEdgeButton->setCurrentColor(settings.value(RIGHT_EDGE_COLOR, RIGHT_EDGE_COLOR_DEFAULT).toString());
   _ui->_marginFgButton->setCurrentColor(settings.value(MARGIN_FG_COLOR, MARGIN_FG_COLOR_DEFAULT).toString());
   _ui->_marginBgButton->setCurrentColor(settings.value(MARGIN_BG_COLOR, MARGIN_BG_COLOR_DEFAULT).toString());
   _ui->_indentationFgButton->setCurrentColor(settings.value(INDENTATION_FG_COLOR, INDENTATION_FG_COLOR_DEFAULT).toString());
   _ui->_indentationBgButton->setCurrentColor(settings.value(INDENTATION_BG_COLOR, INDENTATION_BG_COLOR_DEFAULT).toString());
   _ui->_matchedFgButton->setCurrentColor(settings.value(MATCHED_FG_COLOR, MATCHED_FG_COLOR_DEFAULT).toString());
   _ui->_matchedBgButton->setCurrentColor(settings.value(MATCHED_BG_COLOR, MATCHED_BG_COLOR_DEFAULT).toString());
   _ui->_unmatchedFgButton->setCurrentColor(settings.value(UNMATCHED_FG_COLOR, UNMATCHED_FG_COLOR_DEFAULT).toString());
   _ui->_unmatchedBgButton->setCurrentColor(settings.value(UNMATCHED_BG_COLOR, UNMATCHED_BG_COLOR_DEFAULT).toString());
   _ui->_whitespaceFgButton->setCurrentColor(settings.value(WHITESPACE_FG_COLOR, WHITESPACE_FG_COLOR_DEFAULT).toString());
   _ui->_whitespaceBgButton->setCurrentColor(settings.value(WHITESPACE_BG_COLOR, WHITESPACE_BG_COLOR_DEFAULT).toString());
}
void preferencesDialog::saveGeneral ()
{
   QSettings settings;
   settings.beginGroup(PREFERENCE_SETTINGS);

   settings.setValue(TAB_POLICY,          (_ui->_tabPolicyCombo->currentIndex() == 0));      // true = tabs, false = spaces
   settings.setValue(TAB_SIZE,            _ui->_tabSizeSpin->value());
   settings.setValue(AUTO_INDENT,         _ui->_autoIndentCheck->isChecked());
   settings.setValue(BACK_UNINDENT,       _ui->_unindentCheck->isChecked());
   settings.setValue(AUTO_PAIR,           _ui->_autoPairCheck->isChecked());
   settings.setValue(AUTO_ENCLOSE,        _ui->_autoEncloseCheck->isChecked());
   settings.setValue(LINE_WRAP,           _ui->_lineWrapCheck->isChecked());
   settings.setValue(RIGHT_EDGE,          (_ui->_rightEdgeCheck->isChecked()) ? _ui->_rightEdgeSpin->value() : -1);

   settings.setValue(LINE_NUMBERS,        _ui->_lineNumbersCheck->isChecked());
   settings.setValue(MARKERS,             _ui->_markersCheck->isChecked());
   settings.setValue(CHANGE_BARS,         _ui->_changeBarsCheck->isChecked());
   settings.setValue(INDENTATION,         _ui->_indentationCheck->isChecked());
   settings.setValue(WHITESPACE,          _ui->_whitespaceCheck->isChecked());
   settings.setValue(END_OF_LINE,         _ui->_eolCheck->isChecked());
   settings.setValue(HIGHLIGHT_LINE,      _ui->_highlightCheck->isChecked());
   settings.setValue(MATCH_BRACES,        _ui->_matchCheck->isChecked());
   settings.setValue(MARKER_ICON,         _ui->_markerCombo->currentIndex());

   settings.setValue(BACKGROUND_COLOR,    _ui->_bgButton->currentColorName());
//   settings.setValue(SELECTION_COLOR,     _ui->_selectionButton->currentColorName());
   settings.setValue(HIGHLIGHT_COLOR,     _ui->_highlightButton->currentColorName());
   settings.setValue(RIGHT_EDGE_COLOR,    _ui->_rightEdgeButton->currentColorName());
   settings.setValue(MARGIN_FG_COLOR,     _ui->_marginFgButton->currentColorName());
   settings.setValue(MARGIN_BG_COLOR,     _ui->_marginBgButton->currentColorName());
   settings.setValue(INDENTATION_FG_COLOR,_ui->_indentationFgButton->currentColorName());
   settings.setValue(INDENTATION_BG_COLOR,_ui->_indentationBgButton->currentColorName());
   settings.setValue(MATCHED_FG_COLOR,    _ui->_matchedFgButton->currentColorName());
   settings.setValue(MATCHED_BG_COLOR,    _ui->_matchedBgButton->currentColorName());
   settings.setValue(UNMATCHED_FG_COLOR,  _ui->_unmatchedFgButton->currentColorName());
   settings.setValue(UNMATCHED_BG_COLOR,  _ui->_unmatchedBgButton->currentColorName());
   settings.setValue(WHITESPACE_FG_COLOR, _ui->_whitespaceFgButton->currentColorName());
   settings.setValue(WHITESPACE_BG_COLOR, _ui->_whitespaceBgButton->currentColorName());
}
//-----------------------------------------------------------------------------
// setupGeneral
void preferencesDialog::setupGeneral ()
{
   _ui->_highlightButton->setAlpha(true);
   connect(_ui->_rightEdgeCheck,    SIGNAL(toggled(bool)), _ui->_rightEdgeSpin, SLOT(setEnabled(bool)));
   connect(_ui->_genDefaultsButton, SIGNAL(clicked()),     this, SLOT(defaultGeneral()));
   loadGeneral();
}
