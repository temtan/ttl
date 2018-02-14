// tt_list_box.cpp

#include "ttl_define.h"
#include "tt_string.h"

#include "tt_list_box.h"

// -- TtListBox ----------------------------------------------------------
int
TtListBox::GetItemHeight( void )
{
  return this->SendMessage( LB_GETITEMHEIGHT );
}

void
TtListBox::SetItemHeight( int height )
{
  this->SendMessage( LB_SETITEMHEIGHT, 0, height );
}


int
TtListBox::GetCount() {
  return this->SendMessage( LB_GETCOUNT );
}

unsigned int
TtListBox::GetTextLengthAt( unsigned int index )
{
  return this->SendMessage( LB_GETTEXTLEN, static_cast<WPARAM>( index ) );
}

std::string
TtListBox::GetTextAt( unsigned int index )
{
  TtString::SharedString buf( this->GetTextLengthAt( index ) + 1 );
  this->SendMessage( LB_GETTEXT, index, reinterpret_cast<LPARAM>( buf.GetPointer() ) );
  return buf.ToString();
}

std::string
TtListBox::GetCurrentText( void )
{
  unsigned int index = this->GetCurrent();
  if ( index != LB_ERR ) {
    return this->GetTextAt( index );
  }
  return "";
}


int
TtListBox::GetCurrent( void )
{
  return this->SendMessage( LB_GETCURSEL );
}

void
TtListBox::SetCurrent( int index )
{
  this->SendMessage( LB_SETCURSEL, index );
}

void
TtListBox::UnsetCurrent( void )
{
  this->SetCurrent( -1 );
}


int
TtListBox::Find( unsigned int start, const std::string& str )
{
  return this->SendMessage( LB_FINDSTRING, start, reinterpret_cast<LPARAM>( str.c_str() ) );
}

int
TtListBox::Push( const std::string& str )
{
  return this->SendMessage( LB_ADDSTRING, 0, reinterpret_cast<LPARAM>( str.c_str() ) );
}

int
TtListBox::InsertAt( unsigned int index, const std::string& str )
{
  return this->SendMessage( LB_INSERTSTRING, index, reinterpret_cast<LPARAM>( str.c_str() ) );
}

int
TtListBox::DeleteAt( unsigned int index )
{
  return this->SendMessage( LB_DELETESTRING, index );
}

void
TtListBox::Clear( void )
{
  this->SendMessage( LB_RESETCONTENT );
}

int
TtListBox::LockDraw( void )
{
  return this->SendMessage( WM_SETREDRAW, FALSE );
}

int
TtListBox::UnlockDraw( void )
{
  return this->SendMessage( WM_SETREDRAW, TRUE );
}


// -- TtListBoxMultiSelect -----------------------------------------------
int
TtListBoxMultiSelect::GetSelectedCount( void )
{
  return this->SendMessage( LB_GETSELCOUNT );
}

bool
TtListBoxMultiSelect::IsSelect( unsigned int index )
{
  return this->SendMessage( LB_GETSEL, index ) ? true : false;
}

int
TtListBoxMultiSelect::SetSelect( unsigned int index )
{
  return this->SendMessage( LB_SETSEL, TRUE, static_cast<LPARAM>( index ) );
}

int
TtListBoxMultiSelect::UnsetSelect( unsigned int index )
{
  return this->SendMessage( LB_SETSEL, FALSE, static_cast<LPARAM>( index ) );
}

