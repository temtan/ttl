// tt_tree_view.h

#pragma once

#include "tt_window.h"
#include "tt_window_utility.h"


class TtTreeView;

// -- TtTreeItem ---------------------------------------------------------
class TtTreeItem {
  friend class TtTreeView;
public:
  static const HTREEITEM INVALID_HANDLE;

  explicit TtTreeItem( TtTreeView* tree, HTREEITEM handle );
  void GetInfo( TVITEM& info );
  void SetInfo( TVITEM& info );
  HTREEITEM GetHandle( void );

  bool IsOrigin( void );
  bool IsValid( void );
  bool IsInvalid( void );
  bool IsExpanded( void );
  bool HasChild( void );
  bool IsDescendantOf( TtTreeItem& other );
  bool IsAncestorOf( TtTreeItem& other );

  bool operator ==( const TtTreeItem& other ) const;
  bool operator !=( const TtTreeItem& other ) const;

  TtTreeItem GetParent( void );
  TtTreeItem GetFirstChild( void );
  TtTreeItem GetNextSibling( void );
  TtTreeItem GetPrevSibling( void );

  class ChildEnumerable;
  ChildEnumerable GetChildEnumerable( void );

private:
  TtTreeItem AddChild( HTREEITEM prev, const std::string& text );
public:
  TtTreeItem AddChildPrevious( TtTreeItem& after, const std::string& text = "" );
  TtTreeItem AddChildAfter( TtTreeItem& prev, const std::string& text = "" );
  TtTreeItem AddChildFirst( const std::string& text = "" );
  TtTreeItem AddChildLast( const std::string& text = "" );
  TtTreeItem AddChildSorted( const std::string& text = "" );

private:
  TtTreeItem MoveTo( TtTreeItem& parent, HTREEITEM prev );
public:
  TtTreeItem MoveToPrevious( TtTreeItem& parent, TtTreeItem& prev );
  TtTreeItem MoveToAfter( TtTreeItem& parent, TtTreeItem& prev );
  TtTreeItem MoveToFirst( TtTreeItem& parent );
  TtTreeItem MoveToLast( TtTreeItem& parent );
  TtTreeItem MoveToSorted( TtTreeItem& parent );

  void Remove( void );

  void SortChildren( void );
  // âΩåÃÇ©çƒãAìIÇ…Ç‚Ç¡ÇƒÇ≠ÇÍÇ»Ç¢ÇÃÇ≈îpé~
  // void SortChildrenRecursive( void );

  std::string GetText( void );
  void SetText( const std::string& str );

  struct Rectangle {
    bool is_visible;
    RECT rectangle;
  };
  Rectangle GetRectangle( void );
  Rectangle GetTextRectangle( void );

  void Expand( void );
  void Collapse( void );
  void ToggleExpand( void );

  void SetEditMode( void );
  void SetSelect( void );

  void SetSelectAsDropTarget( void );
  bool IsDropHilighted( void );

  void SetImageIndex( int index );
  void SetSelectedImageIndex( int index );
  void SetBothImageIndex( int index );

  template <class TYPE>
  void SetParameterAs( TYPE parameter ) {
    TVITEM tmp = {TVIF_PARAM};
    tmp.lParam = reinterpret_cast<LPARAM>( reinterpret_cast<void*>( parameter ) );
    tmp.hItem = handle_;
    this->SetInfo( tmp );
  }
  template <class TYPE>
  TYPE GetParameterAs( void ) {
    TVITEM tmp = {TVIF_PARAM};
    tmp.hItem = handle_;
    this->GetInfo( tmp );
    return reinterpret_cast<TYPE>( reinterpret_cast<void*>( tmp.lParam ) );
  }

  class DragHandler;
  DragHandler MakeDragImage( void );

private:
  HTREEITEM   handle_;
  TtTreeView* tree_;
};

// -- DragHandler --------------------------------------------------------
class TtTreeItem::DragHandler : public TtDragHandler {
public:
  explicit DragHandler( void );
  explicit DragHandler( HIMAGELIST handle, TtTreeItem& item );
  TtTreeItem GetItem( void );
private:
  TtTreeItem item_;
};

// -- ChildEnumerable ----------------------------------------------------
class TtTreeItem::ChildEnumerable {
public:
  explicit ChildEnumerable( TtTreeItem& parent );
  class Iterator {
    friend ChildEnumerable;
  private:
    explicit Iterator( TtTreeItem&& current );
  public:
    TtTreeItem& operator *( void );
    Iterator& operator ++( void );
    bool operator ==( const Iterator& other );
    bool operator !=( const Iterator& other );
  private:
    TtTreeItem current_;
    TtTreeItem next_;
  };
  Iterator begin( void );
  Iterator end( void );
private:
  TtTreeItem& parent_;
};

// -- TtTreeItemWith -----------------------------------------------------
template <class TYPE>
class TtTreeItemWith : public TtTreeItem {
public:
  explicit TtTreeItemWith( TtTreeView* tree, HTREEITEM handle ) : TtTreeItem( tree, handle ) {}
  TtTreeItemWith( TtTreeItem& item ) : TtTreeItem( item ) {}

  void SetParameter( TYPE parameter ) {
    this->SetParameterAs<TYPE>( parameter );
  }
  TYPE GetParameter( void ) {
    return this->GetParameterAs<TYPE>();
  }
};


// -- TtTreeView ---------------------------------------------------------
class TtTreeView : public BasedOnTtWindow<WS_TABSTOP, WS_EX_CLIENTEDGE, TtWindowClassName::TreeView> {
public:
  class Style {
  public:
    static const DWORD DISABLEDRAGDROP = TVS_DISABLEDRAGDROP;
    static const DWORD EDITLABELS      = TVS_EDITLABELS;
    static const DWORD HASBUTTONS      = TVS_HASBUTTONS;
    static const DWORD HASLINES        = TVS_HASLINES;
    static const DWORD LINESATROOT     = TVS_LINESATROOT;
    static const DWORD SHOWSELALWAYS   = TVS_SHOWSELALWAYS;
  };

  explicit TtTreeView( void );

  TtTreeItem& GetOrigin( void );

  void SetItemHeight( int height );
  int GetItemHeight( void );

  void SetImageList( TtImageList& image_list );
  void SetImageListState( TtImageList& image_list );

  void EndEditMode( bool do_cancel );

  TtTreeItem GetSelected( void );
  TtTreeItem HitTest( int x, int y );
  TtTreeItem GetDropHilight( void );
  void ClearDropTarget( void );

  void Clear( void );

private:
  TtTreeItem origin_;
};

using TtTreeViewModern = TtWindowWithStyle<TtTreeView, TtTreeView::Style::HASBUTTONS | TtTreeView::Style::HASLINES>;
