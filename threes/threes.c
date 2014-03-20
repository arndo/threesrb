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

#include "threes.h"

int board[ROW_COUNT][COL_COUNT];
int max_value = 3;
int next_value;
int scoretable[12] = {3,9,27,81,243,729,2187,6561,19683,59049,177147,531441};
static struct highscore highscores[NUM_SCORES];

unsigned blue_border = LCD_RGBPACK(0,170,255);
unsigned blue_bg = LCD_RGBPACK(102,204,255);
unsigned red_border = LCD_RGBPACK(255,0,43);
unsigned red_bg = LCD_RGBPACK(255,102,128);
unsigned default_border = LCD_RGBPACK(136,136,136);
unsigned default_highlight = LCD_RGBPACK(255,204,102);
unsigned default_bg = LCD_WHITE;
unsigned backdrop = LCD_RGBPACK(206,206,206);

void init_board( void );
void draw_board( void );

int calculate_score ( void ) {
    int score = 0;
    for( int i=0; i<ROW_COUNT; i++ )
    {
        for( int j=0; j<COL_COUNT; j++ ) {
            int value = board[i][j];
            if( value > 2 ) {
                score = score + scoretable[value - 3];
            }
        }
    }

    return score;
}

int draw_random_card(int min, int max) {
    if( max > 3 && rb->rand() % 100 < 5 )
        return rb->rand() % max + 3;
    return rb->rand() % 3 + min;
}

void draw_card(int row, int col, int value) {

    unsigned card_border = default_border;
    unsigned card_bgcolor = default_bg;
    unsigned card_highlight = default_highlight;
    const fb_data * border_bmp;

    rb->lcd_set_drawmode( DRMODE_SOLID );

    int x = col * CELL_SPACING + col * CARD_WIDTH + MARGIN;
    int y = row * CELL_SPACING + row * CARD_HEIGHT + TOOLBAR_HEIGHT + MARGIN;

    switch (value)
    {
        case 1:
            card_border = blue_border;
            card_bgcolor = blue_bg;
            card_highlight = blue_border;
            border_bmp = threes_border_blue;
            break;
        case 2:
            card_border = red_border;
            card_bgcolor = red_bg;
            card_highlight = red_border;
            border_bmp = threes_border_red;
            break;
        default:
            border_bmp = threes_border;
            break;
    }

    if(value > 0) {
        // Draw the basic card
        rb->lcd_set_foreground(card_bgcolor);
        rb->lcd_fillrect(x, y, CARD_WIDTH, CARD_HEIGHT);
        rb->lcd_set_foreground(card_highlight);
        rb->lcd_fillrect(x, y + CARD_HEIGHT - 4, CARD_WIDTH, 3);
        rb->lcd_set_foreground(card_border);
        rb->lcd_drawrect(x, y, CARD_WIDTH, CARD_HEIGHT);

        // Draw the corners
        rb->lcd_bitmap_part(border_bmp, 0, 0, 11, x, y, 5, 5);
        rb->lcd_bitmap_part(border_bmp, 6, 0, 11, x + CARD_WIDTH - 5, y, 5, 5);
        rb->lcd_bitmap_part(border_bmp, 0, 6, 11, x, y + CARD_HEIGHT - 6, 5, 6);
        rb->lcd_bitmap_part(border_bmp, 6, 6, 11, x + CARD_WIDTH - 5, y + CARD_HEIGHT - 6, 5, 6);

        // Draw the number
        rb->lcd_bitmap_part(threes_numbers_opensans, bm_threes_numbers_opensans.width/14 * (value - 1), 0, bm_threes_numbers_opensans.width,
            x + CARD_WIDTH/2 - bm_threes_numbers_opensans.width/28,
            y + CARD_HEIGHT/2 - bm_threes_numbers_opensans.height/2, bm_threes_numbers_opensans.width/14, bm_threes_numbers_opensans.height);
    } else {
        rb->lcd_set_foreground(backdrop);
        rb->lcd_fillrect(x, y, CARD_WIDTH, CARD_HEIGHT);
    }

    /*if(value > 0) {
        char valuechar[5];
        rb->snprintf(valuechar, 5, "%d", value);

        rb->lcd_set_foreground(LCD_BLACK);
        if(value < 3)
        rb->lcd_set_foreground(LCD_WHITE);
        rb->lcd_set_background(card_bgcolor);
        rb->lcd_putsxy(x + MARGIN, y + MARGIN, valuechar);
    }*/

    rb->lcd_update_rect(x, y,
        CARD_WIDTH,
        CARD_HEIGHT);
}

void draw_board( void ) {
    rb->lcd_set_foreground(backdrop);
    rb->lcd_fillrect(0, 0, LCD_WIDTH, LCD_HEIGHT);

    int i, j;
    for( i=0; i<ROW_COUNT; i++ )
    {
        for( j=0; j<COL_COUNT; j++ ) {
            draw_card(i, j, board[i][j]);
        }
    }

    int x = LCD_WIDTH - MARGIN - 14;
    int y = 1;

    switch( next_value )
    {
        case -1:
            rb->lcd_set_foreground(LCD_BLACK);
            break;
        case 1:
            rb->lcd_set_foreground(blue_bg);
            break;
        case 2:
            rb->lcd_set_foreground(red_bg);
            break;
        default:
            rb->lcd_set_foreground(LCD_WHITE);
            break;
    }
    rb->lcd_fillrect(x, y, 14, 9);

    rb->lcd_setfont(FONT_SYSFIXED);

    if( next_value > 3 ) {
        rb->lcd_set_background(LCD_WHITE);
        rb->lcd_set_foreground(LCD_BLACK);
        rb->lcd_putsxy(x+4, 1, "+");
    }

    char valuechar[100];
    rb->snprintf(valuechar, 100, "Score: %d / HS: %d", calculate_score(), highscores[0].score);

    rb->lcd_set_foreground(LCD_BLACK);
    rb->lcd_set_background(LCD_RGBPACK(206,206,206));
    rb->lcd_putsxy(1, 0, valuechar);
    rb->lcd_putsxy(x - 25, 1, "Next");

    rb->lcd_update();
}

int check_board( void ) {
    int i,j;
    int max = 3;
    bool solveable = false;

    for( i=0; i<ROW_COUNT; i++ )
    {
        for( j=0; j<COL_COUNT; j++ ) {
            if(board[i][j] == 0) {
                solveable = true;
            } else if(board[i][j] > 0 && board[i][j] < 3 && (
                (j < COL_COUNT - 1 && board[i][j] + board[i][j + 1] == 3) ||
                (i < ROW_COUNT - 1 && board[i][j] + board[i + 1][j] == 3 ))) {
                solveable = true;
            } else if( board[i][j] > 2 &&
                ((j < COL_COUNT - 1 && board[i][j] == board[i][j + 1]) ||
                (i < ROW_COUNT - 1 && board[i][j] == board[i + 1][j]))) {
                solveable = true;
            }

            if(board[i][j] > max)
                max = board[i][j];
        }
    }

    if(max-2 < 3)
        max = 5;

    if(solveable)
        return draw_random_card(1, max-2);

    return -1;
}

void shuffle(int *array, size_t n) {
    if (n > 1)
    {
        size_t i;
        for (i = 0; i < n - 1; i++)
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          int t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}

void update_board( int direction ) {
    bool single_move = false;
    bool move = false;

    int i, j, max_i, max_j;

    max_i = COL_COUNT;
    max_j = ROW_COUNT - 1;

    if( direction < 2 ) {
        max_i = ROW_COUNT;
        max_j = COL_COUNT - 1;
    }

    for( i=0; i < max_i; i++ ) {
        single_move = false;

        for( j=0; j < max_j; j++ ) {
            int cur_col, cur_row, next_col, next_row;

            switch( direction ) {
                case LEFT:
                    cur_row = i;
                    cur_col = j;
                    next_row = i;
                    next_col = j + 1;
                    break;
                case RIGHT:
                    cur_row = i;
                    cur_col = max_j - j;
                    next_row = i;
                    next_col = max_j - j - 1;
                    break;
                case UP:
                    cur_row = max_j - j;
                    cur_col = i;
                    next_row = max_j - j - 1;
                    next_col = i;
                    break;
                default:
                    cur_row = j;
                    cur_col = i;
                    next_row = j + 1;
                    next_col = i;
                    break;
            }

            if(single_move) {
                board[cur_row][cur_col] = board[next_row][next_col];
            } else {
                if(board[cur_row][cur_col] == 0) {
                    if(board[next_row][next_col] > 0) {
                        board[cur_row][cur_col] = board[next_row][next_col];
                        single_move = true;
                    }
                } else if(board[cur_row][cur_col] > 0 && board[cur_row][cur_col] < 3) {
                    if(board[cur_row][cur_col] + board[next_row][next_col] == 3) {
                        board[cur_row][cur_col] = 3;
                        single_move = true;
                    }
                } else {
                    if (board[cur_row][cur_col] == board[next_row][next_col]) {
                        board[cur_row][cur_col] = board[cur_row][cur_col] + 1;
                        single_move = true;
                    }
                }
            }
        }

        // If a row/column was pushed mark its end
        if(single_move) {
            switch( direction ) {
                case LEFT:
                    board[i][max_j] = -1;
                    break;
                case RIGHT:
                    board[i][0] = -1;
                    break;
                case UP:
                    board[0][i] = -1;
                    break;
                case DOWN:
                    board[max_j][i] = -1;
                    break;
            }
            move = true;
        }
    }

    if( move ) {
        int toshuf[max_j + 1];
        for( i=0; i < max_j + 1; i++) {
            toshuf[i] = i;
        }
        shuffle(toshuf, max_j + 1);

        for( i=0; i < max_j + 1; i++ ) {
            switch( direction ) {
                case LEFT:
                    if( board[toshuf[i]][max_j] == -1 ) {
                        board[toshuf[i]][max_j] = next_value;
                        next_value = 0;
                    }
                    break;
                case RIGHT:
                    if( board[toshuf[i]][0] == -1 ) {
                        board[toshuf[i]][0] = next_value;
                        next_value = 0;
                    }
                    break;
                case UP:
                    if( board[0][toshuf[i]] == -1 ) {
                        board[0][toshuf[i]] = next_value;
                        next_value = 0;
                    }
                    break;
                case DOWN:
                    if( board[max_j][toshuf[i]] == -1 ) {
                        board[max_j][toshuf[i]] = next_value;
                        next_value = 0;
                    }
                    break;
            }
        }

        next_value = check_board();

        if(next_value == -1) {
            draw_board();

            highscore_update(calculate_score(), 1, "TRB", highscores, NUM_SCORES);
            highscore_save(SCORE_FILE, highscores, NUM_SCORES);
            rb->splash(HZ*2, "Game over! Press MENU for new game.");
        }
    }
}

void init_board( void ) {
    memset(board, 0, sizeof(board));

    for( int i=0; i<START_COUNT; i++ ) {
        int rand_r = rb->rand() % ROW_COUNT;
        int rand_c = rb->rand() % COL_COUNT;
        while(board[rand_r][rand_c] > 0) {
            rand_r = rb->rand() % ROW_COUNT;
            rand_c = rb->rand() % COL_COUNT;
        }
        board[rand_r][rand_c] = draw_random_card(1,3);
    }

    next_value = draw_random_card(1,3);

    draw_board();
}

enum plugin_status plugin_start(const void* parameter) {
    (void)parameter;

    int button;
    bool quit = false;

    rb->lcd_clear_display();
    highscore_load(SCORE_FILE, highscores, NUM_SCORES);

    init_board();

    while(!quit) {
        button=rb->button_get(true);
        switch(button) {
            case THREES_BUTTON_MENU:
                init_board();
                break;
            case THREES_BUTTON_QUIT:
                quit = true;
                break;
            case THREES_BUTTON_LEFT:
                update_board(0);
                break;
            case THREES_BUTTON_RIGHT:
                update_board(1);
                break;
            case THREES_BUTTON_DOWN:
                update_board(2);
                break;
            case THREES_BUTTON_UP:
                update_board(3);
                break;
        }

        draw_board();
    }

    highscore_save(SCORE_FILE, highscores, NUM_SCORES);

    return PLUGIN_OK;
}
