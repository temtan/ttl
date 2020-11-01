// tt_thread.cpp

#include <process.h>
#include <errno.h>

#include <stdexcept>

#include "ttl_define.h"
#include "tt_exception.h"

#include "tt_thread.h"

#pragma comment(lib, "kernel32.lib")


unsigned int __stdcall
TtThread::EntryPoint( void* arg )
{
  _endthreadex( reinterpret_cast<TtThread*>( arg )->Run() );
  // dummy
  return 0;
}


unsigned int
TtThread::GetCurrentThreadID( void )
{
  return ::GetCurrentThreadId();
}


TtThread::TtThread( void ) :
thread_handle_( INVALID_HANDLE_VALUE ),
thread_id_( 0 )
{
}


TtThread::~TtThread()
{
  if ( thread_handle_ != INVALID_HANDLE_VALUE ) {
    ::CloseHandle( thread_handle_ );
  }
}


void
TtThread::Start( void )
{
  if ( thread_handle_ != INVALID_HANDLE_VALUE ) {
    throw TtInvalidOperationException( typeid( *this ) );
  }
  unsigned int tmp_id;
  uintptr_t tmp_handle = _beginthreadex( nullptr, 0, EntryPoint, this, 0, &tmp_id );
  if ( tmp_handle == 0 ) {
    throw TT_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( _beginthreadex ), errno );
  }
  thread_handle_ = reinterpret_cast<HANDLE>( tmp_handle );
  thread_id_ = tmp_id;
}


void
TtThread::Join( void )
{
  if ( thread_handle_ == INVALID_HANDLE_VALUE ) {
    throw TtInvalidOperationException( typeid( *this ) );
  }
  if ( ::WaitForSingleObject( thread_handle_, INFINITE ) != WAIT_OBJECT_0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::WaitForSingleObject ) );
  }
}


void
TtThread::Terminate( DWORD exit_code )
{
  if ( ::TerminateThread( thread_handle_, exit_code ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::TerminateThread ) );
  }
}


bool
TtThread::HasExited( void ) const
{
  if ( thread_handle_ == INVALID_HANDLE_VALUE ) {
    throw TtInvalidOperationException( typeid( *this ) );
  }
  DWORD ret;
  if ( ::GetExitCodeThread( thread_handle_, &ret ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::GetExitCodeThread ) );
  }
  return ret != STILL_ACTIVE;
}


DWORD
TtThread::GetExitCode( void ) const
{
  if ( thread_handle_ == INVALID_HANDLE_VALUE ) {
    throw TtInvalidOperationException( typeid( *this ) );
  }
  DWORD ret;
  if ( ::GetExitCodeThread( thread_handle_, &ret ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::GetExitCodeThread ) );
  }
  if ( ret == STILL_ACTIVE ) {
    throw TtInvalidOperationException( typeid( *this ) );
  }
  return ret;
}


DWORD
TtThread::GetThreadID( void ) const
{
  if ( thread_handle_ == INVALID_HANDLE_VALUE ) {
    throw TtInvalidOperationException( typeid( *this ) );
  }
  return thread_id_;
}

HANDLE
TtThread::GetThreadHandle( void ) const
{
  return thread_handle_;
}


// -- TtFunctionThread ---------------------------------------------------
TtFunctionThread::TtFunctionThread( std::function<unsigned int ( void )> function ) :
function_( function )
{
}

TtFunctionThread::TtFunctionThread( std::function<void ( void )> function ) :
function_( [function] ( void ) -> unsigned int { function(); return 0; } )
{
}


unsigned int
TtFunctionThread::Run( void )
{
  return function_();
}

