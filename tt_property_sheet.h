// tt_property_sheet.h

#pragma once

#include <vector>
#include <map>

#include "tt_panel.h"
#include "tt_form.h"


// -- TtPropertySheet ----------------------------------------------------
class TtPropertySheet : public TtForm {
public:
  // -- Page -------------------------------------------------------------
  class Page : public TtPanel {
    friend class TtPropertySheet;

  private:
    static INT_PTR CALLBACK WindowProcedureForTTLPropertySheetPage( HWND handle, UINT msg, WPARAM w_param, LPARAM l_param );
    static UINT CALLBACK ProcedureForTTLPropertySheetPage( HWND handle, UINT msg, LPPROPSHEETPAGE page );

  protected:
    virtual DWORD  GetStyle( void ) override;
    virtual DWORD  GetExtendedStyle( void ) override;
    virtual bool CreatedInternal( void ) override;

    virtual bool Created( void );

  public:
    using AtPageCreateHandler  = std::function<bool ( void )>;
    using AtPageReleaseHandler = std::function<void ( void )>;

    using AtSetActiveHandler  = std::function<void ( void )>;
    using AtKillActiveHandler = std::function<bool ( void )>;
    using AtApplyHandler      = std::function<bool ( void )>;
    using AtResetHandler      = std::function<void ( void )>;

    struct Handlers {
      AtPageCreateHandler  at_page_create;   // 作成直前
      AtPageReleaseHandler at_page_release;  // 破棄直前
      AtSetActiveHandler   at_set_active;    // 画面 Active 直前
      AtKillActiveHandler  at_kill_active;   // 画面 非Active 直前
      AtApplyHandler       at_apply;         // OK or 更新時
      AtResetHandler       at_reset;         // 取り消し時
    };

    Handlers& GetHandlers( void );

  public:
    explicit Page( const std::string& title = "" );
    void PresetSizeAtPageCreate( int width, int height );

    virtual ~Page();

    TtPropertySheet& GetParentSheet( void ) const;
    const std::string& GetTitle( void ) const;

    virtual void CreatePrimitive( CreateParameter& parameter ) override;

    void TellChanged( void );

  private:
    PROPSHEETPAGE    page_;
    TtPropertySheet* parent_sheet_;
    std::string      title_;
    void*            template_object_;
    Handlers         handlers_;
  };

  // -- TtPropertySheet --------------------------------------------------
private:
  static std::map<DWORD, TtPropertySheet*> TABLE;
  static int CALLBACK ProcedureForTTLPropertySheet( HWND handle, UINT msg, LPARAM l_param );

public:
  explicit TtPropertySheet( bool has_apply_button = true );

  virtual void CreatePrimitive( CreateParameter& parameter ) override;

  void SetTitle( const std::string& title );
  const std::string& GetTitle( void ) const;

  void SetHasApplyButton( bool has_apply_button );
  bool GetHasApplyButton( void ) const;

  void AddPage( Page& page );

  bool ShowDialog( TtWindow& parent );

private:
  std::vector<Page*> pages_;
  bool               has_apply_button_;
};
