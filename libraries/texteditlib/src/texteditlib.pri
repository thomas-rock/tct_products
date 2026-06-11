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
INCLUDEPATH += \
   $$PWD \
   $$TOP/3rdparty/QScintilla_src-2.14.1/src

LIBS        += -ltexteditlib -lqscintilla2_qt6d
LIBS        += -L$$DIST/lib/3rdparty/qscintilla -lqscintilla2_qt6d
