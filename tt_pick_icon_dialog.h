// tt_pick_icon_dialog.h

#pragma once

#include <string>

#include "tt_window.h"


// -- TtPickIconDialog ---------------------------------------------------
class TtPickIconDialog {
public:
  explicit TtPickIconDialog( void );
  explicit TtPickIconDialog( const std::string& path, int index = 0 );

  const std::string& GetPath( void ) const;
  void SetPath( const std::string& path );

  int GetIndex( void ) const;
  void SetIndex( int index );

  bool ShowDialog( void );
  bool ShowDialog( TtWindow& parent );

private:
  std::string  path_;
  int          index_;
};
