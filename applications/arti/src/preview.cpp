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
#include "preview.h"
#include "ui_preview.h"
#include "cmark-gfm.h"
#include "cmark-gfm-core-extensions.h"

#include <QFileInfo>
#include <QTextBrowser>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QWebEngineView>

Preview::Preview(QWidget* parent) : QMainWindow(parent), m_ui(new Ui::Preview)
{
   m_ui->setupUi(this);

   QLabel* title = new QLabel("Filename:  ");
   m_ui->statusbar->addWidget(title);
   m_filename = new QLabel();
   m_ui->statusbar->addWidget(m_filename);

   connect(m_ui->tabWidget, &QTabWidget::tabCloseRequested,
           this,            &Preview::closeTab);

   connect(m_ui->tabWidget, &QTabWidget::currentChanged,
           this,            &Preview::tabChanged);

   connect(m_ui->saveAct, &QAction::triggered,
           this,          &Preview::saveFile);

   connect(m_ui->exitAct, &QAction::triggered,
           this,          &Preview::close);

   setWindowTitle(QString("Artifact Preview"));

   // setup tabWidget tab styling
   QFile style(":/styles/tabWidget.txt");
   if (style.open(QIODevice::ReadOnly)) {
      QTextStream s(&style);
      QString text = s.readAll();
      text.replace("<TAB_COLOR>", "00A080");
      setStyleSheet(text);
   }
}
Preview::~Preview() {delete m_ui;}
//-----------------------------------------------------------------------------
void Preview::addArtifact (const QString& filename, const QString& artifact)
{
   QWebEngineView* viewer = nullptr;
   int index;

   // if artifact viewer exists, overwrite it
   for (index = 0; index < m_ui->tabWidget->count(); index++) {
      if (m_ui->tabWidget->tabToolTip(index) == filename) {
         viewer = qobject_cast<QWebEngineView*>(m_ui->tabWidget->widget(index));
         break;
      }
   }
   // artifact viewer doesn't exists - create it
   if (!viewer)
   {
      viewer = new QWebEngineView();
      index = m_ui->tabWidget->addTab(viewer, QFileInfo(filename).fileName());
      m_ui->tabWidget->setTabToolTip(index, filename);
   }

   m_ui->tabWidget->setCurrentIndex(index);

   if (isLikelyHtml(artifact))
      viewer->setHtml(artifact);

   else if (isLikelyMarkdown(artifact)) {
      qDebug() << "markdown";
      viewer->setHtml(markdownToHtmlGfm(artifact));
   }

   else
      viewer->setHtml(textToHtml(artifact));

   tabChanged(index);
}
//-----------------------------------------------------------------------------
void Preview::tabChanged (int index) {m_filename->setText(m_ui->tabWidget->tabToolTip(index));}
void Preview::saveFile ()
{
   int index = m_ui->tabWidget->currentIndex();
   if (index == -1) return;      // no pages

   QString filename = m_ui->tabWidget->tabToolTip(index);
   QTextBrowser* viewer = qobject_cast<QTextBrowser*>(m_ui->tabWidget->widget(index));
   QString file = QFileDialog::getSaveFileName(this, tr("Artifact filename"), QDir::currentPath());
   if (file.isEmpty()) return;

   QFile fio(file);
   if (!fio.open(QIODevice::WriteOnly)) {
      QString msg = QString("Cannot open file '%1' for writing").arg(file);
      QMessageBox::critical(this, QString("File Error"), msg);
   }
   else {
      QTextStream stream(&fio);
       stream << viewer->toPlainText();
   }
   fio.close();
}
void Preview::closeTab (int index)
{
   QString filename = m_ui->tabWidget->tabToolTip(index);
   m_ui->tabWidget->removeTab(index);
}
//-----------------------------------------------------------------------------
void Preview::closeEvent (QCloseEvent* event)
{
   m_ui->tabWidget->clear();
   QWidget::closeEvent(event);
}

#include <QString>
#include <QRegularExpression>

#include <QString>
#include <QRegularExpression>
#include <QSet>

bool Preview::isLikelyHtml(const QString& text)
{
   const QString t = text.trimmed();

   if (t.contains(QRegularExpression(
          R"(<!DOCTYPE\s+html|<html[\s>]|<body[\s>]|<head[\s>])",
          QRegularExpression::CaseInsensitiveOption))) {
      return true;
   }
   return false;
}

bool Preview::isLikelyMarkdown(const QString& text)
{
   static const QList<QRegularExpression> patterns = {
      // ATX heading only: # Heading
      QRegularExpression(R"(^\s{0,3}#{1,6}\s+\S)",
                         QRegularExpression::MultilineOption),

      // Fenced code block
      QRegularExpression(R"(^\s{0,3}(```|~~~))",
                         QRegularExpression::MultilineOption),

      // GFM table separator row
      QRegularExpression(R"(^\s*\|?\s*:?-{3,}:?\s*(\|\s*:?-{3,}:?\s*)+\|?\s*$)",
                         QRegularExpression::MultilineOption),

      // Blockquote
      QRegularExpression(R"(^\s{0,3}>\s+\S)",
                         QRegularExpression::MultilineOption),

      // Lists
      QRegularExpression(R"(^\s{0,3}[-*+]\s+\S)",
                         QRegularExpression::MultilineOption),
      QRegularExpression(R"(^\s{0,3}\d+[.)]\s+\S)",
                         QRegularExpression::MultilineOption),

      // Markdown links/images
      QRegularExpression(R"(!?\[[^\]\n]+\]\([^) \n]+\))")
   };

   for (const auto& re : patterns) {
      if (re.match(text).hasMatch())
         return true;
   }

   return false;
}

QString Preview::textToHtml(const QString& text)
{
   return QString(R"(
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<style>
pre {
    font-family: Consolas, monospace;
    white-space: pre-wrap;
}
</style>
</head>
<body>
<pre>%1</pre>
</body>
</html>
)").arg(text.toHtmlEscaped());
}
QString Preview::markdownToHtmlGfm(const QString& markdown)
{
   cmark_gfm_core_extensions_ensure_registered();

   cmark_parser* parser = cmark_parser_new(CMARK_OPT_DEFAULT);

   const char* extensionNames[] = {
      "table",
      "strikethrough",
      "autolink",
      "tagfilter",
      "tasklist"
   };

   for (const char* name : extensionNames) {
      cmark_syntax_extension* ext = cmark_find_syntax_extension(name);
      if (ext)
         cmark_parser_attach_syntax_extension(parser, ext);
   }

   QByteArray utf8 = markdown.toUtf8();

   cmark_parser_feed(
      parser,
      utf8.constData(),
      static_cast<size_t>(utf8.size()));

   cmark_node* document = cmark_parser_finish(parser);

   char* html = cmark_render_html(
      document,
      CMARK_OPT_DEFAULT,
      nullptr);

   QString result = html ? QString::fromUtf8(html) : QString();

   if (html)
      cmark_get_default_mem_allocator()->free(html);

   cmark_node_free(document);
   cmark_parser_free(parser);

      return QString(R"(
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<style>
body {
    font-family: "Segoe UI";
    margin: 16px;
    line-height: 1.4;
}

table {
    border-collapse: collapse;
    margin-bottom: 1em;
}

th, td {
    border: 1px solid #808080;
    padding: 4px 8px;
}

th {
    background: #e0e0e0;
}

code {
    font-family: Consolas, monospace;
}

pre {
    border: 1px solid #ccc;
    padding: 8px;
    background: #f8f8f8;
}
</style>
</head>
<body>
%1
</body>
</html>
)").arg(result);

}