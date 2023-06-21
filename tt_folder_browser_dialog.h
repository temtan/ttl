// tt_folder_browser_dialog.h

#pragma once

#include <string>

#include "tt_window.h"


// -- TtFolderBrowserDialog ----------------------------------------------
class TtFolderBrowserDialog {
private:
  static int CALLBACK Callback( HWND handle, UINT msg, LPARAM l_param, LPARAM lpData );

public:
  explicit TtFolderBrowserDialog( void );

  const std::string& GetSelectedPath( void ) const;
  void SetSelectedPath( const std::string& path );

  const std::string& GetDescription( void ) const;
  void SetDescription( const std::string& description );

  void Reset( void );
  bool ShowDialog( void );
  bool ShowDialog( TtWindow& parent );

private:
  int BoxInitialized( TtWindow& box );

private:
  std::string selected_path_;
  std::string description_;
};
