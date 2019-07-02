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

#include "UKTidesgui_impl.h"
#include <wx/wx.h>
#include "wx/dir.h"
#include "UKTides_pi.h"

#include <wx/ffile.h>
#include <wx/filefn.h>
#include <wx/url.h>
#include "json/json.h"

class Position;

Dlg::Dlg(UKTides_pi &_UKTides_pi, wxWindow* parent)
	: DlgDef(parent),
	m_UKTides_pi(_UKTides_pi)
{	
	
	this->Fit();
    dbg=false; //for debug output set to true
 
	wxString blank_name = *GetpSharedDataLocation()
		+ "plugins/UKTides_pi/data/blank.ico";

	wxIcon icon(blank_name, wxBITMAP_TYPE_ICO);
	SetIcon(icon);

	wxString station_icon_name = *GetpSharedDataLocation()
		+ "plugins/UKTides_pi/data/station_icon.png";	

	wxString myOpenCPNiconsPath = StandardPath();
	wxString s = wxFileName::GetPathSeparator();
	wxString destination = myOpenCPNiconsPath + "station_icon.png";

	if (!wxFileExists(destination)) {
		wxCopyFile(station_icon_name, destination, true);		
		//wxMessageBox(_("On first use please re-start OpenCPN\n... to enable the tidal station icons"));		
	}
	
	LoadTidalEventsFromXml();
	RemoveOldDownloads();
	
}

Dlg::~Dlg()
{
}

void Dlg::OnInformation(wxCommandEvent& event)
{

	wxString infolocation = *GetpSharedDataLocation()
		+ "plugins/UKTides_pi/data/pictures/" + "UKTides.html";
	wxLaunchDefaultBrowser("file:///" + infolocation);

}

void Dlg::Addpoint(TiXmlElement* Route, wxString ptlat, wxString ptlon, wxString ptname, wxString ptsym, wxString pttype){
//add point
	TiXmlElement * RoutePoint = new TiXmlElement( "rtept" );
    RoutePoint->SetAttribute("lat", ptlat.mb_str());
    RoutePoint->SetAttribute("lon", ptlon.mb_str());


    TiXmlElement * Name = new TiXmlElement( "name" );
    TiXmlText * text = new TiXmlText( ptname.mb_str() );
    RoutePoint->LinkEndChild( Name );
    Name->LinkEndChild( text );

    TiXmlElement * Symbol = new TiXmlElement( "sym" );
    TiXmlText * text1 = new TiXmlText( ptsym.mb_str() );
    RoutePoint->LinkEndChild( Symbol );
    Symbol->LinkEndChild( text1 );

    TiXmlElement * Type = new TiXmlElement( "type" );
    TiXmlText * text2 = new TiXmlText( pttype.mb_str() );
    RoutePoint->LinkEndChild( Type );
    Type->LinkEndChild( text2 );
    Route->LinkEndChild( RoutePoint );
//done adding point
}

void Dlg::OnDownload(wxCommandEvent& event) {		

	myports.clear();
	myPort outPort;

	wxString s_lat, s_lon;

	wxString urlString = "https://admiraltyapi.azure-api.net/uktidalapi/api/V1/Stations?key=cefba1163a81498c9a1e5d03ea1fed69";
	wxURI url(urlString);

	wxString tmp_file = wxFileName::CreateTempFileName("");

	_OCPN_DLStatus ret = OCPN_downloadFile(url.BuildURI(), tmp_file,
		"UKTides", "", wxNullBitmap, this,
		OCPN_DLDS_ELAPSED_TIME | OCPN_DLDS_ESTIMATED_TIME | OCPN_DLDS_REMAINING_TIME | OCPN_DLDS_SPEED | OCPN_DLDS_SIZE | OCPN_DLDS_CAN_PAUSE | OCPN_DLDS_CAN_ABORT | OCPN_DLDS_AUTO_CLOSE,
		10);

	if (ret == OCPN_DL_ABORTED) {
		
		m_stUKDownloadInfo->SetLabel(_("Aborted"));
		return;
	} else

	if (ret == OCPN_DL_FAILED) {
		wxMessageBox(_("Download failed.\n\nAre you connected to the Internet?"));

		m_stUKDownloadInfo->SetLabel(_("Failed"));
		return;
	}

	else {
		m_stUKDownloadInfo->SetLabel(_("Success"));
	}

    wxString myjson;
	wxFFile fileData;
	fileData.Open(tmp_file, wxT("r"));
	fileData.ReadAll(&myjson);

	// construct the JSON root object
	Json::Value  root;
	// construct a JSON parser
	Json::Reader reader;
	wxString error = _("No tidal stations found");

	if (!reader.parse((std::string)myjson, root)) {
		wxLogMessage(error);
		return;
	}

	if (!root.isMember("features")) {
		// Originator
		wxLogMessage(_("No features found in message"));
		return;
	}

	int i = root["features"].size();
	
	for (int j = 0; j < i; j++) {

		Json::Value  features = root["features"][j];

		if (!features.isMember("properties")) {
			// Originator
			wxLogMessage(_("No properties found in message"));
		}

		string name = features["properties"]["Name"].asString();
		wxString myname(name.c_str(), wxConvUTF8);		
		outPort.Name = myname;

		string id = features["properties"]["Id"].asString();
		wxString myId(id.c_str(), wxConvUTF8);
		outPort.Id = myId;

		string lon = features["geometry"]["coordinates"][0].asString();
		s_lon = lon.c_str(), wxConvUTF8;
		string lat = features["geometry"]["coordinates"][1].asString();
		s_lat = lat.c_str(), wxConvUTF8;

		double myLat, myLon;
		s_lat.ToDouble(&myLat);
		s_lon.ToDouble(&myLon);

		//bool rmWpt = DeleteSingleWaypoint(myId);

		outPort.coordLat = myLat;
		outPort.coordLon = myLon;

		PlugIn_Waypoint * pPoint = new PlugIn_Waypoint(myLat, myLon,
			"", myname, "");	

		pPoint->m_IconName = "station_icon";
		pPoint->m_MarkDescription = myId;
		pPoint->m_GUID = myId;
		bool added = AddSingleWaypoint(pPoint, false);

		myports.push_back(outPort);
	}

	RequestRefresh(m_parent);
	root.clear();

}

void Dlg::OnGetSavedTides(wxCommandEvent& event) {
	
	wxString portName;	
	wxString sId;

	double myLat, myLon;
	myLat = 0;
	myLon = 0;

	LoadTidalEventsFromXml();

	if (mySavedPorts.size() == 0) {
		wxMessageBox("No locations are available, please download and select a tidal station");
		return;
	}

	GetTidalEventDialog GetPortDialog(this, -1, _("Select the Port"), wxPoint(200, 200), wxSize(300, 200), wxRESIZE_BORDER);

	GetPortDialog.dialogText->InsertColumn(0, "", 0, wxLIST_AUTOSIZE);
	GetPortDialog.dialogText->SetColumnWidth(0, 290);
	GetPortDialog.dialogText->InsertColumn(1, "", 0, wxLIST_AUTOSIZE);
	GetPortDialog.dialogText->SetColumnWidth(1, 0);
	GetPortDialog.dialogText->DeleteAllItems();

	int in = 0;
	wxString routeName = "";
	for (list<myPort>::iterator it = mySavedPorts.begin(); it != mySavedPorts.end(); it++) {

		portName = (*it).Name;

		sId = (*it).Id;
		myLat = (*it).coordLat;
		myLon = (*it).coordLon;

		PlugIn_Waypoint * pPoint = new PlugIn_Waypoint(myLat, myLon,
			"", portName, "");

		pPoint->m_IconName = "station_icon";
		pPoint->m_MarkDescription = sId;
		pPoint->m_GUID = sId;
		bool added = AddSingleWaypoint(pPoint, false);
		GetParent()->Refresh();

		GetPortDialog.dialogText->InsertItem(in, "", -1);
		GetPortDialog.dialogText->SetItem(in, 0, portName);
		in++;
	}
	this->Fit();
	this->Refresh();

	long si = -1;
	long itemIndex = -1;
	int f = 0;

	wxString portId;

	wxListItem     row_info;
	wxString       cell_contents_string = wxEmptyString;
	bool foundPort = false;

	if (GetPortDialog.ShowModal() != wxID_OK) {
		// Do nothing
	}
	else {

		for (;;) {
			itemIndex = GetPortDialog.dialogText->GetNextItem(itemIndex,
				wxLIST_NEXT_ALL,
				wxLIST_STATE_SELECTED);

			if (itemIndex == -1) break;

			// Got the selected item index
			if (GetPortDialog.dialogText->IsSelected(itemIndex)) {
				si = itemIndex;
				foundPort = true;
				break;
			}
		}

		if (foundPort) {

			// Set what row it is (m_itemId is a member of the regular wxListCtrl class)
			row_info.m_itemId = si;
			// Set what column of that row we want to query for information.
			row_info.m_col = 0;
			// Set text mask
			row_info.m_mask = wxLIST_MASK_TEXT;

			// Get the info and store it in row_info variable.   
			GetPortDialog.dialogText->GetItem(row_info);
			// Extract the text out that cell
			cell_contents_string = row_info.m_text;								

			for (list<myPort>::iterator it = mySavedPorts.begin(); it != mySavedPorts.end(); it++) {
				wxString portName = (*it).Name;
				portId = (*it).Id;
				if (portName == cell_contents_string) {
					OnShowSavedPortTides(portId);
				}
			}			
		}		
	}

	GetParent()->Refresh();

}

void Dlg::getHWLW(string id)
{
	
	myevents.clear();
	TidalEvent outTidalEvent;

	int daysAhead = m_choice3->GetSelection();
	wxString choiceDays = m_choice3->GetString(daysAhead);

	string duration = "?duration=";
	string urlDays = choiceDays.ToStdString();

	string key = "&key=cefba1163a81498c9a1e5d03ea1fed69";
	string tidalevents = "/TidalEvents";


	wxString urlString = "https://admiraltyapi.azure-api.net/uktidalapi/api/V1/Stations/" + id + tidalevents + duration + urlDays + key;
	wxURI url(urlString);

	wxString tmp_file = wxFileName::CreateTempFileName("");

	_OCPN_DLStatus ret = OCPN_downloadFile(url.BuildURI(), tmp_file,
		"", "", wxNullBitmap, this, OCPN_DLDS_AUTO_CLOSE,
		10);
	
	wxString myjson;
	wxFFile fileData;
	fileData.Open(tmp_file, wxT("r"));
	fileData.ReadAll(&myjson);

	// construct the JSON root object
	Json::Value  root2;
	// construct a JSON parser
	Json::Reader reader2;
	wxString error = "Unable to parse json";

	if (!reader2.parse((std::string)myjson, root2)) {
		wxLogMessage(error);
		return;
	}
	
	if (!root2.isArray()) {
		wxLogMessage(error);
		return;
	}
	else {

		int i = root2.size();

		for (int j = 0; j < i; j++) {

			string type = root2[j]["EventType"].asString();
			if (type == "HighWater") type = "HW";
			else if (type == "LowWater") type = "LW";
			wxString mytype(type.c_str(), wxConvUTF8);
			outTidalEvent.EventType = mytype;

			Json::Value  jdt = root2[j];

			if (jdt.isMember("DateTime")) {
				string datetime = root2[j]["DateTime"].asString();
				wxString mydatetime(datetime.c_str(), wxConvUTF8);
				outTidalEvent.DateTime = ProcessDate(mydatetime);
			}
			else {
				outTidalEvent.DateTime = "N/A";
			}

			if (jdt.isMember("Height")) {
				double height = root2[j]["Height"].asDouble();
				wxString myheight(wxString::Format("%4.2f", height));
				outTidalEvent.Height = myheight;
			}
			else {
				outTidalEvent.Height = "N/A";
			}

			myevents.push_back(outTidalEvent);

		}	
	}

	root2.clear();
	
	for (std::list<myPort>::iterator it = mySavedPorts.begin(); it != mySavedPorts.end(); it++) {

		if ((*it).Id == id) {
			mySavedPorts.erase((it));
		}
	}	

	mySavedPort = SavePortTidalEvents(myevents, id);
	mySavedPorts.push_back(mySavedPort);

	SaveTidalEventsToXml(mySavedPorts);

	OnShow();
}

void Dlg::OnShow()
{
		TideTable* tidetable = new TideTable(this, 7000, _("Tides"), wxPoint(200, 200), wxSize(-1, -1), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
		wxString label = m_titlePortName + _("      (Times are UTC)  ") + _(" (Height in metres)");
		tidetable->itemStaticBoxSizer14Static->SetLabel(label);

		wxString Event;
		wxString EventDT;
		wxString EventHeight;


		if (myevents.empty()) {
			wxMessageBox(_("No tidal data found. Please use right click to select the UK tidal station"));
			return;
		}

		int in = 0;

		for (std::list<TidalEvent>::iterator it = myevents.begin();
			it != myevents.end(); it++) {

			Event = (*it).EventType;
			EventDT = (*it).DateTime;
			EventHeight = (*it).Height;

			tidetable->m_wpList->InsertItem(in, "", -1);
			tidetable->m_wpList->SetItem(in, 0, EventDT);
			tidetable->m_wpList->SetItem(in, 1, Event);
			tidetable->m_wpList->SetItem(in, 2, EventHeight);

			in++;

		}

		AutoSizeHeader(tidetable->m_wpList);
		tidetable->Fit();
		tidetable->Layout();
		tidetable->Show();		

	GetParent()->Refresh();

}

void Dlg::OnShowSavedPortTides(wxString thisPortId) {

	if (mySavedPorts.empty()) {
		wxMessageBox(_("No tidal data found. Please download the locations \n and use right click to select the UK tidal station"));
		return;
	}

	TideTable* tidetable = new TideTable(this, 7000, _("Port Tides Saved"), wxPoint(200, 200), wxSize(-1, -1), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
	
	wxString Event;
	wxString EventDT;
	wxString EventHeight;

	for (std::list<myPort>::iterator it = mySavedPorts.begin(); it != mySavedPorts.end(); it++) {	
		
		if ((*it).Id == thisPortId) {

			wxString m_titlePortTides;
			m_titlePortTides = (*it).Name;

			wxString label = m_titlePortTides + _("      (Times are UTC)  ") + _(" (Height in metres)");
			tidetable->itemStaticBoxSizer14Static->SetLabel(label);

			list<TidalEvent> savedevents = (*it).tidalevents;

			int in = 0;

			for (list<TidalEvent>::iterator itt = savedevents.begin(); itt != savedevents.end(); itt++) {

				Event = (*itt).EventType;
				EventDT = (*itt).DateTime;
				EventHeight = (*itt).Height;

				tidetable->m_wpList->InsertItem(in, "", -1);
				tidetable->m_wpList->SetItem(in, 0, EventDT);
				tidetable->m_wpList->SetItem(in, 1, Event);
				tidetable->m_wpList->SetItem(in, 2, EventHeight);

				in++;

			}
		}
	}

	AutoSizeHeader(tidetable->m_wpList);
	tidetable->Fit();
	tidetable->Layout();
	tidetable->Show();

}
	

void Dlg::AutoSizeHeader(wxListCtrl *const list_ctrl)
{
	if (list_ctrl)
	{
		for (int i = 0; i < list_ctrl->GetColumnCount(); ++i)
		{
			list_ctrl->SetColumnWidth(i, wxLIST_AUTOSIZE);
			const int a_width = list_ctrl->GetColumnWidth(i);
			list_ctrl->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
			const int h_width = list_ctrl->GetColumnWidth(i);
			list_ctrl->SetColumnWidth(i, (std::max)(a_width, h_width));
		}
	}
}

wxString Dlg::getPort(double m_lat, double m_lon) {

	wxString mylat = wxString::Format("%f", m_lat);
	wxString m_portId;	
	
	m_portId = getPortId(m_lat, m_lon);

	if (m_portId.IsEmpty()) {		
		return wxEmptyString;
	}

	getHWLW(m_portId.ToStdString());		

	return mylat;
}

wxString Dlg::getPortId(double m_lat, double m_lon) {

	bool foundPort = false;
	double radius = 0.1;
	double dist = 0;
	double myDist, myBrng;
	double plat;
	double plon;
	wxString m_portId;

	if (myports.empty()) {
		wxMessageBox(_("No active tidal stations found. Please download the locations"));
		return wxEmptyString;
	}

	int sizePorts = myports.size();

	while (!foundPort) {
		for (std::list<myPort>::iterator it = myports.begin();	it != myports.end(); it++) {
				{
					plat = (*it).coordLat;
					plon = (*it).coordLon;

					DistanceBearingMercator(plat, plon, m_lat, m_lon, &myDist, &myBrng);

					if (myDist < radius) {
						m_portId = (*it).Id;
						m_titlePortName = (*it).Name;
						foundPort = true;						
						return m_portId;
					}
				}				
		}	
		radius += 0.1;
	}
	return _("Port not found");
}

wxString Dlg::getSavedPortId(double m_lat, double m_lon) {

	bool foundPort = false;
	double radius = 0.1;
	double dist = 0;
	double myDist, myBrng;
	double plat;
	double plon;
	wxString m_portId;

	if (mySavedPorts.empty()) {
		wxMessageBox(_("No tidal stations found. Please download locations when online"));
		return wxEmptyString;
	}	

	while (!foundPort) {
		for (std::list<myPort>::iterator it = mySavedPorts.begin(); it != mySavedPorts.end(); it++) {
			{
				plat = (*it).coordLat;
				plon = (*it).coordLon;

				DistanceBearingMercator(plat, plon, m_lat, m_lon, &myDist, &myBrng);

				if (myDist < radius) {
					m_portId = (*it).Id;
					m_titlePortName = (*it).Name;
					foundPort = true;					
					return m_portId;
				}
			}
		}
		radius += 0.1;
	}
	return _("Port not found");
}


wxString Dlg::ProcessDate(wxString myLongDate) {

	wxDateTime myDateTime;
	myDateTime.ParseISOCombined(myLongDate);
	return myDateTime.Format(" %a %d-%b-%Y   %H:%M");

}


void Dlg::OnClose(wxCloseEvent& event)
{
	plugin->OnUKTidesDialogClose();
}

wxString Dlg::StandardPath()
{
	wxStandardPathsBase& std_path = wxStandardPathsBase::Get();
	wxString s = wxFileName::GetPathSeparator();

#if defined(__WXMSW__)
	wxString stdPath = std_path.GetConfigDir();
#elif defined(__WXGTK__) || defined(__WXQT__)
	wxString stdPath = std_path.GetUserDataDir();
#elif defined(__WXOSX__)
	wxString stdPath = (std_path.GetUserConfigDir() + s + "opencpn");
#endif

	stdPath += s + "plugins" + s + "UKTides";
	if (!wxDirExists(stdPath)) 
		wxMkdir(stdPath);		

#ifdef __WXOSX__	
	wxString oldPath = (std_path.GetUserConfigDir());
	if (wxDirExists(oldPath) && !wxDirExists(stdPath)) {
		wxLogMessage("UKTides_pi: moving config dir %s to %s", oldPath, stdPath);
		wxRenameFile(oldPath, stdPath);
	}
#endif

	stdPath += s; // is this necessary?
	return stdPath;
}

myPort Dlg::SavePortTidalEvents(list<TidalEvent>myEvents, string portId)
{
	myPort thisPort;
	
	double plat, plon;
	plat = 0;
	plon = 0;

	wxString portName;

	for (std::list<myPort>::iterator it = myports.begin(); it != myports.end(); it++) {
		{
			if ((*it).Id == portId) {
				plat = (*it).coordLat;
				plon = (*it).coordLon;
				portName = (*it).Name;
			}
		}
	}

	wxString dtNow = GetDateStringNow();

	thisPort.Name = portName;
	thisPort.DownloadDate = dtNow;
	thisPort.Id = portId;
	thisPort.coordLat = plat;
	thisPort.coordLon = plon;
	thisPort.tidalevents = myevents;

	return thisPort;

}

void Dlg::SaveTidalEventsToXml(list<myPort>myPorts)
{
		
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "utf-8", "");
	doc.LinkEndChild(decl);

	TiXmlElement * root = new TiXmlElement("TidalEventDataSet");
	doc.LinkEndChild(root);

	for (list<myPort>::iterator it = myPorts.begin(); it != myPorts.end(); it++) {		

		TiXmlElement *Port = new TiXmlElement("Port");
		Port->SetAttribute("Name", (*it).Name);
		Port->SetAttribute("DownloadDate", (*it).DownloadDate);
		Port->SetAttribute("Id", (*it).Id);
		Port->SetAttribute("Latitude", wxString::Format("%.5f", (*it).coordLat));
		Port->SetAttribute("Longitude", wxString::Format("%.5f", (*it).coordLon));

		root->LinkEndChild(Port);

		myevents = (*it).tidalevents;

		for (list<TidalEvent>::iterator it = myevents.begin(); it != myevents.end(); it++) {
			TiXmlElement *t = new TiXmlElement("TidalEvent");

			t->SetAttribute("Event", (*it).EventType.mb_str());
			t->SetAttribute("DateTime", (*it).DateTime.mb_str());
			t->SetAttribute("Height", (*it).Height.mb_str());

			Port->LinkEndChild(t);
		}
	}

	wxString filename = "tidalevents.xml";
	wxString tidal_events_path = StandardPath();

	if (!doc.SaveFile(tidal_events_path + filename))
		wxLogMessage(_("UKTides") + wxString(": ") + _("Failed to save xml file: ") + filename);
}

list<myPort>Dlg::LoadTidalEventsFromXml()
{
	mySavedPorts.clear();

	myPort thisPort;
	TidalEvent thisEvent;	
	
	TiXmlDocument doc;
	wxString name;
	wxString tidal_events_path = StandardPath();

	list<TidalEvent> listEvents;

	wxString filename = tidal_events_path + "tidalevents.xml";
	wxFileName fn(filename);

	SetTitle(_("Tidal Events"));

	if (!doc.LoadFile(filename.mb_str()))
		wxLogMessage(_("No UK tide locations available"));
	else {
		TiXmlHandle root(doc.RootElement());

		if (strcmp(root.Element()->Value(), "TidalEventDataSet"))
			wxMessageBox(_("Invalid xml file"));		

		int count = 0;
		for (TiXmlElement* e = root.FirstChild().Element(); e; e = e->NextSiblingElement())
			count++;

		int i = 0;
		for (TiXmlElement* e = root.FirstChild().Element(); e; e = e->NextSiblingElement(), i++) {
								
			if (!strcmp(e->Value(), "Port")) {
				thisPort.Name = e->Attribute("Name");	
				thisPort.DownloadDate = e->Attribute("DownloadDate");
				thisPort.Id = e->Attribute("Id");
				thisPort.coordLat = AttributeDouble(e, "Latitude", NAN);
				thisPort.coordLon = AttributeDouble(e, "Longitude", NAN);

				listEvents.clear();

				for (TiXmlElement* f = e->FirstChildElement(); f; f = f->NextSiblingElement()) {
					if (!strcmp(f->Value(), "TidalEvent")) {
						thisEvent.EventType = f->Attribute("TidalEvent");						
						thisEvent.DateTime = f->Attribute("DateTime");
						thisEvent.Height = f->Attribute("Height");
					}
					listEvents.push_back(thisEvent);
				}

				thisPort.tidalevents = listEvents;
				mySavedPorts.push_back(thisPort);	
			}				
		}
	}

	return mySavedPorts;

}

double Dlg::AttributeDouble(TiXmlElement *e, const char *name, double def)
{
	const char *attr = e->Attribute(name);
	if (!attr)
		return def;
	char *end;
	double d = strtod(attr, &end);
	if (end == attr)
		return def;
	return d;
}

wxString Dlg::GetDateStringNow() {

	m_dtNow = wxDateTime::Now();
	wxString downloadDate = m_dtNow.Format("%Y-%m-%d  %H:%M");
	return downloadDate;

}

void Dlg::RemoveOldDownloads() {
	wxDateTime dtn, ddt;
	wxString sdt, sddt;
	wxTimeSpan DaySpan;
	DaySpan = wxTimeSpan::Days(7);

	dtn = wxDateTime::Now();

	for (std::list<myPort>::iterator it = mySavedPorts.begin(); it != mySavedPorts.end(); it++) {
		sddt = (*it).DownloadDate;
		ddt.ParseDateTime(sddt);
		ddt.Add(DaySpan);

		if (dtn > ddt) {
			mySavedPorts.erase((it));
		}
	}

	SaveTidalEventsToXml(mySavedPorts);
}

GetTidalEventDialog::GetTidalEventDialog(wxWindow * parent, wxWindowID id, const wxString & title,
	const wxPoint & position, const wxSize & size, long style)
	: wxDialog(parent, id, title, position, size, style)
{

	wxString dimensions = wxT(""), s;
	wxPoint p;
	wxSize  sz;

	sz.SetWidth(size.GetWidth() - 20);
	sz.SetHeight(size.GetHeight() - 70);

	p.x = 6; p.y = 2;

	dialogText = new wxListView(this, wxID_ANY, p, sz, wxLC_NO_HEADER | wxLC_REPORT | wxLC_SINGLE_SEL, wxDefaultValidator, wxT(""));

	wxFont *pVLFont = wxTheFontList->FindOrCreateFont(12, wxFONTFAMILY_SWISS, wxNORMAL, wxFONTWEIGHT_NORMAL,
		FALSE, wxString("Arial"));
	dialogText->SetFont(*pVLFont);

	p.y += sz.GetHeight() + 10;

	p.x += 30;
	wxButton * b = new wxButton(this, wxID_OK, _("OK"), p, wxDefaultSize);
	p.x += 140;
	wxButton * c = new wxButton(this, wxID_CANCEL, _("Cancel"), p, wxDefaultSize);
};
