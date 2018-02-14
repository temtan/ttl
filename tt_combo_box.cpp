// tt_combo_box.cpp

#include "ttl_define.h"
#include "tt_string.h"

#include "tt_combo_box.h"


int
TtComboBox::GetCount( void )
{
  return this->SendMessage( CB_GETCOUNT );
}

unsigned int
TtComboBox::GetTextLengthAt( unsigned int index )
{
  return this->SendMessage( CB_GETLBTEXTLEN, static_cast<WPARAM>( index ) );
}


std::string
TtComboBox::GetTextAt( unsigned int index )
{
  TtString::SharedString buf( this->GetTextLengthAt( index ) + 1 );
  this->SendMessage( CB_GETLBTEXT, index, reinterpret_cast<LPARAM>( buf.GetPointer() ) );
  return buf.ToString();
}

int
TtComboBox::Find( unsigned int start, const std::string& str )
{
  return this->SendMessage( CB_FINDSTRING, start, reinterpret_cast<LPARAM>( str.c_str() ) );
}


int
TtComboBox::Push( const std::string& str )
{
  return this->SendMessage( CB_ADDSTRING, 0, reinterpret_cast<LPARAM>( str.c_str() ) );
}

int
TtComboBox::InsertAt( unsigned int index, const std::string& str )
{
  return this->SendMessage( CB_INSERTSTRING, index, reinterpret_cast<LPARAM>( str.c_str() ) );
}

int
TtComboBox::DeleteAt( unsigned int index )
{
  return this->SendMessage( CB_DELETESTRING, index );
}

void
TtComboBox::Clear( void )
{
  this->SendMessage( CB_RESETCONTENT );
}


int
TtComboBox::GetSelectedIndex( void )
{
  return this->SendMessage( CB_GETCURSEL );
}


std::string
TtComboBox::GetSelectedString( void )
{
  return this->GetTextAt( this->GetSelectedIndex() );
}

int
TtComboBox::SetSelect( unsigned int index )
{
  return this->SendMessage( CB_SETCURSEL, index );
}

int
TtComboBox::SetSelectByString( const std::string& str )
{
  return this->SetSelect(
    this->SendMessage( CB_FINDSTRINGEXACT, 0, reinterpret_cast<LPARAM>( str.c_str() ) ) );
}


#ifdef TT_MAKE_TEMPLATE_INSTANCE_
template class TtComboBoxTable<TtComboBoxDropDownWithEdit, int>;
#endif
