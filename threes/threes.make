#             __________               __   ___.
#   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
#   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
#   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
#   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
#                     \/            \/     \/    \/            \/
# $Id$
#

THREESSRCDIR := $(APPSDIR)/plugins/threes
THREESBUILDDIR := $(BUILDDIR)/apps/plugins/threes

ROCKS += $(THREESBUILDDIR)/threes.rock

THREES_SRC := $(call preprocess, $(THREESSRCDIR)/SOURCES)
THREES_OBJ := $(call c2obj, $(THREES_SRC))

# add source files to OTHER_SRC to get automatic dependencies
OTHER_SRC += $(THREES_SRC)

$(THREESBUILDDIR)/threes.rock: $(THREES_OBJ)
