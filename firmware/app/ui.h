//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#ifndef UI_H_
#define UI_H_

typedef struct {
    char title[60];
    void (*callback)(void);
} ui_menu_item_t;

typedef struct {
    int count;
    char title[60];
    ui_menu_item_t items[];
} ui_menu_t;

void ui_init(void);
void ui_message(char *title, char *text);
void ui_clear(void);
void ui_wait_key_press(uint32_t keymask);
void ui_wait_key_release(uint32_t keymask);

#endif /* UI_H_ */
