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
  this->Close();
}


void
TtThread::Open( bool immediately_start )
{
  if ( this->IsValid() ) {
    throw TtInvalidOperationException( typeid( *this ) );
  }
  unsigned int tmp_id;
  uintptr_t tmp_handle = _beginthreadex( nullptr, 0, EntryPoint, this, immediately_start ? 0 : CREATE_SUSPENDED, &tmp_id );
  if ( tmp_handle == 0 ) {
    throw TT_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( _beginthreadex ), errno );
  }
  thread_handle_ = reinterpret_cast<HANDLE>( tmp_handle );
  thread_id_ = tmp_id;
}


unsigned int
TtThread::Suspend( void )
{
  if ( this->IsInvalid() ) {
    throw TtInvalidOperationException( typeid( *this ) );
  }
  DWORD ret = ::SuspendThread( thread_handle_ );
  if ( ret == -1 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::SuspendThread ) );
  }
  return ret;
}

unsigned int
TtThread::Resume( void )
{
  if ( this->IsInvalid() ) {
    throw TtInvalidOperationException( typeid( *this ) );
  }
  DWORD ret = ::ResumeThread( thread_handle_ );
  if ( ret == -1 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::ResumeThread ) );
  }
  return ret;
}


void
TtThread::Join( void )
{
  if ( this->IsInvalid() ) {
    throw TtInvalidOperationException( typeid( *this ) );
  }
  if ( ::WaitForSingleObject( thread_handle_, INFINITE ) != WAIT_OBJECT_0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::WaitForSingleObject ) );
  }
}


void
TtThread::Terminate( DWORD exit_code )
{
  if ( this->IsInvalid() ) {
    throw TtInvalidOperationException( typeid( *this ) );
  }
  if ( ::TerminateThread( thread_handle_, exit_code ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::TerminateThread ) );
  }
}

void
TtThread::Close( void )
{
  if ( this->IsValid() ) {
    ::CloseHandle( thread_handle_ );
    thread_handle_ = INVALID_HANDLE_VALUE;
    thread_id_ = 0;
  }
}


bool
TtThread::IsValid( void ) const
{
  return thread_handle_ != INVALID_HANDLE_VALUE;
}

bool
TtThread::IsInvalid( void ) const
{
  return NOT( this->IsValid() );
}


bool
TtThread::HasExited( void ) const
{
  if ( this->IsInvalid() ) {
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
  if ( this->IsInvalid() ) {
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
  if ( this->IsInvalid() ) {
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
TtFunctionThread::TtFunctionThread( void ) :
function_( nullptr )
{
}


TtFunctionThread::TtFunctionThread( std::function<unsigned int ( void )> function ) :
function_( function )
{
}

TtFunctionThread::TtFunctionThread( std::function<void ( void )> function ) :
function_( [function] ( void ) -> unsigned int { function(); return 0; } )
{
}

void
TtFunctionThread::SetFunction( std::function<unsigned int ( void )> function )
{
  function_ = function;
}

void
TtFunctionThread::SetFunction( std::function<void ( void )> function )
{
  function_ = [function] ( void ) -> unsigned int { function(); return 0; };
}


std::function<unsigned int ( void )>
TtFunctionThread::GetFunction( void ) const
{
  return function_;
}


void
TtFunctionThread::Open( bool immediately_start )
{
  if ( NOT( function_ ) ) {
    throw TtInvalidOperationException( typeid( *this ) );
  }
  this->TtThread::Open( immediately_start );
}


unsigned int
TtFunctionThread::Run( void )
{
  return function_();
}


// -- TtThreadPool -------------------------------------------------------
TtThreadPool::TtThreadPool( void ) :
pool_( ::CreateThreadpool( NULL ) )
{
  InitializeThreadpoolEnvironment( &callback_environ_ );

  ::SetThreadpoolThreadMaximum( pool_, 1 );
  auto ret = ::SetThreadpoolThreadMinimum( pool_, 1 );
  if ( NOT( ret ) ) {
    ::CloseThreadpool( pool_ );
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::SetThreadpoolThreadMinimum ) );
  }

  cleanup_group_ = ::CreateThreadpoolCleanupGroup();
  if ( NOT( cleanup_group_ ) ) {
    ::CloseThreadpool( pool_ );
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::CreateThreadpoolCleanupGroup ) );
  }

  ::SetThreadpoolCallbackPool( &callback_environ_, pool_ );
  ::SetThreadpoolCallbackCleanupGroup( &callback_environ_, cleanup_group_, NULL );
}


TtThreadPool::~TtThreadPool( void )
{
  this->Close();
}


void
TtThreadPool::StartWorkDirect( DirectFunction function, void* parameter )
{
  auto work = ::CreateThreadpoolWork( function, parameter, &callback_environ_ );
  if ( NOT( work ) ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::CreateThreadpoolWork ) );
  }
  ::SubmitThreadpoolWork( work );
}


void
TtThreadPool::WaitAndCloseAllThreads( void )
{
  ::CloseThreadpoolCleanupGroupMembers( cleanup_group_, FALSE, NULL );
}


void
TtThreadPool::Close( void )
{
  ::CloseThreadpoolCleanupGroup( cleanup_group_ );
  ::CloseThreadpool( pool_ );
}
