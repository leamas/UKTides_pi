///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "UKTidesgui.h"

///////////////////////////////////////////////////////////////////////////

DlgDef::DlgDef( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetForegroundColour( wxColour( 0, 0, 0 ) );
	this->SetBackgroundColour( wxColour( 255, 255, 255 ) );

	m_menubar1 = new wxMenuBar( 0 );
	m_menu1 = new wxMenu();
	m_menuItem1 = new wxMenuItem( m_menu1, wxID_ANY, wxString( _("Download") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu1->Append( m_menuItem1 );

	m_menuItem3 = new wxMenuItem( m_menu1, wxID_ANY, wxString( _("Stations Saved") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu1->Append( m_menuItem3 );

	m_menuItem4 = new wxMenuItem( m_menu1, wxID_ANY, wxString( _("Remove Station Icons") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu1->Append( m_menuItem4 );

	m_menubar1->Append( m_menu1, _("Locations") );

	m_menu2 = new wxMenu();
	m_menuItem2 = new wxMenuItem( m_menu2, wxID_ANY, wxString( _("Guide") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu2->Append( m_menuItem2 );

	m_menubar1->Append( m_menu2, _("Help") );

	this->SetMenuBar( m_menubar1 );

	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizerDateTime;
	sbSizerDateTime = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Important") ), wxHORIZONTAL );

	m_stTimeZone = new wxStaticText( sbSizerDateTime->GetStaticBox(), wxID_ANY, _("All times UTC"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_stTimeZone->Wrap( -1 );
	m_stTimeZone->SetFont( wxFont( 12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Arial") ) );

	sbSizerDateTime->Add( m_stTimeZone, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5 );


	bSizerMain->Add( sbSizerDateTime, 0, wxEXPAND, 5 );

	wxStaticBoxSizer* sbSizerFolder;
	sbSizerFolder = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Port Locations") ), wxVERTICAL );

	m_buttonDownload = new wxButton( sbSizerFolder->GetStaticBox(), wxID_ANY, _(" Download  "), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerFolder->Add( m_buttonDownload, 0, wxALL|wxEXPAND, 5 );

	m_stUKDownloadInfo = new wxStaticText( sbSizerFolder->GetStaticBox(), wxID_ANY, _("Status:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_stUKDownloadInfo->Wrap( -1 );
	sbSizerFolder->Add( m_stUKDownloadInfo, 0, wxALL|wxEXPAND, 5 );


	bSizerMain->Add( sbSizerFolder, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText9 = new wxStaticText( this, wxID_ANY, _("Days ahead: "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	bSizer5->Add( m_staticText9, 0, wxALL, 5 );

	wxString m_choice3Choices[] = { _("1"), _("2"), _("3"), _("4"), _("5"), _("6"), _("7") };
	int m_choice3NChoices = sizeof( m_choice3Choices ) / sizeof( wxString );
	m_choice3 = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice3NChoices, m_choice3Choices, 0 );
	m_choice3->SetSelection( 0 );
	bSizer5->Add( m_choice3, 0, wxALL, 5 );


	bSizerMain->Add( bSizer5, 1, wxEXPAND, 5 );


	this->SetSizer( bSizerMain );
	this->Layout();
	bSizerMain->Fit( this );

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( DlgDef::OnClose ) );
	m_menu1->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( DlgDef::OnDownload ), this, m_menuItem1->GetId());
	m_menu1->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( DlgDef::OnGetSavedTides ), this, m_menuItem3->GetId());
	m_menu1->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( DlgDef::DoRemovePortIcons ), this, m_menuItem4->GetId());
	m_menu2->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( DlgDef::OnInformation ), this, m_menuItem2->GetId());
	m_buttonDownload->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDef::OnDownload ), NULL, this );
}

DlgDef::~DlgDef()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( DlgDef::OnClose ) );
	m_buttonDownload->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDef::OnDownload ), NULL, this );

}
