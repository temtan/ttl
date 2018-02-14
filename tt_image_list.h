// tt_image_list.h

#pragma once

#include "tt_windows_h_include.h"
#include <commctrl.h>

#include "tt_bmp_image.h"
#include "tt_icon.h"
#include "tt_utility.h"

// -- TtImageList --------------------------------------------------------
class TtImageList : public TtUtility::WindowsHandleHolder<HIMAGELIST> {
public:
  static TtImageList GetSmallSystemImageList( void );

  explicit TtImageList( int width, int height, unsigned int flags = ILC_COLOR32 | ILC_MASK, int initial = 8, int grow = 1 );
  explicit TtImageList( HIMAGELIST handle, bool auto_destroy );

  void AddIcon( TtIcon& icon );
  void AddBitmap( TtBmpImage& image );
  void AddBitmap( TtBmpImage& image, TtBmpImage& mask );
  void AddBitmapMasked( TtBmpImage& image );
  void AddBitmapMasked( TtBmpImage& image, COLORREF mask_color );

  int GetCount( void );

  TtIcon GetIcon( int index, UINT flags = ILD_NORMAL );
};
