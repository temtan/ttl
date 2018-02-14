// tt_time.cpp

#include <time.h>
#include "tt_windows_h_include.h"

#include "ttl_define.h"

#include "tt_time.h"

#pragma comment(lib, "kernel32.lib")


// -- TtTime -------------------------------------------------------------
TtTime
TtTime::GetNow( void )
{
  return TtTime();
}


TtTime::TtTime( void )
{
  SYSTEMTIME systemTime;
  ::GetLocalTime( &systemTime );

  serial_second_ = ::time( nullptr );
  year_          = systemTime.wYear;
  month_         = systemTime.wMonth;
  day_of_month_  = systemTime.wDay;
  hour_          = systemTime.wHour;
  minute_        = systemTime.wMinute;
  second_        = systemTime.wSecond;
  milli_second_  = systemTime.wMilliseconds;
}

INT64
TtTime::GetSerialSecond( void ) const
{
  return serial_second_;
}

unsigned int
TtTime::GetYear( void ) const
{
  return year_;
}

unsigned int
TtTime::GetMonth( void ) const
{
  return month_;
}

unsigned int
TtTime::GetDayOfMonth( void ) const
{
  return day_of_month_;
}

unsigned int
TtTime::GetHour( void ) const
{
  return hour_;
}

unsigned int
TtTime::GetMinute( void ) const
{
  return minute_;
}

unsigned int
TtTime::GetSecond( void ) const
{
  return second_;
}

unsigned int
TtTime::GetMilliSecond( void ) const
{
  return milli_second_;
}


std::string
TtTime::GetDateString( void ) const
{
  std::ostringstream os;
  os << year_ << "/" << month_ << "/" << day_of_month_;
  return os.str();
}

std::string
TtTime::GetTimeString( void ) const
{
  std::ostringstream os;
  os << hour_ << ":" << minute_ << ":" << second_;
  return os.str();
}

std::string
TtTime::GetDateTimeString( void ) const
{
  return this->GetDateString() + " " + this->GetTimeString();
}


double
TtTime::operator -( const TtTime& r ) const
{
  return (
    static_cast<double>( serial_second_ ) - static_cast<double>( r.serial_second_ ) +
    (static_cast<double>( milli_second_ ) - static_cast<double>( r.milli_second_ )) / 1000.0 );
}


// -- PerformanceCounter -------------------------------------------------
LONGLONG
PerformanceCounter::Count( Function function )
{
  return PerformanceCounter::Count( 1, function );
}

LONGLONG
PerformanceCounter::Count( unsigned int loop_count, Function function )
{
  PerformanceCounter counter;
  counter.Start();
  for ( unsigned int i = 0; i < loop_count; ++i ) {
    function();
  }
  return counter.GetElapsed();
}


PerformanceCounter::PerformanceCounter( bool auto_start ) :
start_( {0} )
{
  if ( auto_start ) {
    this->Start();
  }
}

void
PerformanceCounter::Start( void )
{
  ::QueryPerformanceCounter( &start_ );
}

LONGLONG
PerformanceCounter::GetElapsed( void )
{
  LARGE_INTEGER tmp;
  ::QueryPerformanceCounter( &tmp );
  return tmp.QuadPart - start_.QuadPart;
}
