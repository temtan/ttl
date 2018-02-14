// tt_window_controls.h

#pragma once

#include "tt_window.h"


// -- TtStatic -----------------------------------------------------------
using TtStatic = BasedOnTtWindow<0, 0, TtWindowClassName::Static>;

// -- TtButton -----------------------------------------------------------
using TtButton = BasedOnTtWindow<WS_TABSTOP, 0, TtWindowClassName::Button>;

// -- TtButtonWithCheck --------------------------------------------------
class TtButtonWithCheck : public TtButton {
public:
  bool GetCheck( void );
  int  SetCheck( bool checking );
};

// -- TtRadioButton ------------------------------------------------------
using TtRadioButton = TtWindowWithStyle<TtButtonWithCheck, BS_AUTORADIOBUTTON>;

// -- TtCheckBox ----------------------------------------------------------
using TtCheckBox = TtWindowWithStyle<TtButtonWithCheck, BS_CHECKBOX | BS_AUTOCHECKBOX>;

// -- TtGroup ------------------------------------------------------------
using TtGroup = TtWindowWithStyle<TtButton, BS_GROUPBOX>;

// -- TtEdit -------------------------------------------------------------
class TtEdit : public BasedOnTtWindow<WS_TABSTOP | ES_AUTOHSCROLL, WS_EX_CLIENTEDGE, TtWindowClassName::Edit> {
public:
  class Style {
  public:
    static const DWORD AUTOHSCROLL = ES_AUTOHSCROLL;
    static const DWORD AUTOVSCROLL = ES_AUTOVSCROLL;
    static const DWORD CENTER      = ES_CENTER;
    static const DWORD LEFT        = ES_LEFT;
    static const DWORD RIGHT       = ES_RIGHT;
    static const DWORD UPPERCASE   = ES_UPPERCASE ;
    static const DWORD LOWERCASE   = ES_LOWERCASE;
    static const DWORD NUMBER      = ES_NUMBER;
    static const DWORD PASSWORD    = ES_PASSWORD;
    static const DWORD MULTILINE   = ES_MULTILINE;
    static const DWORD WANTRETURN  = ES_WANTRETURN;
    static const DWORD READONLY    = ES_READONLY;
  };

public:
  void SetReadOnly( bool flag );

  int GetFirstVisibleLine( void );

  void SetTextLimit( unsigned int size );
};

// -- TtEditWithStyle ----------------------------------------------------
template <DWORD style>
using TtEditWithStyle = TtWindowWithStyle<TtEdit, style>;

using TtEditNumber = TtEditWithStyle<TtEdit::Style::NUMBER>;
using TtEditMultiline = TtEditWithStyle<TtEdit::Style::MULTILINE | TtEdit::Style::WANTRETURN>;


// -- TtStatusBar --------------------------------------------------------
class TtStatusBar : public BasedOnTtWindow<WS_VISIBLE | CCS_BOTTOM | SBARS_SIZEGRIP, 0, TtWindowClassName::StatusBar> {
public:
  explicit TtStatusBar( void );

  void DivideInto( unsigned int count, std::vector<int> positions );

  class TextStyle {
  public:
    static const int Normal             = 0;
    static const int NoBorders          = SBT_NOBORDERS;
    static const int Popout             = SBT_POPOUT;
    static const int RightToLeftReading = SBT_RTLREADING;
    static const int OwnerDraw          = SBT_OWNERDRAW;
  };
  void SetTextAt( int index, const std::string& text, int style = TextStyle::Normal );

  void SetSimpleMode( bool flag );
  void SetTextAsSimpleMode( const std::string& text, int style = TextStyle::Normal );
};
