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
#include "wx/stdpaths.h"

#include <list>
#include <vector>

using namespace std;

struct TidalEvent
{
	wxString EventType;
	wxString DateTime;
	wxString Height;
};

struct myPort
{
	wxString Name;
	wxString DownloadDate;
	wxString Id;
	double coordLat;
	double coordLon;
	list<TidalEvent>tidalevents;
};

class UKTides_pi;
class Position;

class Dlg : public DlgDef
{
public:
	Dlg(UKTides_pi &_UKTides_pi, wxWindow* parent);
	~Dlg();

        void OnDownload( wxCommandEvent& event );	
		void OnGetSavedTides(wxCommandEvent& event);

		void OnInformation(wxCommandEvent& event);
        void Addpoint(TiXmlElement* Route, wxString ptlat, wxString ptlon, wxString ptname, wxString ptsym, wxString pttype);	
			
		UKTides_pi *plugin; 

		wxString rte_start;
	    wxString rte_end;
	
		wxString getPort(double m_lat, double m_lon);
		wxString m_default_configuration_path;
		void AutoSizeHeader(wxListCtrl *const list_ctrl);

		UKTides_pi &m_UKTides_pi;
		wxString StandardPath();	
		list<myPort>myports;	

private:
	
	wxString m_titlePortName;
	
	list<TidalEvent>myevents;
	list<TidalEvent>mySavedEvents;

	myPort mySavedPort;
	list<myPort>mySavedPorts;

	myPort SavePortTidalEvents(list<TidalEvent>myEvents, string portId);
	void SaveTidalEventsToXml(list<myPort>myPorts);

	list<myPort> LoadTidalEventsFromXml();

	double AttributeDouble(TiXmlElement *e, const char *name, double def);
	wxString GetDateStringNow();
	void RemoveOldDownloads();

	void getHWLW(string id);
	wxString getPortId(double m_lat, double m_lon);
	wxString getSavedPortId(double m_lat, double m_lon);
	wxString ProcessDate(wxString myLongDate);
	void OnShow();
	void OnShowSavedPortTides(wxString thisPortId);

	void OnClose( wxCloseEvent& event );
	
    double lat1, lon1, lat2, lon2;
    bool error_found;
    bool dbg;

	wxString     m_gpx_path;	
	wxDateTime m_dtNow;
};


class Position
{
public:

    wxString lat, lon, wpt_num;
    Position *prev, *next; /* doubly linked circular list of positions */
    int routepoint;

};

class GetTidalEventDialog : public wxDialog
{
public:

	GetTidalEventDialog(wxWindow * parent, wxWindowID id, const wxString & title,
		const wxPoint & pos = wxDefaultPosition,
		const wxSize & size = wxDefaultSize,
		long style = wxDEFAULT_DIALOG_STYLE);

	wxListView * dialogText;
	wxString GetText();

	wxStaticBoxSizer* m_pListSizer;
	wxScrolledWindow *itemDialog1;
	wxStaticBox* itemStaticBoxSizer14Static;

	wxButton*     m_OKButton;

private:

	void OnOk(wxCommandEvent & event);

};

#endif
