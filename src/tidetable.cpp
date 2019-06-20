/**************************************************************************
*
* Project:  OpenCPN
* Purpose:  TideTable Support
* Author:   Mike Rossiter after David Register
*
***************************************************************************
*   Copyright (C) 2010 by David S. Register                               *
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
*   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
**************************************************************************/

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/datetime.h>
#include <wx/stattext.h>
#include <wx/clrpicker.h>
#include <wx/event.h>

#include "tidetable.h"


#define    pi        (4.*atan(1.0))
#define    tpi        (2.*pi)
#define    twopi    (2.*pi)
#define    degs    (180./pi)
#define    rads    (pi/180.)



/*!
 * TideTable type definition
 */

IMPLEMENT_DYNAMIC_CLASS( TideTable, wxDialog )

 // RouteProp event table definition

BEGIN_EVENT_TABLE( TideTable, wxDialog )
    //EVT_TEXT( ID_PLANSPEEDCTL, RouteProp::OnPlanSpeedCtlUpdated )
   // EVT_TEXT_ENTER( ID_STARTTIMECTL, RouteProp::OnStartTimeCtlUpdated )
   // EVT_RADIOBOX ( ID_TIMEZONESEL, RouteProp::OnTimeZoneSelected )
  //  EVT_BUTTON( ID_ROUTEPROP_CANCEL, RouteProp::OnRoutepropCancelClick )
  EVT_BUTTON(ID_ROUTEPROP_OK, TideTable::OnRoutepropOkClick)
   // EVT_LIST_ITEM_SELECTED( ID_LISTCTRL, RouteProp::OnRoutepropListClick )
  //  EVT_LIST_ITEM_SELECTED( ID_TRACKLISTCTRL, RouteProp::OnRoutepropListClick )
  //  EVT_BUTTON( ID_ROUTEPROP_SPLIT, RouteProp::OnRoutepropSplitClick )
  //  EVT_BUTTON( ID_ROUTEPROP_EXTEND, RouteProp::OnRoutepropExtendClick )
 //   EVT_BUTTON( ID_ROUTEPROP_PRINT, RouteProp::OnRoutepropPrintClick )
END_EVENT_TABLE()

/*!
 * RouteProp constructors
 */

 bool TideTable::instanceFlag = false;
TideTable* TideTable::single = NULL;
TideTable* TideTable::getInstance(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
{
    if(! instanceFlag)
    {
		single = new TideTable(parent, id, title, pos, size, style);
        instanceFlag = true;
        return single;
    }
    else
    {
        return single;
    }
}

TideTable::TideTable()
{
}

TideTable::TideTable(wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos,
        const wxSize& size, long style )
{
    m_TotalDistCtl = NULL;
    m_wpList = NULL;
    m_nSelected = 0;
    m_pHead = NULL;
    m_pTail = NULL;
    m_pEnroutePoint = NULL;
    m_bStartNow = false;

    m_pRoute = 0;
    m_pEnroutePoint = NULL;
    m_bStartNow = false;
    long wstyle = style;
#ifdef __WXOSX__
    wstyle |= wxSTAY_ON_TOP;
#endif

    SetExtraStyle( GetExtraStyle() | wxWS_EX_BLOCK_EVENTS );
    wxDialog::Create( parent, id, caption, pos, size, style );
        
    m_bcompact = false;
    
    CreateControls();
}



TideTable::~TideTable()
{
    
    delete m_wpList;

    // delete global print route selection dialog
   
    instanceFlag = false;
}


/*!
 * Control creation for TideTable
 */


void TideTable::CreateControls()
{
    
    wxBoxSizer* itemBoxSizer1 = new wxBoxSizer( wxVERTICAL );
    SetSizer( itemBoxSizer1 );
    
        
	itemStaticBoxSizer14Static = new wxStaticBox(this, wxID_ANY, _T("Tides"));
        m_pListSizer = new wxStaticBoxSizer( itemStaticBoxSizer14Static, wxVERTICAL );
        itemBoxSizer1->Add( m_pListSizer, 2, wxEXPAND | wxALL, 1 );
        
        //      Create the list control
        m_wpList = new wxListCtrl( this, ID_LISTCTRL, wxDefaultPosition, wxSize( -1, -1 ),
			wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxLC_EDIT_LABELS );
        
        m_wpList->SetMinSize(wxSize(-1, 100) );
        m_pListSizer->Add( m_wpList, 1, wxEXPAND | wxALL, 6 );
          
        
        
        wxBoxSizer* itemBoxSizerBottom = new wxBoxSizer( wxHORIZONTAL );
        itemBoxSizer1->Add( itemBoxSizerBottom, 0, wxALIGN_LEFT | wxALL | wxEXPAND, 5 );      
             
      wxBoxSizer* itemBoxSizer16 = new wxBoxSizer( wxHORIZONTAL );
      itemBoxSizerBottom->Add( itemBoxSizer16, 0, wxALIGN_RIGHT | wxALL, 3 );
      
      m_OKButton = new wxButton( this, ID_ROUTEPROP_OK, _("OK"), wxDefaultPosition,
      wxDefaultSize, 0 );
      itemBoxSizer16->Add( m_OKButton, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL, 1);
      m_OKButton->SetDefault();
      
      //      To correct a bug in MSW commctl32, we need to catch column width drag events, and do a Refresh()
      //      Otherwise, the column heading disappear.....
      //      Does no harm for GTK builds, so no need for conditional
     
      
      int char_size = GetCharWidth();
      
	  m_wpList->InsertColumn(0, _("Date/Time"), wxLIST_FORMAT_LEFT, char_size * 14);
	  m_wpList->InsertColumn(1, _("HW/LW"), wxLIST_FORMAT_LEFT, char_size * 14);
	  m_wpList->InsertColumn(2, _("Height"), wxLIST_FORMAT_RIGHT, char_size * 9);

      //Set the maximum size of the entire  dialog
      int width, height;
      ::wxDisplaySize( &width, &height );
      SetSizeHints( -1, -1, -1);
      }
      

/*
 * Should we show tooltips?
 */

	  bool TideTable::ShowToolTips()
{
    return TRUE;
}

	  void TideTable::SetDialogTitle(const wxString & title)
{
    SetTitle(title);
}

	  void TideTable::OnRoutepropOkClick(wxCommandEvent& event)
{	
	Hide();
	event.Skip();
}
