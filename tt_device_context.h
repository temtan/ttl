// tt_device_context.h

#pragma once

#include <functional>

#include "tt_windows_h_include.h"

#include "tt_utility.h"


class TtDeviceContext;

// -- TtDeviceContextBase ------------------------------------------------
class TtDeviceContextBase : public TtUtility::WindowsHandleHolder<HDC> {
public:
  explicit TtDeviceContextBase( DestroyFunction destroy_function, HDC handle, bool auto_destroy );

  void DrawRectangle( RECT rect );
  TtDeviceContext CreateCompatibleDeviceContext( void );

  // -- temporal -----
  void SetROP2( int mode );
  void SetBkMode( int mode );
  HGDIOBJ SelectObject( HGDIOBJ object );
  void DeleteObject( HGDIOBJ object );
};


// -- TtDeviceContext ----------------------------------------------------
class TtDeviceContext : public TtDeviceContextBase {
public:
  explicit TtDeviceContext( HDC handle, bool auto_destroy );

  static TtDeviceContext CreateDeviceContextForDisplay( void );
};

// -- TtReleasableDeviceContext ----------------------------------------------
class TtReleasableDeviceContext : public TtDeviceContextBase {
public:
  explicit TtReleasableDeviceContext( HDC handle, HWND window_handle_, bool auto_destroy );

private:
  HWND window_handle_;
};
