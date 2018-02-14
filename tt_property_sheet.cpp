// tt_property_sheet.cpp

#include "ttl_define.h"

#include "tt_thread.h"
#include "tt_exception.h"
#include "tt_utility.h"

#include "tt_property_sheet.h"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "comctl32.lib")


// -- TtPropertySheet::Page ----------------------------------------------
BOOL CALLBACK
TtPropertySheet::Page::WindowProcedureForTTLPropertySheetPage( HWND handle, UINT msg, WPARAM w_param, LPARAM l_param )
{
  if ( msg == WM_INITDIALOG ) {
    Page& page = *(reinterpret_cast<Page*>(
      reinterpret_cast<LPPROPSHEETPAGE>( l_param )->lParam ));
    page.handle_ = handle;
    page.SetFont( TtFont::DEFAULT );
    TtWindow::WINDOW_TABLE.Register( page );
    if ( NOT( page.CreatedInternal() ) ) {
      ::DestroyWindow( page.handle_ );
      return FALSE;
    }
  }
  auto ret = TtWindow::FindAndCallWindowProcedure( handle, msg, w_param, l_param );
  return ( ret.result_type == WMResult::Done ) ? TRUE : FALSE;
}


UINT CALLBACK
TtPropertySheet::Page::ProcedureForTTLPropertySheetPage( HWND handle, UINT msg, LPPROPSHEETPAGE ppsp )
{
  NOT_USE( handle );

  Page& page = *(reinterpret_cast<Page*>( ppsp->lParam ));
  switch ( msg ) {
  case PSPCB_CREATE:
    return TtUtility::Safing( page.handlers_.at_page_create, true )() ? 1u : 0u;

  case PSPCB_RELEASE:
    TtUtility::Safing( page.handlers_.at_page_release )();
    if ( page.handle_ != nullptr ) {
      TtWindow::WINDOW_TABLE.Unregister( page );
      page.handle_ = nullptr;
    }
    break;
  }
  return 0;
}

DWORD
TtPropertySheet::Page::GetStyle( void )
{
  return WS_DLGFRAME;
}

DWORD
TtPropertySheet::Page::GetExtendedStyle( void )
{
  return 0;
}


bool
TtPropertySheet::Page::CreatedInternal( void )
{
  this->TtPanel::CreatedInternal();

  auto previous_notify_handler = this->GetSingleHandler( WM_NOTIFY );
  this->RegisterSingleHandler( WM_NOTIFY, [this, previous_notify_handler] ( WPARAM w_param, LPARAM l_param ) -> WMResult {
    NMHDR* nmhdr = reinterpret_cast<NMHDR*>( l_param );
    switch ( nmhdr->code ) {
    case PSN_SETACTIVE:
      TtUtility::Safing( handlers_.at_set_active )();
      return {WMResult::Done};

    case PSN_KILLACTIVE:
      if ( NOT( TtUtility::Safing( handlers_.at_kill_active, true )() ) ) {
        this->SetWindowLong( DWL_MSGRESULT, TRUE );
      }
      return {WMResult::Done};

    case PSN_APPLY:
      if ( NOT( TtUtility::Safing( handlers_.at_apply, true )() ) ) {
        this->SetWindowLong( DWL_MSGRESULT, TRUE );
      }
      return {WMResult::Done};

    case PSN_RESET:
      TtUtility::Safing( handlers_.at_reset )();
      return {WMResult::Done};
    }
    return previous_notify_handler( w_param, l_param );
  }, false );

  return this->Created();
}

bool
TtPropertySheet::Page::Created( void )
{
  return true;
}

TtPropertySheet::Page::Handlers&
TtPropertySheet::Page::GetHandlers( void )
{
  return handlers_;
}


TtPropertySheet::Page::Page( const std::string& title ) :
TtPanel( false ), // notice_to_parent = false
page_(),
title_( title ),
template_object_( GlobalAlloc( GMEM_FIXED | GMEM_ZEROINIT, sizeof( DLGTEMPLATE ) + 10 ) ),
handlers_( {} )
{
  page_.dwSize      = sizeof( PROPSHEETPAGE );
  page_.dwFlags     = PSP_DEFAULT | PSP_DLGINDIRECT | PSP_USECALLBACK | PSP_USETITLE;
  page_.pResource   = static_cast<const DLGTEMPLATE*>( template_object_ );
  page_.pszTitle    = title_.c_str();
  page_.pfnDlgProc  = TtPropertySheet::Page::WindowProcedureForTTLPropertySheetPage;
  page_.lParam      = reinterpret_cast<long>( this );
  page_.pfnCallback = TtPropertySheet::Page::ProcedureForTTLPropertySheetPage;

  DLGTEMPLATE& dialog_template = *(static_cast<DLGTEMPLATE*>( template_object_ ) );
  dialog_template.style = this->GetStyle();
  dialog_template.dwExtendedStyle = this->GetExtendedStyle();
  dialog_template.cdit = 0;
  dialog_template.x = 0;
  dialog_template.y = 0;
  dialog_template.cx = 0;
  dialog_template.cy = 0;
}

void
TtPropertySheet::Page::PresetSizeAtPageCreate( int width, int height )
{
  DLGTEMPLATE& dialog_template = *(static_cast<DLGTEMPLATE*>( template_object_ ) );
  dialog_template.cx = static_cast<short>( width );
  dialog_template.cy = static_cast<short>( height );
}


TtPropertySheet::Page::~Page()
{
  ::GlobalFree( template_object_ );
}


TtPropertySheet&
TtPropertySheet::Page::GetParentSheet( void ) const
{
  return *parent_sheet_;
}


const std::string&
TtPropertySheet::Page::GetTitle( void ) const
{
  return title_;
}

void
TtPropertySheet::Page::CreatePrimitive( CreateParameter& parameter )
{
  NOT_USE( parameter );
  throw TtInvalidOperationException( typeid( *this ) );
}

void
TtPropertySheet::Page::TellChanged( void )
{
  PropSheet_Changed( parent_->GetHandle(), handle_ );
}


// -- TtPropertySheet ----------------------------------------------------
std::map<DWORD, TtPropertySheet*>
TtPropertySheet::TABLE;

int CALLBACK
TtPropertySheet::ProcedureForTTLPropertySheet( HWND handle, UINT msg, LPARAM l_param )
{
  NOT_USE( handle );
  NOT_USE( l_param );

  switch ( msg ) {
  case PSCB_INITIALIZED: {
    if ( TABLE.find( TtThread::GetCurrentThreadID() ) == TABLE.end() ) {
      throw TT_INTERNAL_EXCEPTION;
    }
    TtPropertySheet& sheet = *TABLE[TtThread::GetCurrentThreadID()];
    sheet.handle_ = handle;
    sheet.SetWindowLong( GWL_EXSTYLE, sheet.GetWindowLong( GWL_EXSTYLE ) & ~WS_EX_CONTEXTHELP );
    if ( NOT( sheet.CreatedInternal() ) ) {
      ::DestroyWindow( sheet.handle_ );
      return 0;
    }
    break;
  }

  case PSCB_PRECREATE:
    break;
  }
  return 0;
}



TtPropertySheet::TtPropertySheet( bool has_apply_button ) :
pages_(),
has_apply_button_( has_apply_button )
{
}


void
TtPropertySheet::CreatePrimitive( CreateParameter& parameter )
{
  NOT_USE( parameter );
  throw TtInvalidOperationException( typeid( *this ) );
}


void
TtPropertySheet::SetHasApplyButton( bool has_apply_button )
{
  has_apply_button_ = has_apply_button;
}

bool
TtPropertySheet::GetHasApplyButton( void ) const
{
  return has_apply_button_;
}


void
TtPropertySheet::AddPage( Page& page )
{
  pages_.push_back( &page );
}


bool
TtPropertySheet::ShowDialog( TtForm& parent )
{
  parent_ = &parent;
  instance_handle_ = parent.GetInstanceHandle();
  TtUtility::UniqueArray<PROPSHEETPAGE> page_array( pages_.size() );
  unsigned int page_count = 0;

  for ( unsigned int i = 0; i < pages_.size(); ++i ) {
    if ( pages_[i]->handlers_.at_page_create == nullptr || pages_[i]->handlers_.at_page_create() ) {
      pages_[i]->parent_ = this;
      pages_[i]->parent_sheet_ = this;
      pages_[i]->page_.hInstance = instance_handle_;
      page_array.GetPointer()[i] = pages_[i]->page_;
      ++page_count;
    }
  }

  PROPSHEETHEADER sheet;
  sheet.dwSize      = sizeof( PROPSHEETHEADER );
  sheet.dwFlags     = PSH_DEFAULT | 0x10 | PSH_USECALLBACK | PSH_PROPSHEETPAGE |
    (has_apply_button_ ? 0 : PSH_NOAPPLYNOW);
  sheet.hwndParent  = parent.GetHandle();
  sheet.hInstance   = instance_handle_;
  sheet.pszCaption  = "";
  sheet.nPages      = page_count;
  sheet.nStartPage  = 0;
  sheet.ppsp        = page_array.GetPointer();
  sheet.pfnCallback = TtPropertySheet::ProcedureForTTLPropertySheet;

  TABLE[TtThread::GetCurrentThreadID()] = this;
  int ret = ::PropertySheet( &sheet );
  TABLE.erase( TtThread::GetCurrentThreadID() );
  if ( ret == -1 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::PropertySheet ) );
  }
  return ret == ID_PSREBOOTSYSTEM || ret == ID_PSRESTARTWINDOWS;
}
