// tt_clipboard.cpp

#include <functional>

#include "ttl_define.h"
#include "tt_exception.h"

#include "tt_clipboard.h"

// -- TtClipboard --------------------------------------------------------
namespace {
  bool open_clipboard( std::function<void ( void )> function ) {
    BOOL ret = ::OpenClipboard( nullptr );
    if ( ret == 0 ) {
      if ( ::GetLastError() == ERROR_ACCESS_DENIED ) {
        return false;
      }
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::OpenClipboard ) );
    }
    ret = ::EmptyClipboard();
    if ( ret == 0 ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF(::EmptyClipboard ) );
    }
    function();
    ret = ::CloseClipboard();
    if ( ret == 0 ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF(::EmptyClipboard ) );
    }
    return true;
  }
}


bool
TtClipboard::SetString( const std::string& data )
{
  return open_clipboard( [&data] ( void ) {
    HGLOBAL handle = ::GlobalAlloc( GHND | GMEM_SHARE , data.size() + 1);
    if ( handle == NULL ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::GlobalAlloc ) );
    }
    char* buffer = reinterpret_cast<char*>( ::GlobalLock( handle ) );
    if ( buffer == NULL ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::GlobalLock ) );
    }
    strncpy_s( buffer, data.size() + 1, data.c_str(), data.size() );

    BOOL ret = ::GlobalUnlock( handle );
    if ( ret == 0 && ::GetLastError() != NO_ERROR ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::GlobalUnlock ) );
    }
    HANDLE temp = ::SetClipboardData( CF_TEXT, handle );
    if ( temp == NULL ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::SetClipboardData ) );
    }
  } );
}
