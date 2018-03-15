// tt_process.cpp

#include "ttl_define.h"
#include "tt_exception.h"
#include "tt_string.h"

#include "tt_process.h"

#pragma comment(lib, "kernel32.lib")


// -- TtPipe -------------------------------------------------------------
TtPipe::Handle
TtPipe::Handle::GetStandardInputHandle( void )
{
  return Handle( ::GetStdHandle( STD_INPUT_HANDLE ) );
}

TtPipe::Handle
TtPipe::Handle::GetStandardOutputHandle( void )
{
  return Handle( ::GetStdHandle( STD_OUTPUT_HANDLE ) );
}

TtPipe::Handle
TtPipe::Handle::GetStandardErrorHandle( void )
{
  return Handle( ::GetStdHandle( STD_ERROR_HANDLE ) );
}


TtPipe::Handle::Handle( HANDLE handle ) :
handle_( handle )
{
}


HANDLE
TtPipe::Handle::GetWindowsHandle( void )
{
  return handle_;
}


void
TtPipe::Handle::Close( void )
{
  if ( handle_ != INVALID_HANDLE_VALUE ) {
    ::CloseHandle( handle_ );
    handle_ = INVALID_HANDLE_VALUE;
  }
}

void
TtPipe::Handle::SetInherit( bool inherit )
{
  Handle tmp = this->Duplicate( inherit );
  ::CloseHandle( handle_ );
  handle_ = tmp.handle_;
}

TtPipe::Handle
TtPipe::Handle::Duplicate( bool inherit )
{
  HANDLE tmp;
  BOOL ret = ::DuplicateHandle( GetCurrentProcess(), handle_,
                                GetCurrentProcess(), &tmp,
                                0, inherit ? TRUE : FALSE ,
                                DUPLICATE_SAME_ACCESS );
  if ( ret == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::DuplicateHandle ) );
  }
  return Handle( tmp );
}


TtPipe::TtPipe( bool inherit_handle, unsigned int buffer_size ) :
read_pipe_( INVALID_HANDLE_VALUE ),
write_pipe_( INVALID_HANDLE_VALUE )
{
  SECURITY_ATTRIBUTES sa = {0};
  sa.nLength = sizeof( sa );
  sa.lpSecurityDescriptor = NULL;
  sa.bInheritHandle = inherit_handle ? TRUE : FALSE;

  if ( ::CreatePipe( &read_pipe_.handle_, &write_pipe_.handle_, &sa, buffer_size ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::CreatePipe ) );
  }
}


TtPipe::Handle&
TtPipe::GetReadPipeHandle( void )
{
  return read_pipe_;
}

TtPipe::Handle&
TtPipe::GetWritePipeHandle( void )
{
  return write_pipe_;
}

// -- TtProcess::Priority ------------------------------------------------
template <>
TtEnumTable<TtProcess::Priority>::TtEnumTable( void ) {
#define REGISTER( NAME ) this->Register( TtProcess::Priority::NAME, #NAME )
  REGISTER( NORMAL );
  REGISTER( IDLE );
  REGISTER( HIGH );
  REGISTER( REALTIME );
  REGISTER( BELOW_NORMAL );
  REGISTER( ABOVE_NORMAL );
#undef REGISTER
}

template <>
TtEnumTable<TtProcess::Priority>
TtEnumBase<TtProcess::Priority>::TABLE;


// -- TtProcess::CreateInfo ----------------------------------------------
TtProcess::CreateInfo::CreateInfo( void ) :
CreateInfo( "" )
{
}


TtProcess::CreateInfo::CreateInfo( const std::string& filename ) :
filename_( filename ),
arguments_(),
current_directory_( "." ),
inherit_handles_( false ),
use_standard_handles_( false ),
standard_input_( TtPipe::Handle::GetStandardInputHandle() ),
standard_output_( TtPipe::Handle::GetStandardOutputHandle() ),
standard_error_( TtPipe::Handle::GetStandardErrorHandle() ),
create_new_process_group_( false ),
priority_( Priority::NORMAL ),
show_state_( TtWindow::ShowState::SHOW ),
use_search_path_( true ),
create_suspended_( false ),
create_new_console_( false )
{
}


// -- TtProcess --------------------------------------------------------
DWORD
TtProcess::GetCurrentProcessID( void )
{
  return ::GetCurrentProcessId();
}

TtProcess::TtProcess( void ) :
process_handle_( INVALID_HANDLE_VALUE ),
process_id_( 0 ),
thread_handle_( INVALID_HANDLE_VALUE ),
thread_id_( 0 )
{
}

TtProcess::~TtProcess()
{
  if ( process_handle_ != INVALID_HANDLE_VALUE ) {
    ::CloseHandle( process_handle_ );
    ::CloseHandle( thread_handle_ );
  }
}

void
TtProcess::Create( CreateInfo& info )
{
  SECURITY_ATTRIBUTES sa = {0};
  sa.nLength = sizeof( sa );

  STARTUPINFO startup_info = {};
  startup_info.cb = sizeof( startup_info );
  startup_info.dwFlags |= STARTF_USESHOWWINDOW;
  startup_info.wShowWindow = static_cast<WORD>( info.show_state_ );
  if ( info.use_standard_handles_ ) {
    startup_info.dwFlags |= STARTF_USESTDHANDLES;
    startup_info.hStdInput  = info.standard_input_.GetWindowsHandle();
    startup_info.hStdOutput = info.standard_output_.GetWindowsHandle();
    startup_info.hStdError  = info.standard_error_.GetWindowsHandle();
  }

  PROCESS_INFORMATION proc = {INVALID_HANDLE_VALUE};

  TtString::SharedString tmp_args( "\"" + info.filename_ + "\" " + info.arguments_ );

  int ret = ::CreateProcess(
    info.use_search_path_ ? nullptr : info.filename_.c_str(),
    tmp_args.GetPointer(),
    nullptr,
    nullptr,
    info.inherit_handles_ ? TRUE : FALSE,
    info.priority_.ToInteger() |
    (info.create_new_process_group_ ? CREATE_NEW_PROCESS_GROUP : 0) |
    (info.create_suspended_         ? CREATE_SUSPENDED         : 0) |
    (info.create_new_console_       ? CREATE_NEW_CONSOLE       : 0),
    nullptr,
    info.current_directory_.c_str(),
    &startup_info,
    &proc );
  if ( ret == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::CreateProcess ) );
  }
  process_handle_ = proc.hProcess;
  thread_handle_  = proc.hThread;
  process_id_     = proc.dwProcessId;
  thread_id_      = proc.dwThreadId;
}

void
TtProcess::Start( void )
{
  if ( ::ResumeThread( thread_handle_ ) == -1 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::ResumeThread ) );
  }
}


void
TtProcess::Wait( void )
{
  if ( process_handle_ == INVALID_HANDLE_VALUE ) {
    throw TtInvalidOperationException( typeid( *this ) );
  }
  if ( ::WaitForSingleObject( process_handle_, INFINITE ) != WAIT_OBJECT_0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::WaitForSingleObject ) );
  }

}

void
TtProcess::Terminate( UINT exit_code )
{
  if ( ::TerminateProcess( process_handle_, exit_code ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::TerminateProcess ) );
  }
}

bool
TtProcess::IsCreated( void ) const
{
  return (process_handle_ != INVALID_HANDLE_VALUE);
}

bool
TtProcess::HasExited( void ) const
{
  if ( process_handle_ == INVALID_HANDLE_VALUE ) {
    throw TtInvalidOperationException( typeid( *this ) );
  }
  DWORD ret;
  if ( ::GetExitCodeProcess( process_handle_, &ret ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::GetExitCodeProcess ) );
  }
  return ret != STILL_ACTIVE;
}


DWORD
TtProcess::GetExitCode( void ) const
{
  if ( process_handle_ == INVALID_HANDLE_VALUE ) {
    throw TtInvalidOperationException( typeid( *this ) );
  }
  DWORD ret;
  if ( ::GetExitCodeProcess( process_handle_, &ret ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::GetExitCodeProcess ) );
  }
  if ( ret == STILL_ACTIVE ) {
    throw TtInvalidOperationException( typeid( *this ) );
  }
  return ret;
}


HANDLE TtProcess::GetProcessHandle( void ) const
{
  return process_handle_;
}

DWORD
TtProcess::GetProcessID( void ) const
{
  if ( process_handle_ == INVALID_HANDLE_VALUE ) {
    throw TtInvalidOperationException( typeid( *this ) );
  }
  return process_id_;
}

HANDLE
TtProcess::GetThreadHandle( void ) const
{
  return thread_handle_;
}

DWORD
TtProcess::GetThreadID( void ) const
{
  if ( process_handle_ == INVALID_HANDLE_VALUE ) {
    throw TtInvalidOperationException( typeid( *this ) );
  }
  return thread_id_;
}
