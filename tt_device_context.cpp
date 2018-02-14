// tt_device_context.cpp

#include "ttl_define.h"
#include "tt_exception.h"

#include "tt_device_context.h"

#pragma comment(lib, "gdi32.lib")


// -- TtDeviceContextBase ----------------------------------------------------
TtDeviceContextBase::TtDeviceContextBase( DestroyFunction destroy_function, HDC handle, bool auto_destroy ) :
WindowsHandleHolder( destroy_function, handle, auto_destroy )
{
}


void
TtDeviceContextBase::DrawRectangle( RECT rect )
{
  if ( NOT( ::Rectangle( handle_, rect.left, rect.top, rect.right, rect.bottom ) ) ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::Rectangle ) );
  }
}


TtDeviceContext
TtDeviceContextBase::CreateCompatibleDeviceContext( void )
{
  HDC handle = ::CreateCompatibleDC( handle_ );
  if ( handle == NULL ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::CreateCompatibleDC ) );
  }
  return TtDeviceContext( handle, true );
}


void
TtDeviceContextBase::SetROP2( int mode )
{
  if ( NOT( ::SetROP2( handle_, mode ) ) ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::SetROP2 ) );
  }
}

void
TtDeviceContextBase::SetBkMode( int mode )
{
  if ( NOT( ::SetBkMode( handle_, mode ) ) ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::SetBkMode ) );
  }
}

HGDIOBJ
TtDeviceContextBase::SelectObject( HGDIOBJ object )
{
  HGDIOBJ ret;
  TtUtility::CallWindowsSystemFunctionWithErrorHandling(
    [&] ( void ) {
      ret = ::SelectObject( handle_, object );
    },
    [] ( void ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::SelectObject ) );
    } );
  return ret;
}

void
TtDeviceContextBase::DeleteObject( HGDIOBJ object )
{
  if ( NOT( ::DeleteObject( object ) ) ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::DeleteObject ) );
  }
}


// -- TtDeviceContext ----------------------------------------------------
TtDeviceContext::TtDeviceContext( HDC handle, bool auto_destroy ) :
TtDeviceContextBase( [handle] ( void ) { ::DeleteDC( handle ); }, handle, auto_destroy )
{
}


TtDeviceContext
TtDeviceContext::CreateDeviceContextForDisplay( void )
{
  HDC hdc = ::CreateCompatibleDC( NULL );
  if ( hdc == NULL ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::CreateCompatibleDC ) );
  }
  return TtDeviceContext( hdc, true );
}

// -- TtReleasableDeviceContext ------------------------------------------
TtReleasableDeviceContext::TtReleasableDeviceContext( HDC handle, HWND window_handle, bool auto_destroy ) :
TtDeviceContextBase( [handle, window_handle] ( void ) { ::ReleaseDC( window_handle, handle ); }, handle, auto_destroy ),
window_handle_( window_handle )
{
}
