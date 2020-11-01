// tt_tool_bar.cpp

#include "ttl_define.h"
#include "tt_exception.h"
#include "tt_string.h"

#include "tt_tool_bar.h"


static void
send_message_if_false_throw(
  TtToolBar*   tool_bar_,
  UINT         msg,
  const char*  msg_string,
  const char*  file,
  unsigned int line,
  WPARAM       w_param,
  LPARAM       l_param )
{
  if ( NOT( tool_bar_->SendMessage( msg, w_param, l_param ) ) ) {
    throw TtWindowsSystemCallException( msg_string, ::GetLastError(), file, line );
  }
}

#define TT_TOOL_BAR_SEND_MESSAGE( tool_bar, msg, w, l ) send_message_if_false_throw( tool_bar, msg, #msg, __FILE__, __LINE__, w, l )

// -- TtToolBar::Button --------------------------------------------------
TtToolBar::Button::Button( TtToolBar* tool_bar, int command_id ) :
tool_bar_( tool_bar ),
command_id_( command_id )
{
}


void
TtToolBar::Button::SetState( BYTE state )
{
  tool_bar_->SendMessage( TB_SETSTATE, command_id_, state );
}

void
TtToolBar::Button::SetBmpImageIndex( int index )
{
  tool_bar_->SendMessage( TB_CHANGEBITMAP, command_id_, index );
}

RECT
TtToolBar::Button::GetRectangle( void )
{
  RECT rect;
  TT_TOOL_BAR_SEND_MESSAGE( tool_bar_, TB_GETRECT, command_id_, reinterpret_cast<LPARAM>( &rect ) );
  return rect;
}


void
TtToolBar::Button::SetCommandID( int command_id )
{
  TT_TOOL_BAR_SEND_MESSAGE( tool_bar_, TB_SETCMDID, this->GetIndex(), command_id );
  command_id_ = command_id;
}

int
TtToolBar::Button::GetCommandID( void )
{
  return command_id_;
}

int
TtToolBar::Button::GetIndex( void )
{
  return tool_bar_->SendMessage( TB_COMMANDTOINDEX, command_id_ );
}

int
TtToolBar::Button::GetBmpImageIndex( void )
{
  return tool_bar_->SendMessage( TB_GETBITMAP, command_id_ );
}


bool
TtToolBar::Button::IsChecked( void )
{
  return tool_bar_->SendMessage( TB_ISBUTTONCHECKED, command_id_ ) != FALSE;
}

bool
TtToolBar::Button::IsEnabled( void )
{
  return tool_bar_->SendMessage( TB_ISBUTTONENABLED, command_id_ ) != FALSE;
}

bool
TtToolBar::Button::IsHidden( void )
{
  return tool_bar_->SendMessage( TB_ISBUTTONHIDDEN, command_id_ ) != FALSE;
}

bool
TtToolBar::Button::IsIndeterminate( void )
{
  return tool_bar_->SendMessage( TB_ISBUTTONINDETERMINATE, command_id_ ) != FALSE;
}

bool
TtToolBar::Button::IsPressed( void )
{
  return tool_bar_->SendMessage( TB_ISBUTTONPRESSED, command_id_ ) != FALSE;
}


void
TtToolBar::Button::SetCheck( bool flag )
{
  flag ? this->Check() : this->Uncheck();
}

void
TtToolBar::Button::Check( void )
{
  TT_TOOL_BAR_SEND_MESSAGE( tool_bar_, TB_CHECKBUTTON, command_id_, TRUE );
}

void
TtToolBar::Button::Uncheck( void )
{
  TT_TOOL_BAR_SEND_MESSAGE( tool_bar_, TB_CHECKBUTTON, command_id_, FALSE );
}

void
TtToolBar::Button::SetEnable( bool flag )
{
  flag ? this->Enable() : this->Disable();
}

void
TtToolBar::Button::Enable( void )
{
  TT_TOOL_BAR_SEND_MESSAGE( tool_bar_, TB_ENABLEBUTTON, command_id_, TRUE );
}

void
TtToolBar::Button::Disable( void )
{
  TT_TOOL_BAR_SEND_MESSAGE( tool_bar_, TB_ENABLEBUTTON, command_id_, FALSE );
}

void
TtToolBar::Button::Press( void )
{
  TT_TOOL_BAR_SEND_MESSAGE( tool_bar_, TB_PRESSBUTTON, command_id_, TRUE );
}

void
TtToolBar::Button::Pull( void )
{
  TT_TOOL_BAR_SEND_MESSAGE( tool_bar_, TB_PRESSBUTTON, command_id_, FALSE );
}


void
TtToolBar::Button::Delete( void )
{
  TT_TOOL_BAR_SEND_MESSAGE( tool_bar_, TB_DELETEBUTTON, this->GetIndex(), 0 );
}


// -- TtToolBar ----------------------------------------------------------
bool
TtToolBar::CreatedInternal( void )
{
  this->SendMessage( TB_BUTTONSTRUCTSIZE, 20 );
  return true;
}

void
TtToolBar::SetExtendedStyle( int style )
{
  this->SendMessage( TB_SETEXTENDEDSTYLE, 0, style );
}


void
TtToolBar::SetButtonSize( int x, int y )
{
  TT_TOOL_BAR_SEND_MESSAGE( this, TB_SETBITMAPSIZE, 0, MAKELPARAM( x, y ) );
}

void
TtToolBar::SetImageSize( int x, int y )
{
  this->SendMessage( TB_SETBITMAPSIZE, 0, MAKELONG( x, y ) );
}


int
TtToolBar::AddStandardSmallBmpImage( void )
{
  TBADDBITMAP tmp = {HINST_COMMCTRL, IDB_STD_SMALL_COLOR};
  return this->SendMessage( TB_ADDBITMAP, 0, reinterpret_cast<LPARAM>( &tmp ) );
}

int
TtToolBar::AddStandardLargeBmpImage( void )
{
  TBADDBITMAP tmp = {HINST_COMMCTRL, IDB_STD_LARGE_COLOR};
  return this->SendMessage( TB_ADDBITMAP, 0, reinterpret_cast<LPARAM>( &tmp ) );
}

int
TtToolBar::AddViewSmallBmpImage( void )
{
  TBADDBITMAP tmp = {HINST_COMMCTRL, IDB_VIEW_SMALL_COLOR};
  return this->SendMessage( TB_ADDBITMAP, 0, reinterpret_cast<LPARAM>( &tmp ) );
}

int
TtToolBar::AddViewLargeBmpImage( void )
{
  TBADDBITMAP tmp = {HINST_COMMCTRL, IDB_VIEW_LARGE_COLOR};
  return this->SendMessage( TB_ADDBITMAP, 0, reinterpret_cast<LPARAM>( &tmp ) );
}

int
TtToolBar::AddBmpImage( int image_count, TtBmpImage& bmp_image )
{
  TBADDBITMAP tmp = {NULL, static_cast<UINT>( reinterpret_cast<LONG_PTR>( bmp_image.GetHandle() ) )};
  return this->SendMessage( TB_ADDBITMAP, image_count, reinterpret_cast<LPARAM>( &tmp ) );
}

int
TtToolBar::AddBmpImage( int image_count, HINSTANCE h_instance, UINT resource_id )
{
  TBADDBITMAP tmp = {h_instance, resource_id};
  return this->SendMessage( TB_ADDBITMAP, image_count, reinterpret_cast<LPARAM>( &tmp ) );
}


void
TtToolBar::SetImageList( TtImageList& image_list )
{
  this->SendMessage( TB_SETIMAGELIST, 0, reinterpret_cast<LPARAM>( image_list.GetHandle() ) );
}


int
TtToolBar::AddString( const std::string& str )
{
  return this->AddStrings( {str} );
}

int
TtToolBar::AddStrings( const std::vector<std::string> v )
{
  std::string tmp = TtString::ToRangedStringFromVector( v );
  return this->SendMessage( TB_ADDSTRING, 0, reinterpret_cast<LPARAM>( tmp.c_str() ) );
}


int
TtToolBar::GetButtonCount( void )
{
  return this->SendMessage( TB_BUTTONCOUNT );
}


RECT
TtToolBar::SetRowsCount( int rows, bool force )
{
  RECT rect;
  this->SendMessage( TB_SETROWS, MAKEWPARAM( rows, force ? FALSE : TRUE ), reinterpret_cast<LPARAM>( &rect ) );
  return rect;
}

int
TtToolBar::GetRowsCount( void )
{
  return this->SendMessage( TB_GETROWS );
}


void
TtToolBar::ResizeButtonAuto( void )
{
  this->SendMessage( TB_AUTOSIZE );
}


TtToolBar::Button
TtToolBar::AddButton( int command_id, int bmp_index, int string_index, int style )
{
  TBBUTTON tmp = {bmp_index, command_id, Button::State::Enabled, static_cast<BYTE>( style ), {0}, 0, string_index};
  TT_TOOL_BAR_SEND_MESSAGE( this, TB_ADDBUTTONS, 1, reinterpret_cast<LPARAM>( &tmp ) );
  return Button( this, command_id );
}

TtToolBar::Button
TtToolBar::AddButtonWithString( int command_id, int bmp_index, const std::string& string, int style )
{
  TBBUTTON tmp = {bmp_index, command_id, Button::State::Enabled, static_cast<BYTE>( style ), {0}, 0, reinterpret_cast<INT_PTR>( string.c_str() )};
  TT_TOOL_BAR_SEND_MESSAGE( this, TB_ADDBUTTONS, 1, reinterpret_cast<LPARAM>( &tmp ) );
  string_table_[command_id] = string;
  return Button( this, command_id );
}


void
TtToolBar::AddSeparator( int command_id )
{
  TBBUTTON tmp = {0, command_id, Button::State::Enabled, static_cast<BYTE>( Button::Style::Separator ), 0, 0, 0};
  TT_TOOL_BAR_SEND_MESSAGE( this, TB_ADDBUTTONS, 1, reinterpret_cast<LPARAM>( &tmp ) );
}


TtPanel::NotifyHandler
TtToolBar::MakeNotifyHandlerForToolTipByStringTable( void )
{
  return [this] ( NMHDR* nmhdr ) -> WMResult {
    if ( nmhdr->code == TBN_GETINFOTIP ) {
      NMTBGETINFOTIP* info = reinterpret_cast<NMTBGETINFOTIP*>( nmhdr );
      if ( auto it = string_table_.find( info->iItem ); it != string_table_.end() ) {
        info->pszText = it->second.data();
        info->cchTextMax = static_cast<int>( it->second.size() );
        return {WMResult::Done};
      }
    }
    return {WMResult::Incomplete};
  };
}


TtToolBar::Button
TtToolBar::GetButton( int command_id )
{
  return Button( this, command_id );
}
