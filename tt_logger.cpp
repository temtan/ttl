// tt_logger.cpp

#include <stdio.h>
#include <errno.h>

#include <sstream>

#include "ttl_define.h"
#include "tt_exception.h"
#include "tt_path.h"
#include "tt_time.h"

#include "tt_logger.h"


// -- TtLogger -----------------------------------------------------------
TtLogFlusher
TtLogger::FLUSH;

TtLogger::TtLogger( void ) :
TtLogger( TtPath::GetExecutingFilePath() + ".log" )
{
}

TtLogger::TtLogger( const std::string& filename ) :
filename_( filename ),
file_stream_( nullptr ),
buffer_()
{
}

TtLogger::~TtLogger()
{
  this->Close();
}

const std::string&
TtLogger::GetFileName( void )
{
  return filename_;
}

void
TtLogger::Open( void )
{
  if ( file_stream_ != nullptr ) {
    throw TtInvalidOperationException( typeid( *this ) );
  }
  int error_number = fopen_s( &file_stream_, filename_.c_str(), "a" );
  if ( error_number != 0 ) {
    throw TtFileAccessException( filename_, error_number );
  }
}

void
TtLogger::Close( void )
{
  if ( file_stream_ != nullptr ) {
    fclose( file_stream_ );
    file_stream_ = nullptr;
  }
}


#ifdef TT_MAKE_TEMPLATE_INSTANCE_
template TtLogger& TtLogger::operator <<<int>( const int& );
#endif

template <>
TtLogger&
TtLogger::operator << <std::string>( const std::string& value )
{
  buffer_.append( value );
  return *this;
}

template <>
TtLogger&
TtLogger::operator << <const char*>( const char* const& value )
{
  buffer_.append( value );
  return *this;
}

void
TtLogger::operator <<( TtLogFlusher& flusher )
{
  NOT_USE( flusher );
  this->Put( buffer_ );
  buffer_.clear();
}

void
TtLogger::Put( const std::string& value )
{
  std::string tmp;
  this->Prefix( tmp );
  tmp.append( value );
  this->Postfix( tmp );
  if ( fputs( tmp.c_str(), file_stream_ ) == EOF ) {
    throw TT_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( fputs ), errno );
  }
}

void
TtLogger::Prefix( std::string& str )
{
  NOT_USE( str );
}

void
TtLogger::Postfix( std::string& str )
{
  NOT_USE( str );
}


// -- TtLoggerStandard ---------------------------------------------------
TtLoggerStandard::TtLoggerStandard( void ) :
TtLogger()
{
}

TtLoggerStandard::TtLoggerStandard( const std::string& filename ) :
TtLogger( filename )
{
}

void
TtLoggerStandard::Prefix( std::string& str )
{
  str.append( TtTime::GetNow().GetDateTimeString() );
}

void
TtLoggerStandard::Postfix( std::string& str )
{
  str.append( "\n" );
}
