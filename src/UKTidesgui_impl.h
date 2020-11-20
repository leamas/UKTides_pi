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

#ifndef _HRGUI_IMPL_H_
#define _HRGUI_IMPL_H_

#ifdef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "UKTidesgui.h"
#include "UKTides_pi.h"
#include <wx/utils.h>
#include <wx/gdicmn.h>
#include <sstream>
#include <cmath>
#include <wx/stdpaths.h>
#include "wx/process.h"
#include "json/reader.h"
#include "json/writer.h"
#include <wx/thread.h>
#include "tinyxml.h"
#include <wx/filedlg.h>
#include "ocpn_plugin.h"
#include <wx/listctrl.h>

#ifdef __WXOSX__
#define UKTIDES_DLG_STYLE wxCLOSE_BOX|wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxSTAY_ON_TOP
#else
#define UKTIDES_DLG_STYLE wxCLOSE_BOX|wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER
#endif
using namespace std;

class UKTides_pi;

// An identifier to notify the application when the // work is done #define ID_COUNTED_COLORS    100


class Dlg : public DlgDef
{
public:
	Dlg(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("UKTides"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = UKTIDES_DLG_STYLE);
	
	UKTides_pi *plugin;
	bool dbg;

protected:

private:

};


#endif



