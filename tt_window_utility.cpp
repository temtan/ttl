// tt_window_utility.cpp

#include "ttl_define.h"
#include "tt_exception.h"

#include "tt_window_utility.h"


// -- TtDragHandler ------------------------------------------------------
void
TtDragHandler::DestroyImageList( HIMAGELIST handle )
{
  if ( handle != INVALID_HANDLE_VALUE ) {
    ::ImageList_Destroy( handle );
  }
}


TtDragHandler::TtDragHandler( void ) :
TtDragHandler( nullptr )
{
}

TtDragHandler::TtDragHandler( HIMAGELIST handle ) :
image_list_handler_( handle, TtDragHandler::DestroyImageList ),
last_entered_( nullptr ),
is_begun_( false ),
is_entered_( false )
{
}



bool
TtDragHandler::IsBegun( void )
{
  return is_begun_;
}

bool
TtDragHandler::IsEntered( void )
{
  return is_entered_;
}


void
TtDragHandler::Begin( int index, int x, int y )
{
  if ( NOT( ::ImageList_BeginDrag( image_list_handler_.get(), index, x, y ) ) ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::ImageList_BeginDrag ) );
  }
  is_begun_ = true;
}

void
TtDragHandler::Enter( int x, int y )
{
  this->InternalEnter( NULL, x, y );
}

void
TtDragHandler::Enter( TtWindow& window, int x, int y )
{
  this->InternalEnter( window.GetHandle(), x, y );
}

void
TtDragHandler::ReEnter( int x, int y, std::function<void ( void )> function )
{
  HWND tmp = last_entered_;
  this->Leave();
  function();
  this->InternalEnter( tmp, x, y );
}

void
TtDragHandler::Move( int x, int y )
{
  if ( NOT( ImageList_DragMove( x, y ) ) ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::ImageList_DragMove ) );
  }
}

void
TtDragHandler::Leave( void )
{
  if ( NOT( ::ImageList_DragLeave( last_entered_ ) ) ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::ImageList_DragLeave ) );
  }
  last_entered_ = nullptr;
  is_entered_ = false;
}

void
TtDragHandler::End( void )
{
  ::ImageList_EndDrag();
  image_list_handler_ = ImageListHandler( nullptr, TtDragHandler::DestroyImageList );
  is_begun_ = false;
}


void
TtDragHandler::InternalEnter( HWND handle, int x, int y )
{
  if ( NOT( ::ImageList_DragEnter( handle, x, y ) ) ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::ImageList_EndDrag ) );
  }
  last_entered_ = handle;
  is_entered_ = true;
}


#ifdef TT_MAKE_TEMPLATE_INSTANCE_
template class TtValueToRadioCheck<int>;
template class TtRadioCheckToValue<int>;
#endif
