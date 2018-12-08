// tt_panel.cpp

#include "ttl_define.h"
#include "tt_panel.h"


// -- TtPanel ------------------------------------------------------------
const char* const
TtPanel::CLASS_NAME = "TTPANEL";

void
TtPanel::Initialize( void )
{
  TtPanel::Initialize( ::GetModuleHandle( nullptr ) );
}

void
TtPanel::Initialize( HINSTANCE h_instance )
{
  static bool initialized = false;
  if ( NOT( initialized ) ) {
    initialized = true;
    WNDCLASS window_class;
    window_class.style         = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc   = TtWindow::WindowProcedureForTTL;
    window_class.cbClsExtra    = 0;
    window_class.cbWndExtra    = 0;
    window_class.hInstance     = h_instance;
    window_class.hIcon         = nullptr;
    window_class.hCursor       = ::LoadCursor( nullptr, IDC_ARROW );
    window_class.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    window_class.lpszMenuName  = nullptr;
    window_class.lpszClassName = TtPanel::CLASS_NAME;
    TtWindow::RegisterClass( &window_class );
  }
}


TtPanel::TtPanel( bool notice_to_parent ) :
notice_to_parent_( notice_to_parent ),
command_table_(),
notify_table_()
{
  TtPanel::Initialize();
}


DWORD
TtPanel::GetStyle( void )
{
  return 0;
}

DWORD
TtPanel::GetExtendedStyle( void )
{
  return 0;
}

PCTSTR
TtPanel::GetClassName( void )
{
  return TtPanel::CLASS_NAME;
}

bool
TtPanel::CreatedInternal( void )
{
  this->RegisterSingleHandler( WM_COMMAND, [this] ( WPARAM w_param, LPARAM l_param ) -> WMResult {
    int id   = LOWORD( w_param );
    int code = HIWORD( w_param );
    HWND child = reinterpret_cast<HWND>( l_param );

    auto it = command_table_.find( id );
    if ( it != command_table_.end() ) {
      for ( auto handler : it->second ) {
        auto result = handler( code, child );
        if ( result.result_type == WMResult::Done ) {
          return result;
        }
      }
    }
    return {notice_to_parent_ ? WMResult::DelegateToParent : WMResult::Done};
  }, false );

  this->RegisterSingleHandler( WM_NOTIFY, [this] ( WPARAM w_param, LPARAM l_param ) -> WMResult {
    int id = static_cast<int>( w_param );
    NMHDR* nmhdr = reinterpret_cast<NMHDR*>( l_param );

    auto it = notify_table_.find( id );
    if ( it != notify_table_.end() ) {
      for ( auto handler : it->second ) {
        auto result = handler( nmhdr );
        if ( result.result_type == WMResult::Done ) {
          return result;
        }
      }
    }
    return {notice_to_parent_ ? WMResult::DelegateToParent : WMResult::Done};
  }, false );

  this->RegisterSingleHandler( WM_MENUCOMMAND, [this] ( WPARAM w_param, LPARAM l_param ) -> WMResult {
    int index = static_cast<int>( w_param );
    HMENU handle = reinterpret_cast<HMENU>( l_param );

    auto it = menu_command_table_.find( handle );
    if ( it != menu_command_table_.end() ) {
      for ( auto handler : it->second ) {
        auto result = handler( TtMenuItem( handle, index ) );
        if ( result.result_type == WMResult::Done ) {
          return result;
        }
      }
      return {WMResult::Done};
    }
    return this->GetSingleHandler( WM_COMMAND )( MAKEWPARAM( TtMenu( handle, false ).GetItemAt( index ).GetCommandID(), 0 ), 0 );
    // return {true, this->SendMessage( WM_COMMAND, MAKEWPARAM( TtMenu( handle ).GetItemAt( index ).GetCommandID(), 0 ), 0 ) };
  }, false );

  return true;
}


void
TtPanel::RegisterWMSize( WMSizeHandler handler, bool do_override )
{
  this->RegisterSingleHandler( WM_SIZE, [handler]( WPARAM w_param, LPARAM l_param ) {
    return handler( static_cast<int>( w_param ), LOWORD( l_param ), HIWORD( l_param ) );
  }, do_override );
}

void
TtPanel::RegisterWMSizing( WMSizingHandler handler, bool do_override )
{
  this->RegisterSingleHandler( WM_SIZING, [handler]( WPARAM w_param, LPARAM l_param ) {
    return handler( static_cast<int>( w_param ), *reinterpret_cast<RECT*>( l_param ) );
  }, do_override );
}

TtPanel::WMSizingHandler
TtPanel::MakeCanChangeOnlyHorizontalHandler( void )
{
  return [this] ( int flag, RECT& rectangle ) -> WMResult {
    switch ( flag ) {
    case WMSZ_LEFT:
    case WMSZ_RIGHT:
      break;

    case WMSZ_TOP:
    case WMSZ_TOPLEFT:
    case WMSZ_TOPRIGHT:
      rectangle.top = this->GetRectangle().top;
      break;

    case WMSZ_BOTTOM:
    case WMSZ_BOTTOMLEFT:
    case WMSZ_BOTTOMRIGHT:
      rectangle.bottom = this->GetRectangle().bottom;
      break;
    }
    return {WMResult::Done};
  };
}

TtPanel::WMSizingHandler
TtPanel::MakeCanChangeOnlyVerticalHandler( void )
{
  return [this] ( int flag, RECT& rectangle ) -> WMResult {
    switch ( flag ) {
    case WMSZ_TOP:
    case WMSZ_BOTTOM:
      break;

    case WMSZ_LEFT:
    case WMSZ_TOPLEFT:
    case WMSZ_BOTTOMLEFT:
      rectangle.left = this->GetRectangle().left;
      break;

    case WMSZ_RIGHT:
    case WMSZ_TOPRIGHT:
    case WMSZ_BOTTOMRIGHT:
      rectangle.right = this->GetRectangle().right;
      break;
    }
    return {WMResult::Done};
  };
}


void
TtPanel::RegisterWMPaint( WMPaintHandler handler, bool do_override )
{
  this->RegisterSingleHandler( WM_PAINT, [handler] ( WPARAM, LPARAM ) {
    return handler();
  }, do_override );
}

void
TtPanel::AddCommandHandler( int id, CommandHandler handler )
{
  command_table_[id].push_back( handler );
}

void
TtPanel::ClearCommandHandler( int id )
{
  command_table_[id].clear();
}


void
TtPanel::AddNotifyHandler( int id, NotifyHandler handler )
{
  notify_table_[id].push_back( handler );
}

void
TtPanel::ClearNotifyHandler( int id )
{
  notify_table_[id].clear();
}


void
TtPanel::AddMenuCommandHandler( TtSubMenuCommand& menu, MenuCommandHandler handler )
{
  menu_command_table_[menu.GetHandle()].push_back( handler );
}

void
TtPanel::RemoveMenuCommandHandler( TtSubMenuCommand& menu )
{
  menu_command_table_.erase( menu.GetHandle() );
}

void
TtPanel::ClearMenuCommandHandler( TtSubMenuCommand& menu )
{
  menu_command_table_[menu.GetHandle()].clear();
}


// -- TtSplitterPanel ----------------------------------------------------
TtSplitterPanel::TtSplitterPanel( int width, int north_limit, int south_limit ) :
splitter_width_( width ),
north_limit_( north_limit ),
south_limit_( south_limit ),
north_window_( nullptr ),
south_window_( nullptr ),

splitter_position_( 0 ),
moving_position_( 0 )
{
}

bool
TtSplitterPanel::CreatedInternal( void )
{
  this->TtPanel::CreatedInternal();
  this->RegisterHandlers();
  return true;
}

int
TtSplitterPanel::GetSplitterWidth( void )
{
  return splitter_width_;
}

int
TtSplitterPanel::GetNorthLimit( void )
{
  return north_limit_;
}

int
TtSplitterPanel::GetSouthLimit( void )
{
  return south_limit_;
}


void
TtSplitterPanel::SetNorthWindow( TtWindow* window )
{
  north_window_ = window;
}

TtWindow*
TtSplitterPanel::GetNorthWindow( void )
{
  return north_window_;
}

void
TtSplitterPanel::SetSouthWindow( TtWindow* window )
{
  south_window_ = window;
}

TtWindow*
TtSplitterPanel::GetSouthWindow( void )
{
  return south_window_;
}


void
TtSplitterPanel::SetSplitterPosition( int position )
{
  splitter_position_ = position;
}

int
TtSplitterPanel::GetSplitterPosition( void )
{
  return splitter_position_;
}


void
TtSplitterPanel::RegisterHandlers( void )
{
  this->RegisterSingleHandler( WM_SIZE, [this] ( WPARAM, LPARAM ) -> WMResult {
    int north_south = this->GetNorthSouth();
    int west_east = this->GetWestEast();
    if ( north_window_ ) {
      north_window_->SetRectangle( this->MakeRectangleFromLL(
        0, splitter_position_, 0, west_east ) );
    }
    if ( south_window_ ) {
      south_window_->SetRectangle( this->MakeRectangleFromLL(
        splitter_position_ + splitter_width_, north_south, 0, west_east ) );
    }
    return {WMResult::Done};
  }, false );

  this->RegisterSingleHandler( WM_MOUSEMOVE, [this] ( WPARAM, LPARAM l_param ) -> WMResult {
    int x = this->GetLatitudeOfLParam( l_param );
    int y = ( TtWindow::GetYOfLParam( l_param ) != x ) ? TtWindow::GetYOfLParam( l_param ) : TtWindow::GetXOfLParam( l_param );
    if ( this->IsCapturing() ||
         ( x >= splitter_position_ && x <= splitter_position_ + splitter_width_ &&
           y >= 0 && y <= this->GetWestEast() ) ) {
      ::SetCursor( ::LoadCursor( NULL, this->GetCursorID() ) );
    }
    if ( this->IsCapturing() ) {
      int new_position = this->GetLatitudeOfLParam( l_param ) - (splitter_width_ / 2);
      this->LimitCheck( new_position );
      this->EraseMoving();
      splitter_position_ = new_position;
      this->DrawMoving();
    }
    return {WMResult::Done};
  }, false );

  this->RegisterSingleHandler( WM_LBUTTONDOWN, [this] ( WPARAM, LPARAM l_param ) -> WMResult {
    int x = this->GetLatitudeOfLParam( l_param );
    if ( x >= splitter_position_ && x <= splitter_position_ + splitter_width_ ) {
      this->SetCapture();
      int new_position = x - (splitter_width_ / 2);
      this->LimitCheck( new_position );
      splitter_position_ = new_position;
      this->DrawMoving();
    }
    return {WMResult::Done};
  }, false );

  this->RegisterSingleHandler( WM_LBUTTONUP, [this] ( WPARAM, LPARAM l_param ) -> WMResult {
    if ( this->IsCapturing() ) {
      this->ReleaseCapture();
      ::SetCursor( reinterpret_cast<HCURSOR>( ::GetClassLongPtr( handle_, GCLP_HCURSOR ) ) );
      int new_position = this->GetLatitudeOfLParam( l_param ) - (splitter_width_ / 2);
      this->LimitCheck( new_position );
      splitter_position_ = new_position;
      this->SendMessage( WM_SIZE );
    }
    return {WMResult::Done};
  }, false );

  this->RegisterSingleHandler( WM_CAPTURECHANGED, [this] ( WPARAM, LPARAM ) -> WMResult {
    this->EraseMoving();
    return {WMResult::Done};
  }, false );

}

void
TtSplitterPanel::LimitCheck( int& position )
{
  if ( position < north_limit_ ) {
    position = north_limit_;
  }
  if ( position > (this->GetNorthSouth() - south_limit_ - splitter_width_) ) {
    position = this->GetNorthSouth() - south_limit_ - splitter_width_;
  }
}


void
TtSplitterPanel::DrawMoving( void )
{
  TtReleasableDeviceContext dc = this->GetDeviceContext();
  HBRUSH brush = ::CreateSolidBrush( RGB( 0, 0, 0 ) );

  dc.SetROP2( R2_NOT );
  dc.SetBkMode( TRANSPARENT );
  HGDIOBJ old_brush = dc.SelectObject( brush );
  dc.DrawRectangle( this->MakeRectangleFromLL(
    splitter_position_, splitter_position_ + splitter_width_, 0, this->GetWestEast() ) );
  dc.SelectObject( old_brush );
  dc.DeleteObject( brush );
}

void
TtSplitterPanel::EraseMoving( void )
{
  this->DrawMoving();
}


// -- TtVirticalSplitterPanel --------------------------------------------
TtVirticalSplitterPanel::TtVirticalSplitterPanel( int width, int north_limit, int south_limit ) :
TtSplitterPanel( width, north_limit, south_limit )
{
}

LPCTSTR
TtVirticalSplitterPanel::GetCursorID( void )
{
  return IDC_SIZEWE;
}

int
TtVirticalSplitterPanel::GetLatitudeOfLParam( LPARAM l_param )
{
  return TtWindow::GetXOfLParam( l_param );
}

RECT
TtVirticalSplitterPanel::MakeRectangleFromLL( int north, int south, int west, int east )
{
  return {north, west, south, east};
}

int
TtVirticalSplitterPanel::GetWestEast( void )
{
  return this->GetHeight();
}

int
TtVirticalSplitterPanel::GetNorthSouth( void )
{
  return this->GetWidth();
}


// -- TtHorizontalSplitterPanel ------------------------------------------
TtHorizontalSplitterPanel::TtHorizontalSplitterPanel( int width, int north_limit , int south_limit ) :
TtSplitterPanel( width, north_limit, south_limit )
{
}

LPCTSTR
TtHorizontalSplitterPanel::GetCursorID( void )
{
  return IDC_SIZENS;
}

int
TtHorizontalSplitterPanel::GetLatitudeOfLParam( LPARAM l_param )
{
  return TtWindow::GetYOfLParam( l_param );
}

RECT
TtHorizontalSplitterPanel::MakeRectangleFromLL( int north, int south, int west, int east )
{
  return {west, north, east, south};
}

int
TtHorizontalSplitterPanel::GetWestEast( void )
{
  return this->GetWidth();
}

int
TtHorizontalSplitterPanel::GetNorthSouth( void )
{
  return this->GetHeight();
}
