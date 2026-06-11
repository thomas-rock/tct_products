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

# Define clean logical names
SUBDIRS += arti artilib commonlib jsonlib texteditlib

# point to .pro files
arti.file            = applications/arti/src/arti.pro
artilib.file         = libraries/artilib/src/artilib.pro
commonlib.file       = libraries/commonlib/src/commonlib.pro
jsonlib.file         = libraries/jsonlib/src/jsonlib.pro
texteditlib.file     = libraries/texteditlib/src/texteditlib.pro

# Establish clean build dependencies using the aliases
arti.depends = artilib commonlib jsonlib texteditlib

