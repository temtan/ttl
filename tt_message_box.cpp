// tt_message_box.cpp

#include "tt_thread.h"

#include "tt_message_box.h"

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")


// -- TtMessageBox::Origin -----------------------------------------------
TtMessageBox::Origin::Origin( unsigned int type, unsigned int icon ) :
type_( type ),
icon_( icon ),
default_button_( TtMessageBox::DefaultButton::FIRST )
{
}

unsigned int
TtMessageBox::Origin::GetType( void ) const
{
  return type_;
}

unsigned int
TtMessageBox::Origin::GetIcon( void ) const
{
  return icon_;
}

void
TtMessageBox::Origin::SetIcon( unsigned int icon )
{
  icon_ = icon;
}

unsigned int
TtMessageBox::Origin::GetDefaultButton( void ) const
{
  return default_button_;
}

void
TtMessageBox::Origin::SetDefaultButton( unsigned int button )
{
  default_button_ = button;
}

int
TtMessageBox::Origin::ShowDialog( void )
{
  return this->ShowDialog( TtExtraordinarilyWindow::Null );
}


std::map<DWORD, TtMessageBox::Origin*>
TtMessageBox::Origin::hook_table;

std::map<DWORD, WNDPROC>
TtMessageBox::Origin::procedure_table;

LRESULT CALLBACK
TtMessageBox::Origin::Hook( int code, WPARAM w_param, LPARAM l_param )
{
  TtMessageBox::Origin* box = hook_table[TtThread::GetCurrentThreadID()];
  const CWPRETSTRUCT* p = reinterpret_cast<const CWPRETSTRUCT*>( l_param );

  if ( p->message == WM_NCCREATE ) {
    ::UnhookWindowsHookEx( box->hook_ );
    box->hook_ = nullptr;
    procedure_table[TtThread::GetCurrentThreadID()] = reinterpret_cast<WNDPROC>( ::GetWindowLongPtr( p->hwnd, GWLP_WNDPROC ) );
    ::SetWindowLongPtr( p->hwnd, GWLP_WNDPROC, ULONG_PTR( Procedure ) );
  }
  return ::CallNextHookEx( box->hook_, code, w_param, l_param );
}

LRESULT CALLBACK
TtMessageBox::Origin::Procedure( HWND handle, UINT msg, WPARAM w_param, LPARAM l_param )
{
  TtMessageBox::Origin* box = hook_table[TtThread::GetCurrentThreadID()];
  WNDPROC proc = procedure_table[TtThread::GetCurrentThreadID()];

  LRESULT ret = ::CallWindowProc( proc, handle, msg, w_param, l_param );
  if ( msg == WM_INITDIALOG ) {
    if ( box->parent_->GetHandle() ) {
      ::SetWindowLongPtr( handle, GWLP_WNDPROC, ULONG_PTR( proc ) );
      TtExtraordinarilyWindow( handle, *(box->parent_) ).SetCenterRelativeToParent();
    }
  }
  return ret;
}

int
TtMessageBox::Origin::ShowDialog( TtWindow& parent )
{
  hook_ = ::SetWindowsHookEx( WH_CALLWNDPROCRET,
                              Hook,
                              ::GetModuleHandle( nullptr ),
                              TtThread::GetCurrentThreadID() );
  parent_ = &parent;
  hook_table[TtThread::GetCurrentThreadID()] = this;
  return this->CallSystemCall();
}


// -- TtMessageBox::Base -------------------------------------------------
template <class STRING_TYPE>
TtMessageBox::Base<STRING_TYPE>::Base( unsigned int       type,
                                       const STRING_TYPE& message,
                                       const STRING_TYPE& caption,
                                       unsigned int       icon ) :
Origin( type, icon ),
message_( message ),
caption_( caption )
{
}

template <class STRING_TYPE>
const STRING_TYPE&
TtMessageBox::Base<STRING_TYPE>::GetMessage( void ) const
{
  return message_;
}

template <class STRING_TYPE>
void
TtMessageBox::Base<STRING_TYPE>::SetMessage( const STRING_TYPE& message )
{
  message_.assign( message );
}

template <class STRING_TYPE>
void
TtMessageBox::Base<STRING_TYPE>::AppendMessage( const STRING_TYPE& message )
{
  message_.append( message );
}

template <class STRING_TYPE>
TtString::Appender<STRING_TYPE>
TtMessageBox::Base<STRING_TYPE>::AppendMessage( void )
{
  return TtString::Appender( message_ );
}

template <class STRING_TYPE>
const STRING_TYPE&
TtMessageBox::Base<STRING_TYPE>::GetCaption( void ) const
{
  return caption_;
}

template <class STRING_TYPE>
void
TtMessageBox::Base<STRING_TYPE>::SetCaption( const STRING_TYPE& caption )
{
  caption_.assign( caption );
}


template <>
int
TtMessageBox::Base<std::string>::CallSystemCall( void )
{
  return ::MessageBox( parent_->GetHandle(), message_.c_str(), caption_.c_str(),
                       type_ | icon_ | default_button_ );
}

template <>
int
TtMessageBox::Base<std::wstring>::CallSystemCall( void )
{
  return ::MessageBoxW( parent_->GetHandle(), message_.c_str(), caption_.c_str(),
                       type_ | icon_ | default_button_ );
}

template class TtMessageBox::Base<std::string>;
template class TtMessageBox::Base<std::wstring>;

#ifdef TT_MAKE_TEMPLATE_INSTANCE_
template class TtMessageBox::ANSI::Ok;
template class TtMessageBox::ANSI::OkCancel;
template class TtMessageBox::ANSI::RetryCancel;
template class TtMessageBox::ANSI::YesNo;
template class TtMessageBox::ANSI::YesNoCancel;
template class TtMessageBox::ANSI::AbortRetryIgnore;
template class TtMessageBox::UTF16::Ok;
template class TtMessageBox::UTF16::OkCancel;
template class TtMessageBox::UTF16::RetryCancel;
template class TtMessageBox::UTF16::YesNo;
template class TtMessageBox::UTF16::YesNoCancel;
template class TtMessageBox::UTF16::AbortRetryIgnore;
#endif
