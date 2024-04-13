// tt_thread.h

#pragma once

#include <functional>

#include "tt_windows_h_include.h"

#include "tt_message_queue.h"


// -- TtThread -----------------------------------------------------------
class TtThread {
private:
  static unsigned int __stdcall EntryPoint( void* arg );

public:
  static unsigned int GetCurrentThreadID( void );

  explicit TtThread( void );

  virtual ~TtThread();

  virtual void Open( bool immediately_start = true );
  unsigned int Suspend( void );
  unsigned int Resume( void );
  void Join( void );
  void Terminate( DWORD exit_code );
  void Close( void );

  bool IsValid( void ) const;
  bool IsInvalid( void ) const;
  bool HasExited( void ) const;

  DWORD GetExitCode( void ) const;
  DWORD GetThreadID( void ) const;
  HANDLE GetThreadHandle( void ) const;

private:
  virtual unsigned int Run( void ) = 0;

private:
  HANDLE thread_handle_;
  DWORD  thread_id_;
};


// -- TtFunctionThread ---------------------------------------------------
class TtFunctionThread : public TtThread {
public:
  explicit TtFunctionThread( void );
  explicit TtFunctionThread( std::function<unsigned int ( void )> function );
  explicit TtFunctionThread( std::function<void ( void )> function );

  void SetFunction( std::function<unsigned int ( void )> function );
  void SetFunction( std::function<void ( void )> function );
  std::function<unsigned int ( void )> GetFunction( void ) const;

  virtual void Open( bool immediately_start = true ) override;

private:
  virtual unsigned int Run( void ) override;

private:
  std::function<unsigned int ( void )> function_;
};


// -- TtThreadPool -------------------------------------------------------
class TtThreadPool {
public:
  explicit TtThreadPool( void );
  ~TtThreadPool( void );

  void Close( void );

  template <class TYPE>
  void StartWork( void (* function )( TYPE ), TYPE parameter ) {
    using Parameter = std::pair<decltype( function ), TYPE>;
    auto callback = [] ( PTP_CALLBACK_INSTANCE, void* tmp, PTP_WORK ) -> void {
      auto parameter = static_cast<Parameter*>( tmp );
      parameter->first( parameter->second );
      delete parameter;
    };
    auto tmp = new Parameter( function, parameter );
    auto work = ::CreateThreadpoolWork( callback, tmp, &callback_environ_ );
    if ( NOT( work ) ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::CreateThreadpoolWork ) );
    }
    ::SubmitThreadpoolWork( work );
  }

  using DirectFunction = void (*)( PTP_CALLBACK_INSTANCE, void*, PTP_WORK );
  void StartWorkDirect( DirectFunction function, void* parameter );

  void WaitAndCloseAllThreads( void );

private:
  PTP_POOL            pool_;
  PTP_CLEANUP_GROUP   cleanup_group_;
  TP_CALLBACK_ENVIRON callback_environ_;
};
