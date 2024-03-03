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
namespace TtMessageBox {
  struct Icon {
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

  struct DefaultButton {
    static const unsigned int FIRST  = MB_DEFBUTTON1;
    static const unsigned int SECOND = MB_DEFBUTTON2;
    static const unsigned int THIRD  = MB_DEFBUTTON3;
    static const unsigned int FOURTH = MB_DEFBUTTON4;
  };

  struct Result {
    static const int ABORT  = IDABORT;
    static const int CANCEL = IDCANCEL;
    static const int IGNORE = IDIGNORE;
    static const int NO     = IDNO;
    static const int OK     = IDOK;
    static const int RETRY  = IDRETRY;
    static const int YES    = IDYES;
  };

  // -- Origin -----------------------------------------------------------
  class Origin {
  private:
    static std::map<DWORD, Origin*> hook_table;
    static std::map<DWORD, WNDPROC> procedure_table;

    static LRESULT CALLBACK Hook( int code, WPARAM w_param, LPARAM l_param );
    static LRESULT CALLBACK Procedure( HWND handle, UINT msg, WPARAM w_param, LPARAM l_param );

  public:
    explicit Origin( unsigned int type, unsigned int icon );

    unsigned int GetType( void ) const;

    unsigned int GetIcon( void ) const;
    void SetIcon( unsigned int icon );

    unsigned int GetDefaultButton( void ) const;
    void SetDefaultButton( unsigned int button );

    int ShowDialog( void );
    int ShowDialog( TtWindow& parent );

  private:
    virtual int CallSystemCall( void ) = 0;

  protected:
    unsigned int type_;
    unsigned int icon_;
    unsigned int default_button_;

    HHOOK     hook_;
    TtWindow* parent_;
  };

  // -- Base -------------------------------------------------------------
  template <class STRING_TYPE>
  class Base : public Origin {
  public:
    using StringType = STRING_TYPE;

  public:
    explicit Base( unsigned int       type,
                   const STRING_TYPE& message,
                   const STRING_TYPE& caption,
                   unsigned int       icon );

    const STRING_TYPE& GetMessage( void ) const;
    void SetMessage( const STRING_TYPE& message );
    void AppendMessage( const STRING_TYPE& message );
    TtString::Appender<STRING_TYPE> AppendMessage( void );

    const STRING_TYPE& GetCaption( void ) const;
    void SetCaption( const STRING_TYPE& caption );

    virtual int CallSystemCall( void ) override;

  private:
    STRING_TYPE  message_;
    STRING_TYPE  caption_;
  };

  // -- ANSI or UTF16 -----
  using ANSIBox = Base<std::string>;
  using UTF16Box = Base<std::wstring>;

  template <unsigned int type, class BASE_TYPE>
  class WithType : public BASE_TYPE {
    using StringType = BASE_TYPE::StringType;

  public:
    static int Show( const StringType& message,
                     const StringType& caption = StringType(),
                     unsigned int      icon    = TtMessageBox::Icon::NONE ) {
      return WithType<type, BASE_TYPE>( message, caption, icon ).ShowDialog();
    }

    static int Show( TtWindow&         parent,
                     const StringType& message,
                     const StringType& caption = StringType(),
                     unsigned int      icon    = TtMessageBox::Icon::NONE ) {
      return WithType<type, BASE_TYPE>( message, caption, icon ).ShowDialog( parent );
    }

  public:
    explicit WithType( const StringType& message = StringType(),
                       const StringType& caption = StringType(),
                       unsigned int       icon    = TtMessageBox::Icon::NONE ) :
    BASE_TYPE( type, message, caption, icon ) {}
  };

  namespace ANSI {
    using Ok               = WithType<MB_OK,               ANSIBox>;
    using OkCancel         = WithType<MB_OKCANCEL,         ANSIBox>;
    using RetryCancel      = WithType<MB_RETRYCANCEL,      ANSIBox>;
    using YesNo            = WithType<MB_YESNO,            ANSIBox>;
    using YesNoCancel      = WithType<MB_YESNOCANCEL,      ANSIBox>;
    using AbortRetryIgnore = WithType<MB_ABORTRETRYIGNORE, ANSIBox>;
  }

  namespace UTF16 {
    using Ok               = WithType<MB_OK,               UTF16Box>;
    using OkCancel         = WithType<MB_OKCANCEL,         UTF16Box>;
    using RetryCancel      = WithType<MB_RETRYCANCEL,      UTF16Box>;
    using YesNo            = WithType<MB_YESNO,            UTF16Box>;
    using YesNoCancel      = WithType<MB_YESNOCANCEL,      UTF16Box>;
    using AbortRetryIgnore = WithType<MB_ABORTRETRYIGNORE, UTF16Box>;
  }

  using Ok               = ANSI::Ok;
  using OkCancel         = ANSI::OkCancel;
  using RetryCancel      = ANSI::RetryCancel;
  using YesNo            = ANSI::YesNo;
  using YesNoCancel      = ANSI::YesNoCancel;
  using AbortRetryIgnore = ANSI::AbortRetryIgnore;
}

// å›ä∑ê´ÇÃà◊
using TtMessageBoxOk               = TtMessageBox::Ok;
using TtMessageBoxOkCancel         = TtMessageBox::OkCancel;
using TtMessageBoxRetryCancel      = TtMessageBox::RetryCancel;
using TtMessageBoxYesNo            = TtMessageBox::YesNo;
using TtMessageBoxYesNoCancel      = TtMessageBox::YesNoCancel;
using TtMessageBoxAbortRetryIgnore = TtMessageBox::AbortRetryIgnore;
