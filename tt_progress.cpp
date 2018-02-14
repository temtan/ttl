// tt_progress.cpp

#include "ttl_define.h"

#include "tt_progress.h"


int
TtProgress::GetMaximum( void )
{
  return this->SendMessage( PBM_GETRANGE );
}

int
TtProgress::SetMaximum( unsigned short max )
{
  return this->SendMessage( PBM_SETRANGE, 0, ( static_cast<int>( max ) & 0x0000FFFF ) << 16 );
}

int
TtProgress::GetAmount( void )
{
  return this->SendMessage( PBM_GETPOS );
}

int
TtProgress::SetAmount( unsigned short position )
{
  return this->SendMessage( PBM_SETPOS, position );
}

int
TtProgress::SetStep( int step )
{
  return this->SendMessage( PBM_SETSTEP, step );
}

int
TtProgress::StepUp( int amount )
{
  return this->SendMessage( PBM_DELTAPOS, amount );
}

int
TtProgress::StepIt( void )
{
  return this->SendMessage( PBM_STEPIT );
}
