// tt_pick_icon_dialog.cpp

#pragma warning(push)
#  pragma warning(disable : 4005)
#  pragma warning(disable : 4917)
#  include <Shlobj.h>
#pragma warning(pop)

#include "tt_dynamic_link_library.h"
#include "tt_window_controls.h"
#include "tt_path.h"
#include "tt_string.h"

#include "tt_pick_icon_dialog.h"


// -- TtPickIconDialog -------------------------------------------------
TtPickIconDialog::TtPickIconDialog( void ) :
TtPickIconDialog( "" )
{
}

TtPickIconDialog::TtPickIconDialog( const std::string& path, int index ) :
path_( path ),
index_( index )
{
}


const std::string&
TtPickIconDialog::GetPath( void ) const
{
  return path_;
}

void
TtPickIconDialog::SetPath( const std::string& path )
{
  path_ = path;
}

int
TtPickIconDialog::GetIndex( void ) const
{
  return index_;
}

void
TtPickIconDialog::SetIndex( int index )
{
  index_ = index;
}


bool
TtPickIconDialog::ShowDialog( void )
{
  return this->ShowDialog( TtExtraordinarilyWindow::Null );
}


bool
TtPickIconDialog::ShowDialog( TtWindow& parent )
{
  std::string full_path = TtPath::ExpandPath( path_ );
  size_t ret;
  WCHAR  buf[MAX_PATH];
  ::mbstowcs_s( &ret, buf, MAX_PATH, full_path.c_str(), _TRUNCATE );
  int index = index_;

  bool flag = (::PickIconDlg( parent.GetHandle(), buf, MAX_PATH, &index ) == 1);
  if ( flag ) {
    char tmp[MAX_PATH];
    ::wcstombs_s( &ret, tmp, MAX_PATH, buf, _TRUNCATE );
    path_ = tmp;
    index_ = index;
  }
  return flag;
}
