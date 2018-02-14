// tt_scroll.cpp

#include "ttl_define.h"

#include "tt_scroll.h"


int
TtScroll::GetTrackPosition( void )
{
  this->GetInfo();
  return info_.nPos;
}

void
TtScroll::SetTrackPosition( int position )
{
  this->GetInfo();
  info_.nPos = position;
  this->SetInfo();
}

int
TtScroll::GetMinimum( void )
{
  this->GetInfo();
  return info_.nMin;
}

void
TtScroll::SetMinimum( int min )
{
  this->GetInfo();
  info_.nMin = min;
  this->SetInfo();
}

int
TtScroll::GetMaximum( void )
{
  this->GetInfo();
  return info_.nMax;
}

void
TtScroll::SetMaximum( int max )
{
  this->GetInfo();
  info_.nMax = max;
  this->SetInfo();
}

void
TtScroll::SetRange( int min, int max )
{
  this->GetInfo();
  info_.nMin = min;
  info_.nMax = max;
  this->SetInfo();
}

unsigned int
TtScroll::GetPageSize( void )
{
  this->GetInfo();
  return info_.nPage;
}

void
TtScroll::SetPageSize( unsigned int page_size )
{
  this->GetInfo();
  info_.nPage = page_size;
  this->SetInfo();
}

bool
TtScroll::GetInfo( void )
{
  info_.fMask = SIF_ALL;
  return GetScrollInfo( this->GetHandle(), SB_CTL, &info_ ) ? true : false;
}

void
TtScroll::SetInfo( void )
{
  info_.fMask |= SIF_DISABLENOSCROLL;
  SetScrollInfo( this->GetHandle(), SB_CTL, &info_, TRUE );
}

bool
TtScroll::Scrolling( int code, int position )
{
  this->GetInfo();
  switch ( code ) {
  case SB_LEFT:
    info_.nPos = info_.nMin;
    break;

  case SB_RIGHT:
    info_.nPos = info_.nMax;
    break;

  case SB_LINELEFT:
    if ( info_.nPos > info_.nMin ) {
      info_.nPos -= 1;
    }
    break;

  case SB_LINERIGHT:
    if ( info_.nPos < info_.nMax - 1 ) {
      info_.nPos += 1;
    }
    break;

  case SB_PAGELEFT:
    info_.nPos -= info_.nPage;
    if ( info_.nPos < info_.nMin ) {
      info_.nPos = info_.nMin;
    }
    break;

  case SB_PAGERIGHT:
    info_.nPos += info_.nPage;
    if ( info_.nPos > info_.nMax ) {
      info_.nPos = info_.nMax;
    }
    break;

  case SB_THUMBPOSITION:
    info_.nPos = position;
    break;

  default:
    return false;
  }
  this->SetInfo();
  return true;
}


void
TtScroll::RegisterWMVerticalScroll( WMScrollHandler handler, bool do_override )
{
  this->RegisterSingleHandler( WM_VSCROLL, [handler]( WPARAM w_param, LPARAM l_param ) {
    return handler( LOWORD( w_param ), HIWORD( w_param ), reinterpret_cast<HWND>( l_param ) );
  }, do_override );
}

void
TtScroll::RegisterWMHorizontalScroll( WMScrollHandler handler, bool do_override )
{
  this->RegisterSingleHandler( WM_HSCROLL, [handler]( WPARAM w_param, LPARAM l_param ) {
    return handler( LOWORD( w_param ), HIWORD( w_param ), reinterpret_cast<HWND>( l_param ) );
  }, do_override );
}


// -- TtScrollVertical ---------------------------------------------------
TtScrollVertical::TtScrollVertical( void )
{
  this->RegisterWMVerticalScroll( [this] ( int code, int position, HWND ) {
    return WMResult( {this->Scrolling( code, position ) ? WMResult::Done : WMResult::Incomplete} );
  } );
}

// -- TtScrollHorizontal -------------------------------------------------
TtScrollHorizontal::TtScrollHorizontal( void )
{
  this->RegisterWMHorizontalScroll( [this] ( int code, int position, HWND ) {
    return WMResult( {this->Scrolling( code, position ) ? WMResult::Done : WMResult::Incomplete} );
  } );
}
