// tt_list_view.cpp

#include "ttl_define.h"
#include "tt_exception.h"

#include "tt_list_view.h"

#pragma warning(push)
#pragma warning(disable : 4365)
#pragma warning(disable : 4800)


// -- TtListViewColumn ---------------------------------------------------
TtListViewColumn::TtListViewColumn( TtListView* list, unsigned int index ) :
list_( list ),
index_( index )
{
}

void
TtListViewColumn::GetInfo( LVCOLUMN& info ) const
{
  if ( ListView_GetColumn( list_->GetHandle(), index_, &info ) == FALSE ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( ListView_GetColumn ) );
  }
}

void
TtListViewColumn::SetInfo( LVCOLUMN& info )
{
  if ( ListView_SetColumn( list_->GetHandle(), index_, &info ) == FALSE ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( ListView_SetColumn ) );
  }
}

unsigned int
TtListViewColumn::GetIndex( void ) const
{
  return index_;
}


bool
TtListViewColumn::IsValid( void )
{
  return index_ != INVALID_INDEX;
}

bool
TtListViewColumn::IsInvalid( void )
{
  return NOT( this->IsValid() );
}


int
TtListViewColumn::GetTextAlign( void ) const
{
  LVCOLUMN tmp = {LVCF_FMT};
  tmp.iSubItem = index_;
  this->GetInfo( tmp );
  return tmp.fmt;
}

void
TtListViewColumn::SetTextAlign( int format )
{
  LVCOLUMN tmp = {LVCF_FMT};
  tmp.iSubItem = index_;
  tmp.fmt = format;
  this->SetInfo( tmp );
}


unsigned int
TtListViewColumn::GetWidth( void ) const
{
  unsigned int ret = ListView_GetColumnWidth( list_->GetHandle(), index_ );
  if ( ret == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( ListView_GetColumnWidth ) );
  }
  return ret;
}

void
TtListViewColumn::SetWidth( unsigned int width )
{
  if ( ListView_SetColumnWidth( list_->GetHandle(), index_, width ) == FALSE ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( ListView_SetColumnWidth ) );
  }
}

void
TtListViewColumn::SetWidthAuto( void )
{
  this->SetWidth( LVSCW_AUTOSIZE );
}

void
TtListViewColumn::SetWidthAutoUseHeader( void )
{
  this->SetWidth( LVSCW_AUTOSIZE_USEHEADER );
}

std::string
TtListViewColumn::GetText( void ) const
{
  LVCOLUMN tmp = {LVCF_TEXT | LVCF_SUBITEM};
  tmp.iSubItem = index_;
  char buf[1024 * 8];
  tmp.pszText = buf;
  tmp.cchTextMax = sizeof( buf );
  this->GetInfo( tmp );
  return std::string( tmp.pszText );
}

void
TtListViewColumn::SetText( const std::string& text )
{
  LVCOLUMN tmp = {LVCF_TEXT | LVCF_SUBITEM};
  tmp.iSubItem = index_;
  tmp.pszText = const_cast<char*>( text.c_str() );
  tmp.cchTextMax = static_cast<int>( text.size() );
  this->SetInfo( tmp );
}


void
TtListViewColumn::Remove( void )
{
  if ( ListView_DeleteColumn( list_->GetHandle(), index_ ) == FALSE ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( ListView_DeleteColumn ) );
  }
}


// -- TtListViewItem -----------------------------------------------------
TtListViewItem::TtListViewItem( TtListView* list, int index ) :
list_( list ),
index_( index )
{
}


void
TtListViewItem::GetInfo( LVITEM& info ) const
{
  if ( ListView_GetItem( list_->GetHandle(), &info ) == FALSE ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( ListView_GetItem ) );
  }
}

void
TtListViewItem::SetInfo( LVITEM& info )
{
  if ( ListView_SetItem( list_->GetHandle(), &info ) == FALSE ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( ListView_SetItem ) );
  }
}

int
TtListViewItem::GetIndex( void ) const
{
  return index_;
}


bool
TtListViewItem::IsValid( void )
{
  return index_ != INVALID_INDEX;
}

bool
TtListViewItem::IsInvalid( void )
{
  return NOT( this->IsValid() );
}


bool
TtListViewItem::IsChecked( void ) const
{
  return ListView_GetCheckState( list_->GetHandle(), index_ );
}

void
TtListViewItem::SetChecked( bool flag )
{
  ListView_SetCheckState( list_->GetHandle(), index_, flag );
}

bool
TtListViewItem::IsFocused( void ) const
{
  return ListView_GetItemState( list_->GetHandle(), index_, LVIS_FOCUSED );
}

void
TtListViewItem::SetFocused( bool flag )
{
  ListView_SetItemState( list_->GetHandle(), index_, flag ? LVIS_FOCUSED : 0, LVIS_FOCUSED );
}

bool
TtListViewItem::IsDropHilighted( void ) const
{
  return ListView_GetItemState( list_->GetHandle(), index_, LVIS_DROPHILITED );
}

void
TtListViewItem::SetDropHilight( bool flag )
{
  ListView_SetItemState( list_->GetHandle(), index_, flag ? LVIS_DROPHILITED : 0, LVIS_DROPHILITED );
}

bool
TtListViewItem::IsSelected( void ) const
{
  return ListView_GetItemState( list_->GetHandle(), index_, LVIS_SELECTED );
}

void
TtListViewItem::SetSelected( bool flag )
{
  ListView_SetItemState( list_->GetHandle(), index_, flag ? LVIS_SELECTED : 0, LVIS_SELECTED );
}

void
TtListViewItem::SetSelectionMark( void )
{
  ListView_SetSelectionMark( list_->GetHandle(), index_ );
}

std::string
TtListViewItem::GetText( void ) const
{
  return this->GetSubItemText( 0 );
}

void
TtListViewItem::SetText( const std::string& text )
{
  this->SetSubItemText( 0, text );
}

std::string
TtListViewItem::GetSubItemText( unsigned int index ) const
{
  char buf[1024 * 8];
  ListView_GetItemText( list_->GetHandle(), index_, index, buf, sizeof( buf ) );
  return std::string( buf );
}

void
TtListViewItem::SetSubItemText( unsigned int index, const std::string& text )
{
  ListView_SetItemText( list_->GetHandle(), index_, index, const_cast<char*>( text.c_str() ) );
}


unsigned int
TtListViewItem::GetImageIndex( void ) const
{
  LVITEM tmp = {LVIF_IMAGE};
  tmp.iItem = index_;
  this->GetInfo( tmp );
  return tmp.iImage;
}

void
TtListViewItem::SetImageIndex( unsigned int index )
{
  LVITEM tmp = {LVIF_IMAGE};
  tmp.iItem = index_;
  tmp.iImage = index;
  this->SetInfo( tmp );
}

void
TtListViewItem::BeginEdit( void )
{
  this->SetFocused( true );
  HWND tmp = ListView_EditLabel( list_->GetHandle(), index_ );
  if ( tmp == NULL ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( ListView_EditLabel ) );
  }
  // tmp テキストエディットコントロールなので何かするのかな？
}

void
TtListViewItem::Remove( void )
{
  if ( ListView_DeleteItem( list_->GetHandle(), index_ ) == FALSE ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( ListView_DeleteItem ) );
  }
}


TtListViewItem::DragHandler
TtListViewItem::MakeDragImage( void )
{
  POINT tmp;
  HIMAGELIST image_list_handle = ListView_CreateDragImage( list_->GetHandle(), index_, &tmp );
  if ( image_list_handle == NULL ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( ListView_CreateDragImage ) );
  }
  return DragHandler( image_list_handle, *this, tmp );
}

// -- DragHandler --------------------------------------------------------
TtListViewItem::DragHandler::DragHandler( void ) :
TtDragHandler(),
item_( nullptr, INVALID_INDEX ),
start_point_(),
drop_hilighted_item_( nullptr, INVALID_INDEX )
{
}

TtListViewItem::DragHandler::DragHandler( HIMAGELIST handle, TtListViewItem& item, POINT start_point ) :
TtDragHandler( handle ),
item_( item ),
start_point_( start_point ),
drop_hilighted_item_( nullptr, INVALID_INDEX )
{
}

TtListViewItem
TtListViewItem::DragHandler::GetItem( void )
{
  return item_;
}

POINT
TtListViewItem::DragHandler::GetStartPoint( void )
{
  return start_point_;
}

void
TtListViewItem::DragHandler::SetDropHilightedItem( TtListViewItem& item )
{
  drop_hilighted_item_ = item;
}

TtListViewItem
TtListViewItem::DragHandler::GetDropHilightedItem( void )
{
  return drop_hilighted_item_;
}

void
TtListViewItem::DragHandler::CancelDropHilighted( void )
{
  if ( drop_hilighted_item_.IsValid() ) {
    drop_hilighted_item_.SetDropHilight( false );
  }
}


// -- TtListView ---------------------------------------------------------
TtListView::TtListView( void )
{
}


bool
TtListView::GetStyleD( int style )
{
  return this->GetWindowLongPtr( GWL_STYLE ) & style;
}

void
TtListView::SetStyleD( int style, bool flag )
{
  this->SetWindowLongPtr( GWL_STYLE, this->GetWindowLongPtr( GWL_STYLE ) ^ (flag ? style : 0) );
}

bool
TtListView::GetExtendedStyleD( int style )
{
  return ListView_GetExtendedListViewStyle( handle_ ) & style;
}

void
TtListView::SetExtendedStyleD( int style, bool flag )
{
  ListView_SetExtendedListViewStyle( handle_, ListView_GetExtendedListViewStyle( handle_ ) ^ (flag ? style : 0) );
}


int
TtListView::GetView( void )
{
  return this->GetWindowLongPtr( GWL_STYLE ) & LVS_TYPEMASK;
}

void
TtListView::SetView( int view )
{
  this->SetWindowLongPtr( GWL_STYLE, this->GetWindowLongPtr( GWL_STYLE ) ^ (view & LVS_TYPEMASK) );
}

bool
TtListView::GetHasCheckBox( void )
{
  return this->GetExtendedStyleD( LVS_EX_CHECKBOXES );
}

void
TtListView::SetHasCheckBox( bool flag )
{
  this->SetExtendedStyleD( LVS_EX_CHECKBOXES, flag );
}

bool
TtListView::GetHasGridLines( void )
{
  return this->GetExtendedStyleD( LVS_EX_GRIDLINES );
}

void
TtListView::SetHasGridLines( bool flag )
{
  this->SetExtendedStyleD( LVS_EX_GRIDLINES, flag );
}

bool
TtListView::GetLabelEdit( void )
{
  return this->GetStyleD( LVS_EDITLABELS );
}

void
TtListView::SetLabelEdit( bool flag )
{
  this->SetStyleD( LVS_EDITLABELS, flag );
}

bool
TtListView::GetScrollable( void )
{
  return NOT( this->GetStyleD( LVS_NOSCROLL ) );
}

void
TtListView::SetScrollable( bool flag )
{
  this->SetStyleD( LVS_EDITLABELS, NOT( flag ) );
}

bool
TtListView::GetMultiselect( void )
{
  return NOT( this->GetStyleD( LVS_SINGLESEL ) );
}

void
TtListView::SetMultiselect( bool flag )
{
  this->SetStyleD( LVS_SINGLESEL, NOT( flag ) );
}

bool
TtListView::GetFullRowSelect( void )
{
  return this->GetExtendedStyleD( LVS_EX_FULLROWSELECT );
}

void
TtListView::SetFullRowSelect( bool flag )
{
  this->SetExtendedStyleD( LVS_EX_FULLROWSELECT, flag );
}

bool
TtListView::GetHeaderDragAndDrop( void )
{
  return this->GetExtendedStyleD( LVS_EX_HEADERDRAGDROP );
}

void
TtListView::SetHeaderDragAndDrop( bool flag )
{
  this->SetExtendedStyleD( LVS_EX_HEADERDRAGDROP, flag );
}

bool
TtListView::GetHideSelection( void )
{
  return NOT( this->GetStyleD( LVS_SHOWSELALWAYS ) );
}

void
TtListView::SetHideSelection( bool flag )
{
  this->SetStyleD( LVS_SHOWSELALWAYS, NOT( flag ) );
}

bool
TtListView::GetSortAscending( void )
{
  return this->GetStyleD( LVS_SORTASCENDING );
}

void
TtListView::SetSortAscending( bool flag )
{
  this->SetStyleD( LVS_SORTASCENDING, flag );
}

bool
TtListView::GetSortDescending( void )
{
  return this->GetStyleD( LVS_SORTDESCENDING );
}

void
TtListView::SetSortDescending( bool flag )
{
  this->SetStyleD( LVS_SORTDESCENDING, flag );
}


TtImageList
TtListView::GetSmallImageList( void )
{
  HIMAGELIST tmp = ListView_GetImageList( handle_, LVSIL_SMALL );
  if ( tmp == NULL ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( ListView_GetImageList ) );
  }
  return TtImageList( tmp, false );
}

void
TtListView::SetSmallImageList( TtImageList& image_list )
{
  TtUtility::CallWindowsSystemFunctionWithErrorHandling(
    [&] ( void ) {
      ListView_SetImageList( handle_, image_list.GetHandle(), LVSIL_SMALL );
    },
    [] ( void ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( ListView_SetImageList ) );
    } );
}

TtImageList
TtListView::GetLargeImageList( void )
{
  HIMAGELIST tmp = ListView_GetImageList( handle_, LVSIL_NORMAL );
  if ( tmp == NULL ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( ListView_GetImageList ) );
  }
  return TtImageList( tmp, false );
}

void
TtListView::SetLargeImageList( TtImageList& image_list )
{
  TtUtility::CallWindowsSystemFunctionWithErrorHandling(
    [&] ( void ) {
      ListView_SetImageList( handle_, image_list.GetHandle(), LVSIL_NORMAL );
    },
    [] ( void ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( ListView_SetImageList ) );
    } );
}

unsigned int
TtListView::GetColumnCount( void )
{
  int ret = Header_GetItemCount( ListView_GetHeader( handle_ ) );
  if ( ret == -1 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( Header_GetItemCount ) );
  }
  return ret;
}

TtListViewColumn
TtListView::MakeNewColumn( void )
{
  return this->InsertNewColumn( this->GetColumnCount() );
}

TtListViewColumn
TtListView::InsertNewColumn( unsigned int index )
{
  LVCOLUMN tmp = {LVCF_TEXT | LVCF_SUBITEM};
  tmp.iSubItem = index;
  char empty_text[] = "";
  tmp.pszText = empty_text;
  int ret = ListView_InsertColumn( handle_, tmp.iSubItem, &tmp );
  if ( ret == -1 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( ListView_InsertColumn ) );
  }
  return TtListViewColumn( this, ret );
}

TtListViewColumn
TtListView::GetColumn( unsigned int index )
{
  return TtListViewColumn( this, index );
}

unsigned int
TtListView::GetItemCount( void )
{
  return ListView_GetItemCount( handle_ );
}

TtListViewItem
TtListView::MakeNewItem( void )
{
  return this->InsertNewItem( this->GetItemCount() );
}


TtListViewItem
TtListView::InsertNewItem( unsigned int index )
{
  LVITEM tmp = {0};
  tmp.iItem = index;
  int ret = ListView_InsertItem( handle_, &tmp );
  if ( ret == -1 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( ListView_InsertItem ) );
  }
  return TtListViewItem( this, ret );
}


TtListViewItem
TtListView::GetItem( unsigned int index )
{
  return TtListViewItem( this, index );
}

std::vector<unsigned int>
TtListView::GetCheckedIndices( void )
{
  int count = this->GetItemCount();
  std::vector<unsigned int> ary;
  for ( int i = 0; i < count; ++i ) {
    if ( ListView_GetCheckState( handle_, i ) ) {
      ary.push_back( i );
    }
  }
  return ary;
}

std::vector<TtListViewItem>
TtListView::GetCheckedItems( void )
{
  int count = this->GetItemCount();
  std::vector<TtListViewItem> ary;
  for ( int i = 0; i < count; ++i ) {
    if ( ListView_GetCheckState( handle_, i ) ) {
      ary.push_back( TtListViewItem( this, i ) );
    }
  }
  return ary;
}

std::vector<unsigned int>
TtListView::GetFocusedIndices( void )
{
  int count = this->GetItemCount();
  std::vector<unsigned int> ary;
  for ( int i = 0; i < count; ++i ) {
    if ( ListView_GetItemState( handle_, i, LVIS_FOCUSED ) ) {
      ary.push_back( i );
    }
  }
  return ary;
}

std::vector<TtListViewItem>
TtListView::GetFocusedItems( void )
{
  int count = this->GetItemCount();
  std::vector<TtListViewItem> ary;
  for ( int i = 0; i < count; ++i ) {
    if ( ListView_GetItemState( handle_, i, LVIS_FOCUSED ) ) {
      ary.push_back( TtListViewItem( this, i ) );
    }
  }
  return ary;
}

unsigned int
TtListView::GetSelectedCount( void )
{
  return ListView_GetSelectedCount( handle_ );
}

std::vector<unsigned int>
TtListView::GetSelectedIndices( void )
{
  int count = this->GetItemCount();
  std::vector<unsigned int> ary;
  for ( int i = 0; i < count; ++i ) {
    if ( ListView_GetItemState( handle_, i, LVIS_SELECTED ) ) {
      ary.push_back( i );
    }
  }
  return ary;
}

std::vector<TtListViewItem>
TtListView::GetSelectedItems( void )
{
  int count = this->GetItemCount();
  std::vector<TtListViewItem> ary;
  for ( int i = 0; i < count; ++i ) {
    if ( ListView_GetItemState( handle_, i, LVIS_SELECTED ) ) {
      ary.push_back( TtListViewItem( this, i ) );
    }
  }
  return ary;
}


TtListViewItem
TtListView::HitTest( int x, int y )
{
  LV_HITTESTINFO info;
  info.pt.x = x;
  info.pt.y = y;

  int ret = ListView_HitTest( handle_, &info );
  return TtListViewItem( this, ret != -1 ? info.iItem : TtListViewItem::INVALID_INDEX );
}

TtListViewItem
TtListView::GetDropHilight( int start )
{
  int ret = ListView_GetNextItem( handle_, start, LVNI_DROPHILITED );
  return TtListViewItem( this, ret != -1 ? ret : TtListViewItem::INVALID_INDEX );
}

TtListViewItem
TtListView::GetSelected( int start )
{
  int ret = ListView_GetNextItem( handle_, start, LVNI_SELECTED );
  return TtListViewItem( this, ret != -1 ? ret : TtListViewItem::INVALID_INDEX );
}


TtListView::ColumHitTestResult
TtListView::HitTestColumn( int x, int y )
{
  HDHITTESTINFO info = {{x, y}};
  LRESULT ret = ::SendMessage( ListView_GetHeader( handle_ ), HDM_HITTEST, 0, reinterpret_cast<LPARAM>( &info ) );
  return {
    (info.flags & HHT_NOWHERE)   ? ColumHitTestResult::Type::OnControl :
    (info.flags & HHT_ONHEADER)  ? ColumHitTestResult::Type::OnHeader :
    (info.flags & HHT_ONDIVIDER) ? ColumHitTestResult::Type::OnDivider :
    (info.flags & HHT_ONDIVOPEN) ? ColumHitTestResult::Type::OnDividerOpen :
    ColumHitTestResult::Type::None,
    TtListViewColumn( this, ret != -1 ? static_cast<unsigned int>( ret ) : TtListViewColumn::INVALID_INDEX ) };
}


void
TtListView::Arrange( int format )
{
  if ( ListView_Arrange( handle_, format ) == FALSE ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( ListView_Arrange ) );
  }
}

void
TtListView::LockDraw( std::function<void ( void )> function )
{
  this->LockDraw();
  function();
  this->UnlockDraw();
}

void
TtListView::LockDraw( void )
{
  this->SendMessage( WM_SETREDRAW, FALSE );
}

void
TtListView::UnlockDraw( void )
{
  this->SendMessage( WM_SETREDRAW, TRUE );
}

void
TtListView::EnsureVisible( unsigned int index )
{
  if ( ListView_EnsureVisible( handle_, index, FALSE ) == FALSE ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( ListView_EnsureVisible ) );
  }
}


int CALLBACK
TtListView::CompareFunctionForTTL( LPARAM x, LPARAM y, LPARAM parameter )
{
  return (*reinterpret_cast<CompareFunction*>( parameter ))( x, y );
}

void
TtListView::SortPrimitive( CompareFunction compare )
{
  if ( ListView_SortItems( handle_, TtListView::CompareFunctionForTTL, &compare ) == FALSE ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( ListView_SortItems ) );
  }
}



void
TtListView::ClearItems( void )
{
  if ( ListView_DeleteAllItems( handle_ ) == FALSE ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( ListView_DeleteAllItems ) );
  }
}

void
TtListView::ClearColumns( void )
{
  while ( this->GetColumnCount() != 0 ) {
    this->GetColumn( 0 ).Remove();
  }
}


#ifdef TT_MAKE_TEMPLATE_INSTANCE_
template void* TtListViewColumn::GetParameterAs<void*>( void ) const;
template void  TtListViewColumn::SetParameterAs<void*>( void* );
template void* TtListViewItem::GetParameterAs<void*>( void ) const;
template void  TtListViewItem::SetParameterAs<void*>( void* );
#endif
