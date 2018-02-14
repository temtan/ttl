// tt_window_controls.cpp

#include "ttl_define.h"

#include "tt_window_controls.h"


// -- TtButtonWithCheck --------------------------------------------------
bool
TtButtonWithCheck::GetCheck( void )
{
  return this->SendMessage( BM_GETCHECK ) ? true : false;
}

int
TtButtonWithCheck::SetCheck( bool checking )
{
  return this->SendMessage( BM_SETCHECK, checking ? BST_CHECKED : BST_UNCHECKED );
}

// -- TtEdit -------------------------------------------------------------
void
TtEdit::SetReadOnly( bool flag )
{
  this->SendMessage( EM_SETREADONLY, flag ? TRUE : FALSE );
}


int
TtEdit::GetFirstVisibleLine( void )
{
  return this->SendMessage( EM_GETFIRSTVISIBLELINE );
}


void
TtEdit::SetTextLimit( unsigned int size )
{
  this->SendMessage( EM_LIMITTEXT, size );
}

// -- TtStatusBar --------------------------------------------------------
TtStatusBar::TtStatusBar( void )
{
}

void
TtStatusBar::DivideInto( unsigned int count, std::vector<int> positions )
{
  TtUtility::UniqueArray<int> array( count );
  for ( unsigned int i = 0; i < positions.size(); ++i ) {
    array.GetPointer()[i] = positions[i];
  }
  this->SendMessage( SB_SETPARTS , count, reinterpret_cast<LPARAM>( array.GetPointer() ) );
}


void
TtStatusBar::SetTextAt( int index, const std::string& text, int style )
{
  this->SendMessage( SB_SETTEXT, MAKEWPARAM( index, style ), reinterpret_cast<LPARAM>( text.c_str() ) );
}


void
TtStatusBar::SetSimpleMode( bool flag )
{
  this->SendMessage( SB_SIMPLE, flag ? TRUE : FALSE );
}

void
TtStatusBar::SetTextAsSimpleMode( const std::string& text, int style )
{
  this->SetTextAt( SB_SIMPLEID, text, style );
}
