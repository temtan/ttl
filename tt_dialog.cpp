// tt_dialog.cpp

#include "ttl_define.h"
#include "tt_exception.h"

#include "tt_dialog.h"

#pragma comment(lib, "user32.lib")


INT_PTR CALLBACK
TtDialog::WindowProcedureForTTLDialog( HWND handle, UINT msg, WPARAM w_param, LPARAM l_param )
{
  if ( msg == WM_INITDIALOG ) {
    TtDialog& dialog = *(reinterpret_cast<TtDialog*>( l_param ));
    dialog.handle_ = handle;
    dialog.SetFont( TtFont::DEFAULT );
    TtWindow::WINDOW_TABLE.Register( dialog );
    if ( NOT( dialog.CreatedInternal() ) ) {
      ::DestroyWindow( dialog.handle_ );
      return FALSE;
    }
  }
  auto ret = TtWindow::FindAndCallWindowProcedure( handle, msg, w_param, l_param );
  switch ( ret.result_type ) {
  case WMResult::DoneAndReturnValueAtDialog:
    ::SetWindowLongPtr( handle, DWLP_MSGRESULT, ret.result );
    // walk through
  case WMResult::Done:
    return TRUE;

  default:
    return FALSE;

  case WMResult::DoneAndDirectReturnAtDialog:
    return ret.result;
  }
}



DWORD
TtDialog::GetStyle( void )
{
  return WS_DLGFRAME | WS_CAPTION | WS_SYSMENU;
}

DWORD
TtDialog::GetExtendedStyle( void )
{
  return 0;
}

bool
TtDialog::CreatedInternal( void )
{
  // TtFrom::CreatedInternal ‚ÍŒÄ‚Ño‚³‚È‚¢
  this->TtPanel::CreatedInternal();
  FORM_TABLE.insert( {handle_, this} );
  this->RegisterWMClose( [this]( void ) -> WMResult {
    this->EndDialog( 0 );
    return {WMResult::Done};
  } );
  return this->Created();
}


TtDialog::TtDialog( void ) :
TtForm(),
template_object_( ::GlobalAlloc( GMEM_FIXED | GMEM_ZEROINIT, sizeof( DLGTEMPLATE ) + 10 ) ),
dialog_result_( 0 )
{
}

TtDialog::~TtDialog()
{
  ::GlobalFree( template_object_ );
}

void
TtDialog::CreatePrimitive( CreateParameter& parameter )
{
  NOT_USE( parameter );
  throw TtInvalidOperationException( typeid( *this ) );
}

int
TtDialog::ShowDialog( TtWindow& parent )
{
  parent_ = &parent;
  instance_handle_ = parent.GetInstanceHandle();
  id_ = 0;

  DLGTEMPLATE& tmp = *(static_cast<DLGTEMPLATE*>( template_object_ ) );
  tmp.style = this->GetStyle();
  tmp.dwExtendedStyle = this->GetExtendedStyle();
  tmp.cdit = 0;
  tmp.x = 100;
  tmp.y = 100;
  tmp.cx = 100;
  tmp.cy = 100;
  INT_PTR ret = ::DialogBoxIndirectParam(
    instance_handle_, &tmp, parent.GetHandle(),
    TtDialog::WindowProcedureForTTLDialog, reinterpret_cast<LPARAM>( this ) );
  if ( ret == -1 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::DialogBoxIndirectParam ) );
  }
  if ( handle_ != nullptr ) {
    TtWindow::WINDOW_TABLE.Unregister( *this );
    handle_ = nullptr;
  }
  return static_cast<int>( ret );
}


void
TtDialog::EndDialog( int result )
{
  if ( ::EndDialog( handle_, result ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::EndDialog ) );
  };
  dialog_result_ = result;
}


int
TtDialog::GetResult( void )
{
  return dialog_result_;
}

// -- TtDialogModeless ---------------------------------------------------
TtDialogModeless::TtDialogModeless( void ) :
TtDialog()
{
}

TtDialogModeless::~TtDialogModeless()
{
}





int
TtDialogModeless::ShowDialog( TtWindow& parent )
{
  parent_ = &parent;
  return this->ShowDialogPrimitive( parent.GetInstanceHandle(), parent.GetHandle() );
}

int
TtDialogModeless::ShowDialog( void )
{
  auto& desktop = TtExtraordinarilyWindow::Desktop;
  parent_ = &desktop;
  return this->ShowDialogPrimitive( desktop.GetInstanceHandle(), desktop.GetHandle() );
}

int
TtDialogModeless::ShowDialogPrimitive( HINSTANCE instance_handle, HWND parent )
{
  instance_handle_ = instance_handle;
  id_ = 0;

  DLGTEMPLATE& tmp = *(static_cast<DLGTEMPLATE*>( template_object_ ) );
  tmp.style = this->GetStyle();
  tmp.dwExtendedStyle = this->GetExtendedStyle();
  tmp.cdit = 0;
  tmp.x = 100;
  tmp.y = 100;
  tmp.cx = 100;
  tmp.cy = 100;
  handle_ = ::CreateDialogIndirectParam(
    instance_handle_, &tmp, parent,
    TtDialog::WindowProcedureForTTLDialog, reinterpret_cast<LPARAM>( this ) );
  if ( handle_ == nullptr ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::CreateDialogIndirectParam ) );
  }
  return 0;
}


void
TtDialogModeless::EndDialog( int result )
{
  if ( ::DestroyWindow( handle_ ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::DestroyWindow ) );
  }
  TtWindow::WINDOW_TABLE.Unregister( *this );
  handle_ = nullptr;
  dialog_result_ = result;
}
