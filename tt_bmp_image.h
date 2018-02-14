// tt_bmp_image.h

#pragma once

#include <string>

#include "tt_windows_h_include.h"
#include "tt_utility.h"

// -- TtBmpImage ---------------------------------------------------------
class TtBmpImage : public TtUtility::WindowsHandleHolder<HBITMAP> {
public:
  enum class Standard {
    CLOSE       = 32754,
    UPARROW     = 32753,
    DNARROW     = 32752,
    RGARROW     = 32751,
    LFARROW     = 32750,
    REDUCE      = 32749,
    ZOOM        = 32748,
    RESTORE     = 32747,
    REDUCED     = 32746,
    ZOOMD       = 32745,
    RESTORED    = 32744,
    UPARROWD    = 32743,
    DNARROWD    = 32742,
    RGARROWD    = 32741,
    LFARROWD    = 32740,
    MNARROW     = 32739,
    COMBO       = 32738,
    UPARROWI    = 32737,
    DNARROWI    = 32736,
    RGARROWI    = 32735,
    LFARROWI    = 32734,

    OLD_CLOSE   = 32767,
    SIZE        = 32766,
    OLD_UPARROW = 32765,
    OLD_DNARROW = 32764,
    OLD_RGARROW = 32763,
    OLD_LFARROW = 32762,
    BTSIZE      = 32761,
    CHECK       = 32760,
    CHECKBOXES  = 32759,
    BTNCORNERS  = 32758,
    OLD_REDUCE  = 32757,
    OLD_ZOOM    = 32756,
    OLD_RESTORE = 32755,
  };

  explicit TtBmpImage( Standard id );
  explicit TtBmpImage( HBITMAP handle, bool auto_destroy );
  static TtBmpImage CreateFromFile( const std::string& path );
  static TtBmpImage CreateFromResourceName( const std::string& name );

  int GetWidth( void ) const;
  int GetHeight( void ) const;

  TtBmpImage ConvertToARGBBitmap( void );
};
