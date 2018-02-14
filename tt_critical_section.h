// tt_critical_section.h

#pragma once

#include <functional>

#include "tt_windows_h_include.h"


// -- TtCriticalSection --------------------------------------------------
class TtCriticalSection {
public:
  explicit TtCriticalSection( void );
  ~TtCriticalSection();

  void Enter( void );
  void Leave( void );

  void EnterExecute( std::function<void ( void )> function );

  class Lock {
  public:
    explicit Lock( TtCriticalSection& critical_section );
    ~Lock();
  private:
    TtCriticalSection& critical_section_;
  };

private:
  CRITICAL_SECTION lock_;
};
