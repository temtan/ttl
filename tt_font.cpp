// tt_font.cpp

#include "ttl_define.h"
#include "tt_exception.h"

#include "tt_font.h"

#pragma comment(lib, "gdi32.lib")


// -- TtFont -------------------------------------------------------------
TtFont
TtFont::DEFAULT( 12, true );


TtFont::TtFont( HFONT handle, bool auto_destroy ) :
WindowsHandleHolder( [handle] ( void ) { ::DeleteObject( handle ); }, handle, auto_destroy )
{
}


TtFont::TtFont( int size, bool auto_destroy ) :
TtFont( ::CreateFont(
  size, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
  SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
  CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
  DEFAULT_PITCH | FF_DONTCARE, nullptr ), auto_destroy )
{
  if ( handle_ == nullptr ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::CreateFont ) );
  }
}
