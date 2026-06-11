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
TEMPLATE     = subdirs
SUBDIRS      = slate texteditlib # chartlib dotlib hexeditlib imagelib pdflib rtflib

# point to .pro files
slate.file        = applications/slate/src/slate.pro
texteditlib.file  = libraries/texteditlib/src/texteditlib.pro


slate.depends = texteditlib # chartlib hexeditlib imagelib pdflib rtflib