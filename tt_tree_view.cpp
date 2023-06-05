// tt_tree_view.cpp

#include "ttl_define.h"
#include "tt_exception.h"

#include "tt_tree_view.h"

#pragma warning(push)
// #pragma warning(disable : 4365)
#pragma warning(disable : 4355)


// -- TtTreeItem ---------------------------------------------------------
const HTREEITEM TtTreeItem::INVALID_HANDLE = static_cast<HTREEITEM>( INVALID_HANDLE_VALUE );


TtTreeItem::TtTreeItem( TtTreeView* tree, HTREEITEM handle ) :
handle_( handle ),
tree_( tree )
{
}

void
TtTreeItem::GetInfo( TVITEM& info )
{
  if ( NOT( TreeView_GetItem( tree_->GetHandle(), &info ) ) ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( TreeView_GetItem ) );
  }
}

void
TtTreeItem::SetInfo( TVITEM& info )
{
  if ( NOT( TreeView_SetItem( tree_->GetHandle(), &info ) ) ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( TreeView_SetItem ) );
  }
}

HTREEITEM
TtTreeItem::GetHandle( void )
{
  return handle_;
}


bool
TtTreeItem::IsOrigin( void )
{
  return handle_ == nullptr;
}

bool
TtTreeItem::IsValid( void )
{
  return handle_ != INVALID_HANDLE;
}

bool
TtTreeItem::IsInvalid( void )
{
  return NOT( this->IsValid() );
}


bool
TtTreeItem::IsExpanded( void )
{
  TVITEM tmp = {TVIF_STATE};
  tmp.hItem = handle_;
  tmp.stateMask = TVIS_EXPANDED;
  this->GetInfo( tmp );
  return (tmp.state & TVIS_EXPANDED ? true : false);
}


bool
TtTreeItem::IsExpandedOnce( void )
{
  TVITEM tmp = {TVIF_STATE};
  tmp.hItem = handle_;
  tmp.stateMask = TVIS_EXPANDEDONCE;
  this->GetInfo( tmp );
  return (tmp.state & TVIS_EXPANDEDONCE ? true : false);
}


bool
TtTreeItem::HasChild( void )
{
  TVITEM tmp = {TVIF_CHILDREN};
  tmp.hItem = handle_;
  this->GetInfo( tmp );
  return tmp.cChildren == 1;
}


bool
TtTreeItem::IsDescendantOf( TtTreeItem& other )
{
  return other.IsAncestorOf( *this );
}

bool
TtTreeItem::IsAncestorOf( TtTreeItem& other )
{
  if ( this->tree_->GetHandle() != other.tree_->GetHandle() ) {
    return false;
  }
  if ( this->IsOrigin() ) {
    return NOT( other.IsOrigin() );
  }

  std::function<bool ( TtTreeItem )> proc = [&] ( TtTreeItem target ) {
    if ( target.IsOrigin() ) {
      return false;
    }
    if ( *this == target ) {
      return true;
    }
    return proc( target.GetParent() );
  };
  return proc( other );
}

void
TtTreeItem::SetCheck( bool state )
{
  TreeView_SetCheckState( tree_->GetHandle(), handle_, state ? TRUE : FALSE );
}

bool
TtTreeItem::GetCheck( void )
{
  return TreeView_GetCheckState( tree_->GetHandle(), handle_ );
}


bool
TtTreeItem::operator ==( const TtTreeItem& other ) const
{
  return this->handle_ == other.handle_;
}

bool
TtTreeItem::operator !=( const TtTreeItem& other ) const
{
  return NOT( this->operator ==( other ) );
}


TtTreeItem
TtTreeItem::GetParent( void )
{
  if ( this->IsOrigin() ) {
    return TtTreeItem( tree_, INVALID_HANDLE );
  }
  HTREEITEM tmp;
  TtUtility::CallWindowsSystemFunctionWithErrorHandling(
    [&] ( void ) {
      tmp = TreeView_GetParent( tree_->GetHandle(), handle_ );
    },
    [] ( void ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( TreeView_GetParent ) );
    } );
  return TtTreeItem( tree_, tmp ? tmp : nullptr );
}

TtTreeItem
TtTreeItem::GetFirstChild( void )
{
  HTREEITEM tmp;
  TtUtility::CallWindowsSystemFunctionWithErrorHandling(
    [&] ( void ) {
      tmp = TreeView_GetChild( tree_->GetHandle(), handle_ );
    },
    [] ( void ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( TreeView_GetChild ) );
    } );
  return TtTreeItem( tree_, tmp ? tmp : INVALID_HANDLE );
}

TtTreeItem
TtTreeItem::GetNextSibling( void )
{
  HTREEITEM tmp;
  TtUtility::CallWindowsSystemFunctionWithErrorHandling(
    [&] ( void ) {
      tmp = TreeView_GetNextSibling( tree_->GetHandle(), handle_ );
    },
    [] ( void ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( TreeView_GetNextSibling ) );
    } );
  return TtTreeItem( tree_, tmp ? tmp : INVALID_HANDLE );
}

TtTreeItem
TtTreeItem::GetPrevSibling( void )
{
  HTREEITEM tmp;
  TtUtility::CallWindowsSystemFunctionWithErrorHandling(
    [&] ( void ) {
      tmp = TreeView_GetPrevSibling( tree_->GetHandle(), handle_ );
    },
    [] ( void ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( TreeView_GetPrevSibling ) );
    } );
  return TtTreeItem( tree_, tmp ? tmp : INVALID_HANDLE );
}


TtTreeItem::ChildEnumerable
TtTreeItem::GetChildEnumerable( void )
{
  return ChildEnumerable( *this );
}


TtTreeItem
TtTreeItem::AddChild( HTREEITEM prev, const std::string& text )
{
  TVINSERTSTRUCT tmp_insert = {0};
  tmp_insert.hParent = handle_ ? handle_ : TVI_ROOT;
  tmp_insert.hInsertAfter = prev;
  if ( NOT( text.empty() ) ) {
    tmp_insert.item.mask = TVIF_TEXT;
    tmp_insert.item.pszText = const_cast<char*>( text.c_str() );
  }
  HTREEITEM tmp_item = TreeView_InsertItem( tree_->GetHandle(), &tmp_insert );
  if ( tmp_item == NULL ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( TreeView_InsertItem ) );
  }
  return TtTreeItem( tree_, tmp_item );
}

TtTreeItem
TtTreeItem::AddChildPrevious( TtTreeItem& after, const std::string& text )
{
  TtTreeItem prev = after.GetPrevSibling();
  return prev.IsValid() ? this->AddChild( prev.handle_, text ) : after.GetParent().AddChildFirst( text );
}


TtTreeItem
TtTreeItem::AddChildAfter( TtTreeItem& prev, const std::string& text )
{
  return this->AddChild( prev.handle_, text );
}

TtTreeItem
TtTreeItem::AddChildFirst( const std::string& text )
{
  return this->AddChild( TVI_FIRST, text );
}

TtTreeItem
TtTreeItem::AddChildLast( const std::string& text )
{
  return this->AddChild( TVI_LAST, text );
}

TtTreeItem
TtTreeItem::AddChildSorted( const std::string& text )
{
  return this->AddChild( TVI_SORT, text );
}


TtTreeItem
TtTreeItem::MoveTo( TtTreeItem& parent, HTREEITEM prev )
{
  if ( parent == *this || parent.IsDescendantOf( *this ) ) {
    throw TtInvalidOperationException( typeid( *this ) );
  }
  TVINSERTSTRUCT tmp_insert = {0};
  tmp_insert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM | TVIF_STATE;
  tmp_insert.item.hItem = handle_;
  char buf[1024 * 16];
  tmp_insert.item.pszText = buf;
  tmp_insert.item.cchTextMax = sizeof( buf );
  this->GetInfo( tmp_insert.item );

  tmp_insert.hParent = parent.handle_ ? parent.handle_ : TVI_ROOT;
  tmp_insert.hInsertAfter = prev;
  HTREEITEM tmp_item = TreeView_InsertItem( tree_->GetHandle(), &tmp_insert );
  if ( tmp_item == NULL ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( TreeView_InsertItem ) );
  }
  TtTreeItem moved = TtTreeItem( tree_, tmp_item );
  if ( this->HasChild() ) {
    for ( TtTreeItem& child : this->GetChildEnumerable() ) {
      child.MoveToLast( moved );
    }
  }
  this->Remove();
  return moved;
}

TtTreeItem
TtTreeItem::MoveToPrevious( TtTreeItem& parent, TtTreeItem& after )
{
  TtTreeItem prev = after.GetPrevSibling();
  return prev.IsValid() ? this->MoveTo( parent, prev.handle_ ) : this->MoveToFirst( parent );
}

TtTreeItem
TtTreeItem::MoveToAfter( TtTreeItem& parent, TtTreeItem& prev )
{
  return this->MoveTo( parent, prev.handle_ );
}

TtTreeItem
TtTreeItem::MoveToFirst( TtTreeItem& parent )
{
  return this->MoveTo( parent, TVI_FIRST );
}

TtTreeItem
TtTreeItem::MoveToLast( TtTreeItem& parent )
{
  return this->MoveTo( parent, TVI_LAST );
}

TtTreeItem
TtTreeItem::MoveToSorted( TtTreeItem& parent )
{
  return this->MoveTo( parent, TVI_SORT );
}


void
TtTreeItem::Remove( void )
{
  if ( TreeView_DeleteItem( tree_->GetHandle(), handle_ ) == FALSE ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( TreeView_DeleteItem ) );
  }
  handle_ = INVALID_HANDLE;
}


void
TtTreeItem::SortChildren( void )
{
  if ( TreeView_SortChildren( tree_->GetHandle(), handle_, FALSE ) == FALSE ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( TreeView_SortChildren ) );
  }
}


std::string
TtTreeItem::GetText( void )
{
  TVITEM tmp = {TVIF_TEXT};
  tmp.hItem = handle_;
  char buf[1024 * 16];
  tmp.pszText = buf;
  tmp.cchTextMax = sizeof( buf );
  this->GetInfo( tmp );
  return std::string( tmp.pszText );
}

void
TtTreeItem::SetText( const std::string& str )
{
  TVITEM tmp = {TVIF_TEXT};
  tmp.hItem = handle_;
  tmp.pszText = const_cast<char*>( str.c_str() );
  this->SetInfo( tmp );
}


TtTreeItem::Rectangle
TtTreeItem::GetRectangle( void )
{
  RECT rc;
  BOOL ret;
  TtUtility::CallWindowsSystemFunctionWithErrorHandling(
    [&] ( void ) {
      ret = TreeView_GetItemRect( tree_->GetHandle(), handle_, &rc, FALSE );
    },
    [] ( void ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( TreeView_GetItemRect ) );
    } );
  return {ret ? true : false, rc};
}

TtTreeItem::Rectangle
TtTreeItem::GetTextRectangle( void )
{
  RECT rc;
  BOOL ret;
  TtUtility::CallWindowsSystemFunctionWithErrorHandling(
    [&] ( void ) {
      ret = TreeView_GetItemRect( tree_->GetHandle(), handle_, &rc, TRUE );
    },
    [] ( void ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( TreeView_GetItemRect ) );
    } );
  return {ret ? true : false, rc};
}


void
TtTreeItem::Expand( void )
{
  if ( TreeView_Expand( tree_->GetHandle(), handle_, TVE_EXPAND ) == FALSE ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( TreeView_Expand ) );
  }
}

void
TtTreeItem::Collapse( void )
{
  if ( TreeView_Expand( tree_->GetHandle(), handle_, TVE_COLLAPSE ) == FALSE ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( TreeView_Expand ) );
  }
}

void
TtTreeItem::ToggleExpand( void )
{
  if ( TreeView_Expand( tree_->GetHandle(), handle_, TVE_TOGGLE ) == FALSE ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( TreeView_Expand ) );
  }
}


void
TtTreeItem::SetEditMode( void )
{
  if ( TreeView_EditLabel( tree_->GetHandle(), handle_ ) == NULL ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( TreeView_EditLabel ) );
  }
}


void
TtTreeItem::SetSelect( void )
{
  if ( TreeView_SelectItem( tree_->GetHandle(), handle_ ) == FALSE ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( TreeView_SelectItem ) );
  }
}


void
TtTreeItem::SetSelectAsDropTarget( void )
{
  if ( TreeView_SelectDropTarget( tree_->GetHandle(), handle_ ) == FALSE ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( TreeView_SelectDropTarget ) );
  }
}

bool
TtTreeItem::IsDropHilighted( void )
{
  return tree_->GetDropHilight().handle_ == this->handle_;
}



void
TtTreeItem::SetImageIndex( int index )
{
  TVITEM tmp = {TVIF_IMAGE};
  tmp.iImage = index;
  tmp.hItem = handle_;
  this->SetInfo( tmp );
}

void
TtTreeItem::SetSelectedImageIndex( int index )
{
  TVITEM tmp = {TVIF_SELECTEDIMAGE};
  tmp.iSelectedImage = index;
  tmp.hItem = handle_;
  this->SetInfo( tmp );
}

void
TtTreeItem::SetBothImageIndex( int index )
{
  TVITEM tmp = {TVIF_IMAGE | TVIF_SELECTEDIMAGE};
  tmp.iImage = index;
  tmp.iSelectedImage = index;
  tmp.hItem = handle_;
  this->SetInfo( tmp );
}


TtTreeItem::DragHandler
TtTreeItem::MakeDragImage( void )
{
  HIMAGELIST image_list_handle = TreeView_CreateDragImage( tree_->GetHandle(), handle_ );
  if ( image_list_handle == NULL ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( TreeView_CreateDragImage ) );
  }
  return DragHandler( image_list_handle, *this );
}


// -- DragHandler --------------------------------------------------------
TtTreeItem::DragHandler::DragHandler( void ) :
TtDragHandler(),
item_( nullptr, INVALID_HANDLE )
{
}

TtTreeItem::DragHandler::DragHandler( HIMAGELIST handle, TtTreeItem& item ) :
TtDragHandler( handle ),
item_( item )
{
}

TtTreeItem
TtTreeItem::DragHandler::GetItem( void )
{
  return item_;
}

// -- ChildEnumerable ----------------------------------------------------
TtTreeItem::ChildEnumerable::ChildEnumerable( TtTreeItem& parent ) :
parent_( parent )
{
}


TtTreeItem::ChildEnumerable::Iterator::Iterator( TtTreeItem&& current ) :
current_( current ),
next_( current.IsValid() ? current.GetNextSibling() : TtTreeItem( current.tree_, INVALID_HANDLE ) )
{
}

TtTreeItem&
TtTreeItem::ChildEnumerable::Iterator::operator *( void )
{
  return current_;
}


TtTreeItem::ChildEnumerable::Iterator&
TtTreeItem::ChildEnumerable::Iterator::operator ++( void )
{
  current_ = next_;
  if ( current_.IsValid() ) {
    next_ = current_.GetNextSibling();
  }
  return *this;
}

bool
TtTreeItem::ChildEnumerable::Iterator::operator ==( const Iterator& other )
{
  return this->current_.handle_ == other.current_.handle_;
}

bool
TtTreeItem::ChildEnumerable::Iterator::operator !=( const Iterator& other )
{
  return NOT( this->operator ==( other ) );
}


TtTreeItem::ChildEnumerable::Iterator
TtTreeItem::ChildEnumerable::begin( void )
{
  return Iterator( parent_.GetFirstChild() );
}

TtTreeItem::ChildEnumerable::Iterator
TtTreeItem::ChildEnumerable::end( void )
{
  return Iterator( TtTreeItem( parent_.tree_, TtTreeItem::INVALID_HANDLE ) );
}


// -- TtTreeView ---------------------------------------------------------
TtTreeView::TtTreeView() :
origin_( this, nullptr )
{
}


TtTreeItem&
TtTreeView::GetOrigin( void )
{
  return origin_;
}


void
TtTreeView::SetItemHeight( int height )
{
  TreeView_SetItemHeight( handle_, height );
}

int
TtTreeView::GetItemHeight( void )
{
  return TreeView_GetItemHeight( handle_ );
}


void
TtTreeView::SetImageList( TtImageList& image_list )
{
  TtUtility::CallWindowsSystemFunctionWithErrorHandling(
    [&] ( void ) {
      TreeView_SetImageList( handle_, image_list.GetHandle(), TVSIL_NORMAL );
    },
    [] ( void ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( TreeView_SetImageList ) );
    } );
}


void
TtTreeView::SetImageListState( TtImageList& image_list )
{
  TtUtility::CallWindowsSystemFunctionWithErrorHandling(
    [&] ( void ) {
      TreeView_SetImageList( handle_, image_list.GetHandle(), TVSIL_STATE );
    },
    [] ( void ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( TreeView_SetImageList ) );
    } );
}


void
TtTreeView::EndEditMode( bool do_cancel )
{
  TreeView_EndEditLabelNow( handle_, do_cancel ? TRUE : FALSE );
  // ドキュメントでは失敗時 FALSE を返すとあるが、実際は違う感じ。
  // if ( TreeView_EndEditLabelNow( handle_, do_cancel ? TRUE : FALSE ) == FALSE ) {
  //   throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( TreeView_EndEditLabelNow ) );
  // }
}


TtTreeItem
TtTreeView::GetSelected( void )
{
  HTREEITEM item;
  TtUtility::CallWindowsSystemFunctionWithErrorHandling(
    [&] ( void ) {
      item = TreeView_GetSelection( handle_ );
    },
    [] ( void ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( TreeView_GetSelection ) );
    } );
  return TtTreeItem( this, item != NULL ? item : TtTreeItem::INVALID_HANDLE );
}


TtTreeItem
TtTreeView::HitTest( int x, int y )
{
  TV_HITTESTINFO info;
  info.pt.x = x;
  info.pt.y = y;

  TtUtility::CallWindowsSystemFunctionWithErrorHandling(
    [&] ( void ) {
      TreeView_HitTest( handle_, &info );
    },
    [] ( void ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( TreeView_HitTest ) );
    } );
  return TtTreeItem( this, info.hItem != NULL ? info.hItem : TtTreeItem::INVALID_HANDLE );
}



TtTreeItem
TtTreeView::GetDropHilight( void )
{
  HTREEITEM item;
  TtUtility::CallWindowsSystemFunctionWithErrorHandling(
    [&] ( void ) {
      item = TreeView_GetDropHilight( handle_ );
    },
    [] ( void ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( TreeView_GetDropHilight ) );
    } );
  return TtTreeItem( this, item != NULL ? item : TtTreeItem::INVALID_HANDLE );
}


void
TtTreeView::ClearDropTarget( void )
{
  if ( TreeView_SelectDropTarget( handle_, NULL ) == FALSE ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( TreeView_SelectDropTarget ) );
  }
}


void
TtTreeView::Clear( void )
{
  if ( TreeView_DeleteAllItems( handle_ ) == FALSE ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( TreeView_DeleteAllItems ) );
  }
}

#ifdef TT_MAKE_TEMPLATE_INSTANCE_
#pragma warning(disable : 4311)
#pragma warning(disable : 4312)
#pragma warning(disable : 4302)
template void TtTreeItem::SetParameterAs<int>( int );
template void TtTreeItem::SetParameterAs<int*>( int* );

template int TtTreeItem::GetParameterAs<int>( void );
template int* TtTreeItem::GetParameterAs<int*>( void );
#endif
