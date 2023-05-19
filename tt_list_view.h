// tt_list_view.h

#pragma once

#include <vector>

#include "tt_exception.h"
#include "tt_window.h"
#include "tt_window_utility.h"

#pragma warning(push)
#pragma warning(disable : 4365)


class TtListView;

// -- TtListViewColumn ---------------------------------------------------
class TtListViewColumn {
  friend class TtListView;

public:
  static const unsigned int INVALID_INDEX = static_cast<unsigned int>( -1 );

  explicit TtListViewColumn( TtListView* list, unsigned int index );
  void GetInfo( LVCOLUMN& info ) const;
  void SetInfo( LVCOLUMN& info );
  unsigned int GetIndex( void ) const;

  bool IsValid( void );
  bool IsInvalid( void );

  int  GetTextAlign( void ) const;
  void SetTextAlign( int format );

  unsigned int GetWidth( void ) const;
  void SetWidth( unsigned int width );
  void SetWidthAuto( void );
  void SetWidthAutoUseHeader( void );

  std::string GetText( void ) const;
  void SetText( const std::string& text );

  void Remove( void );

  // このファイルの TtListView 宣言の後に実装
  template <class TYPE> TYPE GetParameterAs( void ) const;
  template <class TYPE> void SetParameterAs( TYPE data );

private:
  TtListView*  list_;
  unsigned int index_;
};

// -- TtListViewColumnWith -----------------------------------------------
template <class TYPE>
class TtListViewColumnWith : public TtListViewColumn {
public:
  explicit TtListViewColumnWith( TtListView* list, unsigned int index ) : TtListViewColumn( list, index ) {}
  TtListViewColumnWith( const TtListViewColumn& column ) : TtListViewColumn( column ) {}

  void SetParameter( TYPE parameter ) {
    this->SetParameterAs<TYPE>( parameter );
  }
  TYPE GetParameter( void ) {
    return this->GetParameterAs<TYPE>();
  }
};

// -- TtListViewItem -----------------------------------------------------
class TtListViewItem {
  friend class TtListView;

public:
  static const unsigned int INVALID_INDEX = static_cast<unsigned int>( -1 );

  explicit TtListViewItem( TtListView* list, int index );
  void GetInfo( LVITEM& info ) const;
  void SetInfo( LVITEM& info );
  int GetIndex( void ) const;

  bool IsValid( void );
  bool IsInvalid( void );

  bool IsChecked( void ) const;
  void SetChecked( bool flag );

  bool IsFocused( void ) const;
  void SetFocused( bool flag );

  bool IsDropHilighted( void ) const;
  void SetDropHilight( bool flag );

  bool IsSelected( void ) const;
  void SetSelected( bool flag );
  void SetSelectionMark( void );

  std::string GetText( void ) const;
  void SetText( const std::string& text );

  std::string GetSubItemText( unsigned int index ) const;
  void SetSubItemText( unsigned int index, const std::string& text );

  unsigned int GetImageIndex( void ) const;
  void SetImageIndex( unsigned int index );

  void BeginEdit( void );
  void Remove( void );

  template <class TYPE>
  TYPE GetParameterAs( void ) const {
    LVITEM tmp = {LVIF_PARAM};
    tmp.iItem = index_;
    this->GetInfo( tmp );
    return reinterpret_cast<TYPE>( tmp.lParam );
  }
  template <class TYPE>
  void SetParameterAs( TYPE data ) {
    LVITEM tmp = {LVIF_PARAM};
    tmp.iItem = index_;
    tmp.lParam = reinterpret_cast<LPARAM>( data );
    this->SetInfo( tmp );
  }

  class DragHandler;
  DragHandler MakeDragImage( void );

private:
  TtListView*  list_;
  int          index_;
};

// -- DragHandler --------------------------------------------------------
class TtListViewItem::DragHandler : public TtDragHandler {
public:
  explicit DragHandler( void );
  explicit DragHandler( HIMAGELIST handle, TtListViewItem& item, POINT start_point );
  TtListViewItem GetItem( void );
  POINT          GetStartPoint( void );

  void SetDropHilightedItem( TtListViewItem& item );
  TtListViewItem GetDropHilightedItem( void );
  void CancelDropHilighted( void );

private:
  TtListViewItem item_;
  POINT          start_point_;
  TtListViewItem drop_hilighted_item_;
};


// -- TtListViewItemWith -----------------------------------------------------
template <class TYPE>
class TtListViewItemWith : public TtListViewItem {
public:
  explicit TtListViewItemWith( TtListView* list, unsigned int index ) : TtListViewItem( list, index ) {}
  TtListViewItemWith( const TtListViewItem& item ) : TtListViewItem( item ) {}

  void SetParameter( TYPE parameter ) {
    this->SetParameterAs<TYPE>( parameter );
  }
  TYPE GetParameter( void ) {
    return this->GetParameterAs<TYPE>();
  }
};


// -- TtListView ---------------------------------------------------------
class TtListView : public BasedOnTtWindow<WS_TABSTOP | LVS_SHAREIMAGELISTS,  WS_EX_CLIENTEDGE, TtWindowClassName::ListView> {
public:
  explicit TtListView( void );

private:
  bool GetStyleD( int style );
  void SetStyleD( int style, bool flag );
  bool GetExtendedStyleD( int style );
  void SetExtendedStyleD( int style, bool flag );

public:
  int GetView( void );
  void SetView( int view );

  bool GetHasCheckBox( void );
  void SetHasCheckBox( bool flag );

  bool GetHasGridLines( void );
  void SetHasGridLines( bool flag );

  bool GetLabelEdit( void );
  void SetLabelEdit( bool flag );

  bool GetScrollable( void );
  void SetScrollable( bool flag );

  bool GetMultiselect( void );
  void SetMultiselect( bool flag );

  bool GetFullRowSelect( void );
  void SetFullRowSelect( bool flag );

  bool GetHeaderDragAndDrop( void );
  void SetHeaderDragAndDrop( bool flag );

  bool GetHideSelection( void );
  void SetHideSelection( bool flag );

  bool GetSortAscending( void );
  void SetSortAscending( bool flag );

  bool GetSortDescending( void );
  void SetSortDescending( bool flag );

  TtImageList GetSmallImageList( void );
  void SetSmallImageList( TtImageList& image_list );

  TtImageList GetLargeImageList( void );
  void SetLargeImageList( TtImageList& image_list );

  unsigned int GetColumnCount( void );
  TtListViewColumn MakeNewColumn( void );
  TtListViewColumn InsertNewColumn( unsigned int index );
  TtListViewColumn GetColumn( unsigned int index );

  unsigned int GetItemCount( void );
  TtListViewItem MakeNewItem( void );
  TtListViewItem InsertNewItem( unsigned int index );
  TtListViewItem GetItem( unsigned int index );

  std::vector<unsigned int>   GetCheckedIndices( void );
  std::vector<TtListViewItem> GetCheckedItems( void );

  std::vector<unsigned int>   GetFocusedIndices( void );
  std::vector<TtListViewItem> GetFocusedItems( void );

  unsigned int GetSelectedCount( void );
  std::vector<unsigned int>   GetSelectedIndices( void );
  std::vector<TtListViewItem> GetSelectedItems( void );

  TtListViewItem HitTest( int x, int y );
  TtListViewItem GetDropHilight( int start = -1 );
  TtListViewItem GetSelected( int start = -1 );

  class ColumHitTestResult {
  public:
    enum class Type {
      None,
      OnControl,
      OnHeader,
      OnDivider,
      OnDividerOpen,
    } type_;
    TtListViewColumn column_;
    ColumHitTestResult( Type type, TtListViewColumn column ) : type_( type ), column_( column ) {}
  };

  ColumHitTestResult HitTestColumn( int x, int y );

  void Arrange( int format );

  void LockDraw( std::function<void ( void )> function );
  void LockDraw( void );
  void UnlockDraw( void );

  void EnsureVisible( unsigned int index );

private:
  static int CALLBACK CompareFunctionForTTL( LPARAM x, LPARAM y, LPARAM parameter );
public:
  using CompareFunction = std::function<int ( LPARAM x, LPARAM y )>;
  void SortPrimitive( CompareFunction function );

  template<class TYPE>
  void Sort( std::function<int ( TYPE x, TYPE y )> function, bool ascending ) {
    this->SortPrimitive( [function, ascending] ( LPARAM x, LPARAM y ) -> int {
      return (ascending ? 1 : -1) * function( reinterpret_cast<TYPE>( x ), reinterpret_cast<TYPE>( y ) );
    } );
  }

  void ClearItems( void );
  void ClearColumns( void );
};

using TtListViewIcon      = TtWindowWithStyle<TtListView, LVS_ICON>;
using TtListViewSmallIcon = TtWindowWithStyle<TtListView, LVS_SMALLICON>;
using TtListViewList      = TtWindowWithStyle<TtListView, LVS_LIST>;
using TtListViewReport    = TtWindowWithStyle<TtListView, LVS_REPORT>;



// -- TtListViewColumn ---------------------------------------------------
// 一部テンプレート関数の実装

template <class TYPE>
TYPE
TtListViewColumn::GetParameterAs( void ) const
{
  HDITEM item = {HDI_LPARAM};
  if ( Header_GetItem( ListView_GetHeader( list_->GetHandle() ), index_, &item ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( Header_GetItem ) );
  }
  return reinterpret_cast<TYPE>( item.lParam );
}

template <class TYPE>
void
TtListViewColumn::SetParameterAs( TYPE data )
{
  HDITEM item = {HDI_LPARAM};
  item.lParam = reinterpret_cast<LPARAM>( data );
  if ( Header_SetItem( ListView_GetHeader( list_->GetHandle() ), index_, &item ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( Header_SetItem ) );
  }
}
