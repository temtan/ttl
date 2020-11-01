// tt_message_box.h

#pragma once

#include <string>
#include <map>

#include "tt_windows_h_include.h"

#include "tt_window.h"
#include "tt_string.h"

#pragma warning(push)
#  pragma warning(disable : 4574)
#  ifdef ERROR
#    undef ERROR
#  endif
#  ifdef IGNORE
#    undef IGNORE
#  endif
#pragma warning(pop)


// -- TtMessageBox -------------------------------------------------------
class TtMessageBox {
public:
  class Icon {
  public:
    static const unsigned int NONE        = 0;
    static const unsigned int EXCLAMATION = MB_ICONEXCLAMATION;
    static const unsigned int WARNING     = MB_ICONWARNING;
    static const unsigned int INFORMATION = MB_ICONINFORMATION;
    static const unsigned int ASTERISK    = MB_ICONASTERISK;
    static const unsigned int QUESTION    = MB_ICONQUESTION;
    static const unsigned int STOP        = MB_ICONSTOP;
    static const unsigned int ERROR       = MB_ICONERROR;
    static const unsigned int HAND        = MB_ICONHAND;
  };

  class DefaultButton {
  public:
    static const unsigned int FIRST  = MB_DEFBUTTON1;
    static const unsigned int SECOND = MB_DEFBUTTON2;
    static const unsigned int THIRD  = MB_DEFBUTTON3;
    static const unsigned int FOURTH = MB_DEFBUTTON4;
  };

  class Result {
  public:
    static const int ABORT  = IDABORT;
    static const int CANCEL = IDCANCEL;
    static const int IGNORE = IDIGNORE;
    static const int NO     = IDNO;
    static const int OK     = IDOK;
    static const int RETRY  = IDRETRY;
    static const int YES    = IDYES;
  };

private:
  static std::map<DWORD, TtMessageBox*> hook_table;
  static std::map<DWORD, WNDPROC>       procedure_table;

  static LRESULT CALLBACK Hook( int code, WPARAM w_param, LPARAM l_param );
  static LRESULT CALLBACK Procedure( HWND handle, UINT msg, WPARAM w_param, LPARAM l_param );

public:
  explicit TtMessageBox( unsigned int       type,
                         const std::string& message,
                         const std::string& caption,
                         unsigned int       icon );

  unsigned int GetType( void ) const;

  const std::string& GetMessage( void ) const;
  void SetMessage( const std::string& message );
  void AppendMessage( const std::string& message );
  TtString::Appender AppendMessage( void );

  const std::string& GetCaption( void ) const;
  void SetCaption( const std::string& caption );

  unsigned int GetIcon( void ) const;
  void SetIcon( unsigned int icon );

  unsigned int GetDefaultButton( void ) const;
  void SetDefaultButton( unsigned int button );

  int ShowDialog( void );
  int ShowDialog( TtWindow& parent );

private:
  unsigned int type_;
  std::string  message_;
  std::string  caption_;
  unsigned int icon_;
  unsigned int default_button_;

  HHOOK     hook_;
  TtWindow* parent_;
};

// -- TtMessageBoxWithType ----------------------------------------------
template <unsigned int type>
class TtMessageBoxWithType : public TtMessageBox {
public:
  static int Show( const std::string& message,
                   const std::string& caption = "",
                   unsigned int       icon    = TtMessageBox::Icon::NONE ) {
    return TtMessageBoxWithType<type>( message, caption, icon ).ShowDialog();
  }

  static int Show( TtWindow&          parent,
                   const std::string& message,
                   const std::string& caption = "",
                   unsigned int       icon    = TtMessageBox::Icon::NONE ) {
    return TtMessageBoxWithType<type>( message, caption, icon ).ShowDialog( parent );
  }

public:
  explicit TtMessageBoxWithType<type>( const std::string& message = "",
                                       const std::string& caption = "",
                                       unsigned int       icon    = TtMessageBox::Icon::NONE ) :
  TtMessageBox( type, message, caption, icon ) {}
};

using TtMessageBoxOk               = TtMessageBoxWithType<MB_OK>;
using TtMessageBoxOkCancel         = TtMessageBoxWithType<MB_OKCANCEL>;
using TtMessageBoxRetryCancel      = TtMessageBoxWithType<MB_RETRYCANCEL>;
using TtMessageBoxYesNo            = TtMessageBoxWithType<MB_YESNO>;
using TtMessageBoxYesNoCancel      = TtMessageBoxWithType<MB_YESNOCANCEL>;
using TtMessageBoxAbortRetryIgnore = TtMessageBoxWithType<MB_ABORTRETRYIGNORE>;
