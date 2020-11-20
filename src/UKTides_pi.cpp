/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  UKTides Plugin
 * Author:   Mike Rossiter
 *
 ***************************************************************************
 *   Copyright (C) 2017 by Mike Rossiter                                *
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

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include "UKTides_pi.h"
#include "UKTidesgui_impl.h"
#include "UKTidesgui.h"
#include "ocpn_plugin.h" 

class UKTides_pi;
class Dlg;

using namespace std;

// the class factories, used to create and destroy instances of the PlugIn

extern "C" DECL_EXP opencpn_plugin* create_pi(void *ppimgr)
{
    return new UKTides_pi(ppimgr);
}

extern "C" DECL_EXP void destroy_pi(opencpn_plugin* p)
{
    delete p;
}

//---------------------------------------------------------------------------------------------------------
//
//    UKTides PlugIn Implementation
//
//---------------------------------------------------------------------------------------------------------

#include "icons.h"

//---------------------------------------------------------------------------------------------------------
//
//          PlugIn initialization and de-init
//
//---------------------------------------------------------------------------------------------------------




UKTides_pi::UKTides_pi(void *ppimgr)
      :opencpn_plugin_116 (ppimgr)
{
    // Create the PlugIn icons
    initialize_images();

    wxFileName fn;

    auto path = GetPluginDataDir("UKTides_pi");
    fn.SetPath(path);
    fn.AppendDir("data");
    fn.SetFullName("UKTides_panel_icon.png");

    path = fn.GetFullPath();
    
    wxInitAllImageHandlers();

    wxLogDebug(wxString("Using icon path: ") + path);
    if (!wxImage::CanRead(path)) {
        wxLogDebug("Initiating image handlers.");
        wxInitAllImageHandlers();
    }
    wxImage panelIcon(path);
    if (panelIcon.IsOk())
        m_panelBitmap = wxBitmap(panelIcon);
    else
        wxLogWarning("UKTides panel icon has NOT been loaded");
    m_bShowUKTides = false;
}

UKTides_pi::~UKTides_pi(void)
{

     delete _img_UKTidesIcon;

	 if (m_pDialog){

		 wxFileConfig *pConf = GetOCPNConfigObject();;

		 if (pConf) {

			 pConf->SetPath(_T("/Settings/UKTides"));

			
		 }
	 }
     
}

int UKTides_pi::Init(void)
{
      AddLocaleCatalog( _T("opencpn-UKTides_pi") );

      // Set some default private member parameters
      m_hr_dialog_x = 40;
      m_hr_dialog_y = 80;
	  m_hr_dialog_sx = 400;
	  m_hr_dialog_sy = 300;
      ::wxDisplaySize(&m_display_width, &m_display_height);

      //    Get a pointer to the opencpn display canvas, to use as a parent for the POI Manager dialog
      m_parent_window = GetOCPNCanvasWindow();

      //    Get a pointer to the opencpn configuration object
      m_pconfig = GetOCPNConfigObject();

      //    And load the configuration items
      LoadConfig();

      //    This PlugIn needs a toolbar icon, so request its insertion
	if(m_bUKTidesShowIcon) {
#ifdef UKTIDES_USE_SVG
        m_leftclick_tool_id = InsertPlugInToolSVG(_T( "UKTides" ), _svg_UKTides, _svg_UKTides, _svg_UKTides_toggled,
            wxITEM_CHECK, _("UKTides"), _T( "" ), NULL, UKTides_TOOL_POSITION, 0, this);
#else
		m_leftclick_tool_id = InsertPlugInTool(_T(""), _img_UKTidesIcon, _img_UKTidesIcon, wxITEM_CHECK,
            _("UKTides"), _T(""), NULL,
             UKTides_TOOL_POSITION, 0, this);
#endif
    }
	wxMenu dummy_menu;
	m_position_menu_id = AddCanvasContextMenuItem
		(new wxMenuItem(&dummy_menu, -1, _("Select Vessel Start Position")), this);
	SetCanvasContextMenuItemViz(m_position_menu_id, true);

      m_pDialog = NULL;	  

      return (
			  WANTS_OVERLAY_CALLBACK |
			  WANTS_OPENGL_OVERLAY_CALLBACK |
              WANTS_TOOLBAR_CALLBACK    |
              INSTALLS_TOOLBAR_TOOL     |   
			  WANTS_CURSOR_LATLON |
              WANTS_CONFIG           
           );
}

bool UKTides_pi::DeInit(void)
{
      //    Record the dialog position
      if (NULL != m_pDialog)
      {
            //Capture dialog position
            wxPoint p = m_pDialog->GetPosition();
			wxRect r = m_pDialog->GetRect();
            SetUKTidesDialogX(p.x);
            SetUKTidesDialogY(p.y);
			SetUKTidesDialogSizeX(r.GetWidth());
			SetUKTidesDialogSizeY(r.GetHeight());

            m_pDialog->Close();
            delete m_pDialog;
            m_pDialog = NULL;

			m_bShowUKTides = false;
			SetToolbarItemState( m_leftclick_tool_id, m_bShowUKTides );
      }	
    
    SaveConfig();

    RequestRefresh(m_parent_window); // refresh main window

    return true;
}

int UKTides_pi::GetAPIVersionMajor()
{
      return MY_API_VERSION_MAJOR;
}

int UKTides_pi::GetAPIVersionMinor()
{
      return MY_API_VERSION_MINOR;
}

int UKTides_pi::GetPlugInVersionMajor()
{
      return PLUGIN_VERSION_MAJOR;
}

int UKTides_pi::GetPlugInVersionMinor()
{
      return PLUGIN_VERSION_MINOR;
}

wxBitmap *UKTides_pi::GetPlugInBitmap()
{
	return &m_panelBitmap;
}

wxString UKTides_pi::GetCommonName()
{
      return _("UKTides");
}


wxString UKTides_pi::GetShortDescription()
{
      return _("UKTides player");
}

wxString UKTides_pi::GetLongDescription()
{
      return _("UK Tides for over 600 ports");
}

int UKTides_pi::GetToolbarToolCount(void)
{
      return 1;
}

void UKTides_pi::SetColorScheme(PI_ColorScheme cs)
{
      if (NULL == m_pDialog)
            return;

      DimeWindow(m_pDialog);
}


void UKTides_pi::OnToolbarToolCallback(int id)
{

      if(NULL == m_pDialog)
      {
            m_pDialog = new Dlg(m_parent_window);
            m_pDialog->plugin = this;						
            m_pDialog->Move(wxPoint(m_hr_dialog_x, m_hr_dialog_y));	
			m_pDialog->SetSize(m_hr_dialog_sx, m_hr_dialog_sy);	
			
      }

	 // m_pDialog->Fit();
	  //Toggle 
	  m_bShowUKTides = !m_bShowUKTides;	

	  

      //    Toggle dialog? 
      if(m_bShowUKTides) {
		  m_pDialog->Move(wxPoint(m_hr_dialog_x, m_hr_dialog_y));
		  m_pDialog->SetSize(m_hr_dialog_sx, m_hr_dialog_sy);
          m_pDialog->Show(); 
		  
	  }
	  else {
		  m_pDialog->Hide();
	  }
     
      // Toggle is handled by the toolbar but we must keep plugin manager b_toggle updated
      // to actual status to ensure correct status upon toolbar rebuild
      SetToolbarItemState( m_leftclick_tool_id, m_bShowUKTides);

	  //Capture dialog position
	  wxPoint p = m_pDialog->GetPosition();
	  wxRect r = m_pDialog->GetRect();
	  SetUKTidesDialogX(p.x);
	  SetUKTidesDialogY(p.y);
	  SetUKTidesDialogSizeX(r.GetWidth());
	  SetUKTidesDialogSizeY(r.GetHeight());


      RequestRefresh(m_parent_window); // refresh main window
}

bool UKTides_pi::LoadConfig(void)
{
      wxFileConfig *pConf = (wxFileConfig *)m_pconfig;

      if(pConf)
      {
            pConf->SetPath ( _T( "/Settings/UKTides_pi" ) );
			pConf->Read ( _T( "ShowUKTidesIcon" ), &m_bUKTidesShowIcon, 1 );

            m_hr_dialog_x =  pConf->Read ( _T ( "DialogPosX" ), 40L );
            m_hr_dialog_y =  pConf->Read ( _T ( "DialogPosY" ), 140L);
			m_hr_dialog_sx = pConf->Read ( _T ( "DialogSizeX"), 330L);
#ifdef __WXOSX__
			m_hr_dialog_sy = pConf->Read ( _T ( "DialogSizeY"), 250L);
#else
            m_hr_dialog_sy = pConf->Read ( _T ( "DialogSizeY"), 300L);
#endif
            if((m_hr_dialog_x < 0) || (m_hr_dialog_x > m_display_width))
                  m_hr_dialog_x = 40;
            if((m_hr_dialog_y < 0) || (m_hr_dialog_y > m_display_height))
                  m_hr_dialog_y = 140;

            return true;
      }
      else
            return false;
}

bool UKTides_pi::SaveConfig(void)
{
      wxFileConfig *pConf = (wxFileConfig *)m_pconfig;

      if(pConf)
      {
            pConf->SetPath ( _T ( "/Settings/UKTides_pi" ) );
			pConf->Write ( _T ( "ShowUKTidesIcon" ), m_bUKTidesShowIcon );

            pConf->Write ( _T ( "DialogPosX" ),   m_hr_dialog_x );
            pConf->Write ( _T ( "DialogPosY" ),   m_hr_dialog_y );
			pConf->Write ( _T ( "DialogSizeX"),   m_hr_dialog_sx);
			pConf->Write ( _T ( "DialogSizeY"),   m_hr_dialog_sy);
            
            return true;
      }
      else
            return false;
}

void UKTides_pi::OnUKTidesDialogClose()
{
    m_bShowUKTides = false;
    SetToolbarItemState( m_leftclick_tool_id, m_bShowUKTides);
    m_pDialog->Hide();
    SaveConfig();

    RequestRefresh(m_parent_window); // refresh main window
}

void UKTides_pi::OnContextMenuItemCallback(int id)
{

	if (!m_pDialog)
		return;

	if (id == m_position_menu_id){

		m_cursor_lat = GetCursorLat();
		m_cursor_lon = GetCursorLon();

		//m_pDialog->OnContextMenu(m_cursor_lat, m_cursor_lon);
	}
}

void UKTides_pi::SetCursorLatLon(double lat, double lon)
{
	m_cursor_lat = lat;
	m_cursor_lon = lon;
}
