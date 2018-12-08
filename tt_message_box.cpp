// tt_message_box.cpp

#include "tt_thread.h"

#include "tt_message_box.h"

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")


// -- TtMessageBox -------------------------------------------------------
TtMessageBox::TtMessageBox( unsigned int type,
                            const std::string& message,
                            const std::string& caption,
                            unsigned int       icon ) :
type_( type ),
message_( message ),
caption_( caption ),
icon_( icon ),
default_button_( TtMessageBox::DefautButton::FIRST )
{
}

unsigned int
TtMessageBox::GetType( void ) const
{
  return type_;
}

const std::string&
TtMessageBox::GetMessage( void ) const
{
  return message_;
}

void
TtMessageBox::SetMessage( const std::string& message )
{
  message_.assign( message );
}

void
TtMessageBox::AppendMessage( const std::string& message )
{
  message_.append( message );
}

TtString::Appender
TtMessageBox::AppendMessage( void )
{
  return TtString::Appender( message_ );
}

const std::string&
TtMessageBox::GetCaption( void ) const
{
  return caption_;
}

void
TtMessageBox::SetCaption( const std::string& caption )
{
  caption_.assign( caption );
}

unsigned int
TtMessageBox::GetIcon( void ) const
{
  return icon_;
}

void
TtMessageBox::SetIcon( unsigned int icon )
{
  icon_ = icon;
}

unsigned int
TtMessageBox::GetDefaultButton( void ) const
{
  return default_button_;
}

void
TtMessageBox::SetDefaultButton( unsigned int button )
{
  default_button_ = button;
}

int
TtMessageBox::ShowDialog( void )
{
  return ::MessageBox( nullptr, message_.c_str(), caption_.c_str(),
                       type_ | icon_ | default_button_ );
}



std::map<DWORD, TtMessageBox*>
TtMessageBox::hook_table;

std::map<DWORD, WNDPROC>
TtMessageBox::procedure_table;

LRESULT CALLBACK
TtMessageBox::Hook( int code, WPARAM w_param, LPARAM l_param )
{
  TtMessageBox* box = hook_table[TtThread::GetCurrentThreadID()];
  const CWPRETSTRUCT* p = reinterpret_cast<const CWPRETSTRUCT*>( l_param );

  if ( p->message == WM_NCCREATE ) {
    ::UnhookWindowsHookEx( box->hook_ );
    box->hook_ = nullptr;
    procedure_table[TtThread::GetCurrentThreadID()] = reinterpret_cast<WNDPROC>( ::GetClassLongPtr( p->hwnd, GCLP_WNDPROC ) );
    ::SetClassLongPtr( p->hwnd, GCLP_WNDPROC, ULONG_PTR( Procedure ) );
  }
  return ::CallNextHookEx( box->hook_, code, w_param, l_param );
}

LRESULT CALLBACK
TtMessageBox::Procedure( HWND handle, UINT msg, WPARAM w_param, LPARAM l_param )
{
  TtMessageBox* box  = hook_table[TtThread::GetCurrentThreadID()];
  WNDPROC       proc = procedure_table[TtThread::GetCurrentThreadID()];

  LONG ret = ::CallWindowProc( proc, handle, msg, w_param, l_param );
  if ( msg == WM_INITDIALOG ) {
    ::SetClassLongPtr( handle, GCLP_WNDPROC, ULONG_PTR( proc ) );
    TtExtraordinarilyWindow( handle, *(box->parent_) ).SetCenterRelativeToParent();
  }
  return ret;
}

int
TtMessageBox::ShowDialog( TtWindow& parent )
{
  hook_ = ::SetWindowsHookEx( WH_CALLWNDPROCRET,
                              Hook,
                              ::GetModuleHandle( nullptr ),
                              TtThread::GetCurrentThreadID() );
  parent_ = &parent;
  hook_table[TtThread::GetCurrentThreadID()] = this;
  return ::MessageBox( parent.GetHandle(), message_.c_str(), caption_.c_str(),
                       type_ | icon_ | default_button_ );
}


#ifdef TT_MAKE_TEMPLATE_INSTANCE_
template class TtMessageBoxWithType<MB_OK>;
template class TtMessageBoxWithType<MB_OKCANCEL>;
template class TtMessageBoxWithType<MB_RETRYCANCEL>;
template class TtMessageBoxWithType<MB_YESNO>;
template class TtMessageBoxWithType<MB_YESNOCANCEL>;
template class TtMessageBoxWithType<MB_ABORTRETRYIGNORE>;
#endif
