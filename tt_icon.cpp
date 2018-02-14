// tt_icon.cpp

#include "ttl_define.h"
#include "tt_exception.h"
#include "tt_device_context.h"

#include "tt_icon.h"

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")


// -- TtIcon -------------------------------------------------------------
unsigned int
TtIcon::GetIconCountOfFile( const std::string& path )
{
  return reinterpret_cast<unsigned int>( ::ExtractIcon( ::GetModuleHandle( nullptr ), path.c_str(), static_cast<UINT>( -1 ) ) );
}


TtIcon
TtIcon::CreateFromID( Standard id )
{
  HICON handle = ::LoadIcon( NULL, MAKEINTRESOURCE( id ) );
  if ( handle == NULL ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::LoadIcon ) );
  }
  return TtIcon( handle, false );
}

TtIcon
TtIcon::CreateFromResourceName( const std::string& name )
{
  HICON handle = ::LoadIcon( ::GetModuleHandle( nullptr ), name.c_str() );
  if ( handle == NULL ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::LoadIcon ) );
  }
  return TtIcon( handle, false );
}

TtIcon
TtIcon::CreateFromResourceID( DWORD id )
{
  HICON handle = ::LoadIcon( ::GetModuleHandle( nullptr ), MAKEINTRESOURCE( id ) );
  if ( handle == NULL ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::LoadIcon ) );
  }
  return TtIcon( handle, false );
}

TtIcon
TtIcon::CreateFromFileAsLarge( const std::string& path, unsigned int index )
{
  HICON handle;
  UINT ret = ::ExtractIconEx( path.c_str(), index, &handle, nullptr, 1 );
  if ( ret == 0 || ret == -1 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::ExtractIconEx ) );
  }
  return TtIcon( handle, true );
}

TtIcon
TtIcon::CreateFromFileAsSmall( const std::string& path, unsigned int index )
{
  HICON handle;
  UINT ret = ::ExtractIconEx( path.c_str(), index, nullptr, &handle, 1 );
  if ( ret == 0 || ret == -1 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::ExtractIconEx ) );
  }
  return TtIcon( handle, true );
}


TtIcon::TtIcon( HICON handle, bool auto_destroy ) :
WindowsHandleHolder( [handle] ( void ) { ::DestroyIcon( handle ); }, handle, auto_destroy )
{
}


TtBmpImage
TtIcon::GetARGBBitmapAsSmall( void )
{
  int width  = ::GetSystemMetrics( SM_CXSMICON );
  int height = ::GetSystemMetrics( SM_CYSMICON );

  // bitmap ÇÃçÏê¨
  LPVOID bits;
  BITMAPINFO bmi = { { sizeof( BITMAPINFOHEADER ), width, height, 1, 32 } };
  HBITMAP bitmap_handle = ::CreateDIBSection( NULL, &bmi, DIB_RGB_COLORS, &bits, NULL, 0 );
  if ( bitmap_handle == NULL ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::CreateDIBSection ) );
  }
  TtBmpImage bitmap( bitmap_handle, true );

  // HDC Ç≈ï`âÊ
  TtDeviceContext dc = TtDeviceContext::CreateDeviceContextForDisplay();
  HGDIOBJ tmp_prev = dc.SelectObject( bitmap_handle );
  if ( ::DrawIconEx( dc.GetHandle(), 0, 0, handle_, width, height, 0, NULL, DI_NORMAL ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::DrawIconEx ) );
  }
  dc.SelectObject( tmp_prev );

  return bitmap;
}


/* é¿ëïâˆÇµÇ¢
TtBmpImage
TtIcon::GetBmp( void )
{
  ICONINFO info;
  if ( ::GetIconInfo( handle_, &info ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::GetIconInfo ) );
  }
  ::DeleteObject( info.hbmMask );
  return TtBmpImage( info.hbmColor );
}

TtBmpImage
TtIcon::GetMaskBmp( void )
{
  ICONINFO info;
  if ( ::GetIconInfo( handle_, &info ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::GetIconInfo ) );
  }
  ::DeleteObject( info.hbmColor );
  return TtBmpImage( info.hbmMask );
}
*/
