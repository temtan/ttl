// tt_critical_section.cpp

#include "ttl_define.h"

#include "tt_critical_section.h"

#pragma comment(lib, "kernel32.lib")


// -- TtCriticalSection --------------------------------------------------
TtCriticalSection::TtCriticalSection( void ) :
lock_()
{
  ::InitializeCriticalSection( &lock_ );
}

TtCriticalSection::~TtCriticalSection()
{
  ::DeleteCriticalSection( &lock_ );
}

void
TtCriticalSection::Enter( void )
{
  ::EnterCriticalSection( &lock_ );
}

void
TtCriticalSection::Leave( void )
{
  ::LeaveCriticalSection( &lock_ );
}


void
TtCriticalSection::EnterExecute( std::function<void ( void )> function )
{
  this->Enter();
  function();
  this->Leave();
}


TtCriticalSection::Lock::Lock( TtCriticalSection& critical_section ) :
critical_section_( critical_section )
{
  critical_section_.Enter();
}

TtCriticalSection::Lock::~Lock()
{
  critical_section_.Leave();
}
