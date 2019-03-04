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
#include <wx/progdlg.h>
#include <wx/wx.h>
#include "wx/dir.h"
#include <list>
#include <cmath>
#include "UKTides_pi.h"

#include <iostream>
#include <ostream>
#include <string>
#include <map>

#include <wx/ffile.h>
#include <wx/url.h>
#include "json/json.h"

class Position;
class UKTides_pi;

#define FAIL(X) do { error = X; goto failed; } while(0)


Dlg::Dlg(UKTides_pi &_UKTides_pi, wxWindow* parent)
	: DlgDef(parent),
	m_UKTides_pi(_UKTides_pi)
{

	this->Fit();
    dbg=false; //for debug output set to true

	wxString blank_name = *GetpSharedDataLocation()
		+ _T("plugins/UKTides_pi/data/blank.ico");

	wxIcon icon(blank_name, wxBITMAP_TYPE_ICO);
	SetIcon(icon);
}

Dlg::~Dlg()
{
}

void Dlg::OnInformation(wxCommandEvent& event)
{

	wxString infolocation = *GetpSharedDataLocation()
		+ _T("plugins/UKTides_pi/data/pictures/") + _("UKTides.html");
	wxLaunchDefaultBrowser(_T("file:///") + infolocation);

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
	myPorts outPort;

	wxString s_lat, s_lon;

	wxString urlString = _T("https://admiraltyapi.azure-api.net/uktidalapi/api/V1/Stations?key=");
	wxURI url(urlString);

	wxString tmp_file = wxFileName::CreateTempFileName(_T(""));

	_OCPN_DLStatus ret = OCPN_downloadFile(url.BuildURI(), tmp_file,
		_T("UKTides"), _T(""), wxNullBitmap, this,
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
	wxString error = _T("No ports found, please download the locations");

	if (!reader.parse((std::string)myjson, root)) {
		wxMessageBox(error);
		return;
	}

	if (!root.isMember("features")) {
		// Originator
		wxMessageBox("No Source found in message");
	}

	//Json::Value  rootfeatures = root["features"];

	int i = root["features"].size();
	//wxString mysize = wxString::Format(_T("%i"), i);
	//wxMessageBox(mysize);

	for (int j = 0; j < i; j++) {

		Json::Value  features = root["features"][j];

		if (!features.isMember("properties")) {
			// Originator
			wxMessageBox("No properties found in message");
		}

		string name = features["properties"]["Name"].asString();
		wxString myname(name.c_str(), wxConvUTF8);
		//wxMessageBox(myname);
		outPort.Name = myname;

		string id = features["properties"]["Id"].asString();
		wxString myId(id.c_str(), wxConvUTF8);
		//wxMessageBox(myname);
		outPort.Id = myId;

		string lon = features["geometry"]["coordinates"][0].asString();
		s_lon = lon.c_str(), wxConvUTF8;
		string lat = features["geometry"]["coordinates"][1].asString();
		s_lat = lat.c_str(), wxConvUTF8;

		double myLat, myLon;
		s_lat.ToDouble(&myLat);
		s_lon.ToDouble(&myLon);

		outPort.coordLat = myLat;
		outPort.coordLon = myLon;

		PlugIn_Waypoint * pPoint = new PlugIn_Waypoint(myLat, myLon,
			"", myname, "");

		pPoint->m_IconName = _T("anchorage");
		pPoint->m_MarkDescription = myId;
		bool added = AddSingleWaypoint(pPoint, false);

		myports.push_back(outPort);
	}

	RequestRefresh(m_parent);
	root.clear();

}


void Dlg::getHWLW(string id)
{
	myevents.clear();
	myTidalEvents outTidalEvents;

	int daysAhead = m_choice3->GetSelection();
	wxString choiceDays = m_choice3->GetString(daysAhead);

	string duration = "?duration=";
	string urlDays = choiceDays.ToStdString();

	string key = "&key=";
	string tidalevents = "/TidalEvents";


	wxString urlString = "https://admiraltyapi.azure-api.net/uktidalapi/api/V1/Stations/" + id + tidalevents + duration + urlDays + key;
	wxURI url(urlString);

	wxString tmp_file = wxFileName::CreateTempFileName(_T(""));

	_OCPN_DLStatus ret = OCPN_downloadFile(url.BuildURI(), tmp_file,
		_T(""), _T(""), wxNullBitmap, this, OCPN_DLDS_AUTO_CLOSE,
		10);

	wxString myjson;
	wxFFile fileData;
	fileData.Open(tmp_file, wxT("r"));
	fileData.ReadAll(&myjson);

	// construct the JSON root object
	Json::Value  root2;
	// construct a JSON parser
	Json::Reader reader2;
	wxString error = _T("Please download port locations");

	if (!reader2.parse((std::string)myjson, root2)) {
		wxMessageBox(error);
		return;
	}

	if (!root2.isArray()) {
		wxMessageBox(error);
	}
	else {
		//wxMessageBox(_T("Array"));
		int i = root2.size();
		wxString mysize = wxString::Format(_T("%i"), i);
		//wxMessageBox(mysize);

		for (int j = 0; j < i; j++) {

			string type = root2[j]["EventType"].asString();
			if (type == "HighWater") type = "HW";
			else if (type == "LowWater") type = "LW";
			wxString mytype(type.c_str(), wxConvUTF8);
			outTidalEvents.EventType = mytype;

			Json::Value  jdt = root2[j];

			if (jdt.isMember("DateTime")) {
				string datetime = root2[j]["DateTime"].asString();
				wxString mydatetime(datetime.c_str(), wxConvUTF8);
				outTidalEvents.DateTime = ProcessDate(mydatetime);
			}
			else {
				outTidalEvents.DateTime = _T("N/A");
			}

			if (jdt.isMember("Height")) {
				double height = root2[j]["Height"].asDouble();
				wxString myheight(wxString::Format("%4.2f", height));
				outTidalEvents.Height = myheight;
			}
			else {
				outTidalEvents.Height = _T("N/A");
			}

			myevents.push_back(outTidalEvents);

		}

	}
	root2.clear();

	OnShow();
}

void Dlg::OnShow()
{

TideTable* tidetable = new TideTable(this, 7000, _T("Tides"), wxPoint(200, 200), wxSize(-1, -1), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

tidetable->itemStaticBoxSizer14Static->SetLabel(m_titlePortName);

wxString Event;
wxString EventDT;
wxString EventHeight;


if (myevents.empty()) {

	wxMessageBox(_("No Events found. Please download tides"));
	return;
}

int in = 0;

for (std::list<myTidalEvents>::iterator it = myevents.begin();
	it != myevents.end(); it++) {


	Event = (*it).EventType;
	EventDT = (*it).DateTime;
	EventHeight = (*it).Height;


	tidetable->m_wpList->InsertItem(in, _T(""), -1);
	tidetable->m_wpList->SetItem(in, 0, EventDT);
	tidetable->m_wpList->SetItem(in, 1, Event);
	tidetable->m_wpList->SetItem(in, 2, EventHeight);


	in++;

}

AutoSizeHeader(tidetable->m_wpList);

tidetable->Show();

GetParent()->Refresh();

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

	wxString mylat = wxString::Format(_T("%f"), m_lat);
	wxString m_portId = getPortId(m_lat, m_lon);
	//wxMessageBox(m_portId);
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
		wxMessageBox(_("No Ports found. Please download locations"));
		return wxEmptyString;
	}

	int sizePorts = myports.size();

	while (!foundPort) {
		for (std::list<myPorts>::iterator it = myports.begin();	it != myports.end(); it++) {
				{
					plat = (*it).coordLat;
					plon = (*it).coordLon;

					DistanceBearingMercator(plat, plon, m_lat, m_lon, &myDist, &myBrng);

					if (myDist < radius) {
						m_portId = (*it).Id;
						m_titlePortName = (*it).Name;
						foundPort = true;
						//wxMessageBox((*it).Name);
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
	return myDateTime.Format(_T(" %a %d-%b-%Y %H:%M  UTC"));

}


void Dlg::OnClose(wxCloseEvent& event)
{
	plugin->OnUKTidesDialogClose();
}


bool Dlg::OpenXML()
{
    Position my_position;

    my_positions.clear();

	int response = wxID_CANCEL;
	int my_count = 0;

	wxArrayString file_array;
    wxString filename;
	wxFileDialog openDialog( this, _( "Import GPX Route file" ), m_gpx_path, wxT ( "" ),
                wxT ( "GPX files (*.gpx)|*.gpx|All files (*.*)|*.*" ),
                wxFD_OPEN | wxFD_MULTIPLE );
        response = openDialog.ShowModal();
        if( response == wxID_OK ) {
            openDialog.GetPaths( file_array );

            //    Record the currently selected directory for later use
            if( file_array.GetCount() ) {
                wxFileName fn( file_array[0] );
				filename = file_array[0];
                m_gpx_path = fn.GetPath();
            }
        }
		else if(response = wxID_CANCEL){
		return false;
		}

    TiXmlDocument doc;
    wxString error;
    wxProgressDialog *progressdialog = NULL;


	if(!doc.LoadFile(filename.mb_str())){
        FAIL(_("Failed to load file: ") + filename);
	}
    else {
        TiXmlElement *root = doc.RootElement();
        if(!strcmp(root->Value(), "rte"))
            FAIL(_("rte Invalid xml file"));

        int count = 0;
        for(TiXmlElement* e = root->FirstChildElement(); e; e = e->NextSiblingElement())
            count++;

        int i=0;
        for(TiXmlElement* e = root->FirstChildElement(); e; e = e->NextSiblingElement(), i++) {
            if(progressdialog) {
                if(!progressdialog->Update(i))
                    return true;
            } else {
               if(1) {
                    progressdialog = new wxProgressDialog(
                        _("Route"), _("Loading"), count, this,
                        wxPD_CAN_ABORT | wxPD_ELAPSED_TIME | wxPD_REMAINING_TIME);
                }
            }

                for(TiXmlElement* f = e->FirstChildElement(); f; f = f->NextSiblingElement()) {
                    if(!strcmp(f->Value(), "rtept")) {
                        wxString rte_lat = wxString::FromUTF8(f->Attribute("lat"));
						wxString rte_lon = wxString::FromUTF8(f->Attribute("lon"));

						my_position.lat = rte_lat;
						my_position.lon = rte_lon;
						my_positions.push_back(my_position);
					}  //else if(!strcmp(f->Value(), "extensions")) {
                        //rte_start = wxString::FromUTF8(f->Attribute("opencpn:start"));
						//rte_end = wxString::FromUTF8(f->Attribute("opencpn:end"));

                    //}
                }

        }
    }

    delete progressdialog;
    return true;

failed:
    delete progressdialog;

    wxMessageDialog mdlg(this, error, _("UKTides"), wxOK | wxICON_ERROR);
    mdlg.ShowModal();

    return false;
}


void Dlg::Calculate( wxCommandEvent& event, bool write_file, int Pattern  ){
   if(OpenXML()){

	bool error_occured=false;
   // double dist, fwdAz, revAz;


    double lat1,lon1;
   // if(!this->m_Lat1->GetValue().ToDouble(&lat1)){ lat1=0.0;}
   // if(!this->m_Lon1->GetValue().ToDouble(&lon1)){ lon1=0.0;}
	int num_hours;

	num_hours = 1;

	// wxString str_countPts =  wxString::Format(wxT("%d"), (int)num_hours);
    // wxMessageBox(str_countPts,_T("count_hours"));

	lat1 = 0.0;
	lon1 = 0.0;
    //if (error_occured) wxMessageBox(_T("error in conversion of input coordinates"));

    //error_occured=false;
    wxString s;
    if (write_file){
        wxFileDialog dlg(this, _("Export UKTides Positions in GPX file as"), wxEmptyString, wxEmptyString, _T("GPX files (*.gpx)|*.gpx|All files (*.*)|*.*"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
        if (dlg.ShowModal() == wxID_CANCEL){
            error_occured=true;     // the user changed idea...
		    return;
		}

		//dlg.ShowModal();
        s=dlg.GetPath();
        //  std::cout<<s<< std::endl;
        if (dlg.GetPath() == wxEmptyString){ error_occured=true; if (dbg) printf("Empty Path\n");}
    }

    //Validate input ranges
    if (!error_occured){
        if (std::abs(lat1)>90){ error_occured=true; }
        if (std::abs(lon1)>180){ error_occured=true; }
        if (error_occured) wxMessageBox(_("error in input range validation"));
    }

    //Start GPX
    TiXmlDocument doc;
    TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "utf-8", "" );
    doc.LinkEndChild( decl );
    TiXmlElement * root = new TiXmlElement( "gpx" );
    TiXmlElement * Route = new TiXmlElement( "rte" );
    TiXmlElement * RouteName = new TiXmlElement( "name" );
    TiXmlText * text4 = new TiXmlText("Name");

    if (write_file){
        doc.LinkEndChild( root );
        root->SetAttribute("version", "0.1");
        root->SetAttribute("creator", "UKTides_pi by Rasbats");
        root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
        root->SetAttribute("xmlns:gpxx","http://www.garmin.com/xmlschemas/GpxExtensions/v3" );
        root->SetAttribute("xsi:schemaLocation", "http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd");
        root->SetAttribute("xmlns:opencpn","http://www.opencpn.org");
        Route->LinkEndChild( RouteName );
        RouteName->LinkEndChild( text4 );


        TiXmlElement * Extensions = new TiXmlElement( "extensions" );

        TiXmlElement * StartN = new TiXmlElement( "opencpn:start" );
        TiXmlText * text5 = new TiXmlText( "Start" );
        Extensions->LinkEndChild( StartN );
        StartN->LinkEndChild( text5 );

        TiXmlElement * EndN = new TiXmlElement( "opencpn:end" );
        TiXmlText * text6 = new TiXmlText( "End" );
        Extensions->LinkEndChild( EndN );
        EndN->LinkEndChild( text6 );

        Route->LinkEndChild( Extensions );
    }

    switch ( Pattern ) {
    case 1:
        {
        if (dbg) cout<<"UKTides Calculation\n";
        double speed=0;
		int    interval=1;


		speed = speed*interval;

        int n=0;
        //int multiplier=1;
        double lati, loni;
        double latN[100], lonN[100];
		double latF, lonF;

		Position my_point;

		double value, value1;

		for(std::vector<Position>::iterator it = my_positions.begin();  it != my_positions.end(); it++){

			if(!(*it).lat.ToDouble(&value)){ /* error! */ }
				lati = value;
			if(!(*it).lon.ToDouble(&value1)){ /* error! */ }
				loni = value1;

		latN[n] = lati;
		lonN[n] = loni;

		n++;//0,1,2,3
		}

		my_positions.clear();

		n--;//n = 2,  0,1,2
		int routepoints = n+1; //3


		double myDist, myBrng, myDistForBrng;
		int count_pts;
		double remaining_dist, myLast, route_dist;
		remaining_dist = 0;
		route_dist= 0;
		myLast = 0;
		myDistForBrng =0;
		double total_dist = 0;
		int i,c;
		bool skip = false;
		bool inloop = false;
		bool setF = false;

		latF = latN[0];
		lonF = lonN[0];

		// Start of new logic
		for (i=0; i<n; i++){	// n is number of routepoints

			// save the routepoint
			my_point.lat = wxString::Format(wxT("%f"),latN[i]);
			my_point.lon = wxString::Format(wxT("%f"),lonN[i]);
			my_point.routepoint = 1;
			my_point.wpt_num =  wxString::Format(wxT("%d"),(int)i);
			my_points.push_back(my_point);

			if (i==0){ // First F is a routepoint
				latF = latN[i];
				lonF = lonN[i];
			}

			DistanceBearingMercator(latN[i + 1], lonN[i + 1],latF, lonF,  &myDist, &myBrng);

			total_dist = total_dist + myDist;

			if (total_dist > speed){
						// UKTides point after route point
				        //
						route_dist = total_dist - myDist;
						remaining_dist = speed - route_dist;

						DistanceBearingMercator( latN[i + 1], lonN[i + 1], latN[i], lonN[i],&myDist, &myBrng);
						destLoxodrome(latN[i], lonN[i], myBrng, remaining_dist, &lati, &loni);

						// Put in DR after many route points
						my_point.lat = wxString::Format(wxT("%f"),lati);
						my_point.lon = wxString::Format(wxT("%f"),loni);
						my_point.routepoint = 0;
						my_points.push_back(my_point);

						latF = lati;
						lonF = loni;

						total_dist = 0;

						//
				        //
						DistanceBearingMercator(latN[i + 1], lonN[i + 1], latF, lonF, &myDistForBrng, &myBrng);

				        if (myDistForBrng > speed){

							// put in the UKTides positions
							//
							count_pts = (int)floor(myDistForBrng/speed);
							//
							remaining_dist = myDistForBrng - (count_pts*speed);
							DistanceBearingMercator(latN[i + 1], lonN[i + 1], latF, lonF, &myDistForBrng, &myBrng);

							for (c = 1; c <= count_pts ; c++){
								destLoxodrome(latF, lonF, myBrng, speed*c, &lati, &loni);
								// print mid points
								my_point.lat = wxString::Format(wxT("%f"),lati);
								my_point.lon = wxString::Format(wxT("%f"),loni);
								my_point.routepoint = 0;
								my_points.push_back(my_point);
								//	End of prints
								}

							latF = lati;
							lonF = loni;

							total_dist = 0;
							//
							//
							// All the UKTides positions inserted
						}

						if (total_dist == 0){
							DistanceBearingMercator(latN[i + 1], lonN[i + 1], latF, lonF, &myDistForBrng, &myBrng);
							total_dist = myDistForBrng;
							latF = latN[i+1];
							lonF = lonN[i+1];
						}

			}
			else{
				//
				latF = latN[i+1];
				lonF = lonN[i+1];
				//
				//
				//
				//
			}   //

		}
		// End of new logic
		// print the last routepoint
		my_point.lat = wxString::Format(wxT("%f"),latN[i]);
		my_point.lon = wxString::Format(wxT("%f"),lonN[i]);
		my_point.routepoint = 1;
		my_point.wpt_num =  wxString::Format(wxT("%d"),(int)i);
		my_points.push_back(my_point);
		//


		for(std::vector<Position>::iterator itOut = my_points.begin();  itOut != my_points.end(); itOut++){
			//wxMessageBox((*it).lat, _T("*it.lat"));

        double value, value1;
		if(!(*itOut).lat.ToDouble(&value)){ /* error! */ }
			lati = value;
		if(!(*itOut).lon.ToDouble(&value1)){ /* error! */ }
			loni = value1;

		if ((*itOut).routepoint == 1){
			if (write_file){Addpoint(Route, wxString::Format(wxT("%f"),lati), wxString::Format(wxT("%f"),loni), (*itOut).wpt_num ,_T("diamond"),_T("WPT"));}
		}
		else{
			if ((*itOut).routepoint == 0){
				if (write_file){Addpoint(Route, wxString::Format(wxT("%f"),lati), wxString::Format(wxT("%f"),loni), _T("UKTides") ,_T("square"),_T("WPT"));}
			}
		}

		}

		my_points.clear();
        break;

		}


      default:
      {            // Note the colon, not a semicolon
        cout<<"Error, bad input, quitting\n";
        break;
      }
    }

       if (write_file){
            root->LinkEndChild( Route );
            // check if string ends with .gpx or .GPX
            if (!wxFileExists(s)){
                 s = s + _T(".gpx");
            }
            wxCharBuffer buffer=s.ToUTF8();
            if (dbg) std::cout<< buffer.data()<<std::endl;
            doc.SaveFile( buffer.data() );}
    //} //end of if no error occured

    if (error_occured==true)  {
        wxLogMessage(_("Error in calculation. Please check input!") );
        wxMessageBox(_("Error in calculation. Please check input!") );
    }
  }
}
