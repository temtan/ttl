// tt_scroll.h

#pragma once

#include "tt_window.h"


// -- TtScroll -----------------------------------------------------------
class TtScroll : public BasedOnTtWindow<0, WS_EX_CLIENTEDGE, TtWindowClassName::ScrollBar> {
public:
  int GetTrackPosition( void );
  void SetTrackPosition( int position );

  int  GetMinimum( void );
  void SetMinimum( int min );
  int  GetMaximum( void );
  void SetMaximum( int max );
  void SetRange( int min, int max );

  unsigned int GetPageSize( void );
  void SetPageSize( unsigned int page_size );

protected:
  bool GetInfo( void );
  void SetInfo( void );

  bool Scrolling( int code, int position );

  using WMScrollHandler = std::function<WMResult ( int code, int position, HWND handle )>;
  void RegisterWMVerticalScroll( WMScrollHandler handler, bool do_override = false );
  void RegisterWMHorizontalScroll( WMScrollHandler handler, bool do_override = false );

protected:
  SCROLLINFO info_;
};

// -- TtScrollVertical ---------------------------------------------------
class TtScrollVertical : public TtWindowWithStyle<TtScroll, SBS_VERT> {
public:
  explicit TtScrollVertical( void );
};

// -- TtScrollHorizontal -------------------------------------------------
class TtScrollHorizontal : public TtWindowWithStyle<TtScroll, SBS_HORZ> {
public:
  explicit TtScrollHorizontal( void );
};
