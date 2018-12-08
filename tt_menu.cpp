// tt_menu.cpp

#include "tt_windows_h_include.h"

#include "ttl_define.h"
#include "tt_exception.h"
#include "tt_string.h"

#include "tt_menu.h"

#pragma comment(lib, "user32.lib")



// -- TtMenuItem ---------------------------------------------------------
TtMenuItem::TtMenuItem( HMENU parent, int index ) :
parent_( parent ),
index_( index )
{
}


HMENU
TtMenuItem::GetParentHandle( void )
{
  return parent_;
}

int
TtMenuItem::GetIndex( void )
{
  return index_;
}


bool
TtMenuItem::GetEnabled( void )
{
  MENUITEMINFO info = {sizeof( MENUITEMINFO )};
  info.fMask  = MIIM_STATE;
  this->GetInfo( info );
  return (info.fState & MFS_GRAYED) ? false : true;
}

void
TtMenuItem::SetEnabled( bool flag )
{
  MENUITEMINFO info = {sizeof( MENUITEMINFO )};
  info.fMask  = MIIM_STATE;
  info.fState = flag ? MFS_ENABLED : MFS_GRAYED;
  this->SetInfo( info );
}


bool
TtMenuItem::GetCheck( void )
{
  MENUITEMINFO info = {sizeof( MENUITEMINFO )};
  info.fMask  = MIIM_STATE;
  this->GetInfo( info );
  return (info.fState & MFS_CHECKED) ? true : false;
}

void
TtMenuItem::SetCheck( bool flag )
{
  MENUITEMINFO info = {sizeof( MENUITEMINFO )};
  info.fMask  = MIIM_STATE;
  info.fState = flag ? MFS_CHECKED : MFS_UNCHECKED;
  this->SetInfo( info );
}

bool
TtMenuItem::GetHilight( void )
{
  MENUITEMINFO info = {sizeof( MENUITEMINFO )};
  info.fMask  = MIIM_STATE;
  this->GetInfo( info );
  return (info.fState & MFS_HILITE) ? true : false;
}

void
TtMenuItem::SetHilight( bool flag )
{
  MENUITEMINFO info = {sizeof( MENUITEMINFO )};
  info.fMask  = MIIM_STATE;
  info.fState = flag ? MFS_HILITE : MFS_UNHILITE;
  this->SetInfo( info );
}


void
TtMenuItem::SetText( const std::string& text )
{
  MENUITEMINFO info = {sizeof( MENUITEMINFO )};
  info.fMask      = MIIM_STRING;
  info.dwTypeData = const_cast<char*>( text.c_str() );
  this->SetInfo( info );
}

std::string
TtMenuItem::GetText( void )
{
  MENUITEMINFO info = {sizeof( MENUITEMINFO )};
  info.fMask      = MIIM_STRING;
  info.dwTypeData = NULL;
  this->GetInfo( info );
  TtString::UniqueString buf( info.cch );
  info.dwTypeData = buf.GetPointer();
  info.cch++;
  this->GetInfo( info );
  return buf.GetPointer();
}


void
TtMenuItem::SetCommandID( int id )
{
  MENUITEMINFO info = {sizeof( MENUITEMINFO )};
  info.fMask = MIIM_ID;
  info.wID   = id;
  this->SetInfo( info );
}

int
TtMenuItem::GetCommandID( void )
{
  MENUITEMINFO info = {sizeof( MENUITEMINFO )};
  info.fMask = MIIM_ID;
  this->GetInfo( info );
  return info.wID;
}

void
TtMenuItem::SetBmpImage( TtBmpImage& bmp )
{
  MENUITEMINFO info = {sizeof( MENUITEMINFO )};
  info.fMask = MIIM_BITMAP;
  info.hbmpItem = bmp.GetHandle();
  this->SetInfo( info );
}


void
TtMenuItem::CheckRadio( void )
{
  TtMenu( parent_, false ).CheckRadioItem( index_ );
}


void
TtMenuItem::SetInfo( MENUITEMINFO& info )
{
  if ( ::SetMenuItemInfo( parent_, index_, TRUE, &info ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::SetMenuItemInfo ) );
  }
}

void
TtMenuItem::GetInfo( MENUITEMINFO& info )
{
  if ( ::GetMenuItemInfo( parent_, index_, TRUE, &info ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::GetMenuItemInfo ) );
  }
}



// -- TtMenu -------------------------------------------------------------
TtMenu::TtMenu( HMENU handle, bool auto_destroy ) :
WindowsHandleHolder( [handle] ( void ) { ::DestroyMenu( handle ); }, handle, auto_destroy )
{
}


int
TtMenu::GetItemCount( void )
{
  int ret = ::GetMenuItemCount( handle_ );
  if ( ret == -1 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::GetMenuItemCount ) );
  }
  return ret;
}


TtMenuItem
TtMenu::AppendNewItem( int command_id, const std::string& text )
{
  return this->InsertNewItem( this->GetItemCount(), command_id, text );
}

TtMenuItem
TtMenu::InsertNewItem( int index, int command_id, const std::string& text )
{
  MENUITEMINFO info = {sizeof( MENUITEMINFO )};
  info.fMask      = MIIM_ID | MIIM_STRING;
  info.wID        = command_id;
  info.dwTypeData = const_cast<char*>( text.c_str() );
  info.cch        = static_cast<UINT>( text.size() );
  this->InsertInfo( index, info );
  return TtMenuItem( handle_, index );
}


void
TtMenu::AppendMenu( TtSubMenu& sub_menu, const std::string& text )
{
  return this->InsertMenu( this->GetItemCount(), sub_menu, text );
}

void
TtMenu::InsertMenu( int index, TtSubMenu& sub_menu, const std::string& text )
{
  MENUITEMINFO info = {sizeof( MENUITEMINFO )};
  info.fMask      = MIIM_SUBMENU | MIIM_STRING;
  info.hSubMenu   = sub_menu.handle_;
  info.dwTypeData = const_cast<char*>( text.c_str() );
  info.cch        = static_cast<UINT>( text.size() );
  this->InsertInfo( index, info );
}


TtMenuItem
TtMenu::GetItemAt( int index )
{
  return TtMenuItem( handle_, index );
}

TtMenuItem
TtMenu::GetLastItem( void )
{
  return this->GetItemAt( this->GetItemCount() - 1 );
}

TtSubMenu
TtMenu::GetSubMenuAt( int index )
{
  MENUITEMINFO info = {sizeof( MENUITEMINFO )};
  info.fMask = MIIM_SUBMENU;
  TtMenuItem( handle_, index ).GetInfo( info );
  return TtSubMenu( info.hSubMenu, false );
}


void
TtMenu::CheckRadioItem( int index, int group_start, int group_end )
{
  if ( group_end == -1 ) {
    group_end = this->GetItemCount() - 1;
  }
  if ( ::CheckMenuRadioItem( handle_, group_start, group_end, index, MF_BYPOSITION ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::CheckMenuRadioItem ) );
  }
}


void
TtMenu::DeleteAt( int index )
{
  if ( ::DeleteMenu( handle_, index, MF_BYPOSITION ) != TRUE ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::GetMenuItemCount ) );
  }
}

void
TtMenu::Clear( void )
{
  while ( this->GetItemCount() != 0 ) {
    this->DeleteAt( 0 );
  }
}


void
TtMenu::SetInfoOfMenuCommand( bool flag )
{
  this->ChangeInfoOfStyle( MNS_NOTIFYBYPOS, flag );
}


void
TtMenu::SetInfoOfCheckOrBMP( bool flag )
{
  this->ChangeInfoOfStyle( MNS_CHECKORBMP, flag );
}


void
TtMenu::SetInfoOfNoCheck( bool flag )
{
  this->ChangeInfoOfStyle( MNS_NOCHECK, flag );
}



void
TtMenu::InsertInfo( int index, MENUITEMINFO& info )
{
  if ( ::InsertMenuItem( handle_, index, TRUE, &info ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::InsertMenuItem ) );
  }
}


void
TtMenu::SetInfo( MENUINFO& info )
{
  if ( ::SetMenuInfo( handle_, &info ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::SetMenuInfo ) );
  }
}

void
TtMenu::GetInfo( MENUINFO& info )
{
  if ( ::GetMenuInfo( handle_, &info ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::GetMenuInfo ) );
  }
}


void
TtMenu::ChangeInfoOfStyle( DWORD style, bool flag )
{
  MENUINFO info = {sizeof( MENUINFO )};
  info.fMask = MIM_STYLE;
  this->GetInfo( info );
  info.dwStyle = flag ? (info.dwStyle | style) : (info.dwStyle & ~style);
  this->SetInfo( info );
}


// -- TtSubMenu ----------------------------------------------------------
TtSubMenu
TtSubMenu::Create( bool auto_destroy )
{
  HMENU handle = ::CreatePopupMenu();
  if ( handle == NULL ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::CreatePopupMenu ) );
  }
  TtSubMenu menu( handle, auto_destroy );
  menu.SetInfoOfCheckOrBMP( true );
  return menu;
}

TtSubMenu::TtSubMenu( HMENU handle, bool auto_destroy ) :
TtMenu( handle, auto_destroy )
{
}

void
TtSubMenu::AppendSeparator( void )
{
  this->InsertSeparator( this->GetItemCount() );
}

void
TtSubMenu::InsertSeparator( int index )
{
  MENUITEMINFO info = {sizeof( MENUITEMINFO )};
  info.fMask      = MIIM_FTYPE;
  info.fType      = MFT_SEPARATOR;
  this->InsertInfo( index, info );
}


void
TtSubMenu::PopupAbsolute( TtWindow& parent, int x, int y )
{
  if ( ::TrackPopupMenuEx( handle_, TPM_LEFTALIGN | TPM_TOPALIGN, x, y, parent.GetHandle(), nullptr ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::TrackPopupMenuEx ) );
  }
}

void
TtSubMenu::PopupRelative( TtWindow& parent, int x, int y )
{
  POINT point = {x, y};
  if ( ::ClientToScreen( parent.GetHandle(), &point ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::ClientToScreen ) );
  }
  this->PopupAbsolute( parent, point.x, point. y );
}

// -- TtSubMenuCommand ---------------------------------------------------
TtSubMenuCommand
TtSubMenuCommand::Create( bool auto_destroy )
{
  TtSubMenuCommand menu( TtSubMenu::Create( false ).GetHandle(), auto_destroy );
  menu.SetInfoOfMenuCommand( true );
  return menu;
}

TtSubMenuCommand::TtSubMenuCommand( HMENU handle, bool auto_destroy ) :
TtSubMenu( handle, auto_destroy )
{
}


// -- TtMenuBar ----------------------------------------------------------
TtMenuBar::TtMenuBar( bool auto_destroy ) :
TtMenu( ::CreateMenu(), auto_destroy )
{
  if ( handle_ == NULL ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::CreateMenu ) );
  }
  this->SetInfoOfMenuCommand( true );
}


TtMenuBar::TtMenuBar( HMENU handle, bool auto_destroy ) :
TtMenu( handle, auto_destroy )
{
}


// -- TtSubMenuCommandMaker ----------------------------------------------
TtSubMenuCommandMaker::TtSubMenuCommandMaker( void ) :
root_(),
at_make_menu_()
{
}


TtSubMenuCommandMaker::ElementVector&
TtSubMenuCommandMaker::GetRoot( void )
{
  return root_;
}


void
TtSubMenuCommandMaker::SetAtMakeMenu( std::function<void ( TtSubMenuCommand& )> func )
{
  at_make_menu_ = func;
}


TtSubMenuCommand
TtSubMenuCommandMaker::MakeMenu( void )
{
  std::function<TtSubMenuCommand ( ElementVector& )> func =  [this, &func] ( ElementVector& v ) -> TtSubMenuCommand {
    TtSubMenuCommand current = TtSubMenuCommand::Create();
    at_make_menu_( current );
    for ( auto& element : v ) {
      if ( element->IsSubMenu() ) {
        SubMenu* tmp = element->CastTo<SubMenu>();
        current.AppendMenu( func( tmp->elements_ ), tmp->name_ );
      }
      else if ( element->IsSeparator() ) {
        current.AppendSeparator();
      }
      else if ( element->IsItem() ) {
        Item* tmp = element->CastTo<Item>();
        tmp->AfterMake( current.AppendNewItem( 0, tmp->GetName() ) );
      }
      else {
        throw TT_INTERNAL_EXCEPTION;
      }
    }
    return current;
  };
  return func( root_ );
}
