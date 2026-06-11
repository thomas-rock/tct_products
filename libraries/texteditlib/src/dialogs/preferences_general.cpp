//=============================================================================
// Treasure Coast Technologies, Inc.          www.TreasureCoastTechnologies.com
//
//   This confidential and proprietary code may be used only as
//   authorized by a licensing agreement from RockSolid Solutions.
//   In the event of publication, the following notice is applicable:
//
//        COPYRIGHT © 2026 to the present, Treasure Coast Technologies, Inc.
//        ALL RIGHTS RESERVED
//
//   The entire notice above must be reproduced on all authorized copies.
//
//=============================================================================
//
// Description :
//
//=============================================================================

#include "preferences_dialog.h"
#include "ui_preferences_dialog.h"
#include "editor_settings_p.h"

using namespace texteditLib;

void PreferencesDialog::defaultGeneral ()
{
   m_ui->_tabPolicyCombo->setCurrentIndex((TAB_POLICY_DEFAULT) ? 0 : 1);    // true = tabs, false = spaces
   m_ui->_tabSizeSpin->setValue(TAB_SIZE_DEFAULT);
   m_ui->_autoIndentCheck->setChecked(AUTO_INDENT_DEFAULT);
   m_ui->_unindentCheck->setChecked(BACK_UNINDENT_DEFAULT);
   m_ui->_autoPairCheck->setChecked(AUTO_PAIR_DEFAULT);
   m_ui->_autoEncloseCheck->setChecked(AUTO_ENCLOSE_DEFAULT);
   m_ui->_lineWrapCheck->setChecked(LINE_WRAP_DEFAULT);
   m_ui->_rightEdgeCheck->setChecked(RIGHT_EDGE_DEFAULT != -1);
   m_ui->_rightEdgeSpin->setValue((RIGHT_EDGE_DEFAULT == -1) ? 80 : RIGHT_EDGE_DEFAULT);

   m_ui->_lineNumbersCheck->setChecked(LINE_NUMBERS_DEFAULT);
   m_ui->_markersCheck->setChecked(MARKERS_DEFAULT);
   m_ui->_changeBarsCheck->setChecked(CHANGE_BARS_DEFAULT);
   m_ui->_indentationCheck->setChecked(INDENTATION_DEFAULT);
   m_ui->_whitespaceCheck->setChecked(WHITESPACE_DEFAULT);
   m_ui->_eolCheck->setChecked(END_OF_LINE_DEFAULT);
   m_ui->_highlightCheck->setChecked(HIGHLIGHT_LINE_DEFAULT);
   m_ui->_matchCheck->setChecked(MATCH_BRACES_DEFAULT);
   m_ui->_markerCombo->setCurrentIndex(MARKER_ICON_DEFAULT);

   m_ui->_bgButton->setCurrentColor(BACKGROUND_COLOR_DEFAULT);
//   _ui->_selectionButton->setCurrentColor(SELECTION_COLOR_DEFAULT);
   m_ui->_highlightButton->setCurrentColor(HIGHLIGHT_COLOR_DEFAULT);
   m_ui->_rightEdgeButton->setCurrentColor(RIGHT_EDGE_COLOR_DEFAULT);
   m_ui->_marginFgButton->setCurrentColor(MARGIN_FG_COLOR_DEFAULT);
   m_ui->_marginBgButton->setCurrentColor(MARGIN_BG_COLOR_DEFAULT);
   m_ui->_indentationFgButton->setCurrentColor(INDENTATION_FG_COLOR_DEFAULT);
   m_ui->_indentationBgButton->setCurrentColor(INDENTATION_BG_COLOR_DEFAULT);
   m_ui->_matchedFgButton->setCurrentColor(MATCHED_FG_COLOR_DEFAULT);
   m_ui->_matchedBgButton->setCurrentColor(MATCHED_BG_COLOR_DEFAULT);
   m_ui->_unmatchedFgButton->setCurrentColor(UNMATCHED_FG_COLOR_DEFAULT);
   m_ui->_unmatchedBgButton->setCurrentColor(UNMATCHED_BG_COLOR_DEFAULT);
   m_ui->_whitespaceFgButton->setCurrentColor(WHITESPACE_FG_COLOR_DEFAULT);
   m_ui->_whitespaceBgButton->setCurrentColor(WHITESPACE_BG_COLOR_DEFAULT);
}
void PreferencesDialog::loadGeneral ()
{
   QSettings settings;
   settings.beginGroup(PREFERENCE_SETTINGS);

   m_ui->_tabPolicyCombo->setCurrentIndex((settings.value(TAB_POLICY, TAB_POLICY_DEFAULT).toBool()) ? 0 : 1);      // true = tabs, false = spaces
   m_ui->_tabSizeSpin->setValue(settings.value(TAB_SIZE, TAB_SIZE_DEFAULT).toInt());
   m_ui->_autoIndentCheck->setChecked(settings.value(AUTO_INDENT, AUTO_INDENT_DEFAULT).toBool());
   m_ui->_unindentCheck->setChecked(settings.value(BACK_UNINDENT, BACK_UNINDENT_DEFAULT).toBool());
   m_ui->_autoPairCheck->setChecked(settings.value(AUTO_PAIR, AUTO_PAIR_DEFAULT).toBool());
   m_ui->_autoEncloseCheck->setChecked(settings.value(AUTO_ENCLOSE, AUTO_ENCLOSE_DEFAULT).toBool());
   m_ui->_lineWrapCheck->setChecked(settings.value(LINE_WRAP, LINE_WRAP_DEFAULT).toBool());
   int rightEdge = settings.value(RIGHT_EDGE, RIGHT_EDGE_DEFAULT).toInt();
   m_ui->_rightEdgeCheck->setChecked(rightEdge != -1);
   if (rightEdge != -1)
      m_ui->_rightEdgeSpin->setValue(rightEdge);

   m_ui->_lineNumbersCheck->setChecked(settings.value(LINE_NUMBERS, LINE_NUMBERS_DEFAULT).toBool());
   m_ui->_changeBarsCheck->setChecked(settings.value(CHANGE_BARS, CHANGE_BARS_DEFAULT).toBool());
   m_ui->_markersCheck->setChecked(settings.value(MARKERS, MARKERS_DEFAULT).toBool());
   m_ui->_indentationCheck->setChecked(settings.value(INDENTATION, INDENTATION_DEFAULT).toBool());
   m_ui->_whitespaceCheck->setChecked(settings.value(WHITESPACE, WHITESPACE_DEFAULT).toBool());
   m_ui->_eolCheck->setChecked(settings.value(END_OF_LINE, END_OF_LINE_DEFAULT).toBool());
   m_ui->_highlightCheck->setChecked(settings.value(HIGHLIGHT_LINE, HIGHLIGHT_LINE_DEFAULT).toBool());
   m_ui->_matchCheck->setChecked(settings.value(MATCH_BRACES, MATCH_BRACES_DEFAULT).toBool());
   m_ui->_markerCombo->setCurrentIndex(settings.value(MARKER_ICON, MARKER_ICON_DEFAULT).toInt());

   m_ui->_bgButton->setCurrentColor(settings.value(BACKGROUND_COLOR, BACKGROUND_COLOR_DEFAULT).toString());
//   _ui->_selectionButton->setCurrentColor(settings.value(SELECTION_COLOR, SELECTION_COLOR_DEFAULT).toString());
   m_ui->_highlightButton->setCurrentColor(settings.value(HIGHLIGHT_COLOR, HIGHLIGHT_COLOR_DEFAULT).toString());
   m_ui->_rightEdgeButton->setCurrentColor(settings.value(RIGHT_EDGE_COLOR, RIGHT_EDGE_COLOR_DEFAULT).toString());
   m_ui->_marginFgButton->setCurrentColor(settings.value(MARGIN_FG_COLOR, MARGIN_FG_COLOR_DEFAULT).toString());
   m_ui->_marginBgButton->setCurrentColor(settings.value(MARGIN_BG_COLOR, MARGIN_BG_COLOR_DEFAULT).toString());
   m_ui->_indentationFgButton->setCurrentColor(settings.value(INDENTATION_FG_COLOR, INDENTATION_FG_COLOR_DEFAULT).toString());
   m_ui->_indentationBgButton->setCurrentColor(settings.value(INDENTATION_BG_COLOR, INDENTATION_BG_COLOR_DEFAULT).toString());
   m_ui->_matchedFgButton->setCurrentColor(settings.value(MATCHED_FG_COLOR, MATCHED_FG_COLOR_DEFAULT).toString());
   m_ui->_matchedBgButton->setCurrentColor(settings.value(MATCHED_BG_COLOR, MATCHED_BG_COLOR_DEFAULT).toString());
   m_ui->_unmatchedFgButton->setCurrentColor(settings.value(UNMATCHED_FG_COLOR, UNMATCHED_FG_COLOR_DEFAULT).toString());
   m_ui->_unmatchedBgButton->setCurrentColor(settings.value(UNMATCHED_BG_COLOR, UNMATCHED_BG_COLOR_DEFAULT).toString());
   m_ui->_whitespaceFgButton->setCurrentColor(settings.value(WHITESPACE_FG_COLOR, WHITESPACE_FG_COLOR_DEFAULT).toString());
   m_ui->_whitespaceBgButton->setCurrentColor(settings.value(WHITESPACE_BG_COLOR, WHITESPACE_BG_COLOR_DEFAULT).toString());
}
void PreferencesDialog::saveGeneral ()
{
   QSettings settings;
   settings.beginGroup(PREFERENCE_SETTINGS);

   settings.setValue(TAB_POLICY,          (m_ui->_tabPolicyCombo->currentIndex() == 0));      // true = tabs, false = spaces
   settings.setValue(TAB_SIZE,            m_ui->_tabSizeSpin->value());
   settings.setValue(AUTO_INDENT,         m_ui->_autoIndentCheck->isChecked());
   settings.setValue(BACK_UNINDENT,       m_ui->_unindentCheck->isChecked());
   settings.setValue(AUTO_PAIR,           m_ui->_autoPairCheck->isChecked());
   settings.setValue(AUTO_ENCLOSE,        m_ui->_autoEncloseCheck->isChecked());
   settings.setValue(LINE_WRAP,           m_ui->_lineWrapCheck->isChecked());
   settings.setValue(RIGHT_EDGE,          (m_ui->_rightEdgeCheck->isChecked()) ? m_ui->_rightEdgeSpin->value() : -1);

   settings.setValue(LINE_NUMBERS,        m_ui->_lineNumbersCheck->isChecked());
   settings.setValue(MARKERS,             m_ui->_markersCheck->isChecked());
   settings.setValue(CHANGE_BARS,         m_ui->_changeBarsCheck->isChecked());
   settings.setValue(INDENTATION,         m_ui->_indentationCheck->isChecked());
   settings.setValue(WHITESPACE,          m_ui->_whitespaceCheck->isChecked());
   settings.setValue(END_OF_LINE,         m_ui->_eolCheck->isChecked());
   settings.setValue(HIGHLIGHT_LINE,      m_ui->_highlightCheck->isChecked());
   settings.setValue(MATCH_BRACES,        m_ui->_matchCheck->isChecked());
   settings.setValue(MARKER_ICON,         m_ui->_markerCombo->currentIndex());

   settings.setValue(BACKGROUND_COLOR,    m_ui->_bgButton->currentColorName());
//   settings.setValue(SELECTION_COLOR,     m_ui->_selectionButton->currentColorName());
   settings.setValue(HIGHLIGHT_COLOR,     m_ui->_highlightButton->currentColorName());
   settings.setValue(RIGHT_EDGE_COLOR,    m_ui->_rightEdgeButton->currentColorName());
   settings.setValue(MARGIN_FG_COLOR,     m_ui->_marginFgButton->currentColorName());
   settings.setValue(MARGIN_BG_COLOR,     m_ui->_marginBgButton->currentColorName());
   settings.setValue(INDENTATION_FG_COLOR,m_ui->_indentationFgButton->currentColorName());
   settings.setValue(INDENTATION_BG_COLOR,m_ui->_indentationBgButton->currentColorName());
   settings.setValue(MATCHED_FG_COLOR,    m_ui->_matchedFgButton->currentColorName());
   settings.setValue(MATCHED_BG_COLOR,    m_ui->_matchedBgButton->currentColorName());
   settings.setValue(UNMATCHED_FG_COLOR,  m_ui->_unmatchedFgButton->currentColorName());
   settings.setValue(UNMATCHED_BG_COLOR,  m_ui->_unmatchedBgButton->currentColorName());
   settings.setValue(WHITESPACE_FG_COLOR, m_ui->_whitespaceFgButton->currentColorName());
   settings.setValue(WHITESPACE_BG_COLOR, m_ui->_whitespaceBgButton->currentColorName());
}
//-----------------------------------------------------------------------------
// setupGeneral
void PreferencesDialog::setupGeneral ()
{
   m_ui->_highlightButton->setAlpha(true);
   connect(m_ui->_rightEdgeCheck,    SIGNAL(toggled(bool)), m_ui->_rightEdgeSpin, SLOT(setEnabled(bool)));
   connect(m_ui->_genDefaultsButton, SIGNAL(clicked()),     this, SLOT(defaultGeneral()));
   loadGeneral();
}
