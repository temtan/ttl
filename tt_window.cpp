// tt_window.cpp

#include "tt_windows_h_include.h"

#include "ttl_define.h"
#include "tt_exception.h"
#include "tt_font.h"
#include "tt_string.h"

#include "tt_window.h"

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "shell32.lib")


// -----------------------------------------------------------------------
// Window Message Handler ŠÖ˜A
TtWindow::WindowTable::WindowTable( void )
{
}

void
TtWindow::WindowTable::Register( TtWindow& window )
{
  this->insert( {window.handle_, &window} );
}

void
TtWindow::WindowTable::Unregister( TtWindow& window )
{
  this->erase( window.handle_ );
}

TtWindow*
TtWindow::WindowTable::Find( HWND handle )
{
  auto it = TtWindow::WINDOW_TABLE.find( handle );
  return ( it != TtWindow::WINDOW_TABLE.end() ) ? it->second : nullptr;
}

TtWindow::WindowTable
TtWindow::WINDOW_TABLE;



LRESULT CALLBACK
TtWindow::WindowProcedureForTTL( HWND handle, UINT msg, WPARAM w_param, LPARAM l_param )
{
  WMResult ret = TtWindow::FindAndCallWindowProcedure( handle, msg, w_param, l_param );
  return ( ret.result_type == WMResult::Done ) ? ret.result : ::DefWindowProc( handle, msg, w_param, l_param );
}


TtWindow::WMResult
TtWindow::FindAndCallWindowProcedure( HWND handle, UINT msg, WPARAM w_param, LPARAM l_param )
{
  TtWindow* window = TtWindow::WINDOW_TABLE.Find( handle );
  for (;;) {
    if ( window == nullptr ) {
      return WMResult( {WMResult::Incomplete} );
    }
    WMResult ret = window->WindowProcedure( msg, w_param, l_param );
    if ( ret.result_type != WMResult::DelegateToParent ) {
      return ret;
    }
    window = window->GetParent();
  }
  /*
  TtWindow* window = TtWindow::WINDOW_TABLE.Find( handle );
  return ( window != nullptr ) ?
    window->WindowProcedure( msg, w_param, l_param ) : WMResult( {false} );
   */
}


TtWindow::WMResult
TtWindow::WindowProcedure( UINT msg, WPARAM w_param, LPARAM l_param )
{
  auto it = handler_table_.find( msg );
  if ( it != handler_table_.end() && it->second ) {
    auto result = it->second( w_param, l_param );
    if ( result.result_type != WMResult::Incomplete ) {
      return result;
    }
  }
  if ( window_procedure_super_ != nullptr ) {
    return {WMResult::Done, ::CallWindowProc( window_procedure_super_, handle_, msg, w_param, l_param ) };
  }
  return {WMResult::Incomplete};
}


void
TtWindow::RegisterSingleHandler( UINT msg, SingleHandler handler, bool do_override )
{
  auto it = handler_table_.find( msg );
  if ( it != handler_table_.end() ) {
    if ( do_override ) {
      it->second = [handler, super_handler = it->second] ( WPARAM w_param, LPARAM l_param ) {
        if ( handler ) {
          WMResult ret = handler( w_param, l_param );
          if ( ret.result_type != WMResult::Incomplete ) {
            return ret;
          }
        }
        return super_handler ? super_handler( w_param, l_param ) : WMResult( {WMResult::Incomplete} );
      };
    }
    else {
      it->second = handler;
    }
  }
  else {
    handler_table_.insert( {msg, handler} );
  }
}

void
TtWindow::UnregisterSingleHandler( UINT msg )
{
  auto it = handler_table_.find( msg );
  if ( it != handler_table_.end() ) {
    handler_table_.erase( msg );
  }
}

TtWindow::SingleHandler
TtWindow::GetSingleHandler( UINT msg )
{
  auto it = handler_table_.find( msg );
  if ( it == handler_table_.end() ) {
    return nullptr;
  }
  return it->second;
}


void
TtWindow::OverrideWindowProcedureByTTL( void )
{
  window_procedure_super_ = reinterpret_cast<WNDPROC>( this->GetWindowLongPtr( GWLP_WNDPROC ) );
  this->SetWindowLongPtr( GCLP_WNDPROC, reinterpret_cast<ULONG_PTR>( TtWindow::WindowProcedureForTTL ) );
  if ( WINDOW_TABLE.Find( handle_ ) == nullptr ) {
    WINDOW_TABLE.Register( *this );
  }
}


POINT
TtWindow::GetCursorPosition( void )
{
  POINT point;
  if ( ::GetCursorPos( &point ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::GetCursorPos ) );
  }
  return point;
}


void
TtWindow::SetCursorPosition( POINT point )
{
  if ( ::SetCursorPos( point.x, point.y ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::SetCursorPos ) );
  }
}


int
TtWindow::GetXOfLParam( LPARAM l_param )
{
  return static_cast<int>( static_cast<short>( LOWORD( l_param ) ) );
}

int
TtWindow::GetYOfLParam( LPARAM l_param )
{
  return static_cast<int>( static_cast<short>( HIWORD( l_param ) ) );
}

// -----------------------------------------------------------------------
void
TtWindow::InitializeCommonControls( void )
{
  ::InitCommonControls();
  ::CoInitialize( nullptr );
}

void
TtWindow::RegisterClass( WNDCLASS* window_class )
{
  if ( ::RegisterClass( window_class ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::RegisterClass ) );
  }
}


bool
TtWindow::CreatedInternal( void )
{
  return true;
}


TtWindow::TtWindow( void ) :
parent_( nullptr ),
handle_( nullptr ),
id_( 0 ),
instance_handle_( nullptr ),
window_procedure_super_( nullptr ),
handler_table_()
{
}

TtWindow::~TtWindow()
{
  if ( handle_ != nullptr && WINDOW_TABLE.Find( handle_ ) != nullptr ) {
    WINDOW_TABLE.Unregister( *this );
  }
}


// -- CreateParameter ----------------------------------------------------
TtWindow::CreateParameter::CreateParameter( void ) :
CreateParameter( nullptr )
{
}

TtWindow::CreateParameter::CreateParameter( TtWindow* parent ) :
CreateParameter( parent, 0 )
{
}

TtWindow::CreateParameter::CreateParameter( TtWindow* parent, int id ) :
parent_( parent ),
id_( id ),
instance_handle_( parent ? parent->GetInstanceHandle() : ::GetModuleHandle( nullptr ) ),
lp_param_( nullptr ),
is_child_( true ),
group_start_( false )
{
}

TtWindow::CreateParameter::CreateParameter( const CreateParameter& other ) :
parent_( other.parent_ ),
id_( other.id_ ),
instance_handle_( other.instance_handle_ ),
lp_param_( other.lp_param_ ),
is_child_( other.is_child_ ),
group_start_( other.group_start_ )
{
}

TtWindow::CreateParameter
TtWindow::CreateParameter::AlterParent( TtWindow* parent ) const
{
  CreateParameter tmp( *this );
  tmp.parent_ = parent;
  return tmp;
}

TtWindow::CreateParameter
TtWindow::CreateParameter::AlterId( int id ) const
{
  CreateParameter tmp( *this );
  tmp.id_ = id;
  return tmp;
}

TtWindow::CreateParameter
TtWindow::CreateParameter::AlterInstanceHandle( HINSTANCE instance_handle ) const
{
  CreateParameter tmp( *this );
  tmp.instance_handle_ = instance_handle;
  return tmp;
}

TtWindow::CreateParameter
TtWindow::CreateParameter::AlterLPParam( LPVOID lp_param ) const
{
  CreateParameter tmp( *this );
  tmp.lp_param_ = lp_param;
  return tmp;
}

TtWindow::CreateParameter
TtWindow::CreateParameter::AlterIsChild( bool is_child ) const
{
  CreateParameter tmp( *this );
  tmp.is_child_ = is_child;
  return tmp;
}

TtWindow::CreateParameter
TtWindow::CreateParameter::AlterGroupStart( bool group_start ) const
{
  CreateParameter tmp( *this );
  tmp.group_start_ = group_start;
  return tmp;
}



void
TtWindow::Create( void )
{
  this->Create( CreateParameter() );
}

void
TtWindow::Create( CreateParameter parameter )
{
  this->CreatePrimitive( parameter );
}


void
TtWindow::CreatePrimitive( CreateParameter& parameter )
{
  parent_ = parameter.parent_;
  instance_handle_ = parameter.instance_handle_;
  id_ = parameter.id_;

  DWORD style = ( this->GetStyle() |
                  ((parameter.parent_ != nullptr && parameter.is_child_) ? WS_CHILD : 0) |
                  (parameter.group_start_ ? WS_GROUP : 0) );

  handle_ = CreateWindowEx(
    this->GetExtendedStyle(),
    this->GetClassName(),
    nullptr,
    style,
    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
    parameter.parent_ ? parameter.parent_->GetHandle() : nullptr,
    reinterpret_cast<HMENU>( static_cast<int64_t>( parameter.id_ ) ),
    instance_handle_,
    parameter.lp_param_ );
  if ( handle_ == nullptr ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::CreateWindowEx ) );
  }

  this->SetFont( TtFont::DEFAULT );
  WINDOW_TABLE.Register( *this );

  if ( NOT( this->CreatedInternal() ) ) {
    ::DestroyWindow( handle_ );
    handle_ = nullptr;
  }
}


int
TtWindow::Close( void )
{
  return this->SendMessage( WM_CLOSE );
}


int
TtWindow::SendMessage( UINT msg, WPARAM w_param, LPARAM l_param )
{
  return static_cast<int>( ::SendMessage( handle_, msg, w_param, l_param ) );
}

void
TtWindow::PostMessage( UINT msg, WPARAM w_param, LPARAM l_param )
{
  if ( ::PostMessage( handle_, msg, w_param, l_param ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::PostMessage ) );
  }
}


LONG_PTR
TtWindow::GetWindowLongPtr( int index )
{
  LONG_PTR tmp;
  TtUtility::CallWindowsSystemFunctionWithErrorHandling(
    [&] ( void ) {
      tmp = ::GetWindowLongPtr( handle_, index );
    },
    [] ( void ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::GetWindowLongPtr ) );
    } );
  return tmp;
}

void
TtWindow::SetWindowLongPtr( int index, LONG_PTR value )
{
  TtUtility::CallWindowsSystemFunctionWithErrorHandling(
    [&] ( void ) {
      ::SetWindowLongPtr( handle_, index, value );
    },
    [] ( void ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::SetWindowLongPtr ) );
    } );
}


ULONG_PTR
TtWindow::GetClassLongPtr( int index )
{
  ULONG_PTR tmp;
  TtUtility::CallWindowsSystemFunctionWithErrorHandling(
    [&] ( void ) {
      tmp = ::GetClassLongPtr( handle_, index );
    },
    [] ( void ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::GetClassLongPtr ) );
    } );
  return tmp;
}

void
TtWindow::SetClassLongPtr( int index, LONG_PTR value )
{
  TtUtility::CallWindowsSystemFunctionWithErrorHandling(
    [&] ( void ) {
      ::SetClassLongPtr( handle_, index, value );
    },
    [] ( void ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::SetClassLongPtr ) );
    } );
}


void
TtWindow::ChangeWindowStyle( DWORD style )
{
  this->SetWindowLongPtr( GWL_STYLE, style );
  this->Show();
  if ( ::SetWindowPos( handle_, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOOWNERZORDER ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::SetWindowPos ) );
  }
}


TtWindow*
TtWindow::GetParent( void )
{
  return parent_;
}

HWND
TtWindow::GetHandle( void )
{
  return handle_;
}

int
TtWindow::GetID( void )
{
  return id_;
}

HINSTANCE
TtWindow::GetInstanceHandle( void )
{
  return instance_handle_;
}


RECT
TtWindow::GetRectangle( void )
{
  RECT rect;
  if ( ::GetWindowRect( handle_, &rect ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::GetWindowRect ) );
  }
  return rect;
}

POINT
TtWindow::GetPoint( void )
{
  RECT rect = this->GetRectangle();
  return {rect.left, rect.top};
}

int
TtWindow::GetWidth( void )
{
  RECT rect = this->GetRectangle();
  return rect.right - rect.left;
}

int
TtWindow::GetHeight( void )
{
  RECT rect = this->GetRectangle();
  return rect.bottom - rect.top;
}

POINT
TtWindow::GetRelativePointToClient( void )
{
  return this->GetParent()->ConvertToClientPoint( this->GetPoint() );
}


int
TtWindow::GetClientWidth( void )
{
  return this->GetClientEndPoint().x;
}

int
TtWindow::GetClientHeight( void )
{
  return this->GetClientEndPoint().y;
}

POINT
TtWindow::GetClientEndPoint( void )
{
  RECT rect;
  if ( ::GetClientRect( handle_, &rect ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::GetClientRect ) );
  }
  return {rect.right, rect.bottom};
}

void
TtWindow::SetClientSize( int width, int height, bool has_menu )
{
  RECT rect = {50, 50, width + 50, height + 50};
  if ( ::AdjustWindowRectEx( &rect, this->GetStyle(), has_menu, this->GetExtendedStyle() ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::AdjustWindowRect ) );
  }
  POINT p = this->GetPoint();
  this->Move( p.x, p.y, rect.right - rect.left, rect.bottom - rect.top, TRUE );
}

POINT
TtWindow::GetClientOffset( void )
{
  POINT p = this->ConvertToClientPoint( this->GetPoint() );
  return {-1 * p.x, -1 * p.y};
}


WINDOWPLACEMENT
TtWindow::GetWindowPlacement( void )
{
  WINDOWPLACEMENT placement;
  if ( ::GetWindowPlacement( handle_, &placement ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::GetWindowPlacement ) );
  }
  return placement;
}

void
TtWindow::SetWindowPlacement( WINDOWPLACEMENT& placement )
{
  placement.length = sizeof( WINDOWPLACEMENT );
  if ( ::SetWindowPlacement( handle_, &placement ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::SetWindowPlacement ) );
  }
}


POINT
TtWindow::ConvertToClientPoint( POINT point )
{
  POINT new_point = point;
  if ( ::ScreenToClient( handle_, &new_point ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::ScreenToClient ) );
  }
  return new_point;
}

POINT
TtWindow::ConvertToScreenPoint( POINT point )
{
  POINT new_point = point;
  if ( ::ClientToScreen( handle_, &new_point ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::ClientToScreen ) );
  }
  return new_point;
}


TtReleasableDeviceContext
TtWindow::GetDeviceContext( void )
{
  HDC tmp = ::GetDC( handle_ );
  if ( tmp == NULL ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::GetDC ) );
  }
  return TtReleasableDeviceContext( tmp, handle_, true );
}

// -- ShowState ----------------------------------------------------------
template <>
TtEnumTable<TtWindow::ShowState>::TtEnumTable( void ) {
#define REGISTER( NAME ) this->Register( TtWindow::ShowState::NAME, #NAME )
  REGISTER( FORCE_MINIMIZE );
  REGISTER( HIDE );
  REGISTER( RESTORE );
  REGISTER( MINIMIZE );
  REGISTER( MAXIMIZE );
  REGISTER( SHOW );
  REGISTER( SHOW_NO_ACTIVE );
  REGISTER( SHOW_MINIMIZED );
  REGISTER( SHOW_MINIMIZED_NO_ACTIVE );
  REGISTER( SHOW_RESTORED );
  REGISTER( SHOW_RESTORED_NO_ACTIVATE );
  REGISTER( SHOW_DEFAULT );
#undef REGISTER
}

TtEnumTable<TtWindow::ShowState>
TtEnumBase<TtWindow::ShowState>::TABLE;



void
TtWindow::Show( void )
{
  this->SetShowState( ShowState::SHOW );
}

void
TtWindow::Hide( void )
{
  this->SetShowState( ShowState::HIDE );
}

void
TtWindow::Minimize( void )
{
  this->SetShowState( ShowState::MINIMIZE );
}

void
TtWindow::Maximize( void )
{
  this->SetShowState( ShowState::MAXIMIZE );
}

void
TtWindow::Restore()
{
  this->SetShowState( ShowState::RESTORE );
}

void
TtWindow::SetShowState( TtEnum<ShowState> state )
{
  ::ShowWindow( handle_, state.ToInteger() );
}

bool
TtWindow::IsMinimized( void )
{
  return ::IsIconic( handle_ ) ? true : false;
}

bool
TtWindow::IsMaximized( void )
{
  return ::IsZoomed( handle_ ) ? true : false;
}

bool
TtWindow::GetVisible( void )
{
  return ::IsWindowVisible( handle_ ) ? true : false;
}


bool
TtWindow::GetEnabled( void )
{
  return ::IsWindowEnabled( handle_ ) ? true : false;
}

void
TtWindow::SetEnabled( bool flag )
{
  ::EnableWindow( handle_, flag );
}


void
TtWindow::SetActive( void )
{
  ::SetActiveWindow( handle_ );
}


void
TtWindow::SetFocus( void )
{
  ::SetFocus( handle_ );
}


void
TtWindow::SetPosition( int x, int y )
{
  RECT rect = this->GetRectangle();
  this->Move( x, y, rect.right - rect.left, rect.bottom - rect.top, TRUE );
}

void
TtWindow::SetSize( int width, int height )
{
  RECT rect = this->GetRectangle();
  this->Move( rect.left, rect.top, width, height, TRUE );
}

void
TtWindow::SetPositionSize( int x, int y, int width, int height )
{
  this->Move( x, y, width, height, TRUE );
}

void
TtWindow::SetRectangle( RECT rect )
{
  this->Move( rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE );
}

void
TtWindow::Move( int x, int y, int width, int height, bool do_repaint )
{
  if ( ::MoveWindow( handle_, x, y, width, height, do_repaint ? TRUE : FALSE ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::MoveWindow ) );
  }
}

void
TtWindow::SetCenterRelativeTo( TtWindow& reference )
{
  RECT ref_rect = reference.GetRectangle();
  RECT this_rect = this->GetRectangle();
  TtWindow& desktop = TtExtraordinarilyWindow::Desktop;

  int x = (ref_rect.right + ref_rect.left) / 2 - (this_rect.right - this_rect.left) / 2;
  if ( x < 0 ) {
    x = 0;
  }
  else if ( x + this_rect.right - this_rect.left > desktop.GetRectangle().right ) {
    x = desktop.GetRectangle().right - this->GetWidth();
  }

  int y = (ref_rect.bottom + ref_rect.top) / 2 - (this_rect.bottom - this_rect.top) / 2;
  if ( y < 0 ) {
    y = 0;
  }
  else if ( y + this_rect.bottom - this_rect.top > desktop.GetRectangle().bottom ) {
    y = desktop.GetRectangle().bottom - this->GetHeight();
  }

  this->SetPosition( x, y );
}


void
TtWindow::SetCenterRelativeToParent( void )
{
  this->SetCenterRelativeTo( *parent_ );
}


void
TtWindow::SetBottom( void )
{
  this->SetUnder( HWND_BOTTOM );
}

void
TtWindow::SetTop( void )
{
  this->SetUnder( HWND_TOP );
}

void
TtWindow::SetUnder( TtWindow& window )
{
  this->SetUnder( window.handle_ );
}

void
TtWindow::SetUnder( HWND window )
{
  if ( ::SetWindowPos( handle_, window, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::SetWindowPos ) );
  }
}

void
TtWindow::SetTopMost( bool flag )
{
  if ( ::SetWindowPos( handle_, flag ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::SetWindowPos ) );
  }
}


void
TtWindow::SetFont( TtFont& font )
{
  this->SendMessage( WM_SETFONT, reinterpret_cast<WPARAM>( font.GetHandle() ), true );
}

void
TtWindow::SetIconAsLarge( TtIcon& icon )
{
  this->SendMessage( WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>( icon.GetHandle() ) );
}

void
TtWindow::SetIconAsSmall( TtIcon& icon )
{
  this->SendMessage( WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>( icon.GetHandle() ) );
}



int
TtWindow::SetForground( void )
{
  return ::SetForegroundWindow( handle_ );
}


void
TtWindow::SetCapture( void )
{
  TtUtility::CallWindowsSystemFunctionWithErrorHandling(
    [&] ( void ) {
      ::SetCapture( handle_ );
    },
    [] ( void ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::SetCapture ) );
    } );
}

bool
TtWindow::IsCapturing( void )
{
  HWND ret;
  TtUtility::CallWindowsSystemFunctionWithErrorHandling(
    [&] ( void ) {
      ret = ::GetCapture();
    },
    [] ( void ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::GetCapture ) );
    } );
  return ret == handle_;
}

void
TtWindow::ReleaseCapture( void )
{
  TtUtility::CallWindowsSystemFunctionWithErrorHandling(
    [] ( void ) {
      ::ReleaseCapture();
    },
    [] ( void ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::ReleaseCapture ) );
    } );
}


void
TtWindow::Update( void )
{
  if ( ::UpdateWindow( handle_ ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::UpdateWindow ) );
  }
}


void
TtWindow::Invalidate( void )
{
  ::InvalidateRect( handle_, NULL , TRUE );
}


void
TtWindow::StopRedraw( void )
{
  this->SendMessage( WM_SETREDRAW, FALSE );
}

void
TtWindow::StopRedraw( std::function<void ( void )> function )
{
  this->StopRedraw();
  TtUtility::DestructorCall obj( [this] ( void ) { this->RestartRedraw(); } );
  function();
}

void
TtWindow::RestartRedraw( void )
{
  this->SendMessage( WM_SETREDRAW, TRUE );
  this->Invalidate();
}


int
TtWindow::GetTextLength( void )
{
  return ::GetWindowTextLength( handle_ );
}

std::string
TtWindow::GetText( void )
{
  int length = this->GetTextLength();
  if ( length == 0 ) {
    return std::string();
  }
  TtString::SharedString buf( length + 1 );
  if ( ::GetWindowText( handle_, buf.GetPointer(), static_cast<int>( buf.GetCapacity() ) ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::GetWindowText ) );
  }
  return buf.ToString();
}

void
TtWindow::SetText( const std::string& str )
{
  if ( ::SetWindowText( handle_, str.c_str() ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::SetWindowText ) );
  }
}


void
TtWindow::DragAcceptFiles( bool flag )
{
  ::DragAcceptFiles( handle_, flag );
}


// -- TtExtraordinarilyWindow --------------------------------------------
TtExtraordinarilyWindow
TtExtraordinarilyWindow::Desktop( ::GetDesktopWindow() );

TtExtraordinarilyWindow::TtExtraordinarilyWindow( HWND handle ) :
TtWindow()
{
  handle_ = handle;
}

TtExtraordinarilyWindow::TtExtraordinarilyWindow( HWND handle, TtWindow& parent ) :
TtExtraordinarilyWindow( handle )
{
  parent_ = &parent;
}

DWORD
TtExtraordinarilyWindow::GetStyle( void )
{
  return 0;
}

DWORD
TtExtraordinarilyWindow::GetExtendedStyle( void )
{
  return 0;
}

PCTSTR
TtExtraordinarilyWindow::GetClassName( void )
{
  return 0;
}


// -- TtWindowClassName --------------------------------------------------
const char* TtWindowClassName::Static::ClassName     = "STATIC";
const char* TtWindowClassName::Button::ClassName     = "BUTTON";
const char* TtWindowClassName::Edit::ClassName       = "EDIT";
const char* TtWindowClassName::ListBox::ClassName    = "LISTBOX";
const char* TtWindowClassName::ComboBox::ClassName   = "COMBOBOX";
const char* TtWindowClassName::ScrollBar::ClassName  = "SCROLLBAR";
const char* TtWindowClassName::TrackBar::ClassName   = TRACKBAR_CLASS;
const char* TtWindowClassName::TabControl::ClassName = WC_TABCONTROL;
const char* TtWindowClassName::Progress::ClassName   = PROGRESS_CLASS;
const char* TtWindowClassName::ListView::ClassName   = WC_LISTVIEW;
const char* TtWindowClassName::TreeView::ClassName   = WC_TREEVIEW;
const char* TtWindowClassName::ToolBar::ClassName    = TOOLBARCLASSNAME;
const char* TtWindowClassName::StatusBar::ClassName  = STATUSCLASSNAME;

#ifdef TT_MAKE_TEMPLATE_INSTANCE_
template class BasedOnTtWindow<0, 0, TtWindowClassName::Static>;
template class TtWindowWithStyle<BasedOnTtWindow<0, 0, TtWindowClassName::Static>, 0>;
#endif
