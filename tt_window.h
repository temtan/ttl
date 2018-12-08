// tt_window.h

#pragma once

#define OEMRESOURCE

#include "tt_windows_h_include.h"
#include <commctrl.h>

#include <string>
#include <unordered_map>

#include "tt_enum.h"
#include "tt_image_list.h"
#include "tt_font.h"
#include "tt_icon.h"
#include "tt_device_context.h"
#include "tt_utility.h"



// -- TtWindow -----------------------------------------------------------
class TtWindow {
  // ---------------------------------------------------------------------
  // Window Message Handler 関連
public:
  class WindowTable : private std::unordered_map<HWND, TtWindow*> {
  public:
    explicit WindowTable( void );
    void Register( TtWindow& window );
    void Unregister( TtWindow& window );
    TtWindow* Find( HWND handle );
  };
  static WindowTable WINDOW_TABLE;

  static LRESULT CALLBACK WindowProcedureForTTL( HWND handle, UINT msg, WPARAM w_param, LPARAM l_param );

  struct WMResult {
    enum ResultType : int {
      Done,
      Incomplete,
      DelegateToParent,
    };
    ResultType result_type;
    LRESULT    result;
  };

  static WMResult FindAndCallWindowProcedure( HWND handle, UINT msg, WPARAM w_param, LPARAM l_param );

  WMResult WindowProcedure( UINT msg, WPARAM w_param, LPARAM l_param );

  using SingleHandler = std::function<WMResult ( WPARAM, LPARAM )>;
  using HandlerTable = std::unordered_map<UINT, SingleHandler>;

  void RegisterSingleHandler( UINT msg, SingleHandler handler, bool do_override );
  void UnregisterSingleHandler( UINT msg );
  SingleHandler GetSingleHandler( UINT msg );

  void OverrideWindowProcedureByTTL( void );

  static POINT GetCursorPosition( void );
  static void SetCursorPosition( POINT point );
  static int GetXOfLParam( LPARAM l_param );
  static int GetYOfLParam( LPARAM l_param );

  // ---------------------------------------------------------------------
public:
  static void InitializeCommonControls( void );
  static void RegisterClass( WNDCLASS* window_class );

  // 以下のメンバ関数は継承先クラスでオーバーライドすること
protected:
  virtual DWORD  GetStyle( void ) = 0;
  virtual DWORD  GetExtendedStyle( void ) = 0;
  virtual PCTSTR GetClassName( void ) = 0;
  virtual bool CreatedInternal( void );

public:
  explicit TtWindow( void );
  virtual ~TtWindow();
  explicit TtWindow ( TtWindow const & ) = delete ;
  void operator = ( TtWindow const & ) = delete ;

  // -- CreateParameter --------------------------------------------------
  struct CreateParameter {
  public:
    CreateParameter( void );
    CreateParameter( TtWindow* parent );
    CreateParameter( TtWindow* parent, int id );
    CreateParameter( const CreateParameter& other );

    CreateParameter AlterParent( TtWindow* parent ) const;
    CreateParameter AlterId( int id ) const;
    CreateParameter AlterInstanceHandle( HINSTANCE instance_handle ) const;
    CreateParameter AlterLPParam( LPVOID lp_param ) const;
    CreateParameter AlterIsChild( bool is_child ) const;
    CreateParameter AlterGroupStart( bool group_start ) const;

  public:
    TtWindow* parent_;
    int       id_;
    HINSTANCE instance_handle_;
    LPVOID    lp_param_;
    bool      is_child_;
    bool      group_start_;
  };
  // ---------------------------------------------------------------------

  void Create( void );
  void Create( CreateParameter parameter );
  virtual void CreatePrimitive( CreateParameter& parameter );

  int Close( void );

  int SendMessage( UINT msg, WPARAM w_param = 0, LPARAM l_param = 0 );
  void PostMessage( UINT msg, WPARAM w_param = 0, LPARAM l_param = 0 );

  LONG GetWindowLong( int index );
  void SetWindowLong( int index, LONG value );

  LONG_PTR GetWindowLongPtr( int index );
  void SetWindowLongPtr( int index, LONG_PTR value );

  ULONG_PTR GetClassLongPtr( int index );
  void SetClassLongPtr( int index, LONG_PTR value );

  void ChangeWindowStyle( DWORD style );

  TtWindow* GetParent( void );
  HWND      GetHandle( void );
  int       GetID( void );
  HINSTANCE GetInstanceHandle( void );

  RECT GetRectangle( void );
  POINT GetPoint( void );
  int GetWidth( void );
  int GetHeight( void );
  POINT GetRelativePointToClient( void );

  int GetClientWidth( void );
  int GetClientHeight( void );
  POINT GetClientEndPoint( void );
  void SetClientSize( int width, int height, bool has_menu );
  POINT GetClientOffset( void );

  WINDOWPLACEMENT GetWindowPlacement( void );
  void SetWindowPlacement( WINDOWPLACEMENT& placement );

  POINT ConvertToClientPoint( POINT point );
  POINT ConvertToScreenPoint( POINT point );

  TtReleasableDeviceContext GetDeviceContext( void );

  // -- ShowState --------------------------------------------------------
  enum class ShowState {
    FORCE_MINIMIZE            = SW_FORCEMINIMIZE,
    HIDE                      = SW_HIDE,
    RESTORE                   = SW_RESTORE,
    MINIMIZE                  = SW_MINIMIZE,
    MAXIMIZE                  = SW_MAXIMIZE,
    SHOW                      = SW_SHOW,
    SHOW_NO_ACTIVE            = SW_SHOWNA,
    SHOW_MINIMIZED            = SW_SHOWMINIMIZED,
    SHOW_MINIMIZED_NO_ACTIVE  = SW_SHOWMINNOACTIVE,
    SHOW_RESTORED             = SW_SHOWNORMAL,
    SHOW_RESTORED_NO_ACTIVATE = SW_SHOWNOACTIVATE,
    SHOW_DEFAULT              = SW_SHOWDEFAULT,
  };

  void Show( void );
  void Hide( void );
  void Minimize( void );
  void Maximize( void );
  void Restore( void );
  void SetShowState( TtEnum<ShowState> state );
  bool IsMinimized( void );
  bool IsMaximized( void );
  bool GetVisible( void );

  bool GetEnabled( void );
  void SetEnabled( bool flag );

  void SetActive( void );
  void SetFocus( void );

  void SetPosition( int x, int y );
  void SetSize( int width, int height );
  void SetPositionSize( int x, int y, int width, int height );
  void SetRectangle( RECT rect );
  void Move( int x, int y, int width, int height, bool do_repaint );
  void SetCenterRelativeTo( TtWindow& reference );
  void SetCenterRelativeToParent( void );

  void SetBottom( void );
  void SetTop( void );
  void SetUnder( TtWindow& window );
  void SetUnder( HWND window );
  void SetTopMost( bool flag );

  void SetFont( TtFont& font );
  void SetIconAsLarge( TtIcon& icon );
  void SetIconAsSmall( TtIcon& icon );

  int SetForground( void );

  void SetCapture( void );
  bool IsCapturing( void );
  static void ReleaseCapture( void );

  void Update( void );
  void Invalidate( void );

  void StopRedraw( void );
  void StopRedraw( std::function<void ( void )> function );
  void RestartRedraw( void );

  int GetTextLength( void );
  std::string GetText( void );
  void SetText( const std::string& str );

protected:
  TtWindow*    parent_;
  HWND         handle_;
  int          id_;
  HINSTANCE    instance_handle_;
  WNDPROC      window_procedure_super_;
  HandlerTable handler_table_;
};

// -- TtExtraordinarilyWindow --------------------------------------------
class TtExtraordinarilyWindow : public TtWindow {
public:
  static TtExtraordinarilyWindow Desktop;

public:
  explicit TtExtraordinarilyWindow( HWND handle );
  explicit TtExtraordinarilyWindow( HWND handle, TtWindow& parent );

protected:
  virtual DWORD  GetStyle( void ) override;
  virtual DWORD  GetExtendedStyle( void ) override;
  virtual PCTSTR GetClassName( void ) override;
};

// -- TtWindowWithStyle --------------------------------------------------
template <class BASE, DWORD style>
class TtWindowWithStyle : public BASE {
protected:
  virtual DWORD GetStyle( void ) override {
    return this->BASE::GetStyle() | style;
  }
};

// -- BasedOnTtWindow --------------------------------------------------
template <DWORD style, DWORD extended_style, typename WithClassName>
class BasedOnTtWindow : public TtWindow {
protected:
  virtual DWORD GetStyle( void ) override {
    return style;
  }
  virtual DWORD  GetExtendedStyle( void ) override {
    return extended_style;
  }
  virtual PCTSTR GetClassName( void ) override {
    return WithClassName::ClassName;
  }
};

namespace TtWindowClassName {
  struct Static     { static const char* ClassName; };
  struct Button     { static const char* ClassName; };
  struct Edit       { static const char* ClassName; };
  struct ListBox    { static const char* ClassName; };
  struct ComboBox   { static const char* ClassName; };
  struct ScrollBar  { static const char* ClassName; };
  struct TrackBar   { static const char* ClassName; };
  struct TabControl { static const char* ClassName; };
  struct Progress   { static const char* ClassName; };
  struct ListView   { static const char* ClassName; };
  struct TreeView   { static const char* ClassName; };
  struct ToolBar    { static const char* ClassName; };
  struct StatusBar  { static const char* ClassName; };
}
