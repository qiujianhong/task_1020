/*
 * \file dti.c
 * \author Wei Yongming <ymwei@minigui.org>
 * \date 1998/12/xx
 *
 * This file defines the desktop interface funtions.  You should always 
 * include the file in your projects for MiniGUI-Threads.
 *
 \verbatim
    Copyright (C) 2002-2005 Feynman Software.
    Copyright (C) 1999-2002 Wei Yongming.
  
    This file is part of MiniGUI, a compact cross-platform Graphics 
    User Interface (GUI) support system for real-time embedded systems.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    If you are using MiniGUI for developing commercial, proprietary, or other
    software not covered by the GPL terms, you must have a commercial license
    for MiniGUI. Please see http://www.minigui.com/product/index.html for 
    how to obtain this. If you are interested in the commercial MiniGUI 
    licensing, please write to sales@minigui.com. 

 \endverbatim
 */

/*
 * $Id: dti.c,v 1.2 2012/09/22 03:41:24 gujun Exp $
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     and ThreadX version 1.6.x
 *             Copyright (C) 1998-2002 Wei Yongming.
 *             Copyright (C) 2002-2005 Feynman Software.
 */

#include <minigui/common.h>

#ifndef _LITE_VERSION

#ifdef __WINBOND_SWLINUX__

int user_thread (int (*)(char **), char **, unsigned long);
int minigui_entry (int argc, const char* argv[]);

int start_minigui (char **data)
{
    return minigui_entry (0, NULL);
}

void Start (void)
{
    user_thread (start_minigui, (char**)NULL, 0);
}

#endif /* __WINBOND_SWLINUX__ */

#endif /* !_LITE_VERSION */

