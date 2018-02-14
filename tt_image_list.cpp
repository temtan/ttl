// tt_image_list.cpp

#include "tt_windows_h_include.h"
#include <commctrl.h>
#include <commoncontrols.h>

#include "ttl_define.h"
#include "tt_exception.h"

#include "tt_image_list.h"

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shell32.lib")


// -- TtImageList --------------------------------------------------------
TtImageList
TtImageList::GetSmallSystemImageList( void )
{
  HIMAGELIST handle;
  HRESULT ret = ::SHGetImageList( SHIL_SMALL, IID_IImageList, reinterpret_cast<void**>( &handle ) );
  if ( ret != S_OK ) {
    throw TtWindowsSystemCallException( FUNC_NAME_OF( ::SHGetImageList ), ret, __FILE__, __LINE__ );
  }
  return TtImageList( handle, false );
}


TtImageList::TtImageList( int width, int height, unsigned int flags, int initial, int grow ) :
TtImageList( ::ImageList_Create( width, height, flags, initial, grow ), true )
{
  if ( handle_ == nullptr ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::ImageList_Create ) );
  }
}

TtImageList::TtImageList( HIMAGELIST handle, bool auto_destroy ) :
WindowsHandleHolder( [handle] ( void ) { ::ImageList_Destroy( handle ); }, handle, auto_destroy )
{
}


void
TtImageList::AddIcon( TtIcon& icon )
{
  if ( ImageList_AddIcon( handle_, icon.GetHandle() ) == -1 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( ImageList_AddIcon ) );
  }
}

void
TtImageList::AddBitmap( TtBmpImage& image )
{
  if ( ::ImageList_Add( handle_, image.GetHandle(), nullptr ) == -1 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::ImageList_Add ) );
  }
}

void
TtImageList::AddBitmap( TtBmpImage& image, TtBmpImage& mask )
{
  if ( ::ImageList_Add( handle_, image.GetHandle(), mask.GetHandle() ) == -1 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::ImageList_Add ) );
  }
}

void
TtImageList::AddBitmapMasked( TtBmpImage& image )
{
  this->AddBitmapMasked( image, 0 );
}

void
TtImageList::AddBitmapMasked( TtBmpImage& image, COLORREF mask_color )
{
  if ( ::ImageList_AddMasked( handle_, image.GetHandle(), mask_color ) == -1 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::ImageList_AddMasked ) );
  }
}


int
TtImageList::GetCount( void )
{
  return ImageList_GetImageCount( handle_ );
}


TtIcon
TtImageList::GetIcon( int index, UINT flags )
{
  HICON tmp = ::ImageList_GetIcon( handle_, index, flags );
  if ( tmp == NULL ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::ImageList_GetIcon ) );
  }
  return TtIcon( tmp, true );
}
