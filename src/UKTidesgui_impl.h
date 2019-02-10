/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  UKTides Plugin
 * Author:   Mike Rossiter
 *
 ***************************************************************************
 *   Copyright (C) 2019 by Mike Rossiter                                   *
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

#ifndef _CALCULATORGUI_IMPL_H_
#define _CALCULATORGUI_IMPL_H_

#ifdef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "UKTidesgui.h"
#include "UKTides_pi.h"
#include "NavFunc.h"
#include "tidetable.h"
#include "tinyxml.h"

#include <list>
#include <vector>

using namespace std;

struct myTidalEvents
{
	wxString EventType;
	wxString DateTime;
	wxString Height;
};

struct myPorts
{
	wxString Name;
	wxString Id;
	double coordLat;
	double coordLon;
};

class UKTides_pi;
class Position;

class Dlg : public DlgDef
{
public:
        Dlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("UKTides"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxCLOSE_BOX|wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );

        void OnDownload( wxCommandEvent& event );
		
		bool OpenXML();
		
		vector<Position> my_positions;
		vector<Position> my_points;

        void Calculate( wxCommandEvent& event, bool Export, int Pattern );

		void OnInformation(wxCommandEvent& event);
        void Addpoint(TiXmlElement* Route, wxString ptlat, wxString ptlon, wxString ptname, wxString ptsym, wxString pttype);

        
		
			
		UKTides_pi *plugin; 

		wxString rte_start;
	    wxString rte_end;
	
		wxString getPort(double m_lat, double m_lon);
		wxString m_default_configuration_path;
		void AutoSizeHeader(wxListCtrl *const list_ctrl);

private:
	
	wxString m_titlePortName;
	list<myPorts>myports;
	list<myTidalEvents>myevents;
	
	
	void getHWLW(string id);
	wxString getPortId(double m_lat, double m_lon);
	wxString ProcessDate(wxString myLongDate);
	void OnShow();

	void OnClose( wxCloseEvent& event );
	
    double lat1, lon1, lat2, lon2;
    bool error_found;
    bool dbg;

	wxString     m_gpx_path;		
};


class Position
{
public:

    wxString lat, lon, wpt_num;
    Position *prev, *next; /* doubly linked circular list of positions */
    int routepoint;

};

#endif
