// tt_icon.h

#pragma once

#include "tt_windows_h_include.h"

#include "tt_bmp_image.h"
#include "tt_utility.h"


// -- TtIcon -------------------------------------------------------------
class TtIcon : public TtUtility::WindowsHandleHolder<HICON> {
public:
  enum class Standard : DWORD {
    APPLICATION = 32512,
    HAND        = 32513,
    ERROR_MARK  = 32513,
    QUESTION    = 32514,
    EXCLAMATION = 32515,
    WARNING     = 32515,
    ASTERISK    = 32516,
    INFORMATION = 32516,
    SHIELD      = 32518,
  };

public:
  static unsigned int GetIconCountOfFile( const std::string& path );

  static TtIcon CreateFromID( Standard id );
  static TtIcon CreateFromResourceName( const std::string& name );
  static TtIcon CreateFromResourceID( DWORD id );
  static TtIcon CreateFromFileAsLarge( const std::string& path, unsigned int index );
  static TtIcon CreateFromFileAsSmall( const std::string& path, unsigned int index );

public:
  explicit TtIcon( HICON handle, bool auto_destroy );

  TtBmpImage GetARGBBitmapAsSmall( void );

  // ŽÀ‘•‰ö‚µ‚¢
  // TtBmpImage GetBmp( void );
  // TtBmpImage GetMaskBmp( void );
};
