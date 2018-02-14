// tt_clipboard.h

#pragma once

#include <string>

#include "tt_windows_h_include.h"


// -- TtClipboard --------------------------------------------------------
class TtClipboard {
public:
  static bool SetString( const std::string& data );
};
