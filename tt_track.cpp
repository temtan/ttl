// tt_track.cpp

#include "ttl_define.h"
#include "tt_window.h"

#include "tt_track.h"


int
TtTrack::GetTrackPosition( void )
{
  return this->SendMessage( TBM_GETPOS );
}

void
TtTrack::SetTrackPosition( int position )
{
  this->SendMessage( TBM_SETPOS, TRUE, position );
}

int
TtTrack::GetMinimum( void )
{
  return this->SendMessage( TBM_GETRANGEMIN );
}

void
TtTrack::SetMinimum( int min )
{
  this->SendMessage( TBM_SETRANGEMIN, TRUE, min );
}

int
TtTrack::GetMaximum( void )
{
  return this->SendMessage( TBM_GETRANGEMAX );
}

void
TtTrack::SetMaximum( int max )
{
  this->SendMessage( TBM_SETRANGEMAX, TRUE, max );
}

void
TtTrack::SetRange( int min, int max )
{
  this->SetMinimum( min );
  this->SetMaximum( max );
}

unsigned int
TtTrack::GetPageSize( void )
{
  return this->SendMessage( TBM_GETPAGESIZE );
}

void
TtTrack::SetPageSize( unsigned int page_size )
{
  this->SendMessage( TBM_SETPAGESIZE, 0, static_cast<LPARAM>( page_size ) );
}

unsigned int
TtTrack::GetLineSize( void )
{
  return this->SendMessage( TBM_GETLINESIZE );
}

void
TtTrack::SetLineSize( unsigned int line_size )
{
  this->SendMessage( TBM_SETLINESIZE, 0, static_cast<LPARAM>( line_size ) );
}

void
TtTrack::SetTickFrequency( unsigned int frequency )
{
  this->SendMessage( TBM_SETTICFREQ, frequency );
}

void
TtTrack::ClearTick( void )
{
  this->SendMessage( TBM_CLEARTICS, TRUE );
}
