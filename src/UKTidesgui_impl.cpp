/******************************************************************************
*
* Project:  OpenCPN
* Purpose:  UKTides Plugin
* Author:   Mike Rossiter
*
***************************************************************************
*   Copyright (C) 2017 by Mike Rossiter                                   *
*   $EMAIL$                                                               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************
*/


#include "UKTidesgui_impl.h"
#include <wx/progdlg.h>
#include <wx/wx.h>
#include "UKTides_pi.h"

#include <stdio.h>
#include <wx/timer.h>
#include "wx/textfile.h"

class GribRecordSet;


void assign(char *dest, char *arrTest2)
{
	strcpy(dest, arrTest2);
}

#define BUFSIZE 0x10000

Dlg::Dlg(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : DlgDef(parent, id, title, pos, size, style)
{
	this->Fit();
	dbg = false; //for debug output set to true
	

	wxFileConfig *pConf = GetOCPNConfigObject();

	if (pConf) {
		pConf->SetPath(_T("/Settings/UKTides_pi"));
	}
}

