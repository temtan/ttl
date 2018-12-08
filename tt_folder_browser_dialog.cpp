// tt_folder_browser_dialog.cpp

#pragma warning(push)
#  pragma warning( disable: 4005 )
#  pragma warning( disable: 4917 )
#  include <shlobj.h>
#pragma warning(pop)

#include "ttl_define.h"
#include "tt_exception.h"

#include "tt_folder_browser_dialog.h"

#pragma comment(lib, "shell32.lib")


int CALLBACK
TtFolderBrowserDialog::Callback( HWND handle, UINT msg, LPARAM l_param, LPARAM lpData )
{
  NOT_USE( l_param );
  switch ( msg ) {
  case BFFM_INITIALIZED: {
    TtExtraordinarilyWindow box( handle );
    TtFolderBrowserDialog* dialog = reinterpret_cast<TtFolderBrowserDialog*>( lpData );
    dialog->BoxInitialized( box );
    break;
  }
  }
  return 0;
}


TtFolderBrowserDialog::TtFolderBrowserDialog( void ) :
selected_path_(),
description_()
{
}

const std::string&
TtFolderBrowserDialog::GetSelectedPath( void ) const
{
  return selected_path_;
}

void
TtFolderBrowserDialog::SetSelectedPath( const std::string& path )
{
  selected_path_ = path;
}

const std::string&
TtFolderBrowserDialog::GetDescription( void ) const
{
  return description_;
}

void
TtFolderBrowserDialog::SetDescription( const std::string& description )
{
  description_ = description;
}

void
TtFolderBrowserDialog::Reset( void )
{
  selected_path_.clear();
  description_.clear();
}

bool
TtFolderBrowserDialog::ShowDialog( TtWindow& parent )
{
  BROWSEINFO bi = {0};

  bi.hwndOwner = parent.GetHandle();
  bi.pidlRoot = nullptr;

  bi.pszDisplayName = nullptr;

  bi.lpszTitle = description_.c_str();
  bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_DONTGOBELOWDOMAIN | BIF_NEWDIALOGSTYLE;

  bi.lpfn = TtFolderBrowserDialog::Callback;
  bi.lParam = reinterpret_cast<LPARAM>( this );

  // ITEMIDLIST* item_id_list = ::SHBrowseForFolder( &bi );
  auto item_id_list = ::SHBrowseForFolder( &bi );

  if ( item_id_list == nullptr ) {
    return false;
  }

  char tmp[MAX_PATH + 16];
  if ( ::SHGetPathFromIDList( item_id_list, tmp ) == FALSE ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::SHGetPathFromIDList ) );
  }
  selected_path_ = tmp;
  ::CoTaskMemFree( item_id_list );

  return true;
}

int
TtFolderBrowserDialog::BoxInitialized( TtWindow& box )
{
  box.SendMessage( BFFM_SETSELECTIONA, 1, reinterpret_cast<LPARAM>( selected_path_.c_str() ) );
  return 0;
}
