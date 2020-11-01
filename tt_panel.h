// tt_panel.h

#pragma once

#include <unordered_map>

#include "tt_window.h"
#include "tt_device_context.h"
#include "tt_menu.h"


// -- TtPanel ------------------------------------------------------------
class TtPanel : public TtWindow {
public:
  static const char* const CLASS_NAME;

public:
  static void Initialize( void );
  static void Initialize( HINSTANCE h_instance );

public:
  explicit TtPanel( bool notice_to_parent = true );

  virtual DWORD  GetStyle( void ) override;
  virtual DWORD  GetExtendedStyle( void ) override;
  virtual PCTSTR GetClassName( void ) override;
  virtual bool CreatedInternal( void ) override;

  using WMSizeHandler = std::function<WMResult ( int flag, int width, int height )>;
  void RegisterWMSize( WMSizeHandler handler, bool do_override = false );

  using WMSizingHandler = std::function<WMResult ( int flag, RECT& rectangle )>;
  void RegisterWMSizing( WMSizingHandler handler, bool do_override = false );

  WMSizingHandler MakeCanChangeOnlyHorizontalHandler( void );
  WMSizingHandler MakeCanChangeOnlyHorizontalHandlerWithMinimumWidth( int width_min );
  WMSizingHandler MakeCanChangeOnlyVerticalHandler( void );
  WMSizingHandler MakeCanChangeOnlyVerticalHandlerWithMinimumHeight( int height_min );
  WMSizingHandler MakeMinimumSizedHandler( int width_min, int height_min );

  using WMPaintHandler = std::function<WMResult ( void )>;
  void RegisterWMPaint( WMPaintHandler handler, bool do_override = false );

  using CommandHandler = std::function<WMResult ( int code, HWND child )>;
  void AddCommandHandler( int id, CommandHandler handler );
  void ClearCommandHandler( int id );
  WMResult CallCommandHandler( int id, int code, HWND child );

  using NotifyHandler  = std::function<WMResult ( NMHDR* nmhdr )>;
  void AddNotifyHandler( int id, NotifyHandler handler );
  void ClearNotifyHandler( int id );

  using MenuCommandHandler = std::function<WMResult ( TtMenuItem& item )>;
  void AddMenuCommandHandler( TtSubMenuCommand& menu, MenuCommandHandler handler );
  void RemoveMenuCommandHandler( TtSubMenuCommand& menu );
  void ClearMenuCommandHandler( TtSubMenuCommand& menu );

private:
  bool                                                       notice_to_parent_;
  std::unordered_map<int, std::vector<CommandHandler>>       command_table_;
  std::unordered_map<int, std::vector<NotifyHandler>>        notify_table_;
  std::unordered_map<HMENU, std::vector<MenuCommandHandler>> menu_command_table_;
};


// -- TtSplitterPanel ----------------------------------------------------
class TtSplitterPanel : public TtPanel {
public:
  explicit TtSplitterPanel( int width, int north_limit, int south_limit );

  virtual bool CreatedInternal( void ) override;

  int GetSplitterWidth( void );
  int GetNorthLimit( void );
  int GetSouthLimit( void );

  void SetNorthWindow( TtWindow* front );
  TtWindow* GetNorthWindow( void );
  void SetSouthWindow( TtWindow* rear );
  TtWindow* GetSouthWindow( void );

  void SetSplitterPosition( int position );
  int GetSplitterPosition( void );

private:
  void RegisterHandlers( void );
  void LimitCheck( int& position );
  void DrawMoving( void );
  void EraseMoving( void );

protected:
  virtual LPCTSTR GetCursorID( void ) = 0;
  virtual int GetLatitudeOfLParam( LPARAM l_param ) = 0;
  virtual RECT MakeRectangleFromLL( int north, int south, int west, int east ) = 0;
  virtual int GetWestEast( void ) = 0;
  virtual int GetNorthSouth( void ) = 0;

private:
  int splitter_width_;
  int north_limit_;
  int south_limit_;
  TtWindow* north_window_;
  TtWindow* south_window_;

  int splitter_position_;
  int moving_position_;
};

// -- TtVirticalSplitterPanel --------------------------------------------
class TtVirticalSplitterPanel : public TtSplitterPanel {
public:
  explicit TtVirticalSplitterPanel( int width = 4, int north_limit = 0, int south_limit = 0 );
protected:
  virtual LPCTSTR GetCursorID( void ) override;
  virtual int GetLatitudeOfLParam( LPARAM l_param ) override;
  virtual RECT MakeRectangleFromLL( int north, int south, int west, int east ) override;
  virtual int GetWestEast( void ) override;
  virtual int GetNorthSouth( void ) override;
};

// -- TtHorizontalSplitterPanel ------------------------------------------
class TtHorizontalSplitterPanel : public TtSplitterPanel {
public:
  explicit TtHorizontalSplitterPanel( int width = 4, int north_limit = 0, int south_limit = 0 );
protected:
  virtual LPCTSTR GetCursorID( void ) override;
  virtual int GetLatitudeOfLParam( LPARAM l_param ) override;
  virtual RECT MakeRectangleFromLL( int north, int south, int west, int east ) override;
  virtual int GetWestEast( void ) override;
  virtual int GetNorthSouth( void ) override;
};
