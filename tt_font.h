// tt_font.h

#pragma once

#include "tt_windows_h_include.h"
#include "tt_utility.h"


// -- TtFont -------------------------------------------------------------
class TtFont : public TtUtility::WindowsHandleHolder<HFONT> {
public:
  static TtFont DEFAULT;

  explicit TtFont( HFONT handle, bool auto_destroy );
  explicit TtFont( int size, bool auto_destroy = true );
};
