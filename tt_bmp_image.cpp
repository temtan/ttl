// tt_bmp_image.cpp

#include <stdio.h>
#include <errno.h>

#include "tt_windows_h_include.h"

#include "ttl_define.h"
#include "tt_exception.h"
#include "tt_string.h"
#include "tt_device_context.h"

#include "tt_bmp_image.h"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")


// -- TtBmpImage ---------------------------------------------------------
TtBmpImage::TtBmpImage( Standard id ) :
TtBmpImage( ::LoadBitmap( nullptr, (const char *)MAKEINTRESOURCE( id ) ), true )
{
  if ( handle_ == nullptr ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::LoadBitmap ) );
  }
}


TtBmpImage::TtBmpImage( HBITMAP handle, bool auto_destroy ) :
WindowsHandleHolder( [handle] ( void ) { ::DeleteObject( handle ); }, handle, auto_destroy )
{
}


TtBmpImage
TtBmpImage::CreateFromFile( const std::string& path )
{
  HBITMAP handle = static_cast<HBITMAP>( ::LoadImage( ::GetModuleHandle( nullptr ), path.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION ) );
  if ( handle == NULL ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::LoadImage ) );
  }
  return TtBmpImage( handle, true );
}

TtBmpImage
TtBmpImage::CreateFromResourceName( const std::string& name )
{
  HBITMAP handle = ::LoadBitmap( ::GetModuleHandle( nullptr ), name.c_str() );
  if ( handle == NULL ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::LoadBitmap ) );
  }
  return TtBmpImage( handle, true );
}


int
TtBmpImage::GetWidth( void ) const
{
  BITMAP tmp;
  if ( ::GetObject( handle_, sizeof( BITMAP ), &tmp ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::GetObject ) );
  }
  return tmp.bmWidth;
}

int
TtBmpImage::GetHeight( void ) const
{
  BITMAP tmp;
  if ( ::GetObject( handle_, sizeof( BITMAP ), &tmp ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::GetObject ) );
  }
  return tmp.bmHeight;
}


TtBmpImage
TtBmpImage::ConvertToARGBBitmap( void )
{
  // bitmap ‚Ìì¬
  LPVOID bits;
  BITMAPINFO bmi = { { sizeof( BITMAPINFOHEADER ), this->GetWidth(), this->GetHeight(), 1, 32 } };
  HBITMAP bitmap_handle = ::CreateDIBSection( NULL, &bmi, DIB_RGB_COLORS, &bits, NULL, 0 );
  if ( bitmap_handle == NULL ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::CreateDIBSection ) );
  }
  TtBmpImage bitmap( bitmap_handle, true );

  // HDC ‚Å•`‰æ
  TtDeviceContext dc_to = TtDeviceContext::CreateDeviceContextForDisplay();
  HGDIOBJ tmp_prev = dc_to.SelectObject( bitmap_handle );

  TtDeviceContext dc_from = dc_to.CreateCompatibleDeviceContext();
  dc_from.SelectObject( handle_ );

  auto ret = ::BitBlt( dc_to.GetHandle(), 0, 0, this->GetWidth(), this->GetHeight(), dc_from.GetHandle(), 0, 0, SRCCOPY );
  if ( ret == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::BitBlt ) );
  }
  dc_to.SelectObject( tmp_prev );
  return bitmap;
}
