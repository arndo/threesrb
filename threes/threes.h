/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2014 Arnd Oberländer
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/

#include "plugin.h"

#include "lib/highscore.h"

#include "pluginbitmaps/threes_border.h"
#include "pluginbitmaps/threes_border_blue.h"
#include "pluginbitmaps/threes_border_red.h"
#include "pluginbitmaps/threes_numbers_opensans.h"

#define ROW_COUNT 4
#define COL_COUNT 4
#define TOOLBAR_HEIGHT 11
#define MARGIN 1
#define CELL_SPACING 2
#define CARD_WIDTH  (LCD_WIDTH - 2*MARGIN - 3*CELL_SPACING)/ROW_COUNT
#define CARD_HEIGHT (LCD_HEIGHT - TOOLBAR_HEIGHT - 2*MARGIN - 3*CELL_SPACING)/COL_COUNT
#define START_COUNT 9
#define SCORE_FILE  PLUGIN_GAMES_DATA_DIR "/threes.score"
#define NUM_SCORES  5

/*
 * Direction
 *          3 - UP
 *  0 - LEFT        1 - RIGHT
 *          2 - DOWN
 */
#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3

/* variable button definitions */
#if CONFIG_KEYPAD == RECORDER_PAD
#define THREES_LEFT         BUTTON_LEFT
#define THREES_RIGHT        BUTTON_RIGHT
#define THREES_UP           BUTTON_UP
#define THREES_DOWN         BUTTON_DOWN
#define THREES_QUIT         BUTTON_OFF
#define THREES_SHUFFLE      BUTTON_F1

#elif CONFIG_KEYPAD == ARCHOS_AV300_PAD
#define THREES_LEFT         BUTTON_LEFT
#define THREES_RIGHT        BUTTON_RIGHT
#define THREES_UP           BUTTON_UP
#define THREES_DOWN         BUTTON_DOWN
#define THREES_QUIT         BUTTON_OFF
#define THREES_SHUFFLE      BUTTON_F1

#elif CONFIG_KEYPAD == PLAYER_PAD
#define THREES_LEFT         BUTTON_LEFT
#define THREES_RIGHT        BUTTON_RIGHT
#define THREES_UP_PRE       BUTTON_ON
#define THREES_UP           (BUTTON_ON | BUTTON_REL)
#define THREES_DOWN         BUTTON_MENU
#define THREES_QUIT         BUTTON_STOP
#define THREES_SHUFFLE      (BUTTON_ON | BUTTON_LEFT)

#elif CONFIG_KEYPAD == ONDIO_PAD
#define THREES_LEFT         BUTTON_LEFT
#define THREES_RIGHT        BUTTON_RIGHT
#define THREES_UP           BUTTON_UP
#define THREES_DOWN         BUTTON_DOWN
#define THREES_QUIT         BUTTON_OFF
#define THREES_SHUFFLE      (BUTTON_MENU | BUTTON_LEFT)

#elif (CONFIG_KEYPAD == IRIVER_H100_PAD) || \
      (CONFIG_KEYPAD == IRIVER_H300_PAD)
#define THREES_LEFT         BUTTON_LEFT
#define THREES_RIGHT        BUTTON_RIGHT
#define THREES_UP           BUTTON_UP
#define THREES_DOWN         BUTTON_DOWN
#define THREES_QUIT         BUTTON_OFF
#define THREES_SHUFFLE      BUTTON_MODE

#define THREES_RC_QUIT BUTTON_RC_STOP

#elif (CONFIG_KEYPAD == IPOD_4G_PAD) || \
      (CONFIG_KEYPAD == IPOD_3G_PAD) || \
      (CONFIG_KEYPAD == IPOD_1G2G_PAD)

#define THREES_SCROLLWHEEL
#define THREES_LEFT         BUTTON_LEFT
#define THREES_RIGHT        BUTTON_RIGHT
#define THREES_UP           BUTTON_MENU
#define THREES_DOWN         BUTTON_PLAY
#define THREES_NEXT         BUTTON_SCROLL_FWD
#define THREES_PREV         BUTTON_SCROLL_BACK
#define THREES_QUIT         (BUTTON_SELECT | BUTTON_MENU)
#define THREES_SHUFFLE      (BUTTON_SELECT | BUTTON_LEFT)

#elif CONFIG_KEYPAD == IAUDIO_X5M5_PAD

#define THREES_LEFT         BUTTON_LEFT
#define THREES_RIGHT        BUTTON_RIGHT
#define THREES_UP           BUTTON_UP
#define THREES_DOWN         BUTTON_DOWN
#define THREES_QUIT         BUTTON_POWER
#define THREES_SHUFFLE      BUTTON_REC

#elif CONFIG_KEYPAD == GIGABEAT_PAD

#define THREES_LEFT         BUTTON_LEFT
#define THREES_RIGHT        BUTTON_RIGHT
#define THREES_UP           BUTTON_UP
#define THREES_DOWN         BUTTON_DOWN
#define THREES_QUIT         BUTTON_POWER
#define THREES_SHUFFLE      BUTTON_MENU

#elif CONFIG_KEYPAD == SANSA_E200_PAD

#define THREES_SCROLLWHEEL
#define THREES_LEFT         BUTTON_LEFT
#define THREES_RIGHT        BUTTON_RIGHT
#define THREES_UP           BUTTON_UP
#define THREES_DOWN         BUTTON_DOWN
#define THREES_NEXT         BUTTON_SCROLL_FWD
#define THREES_PREV         BUTTON_SCROLL_BACK
#define THREES_QUIT         BUTTON_POWER
#define THREES_SHUFFLE      (BUTTON_REC | BUTTON_LEFT)

#elif (CONFIG_KEYPAD == SANSA_FUZE_PAD)

#define THREES_SCROLLWHEEL
#define THREES_LEFT         BUTTON_LEFT
#define THREES_RIGHT        BUTTON_RIGHT
#define THREES_UP           BUTTON_UP
#define THREES_DOWN         BUTTON_DOWN
#define THREES_NEXT         BUTTON_SCROLL_FWD
#define THREES_PREV         BUTTON_SCROLL_BACK
#define THREES_QUIT         (BUTTON_HOME|BUTTON_REPEAT)
#define THREES_SHUFFLE      (BUTTON_SELECT | BUTTON_LEFT)

#elif CONFIG_KEYPAD == SANSA_C200_PAD

#define THREES_LEFT         BUTTON_LEFT
#define THREES_RIGHT        BUTTON_RIGHT
#define THREES_UP           BUTTON_UP
#define THREES_DOWN         BUTTON_DOWN
#define THREES_QUIT         BUTTON_POWER
#define THREES_SHUFFLE      (BUTTON_REC | BUTTON_LEFT)

#elif CONFIG_KEYPAD == SANSA_CLIP_PAD

#define THREES_LEFT         BUTTON_LEFT
#define THREES_RIGHT        BUTTON_RIGHT
#define THREES_UP           BUTTON_UP
#define THREES_DOWN         BUTTON_DOWN
#define THREES_QUIT         BUTTON_POWER
#define THREES_SHUFFLE      (BUTTON_HOME | BUTTON_LEFT)

#elif CONFIG_KEYPAD == SANSA_M200_PAD

#define THREES_LEFT         BUTTON_LEFT
#define THREES_RIGHT        BUTTON_RIGHT
#define THREES_UP           BUTTON_UP
#define THREES_DOWN         BUTTON_DOWN
#define THREES_QUIT         BUTTON_POWER
#define THREES_SHUFFLE      (BUTTON_SELECT | BUTTON_LEFT)

#elif CONFIG_KEYPAD == IRIVER_H10_PAD

#define THREES_LEFT         BUTTON_LEFT
#define THREES_RIGHT        BUTTON_RIGHT
#define THREES_UP           BUTTON_SCROLL_UP
#define THREES_DOWN         BUTTON_SCROLL_DOWN
#define THREES_QUIT         BUTTON_POWER
#define THREES_SHUFFLE      (BUTTON_PLAY | BUTTON_LEFT)

#elif CONFIG_KEYPAD == GIGABEAT_S_PAD

#define THREES_LEFT         BUTTON_LEFT
#define THREES_RIGHT        BUTTON_RIGHT
#define THREES_UP           BUTTON_UP
#define THREES_DOWN         BUTTON_DOWN
#define THREES_QUIT         BUTTON_BACK
#define THREES_SHUFFLE      BUTTON_MENU

#elif CONFIG_KEYPAD == MROBE100_PAD

#define THREES_LEFT         BUTTON_LEFT
#define THREES_RIGHT        BUTTON_RIGHT
#define THREES_UP           BUTTON_UP
#define THREES_DOWN         BUTTON_DOWN
#define THREES_QUIT         BUTTON_POWER
#define THREES_SHUFFLE      BUTTON_MENU

#elif CONFIG_KEYPAD == IAUDIO_M3_PAD

#define THREES_LEFT         BUTTON_RC_REW
#define THREES_RIGHT        BUTTON_RC_FF
#define THREES_UP           BUTTON_RC_VOL_UP
#define THREES_DOWN         BUTTON_RC_VOL_DOWN
#define THREES_QUIT         BUTTON_RC_REC
#define THREES_SHUFFLE      BUTTON_RC_MODE

#define THREES_RC_QUIT      BUTTON_REC

#elif CONFIG_KEYPAD == COWON_D2_PAD

#define THREES_QUIT         BUTTON_POWER

#elif CONFIG_KEYPAD == CREATIVEZVM_PAD

#define THREES_LEFT         BUTTON_LEFT
#define THREES_RIGHT        BUTTON_RIGHT
#define THREES_UP           BUTTON_UP
#define THREES_DOWN         BUTTON_DOWN
#define THREES_QUIT         BUTTON_BACK
#define THREES_SHUFFLE      BUTTON_CUSTOM

#elif CONFIG_KEYPAD == PHILIPS_HDD1630_PAD

#define THREES_LEFT         BUTTON_LEFT
#define THREES_RIGHT        BUTTON_RIGHT
#define THREES_UP           BUTTON_UP
#define THREES_DOWN         BUTTON_DOWN
#define THREES_QUIT         BUTTON_POWER
#define THREES_SHUFFLE      BUTTON_MENU
#define THREES_SOLVE        BUTTON_VIEW
#define THREES_STEP_BY_STEP BUTTON_PLAYLIST
#define THREES_TOGGLE       BUTTON_SELECT

#elif CONFIG_KEYPAD == PHILIPS_HDD6330_PAD

#define THREES_LEFT         BUTTON_LEFT
#define THREES_RIGHT        BUTTON_RIGHT
#define THREES_UP           BUTTON_UP
#define THREES_DOWN         BUTTON_DOWN
#define THREES_QUIT         BUTTON_POWER
#define THREES_SHUFFLE      BUTTON_MENU

#elif CONFIG_KEYPAD == PHILIPS_SA9200_PAD

#define THREES_LEFT         BUTTON_PREV
#define THREES_RIGHT        BUTTON_NEXT
#define THREES_UP           BUTTON_UP
#define THREES_DOWN         BUTTON_DOWN
#define THREES_QUIT         BUTTON_POWER
#define THREES_SHUFFLE      BUTTON_MENU

#elif CONFIG_KEYPAD == ONDAVX747_PAD || CONFIG_KEYPAD == MROBE500_PAD

#define THREES_QUIT         BUTTON_POWER

#elif CONFIG_KEYPAD == ONDAVX777_PAD

#define THREES_QUIT         BUTTON_POWER

#elif CONFIG_KEYPAD == SAMSUNG_YH_PAD

#define THREES_LEFT         BUTTON_LEFT
#define THREES_RIGHT        BUTTON_RIGHT
#define THREES_UP           BUTTON_UP
#define THREES_DOWN         BUTTON_DOWN
#define THREES_QUIT         BUTTON_REC
#define THREES_SHUFFLE      BUTTON_PLAY

#elif CONFIG_KEYPAD == PBELL_VIBE500_PAD

#define THREES_LEFT         BUTTON_PREV
#define THREES_RIGHT        BUTTON_NEXT
#define THREES_UP           BUTTON_UP
#define THREES_DOWN         BUTTON_DOWN
#define THREES_QUIT         BUTTON_REC
#define THREES_SHUFFLE      BUTTON_PLAY

#elif CONFIG_KEYPAD == MPIO_HD200_PAD

#define THREES_LEFT         BUTTON_VOL_DOWN
#define THREES_RIGHT        BUTTON_VOL_UP
#define THREES_UP           BUTTON_REW
#define THREES_DOWN         BUTTON_FF
#define THREES_QUIT         (BUTTON_REC|BUTTON_PLAY)
#define THREES_SHUFFLE      BUTTON_FUNC

#elif CONFIG_KEYPAD == MPIO_HD300_PAD

#define THREES_LEFT         BUTTON_REW
#define THREES_RIGHT        BUTTON_FF
#define THREES_UP           BUTTON_UP
#define THREES_DOWN         BUTTON_DOWN
#define THREES_QUIT         (BUTTON_MENU|BUTTON_REPEAT)
#define THREES_SHUFFLE      BUTTON_PLAY

#elif CONFIG_KEYPAD == SANSA_FUZEPLUS_PAD

#define THREES_LEFT         BUTTON_LEFT
#define THREES_RIGHT        BUTTON_RIGHT
#define THREES_UP           BUTTON_UP
#define THREES_DOWN         BUTTON_DOWN
#define THREES_QUIT         BUTTON_POWER
#define THREES_SHUFFLE      BUTTON_PLAYPAUSE

#elif CONFIG_KEYPAD == SANSA_CONNECT_PAD

#define THREES_LEFT         BUTTON_LEFT
#define THREES_RIGHT        BUTTON_RIGHT
#define THREES_UP           BUTTON_UP
#define THREES_DOWN         BUTTON_DOWN
#define THREES_QUIT         BUTTON_POWER
#define THREES_SHUFFLE      BUTTON_VOL_UP

#elif CONFIG_KEYPAD == SAMSUNG_YPR0_PAD

#define THREES_LEFT         BUTTON_LEFT
#define THREES_RIGHT        BUTTON_RIGHT
#define THREES_UP           BUTTON_UP
#define THREES_DOWN         BUTTON_DOWN
#define THREES_QUIT         BUTTON_BACK
#define THREES_SHUFFLE      BUTTON_MENU

#elif CONFIG_KEYPAD == HM60X_PAD

#define THREES_LEFT         BUTTON_LEFT
#define THREES_RIGHT        BUTTON_RIGHT
#define THREES_UP           BUTTON_UP
#define THREES_DOWN         BUTTON_DOWN
#define THREES_QUIT         BUTTON_POWER
#define THREES_SHUFFLE      (BUTTON_UP|BUTTON_POWER)

#elif CONFIG_KEYPAD == HM801_PAD

#define THREES_LEFT         BUTTON_LEFT
#define THREES_RIGHT        BUTTON_RIGHT
#define THREES_UP           BUTTON_UP
#define THREES_DOWN         BUTTON_DOWN
#define THREES_QUIT         BUTTON_POWER
#define THREES_SHUFFLE      BUTTON_PREV

#else
#error No keymap defined!
#endif

#ifdef HAVE_TOUCHSCREEN
#ifndef THREES_LEFT
#define THREES_LEFT         BUTTON_MIDLEFT
#endif
#ifndef THREES_RIGHT
#define THREES_RIGHT        BUTTON_MIDRIGHT
#endif
#ifndef THREES_UP
#define THREES_UP           BUTTON_TOPMIDDLE
#endif
#ifndef THREES_DOWN
#define THREES_DOWN         BUTTON_BOTTOMMIDDLE
#endif
#ifndef THREES_QUIT
#define THREES_QUIT         BUTTON_TOPLEFT
#endif
#ifndef THREES_SHUFFLE
#define THREES_SHUFFLE      BUTTON_TOPRIGHT
#endif
#endif
