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

  void Start( void );
  void Join( void );
  void Terminate( DWORD exit_code );

  bool HasExited( void ) const;

  DWORD GetExitCode( void ) const;
  DWORD GetThreadID( void ) const;

private:
  virtual unsigned int Run( void ) = 0;

private:
  HANDLE thread_handle_;
  DWORD  thread_id_;
};


// -- TtFunctionThread ---------------------------------------------------
class TtFunctionThread : public TtThread {
public:
  explicit TtFunctionThread( std::function<unsigned int ( void )> function );
  explicit TtFunctionThread( std::function<void ( void )> function );

private:
  virtual unsigned int Run( void ) override;

private:
  std::function<unsigned int ( void )> function_;
};
