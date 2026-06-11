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
// Last checked in by $Author: thomas@x-tekcorp.com $   $Revision: 937 $
//
// $Date: 2015-02-16 19:27:06 -0500 (Mon, 16 Feb 2015) $
//=============================================================================
// $Id: main.cpp 937 2015-02-17 00:27:06Z thomas@x-tekcorp.com $
//=============================================================================
//
// Description :
//
// Notes :
//
//=============================================================================
#include <QtWidgets>

#include "XRtfEditor.h"

int main(int argv, char **args)
{
    QApplication app(argv, args);

    QMainWindow mw;
    mw.setWindowTitle("RTF Editor");
    xpert::XRtfEditor* editor = new xpert::XRtfEditor;
    mw.setCentralWidget(editor);

    QToolBar* fileTb = mw.addToolBar("File");
    QAction* exit = fileTb->addAction(QIcon(":common/exit"), "Exit");
    QObject::connect(exit, SIGNAL(triggered()), &mw, SLOT(close()));
    QAction* save = fileTb->addAction(QIcon(":common/fileSave"), "Save");
//    QObject::connect(save, SIGNAL(triggered()), editor, SLOT(data()));

    QToolBar* editTb = mw.addToolBar("Edit");
    QAction* undo = editTb->addAction(QIcon(":common/editUndo"), "Undo");
    QObject::connect(undo, SIGNAL(triggered()), editor, SLOT(undo()));
    QAction* redo = editTb->addAction(QIcon(":common/editRedo"), "Redo");
    QObject::connect(redo, SIGNAL(triggered()), editor, SLOT(redo()));
    editTb->addSeparator();
    QAction* cut = editTb->addAction(QIcon(":common/editCut"), "Cut");
    QObject::connect(cut, SIGNAL(triggered()), editor, SLOT(cut()));
    QAction* copy = editTb->addAction(QIcon(":common/editCopy"), "Copy");
    QObject::connect(copy, SIGNAL(triggered()), editor, SLOT(copy()));
    QAction* paste = editTb->addAction(QIcon(":common/editPaste"), "Paster");
    QObject::connect(paste, SIGNAL(triggered()), editor, SLOT(paste()));

    QObject::connect(editor, SIGNAL(copyAvailable(bool)), copy, SLOT(setEnabled(bool)));
    QObject::connect(editor, SIGNAL(copyAvailable(bool)), cut, SLOT(setEnabled(bool)));
    QObject::connect(editor, SIGNAL(redoAvailable(bool)), redo, SLOT(setEnabled(bool)));
    QObject::connect(editor, SIGNAL(undoAvailable(bool)), undo, SLOT(setEnabled(bool)));

    QToolBar* windowTb = mw.addToolBar("Window");
    QAction* action = windowTb->addAction(QIcon(":common/windowVSplit"), "VSplit");
    QObject::connect(action, SIGNAL(triggered()), editor, SLOT(vSplit()));
    action = windowTb->addAction(QIcon(":common/windowHSplit"), "HSplit");
    QObject::connect(action, SIGNAL(triggered()), editor, SLOT(hSplit()));
    action = windowTb->addAction(QIcon(":common/windowUnsplit"), "Unsplit");
    QObject::connect(action, SIGNAL(triggered()), editor, SLOT(unSplit()));

   // load a file
   QString filename = "C:/XTekProjects/trunk/xpert/common/editors/rtfedit/doc.xml";
   QFile file(filename);
   file.open(QIODevice::ReadOnly);
   QTextStream text(&file);
   editor->edit(filename, text.readAll());

   // load another file
   QString filename2 = "C:/XTekProjects/trunk/xpert/common/editors/rtfedit/doc2.xml";
   QFile file2(filename2);
   file2.open(QIODevice::ReadOnly);
   QTextStream text2(&file2);
   editor->edit(filename2, text2.readAll());

   mw.show();
   return app.exec();
}

