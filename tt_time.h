// tt_time.h

#pragma once

#include <functional>

#include "tt_windows_h_include.h"


// -- TtTime -------------------------------------------------------------
class TtTime {
public:
  static TtTime GetNow( void );

public:
  explicit TtTime( void );

  INT64 GetSerialSecond( void ) const;

  unsigned int GetYear( void ) const;
  unsigned int GetMonth( void ) const;
  unsigned int GetDayOfMonth( void ) const;
  unsigned int GetHour( void ) const;
  unsigned int GetMinute( void ) const;
  unsigned int GetSecond( void ) const;
  unsigned int GetMilliSecond( void ) const;

  std::string GetDateString( void ) const;
  std::string GetTimeString( void ) const;
  std::string GetDateTimeString( void ) const;

  double operator -( const TtTime& r ) const;

private:
  INT64        serial_second_;
  unsigned int year_;
  unsigned int month_;
  unsigned int day_of_month_;
  unsigned int hour_;
  unsigned int minute_;
  unsigned int second_;
  unsigned int milli_second_;
};


// -- PerformanceCounter -------------------------------------------------
class PerformanceCounter {
public:
  using Function = std::function<void ( void )>;

  static LONGLONG Count( Function function );
  static LONGLONG Count( unsigned int loop_count, Function function );

public:
  explicit PerformanceCounter( bool auto_start = false );

  void Start( void );
  LONGLONG GetElapsed( void );

  LONGLONG GetElapsedAndStart( void );

private:
  LARGE_INTEGER start_;
};
