// tt_form.h

#pragma once

#include <map>
#include <unordered_map>
#include <vector>

#include "tt_window.h"
#include "tt_panel.h"
#include "tt_menu.h"


// -- TtForm -------------------------------------------------------------
class TtForm : public TtPanel {
public:
  // -- AcceleratorMap ---------------------------------------------------
  class AcceleratorMap {
    friend class TtForm;
  public:
    class ShortcutKey {
    public:
      struct Modifier {
        enum {
          None    = 0,
          Shift   = FSHIFT,
          Control = FCONTROL,
          Alt     = FALT,
        };
      };

      static ShortcutKey GetMdifierFromKeyboard( WORD key );

      ShortcutKey( void );
      ShortcutKey( bool shift, bool control, bool alt, WORD key );
      ShortcutKey( WORD key );
      ShortcutKey( BYTE modifier, WORD key );

      bool WithShift( void ) const;
      bool WithControl( void ) const;
      bool WithAlt( void ) const;
      BYTE GetModifier( void ) const;
      WORD GetVirtualKey( void ) const;

      std::string GetText( void ) const;
      std::string Serialize( void ) const;
      static ShortcutKey Deserialize( const std::string& data );

      // std::map —p
      bool operator < ( const ShortcutKey& other ) const;

    private:
      BYTE modifier_;
      WORD key_;
    };

    explicit AcceleratorMap( void );
    void Register( ShortcutKey key, WORD command_id );

  private:
    std::map<ShortcutKey, WORD> table_;
  };

  // -- AcceleratorTable -------------------------------------------------
  class AcceleratorTable {
    friend class TtForm;
  private:
    explicit AcceleratorTable( HACCEL handle, TtForm& form );
    bool Translate( MSG* msg );
    void Destroy( void );

  private:
    HACCEL  handle_;
    TtForm& form_;
  };

protected:
  static std::unordered_map<HWND, TtForm*> FORM_TABLE;
  static AcceleratorTable*                 ACCELERATOR_TABLE;

public:
  static WNDCLASS MakeDefaultWindowClass( HINSTANCE h_instance, const char* class_name );
  static void RegisterAccelerator( TtForm& form, AcceleratorMap& map );
  static int LoopDispatchMessage( void );
  static void DispatchOneMessage( MSG& msg );
  static void DoEvent( void );

private:
  bool IsDialogMessage( MSG* msg );

protected:
  virtual DWORD GetStyle( void ) override;
  virtual DWORD GetExtendedStyle( void ) override;
  virtual bool CreatedInternal( void ) override;

  virtual bool Created( void );

public:
  explicit TtForm( void );
  virtual ~TtForm();

  void SetMenu( TtMenuBar& menu );

protected:
  TtSubMenu GetSystemMenu( void );

public:
  using WMCloseHandler = std::function<WMResult ( void )>;
  void RegisterWMClose( WMCloseHandler handler, bool do_override = false );

  using WMDestroyHandler = std::function<WMResult ( void )>;
  void RegisterWMDestroy( WMDestroyHandler handler, bool do_override = false  );

  using WMMouseMoveHandler = std::function<WMResult ( int key, int x, int y )>;
  void RegisterWMMouseMove( WMMouseMoveHandler handler, bool do_override = false );

  using WMDropFilesHandler = std::function<WMResult ( HDROP drop )>;
  void RegisterWMDropFiles( WMDropFilesHandler handler, bool do_override = false );

protected:
  int exit_code_;
};
