/*
 *  Copyright (C) 2002-2020  The DOSBox Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef DOSBOX_MAPPER_H
#define DOSBOX_MAPPER_H

#include "include/menu.h"

enum MapKeys {
    MK_nothing,
	MK_f1,MK_f2,MK_f3,MK_f4,MK_f5,MK_f6,MK_f7,MK_f8,MK_f9,MK_f10,MK_f11,MK_f12,
	MK_return,MK_kpminus,MK_kpplus,MK_minus,MK_equals,MK_scrolllock,MK_printscreen,MK_pause,MK_home,MK_rightarrow,
	MK_1, MK_2, MK_3, MK_4,
    MK_c, MK_d, MK_f, MK_m, MK_r, MK_s, MK_v, MK_w,
    MK_escape,MK_delete,
    MK_lbracket,MK_rbracket,MK_leftarrow,

    MK_MAX
};

typedef void (MAPPER_Handler)(bool pressed);
void MAPPER_AddHandler(MAPPER_Handler * handler,MapKeys key,Bitu mods,char const * const eventname,char const * const buttonname,DOSBoxMenu::item **ret_menuitem=NULL);
void MAPPER_Init(void);
void MAPPER_StartUp();
void MAPPER_Run(bool pressed);
void MAPPER_RunEvent(Bitu);
void MAPPER_RunInternal();
void MAPPER_LosingFocus(void);

std::string mapper_event_keybind_string(const std::string &x);

#define MMOD1 0x1
#define MMOD2 0x2
#define MMOD3 0x4
#define MMODHOST 0x8

#endif
