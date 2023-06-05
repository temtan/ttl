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

TtLogger::TtLogger( const std::string& path ) :
path_( path ),
file_writer_( std::nullopt ),
buffer_()
{
}

TtLogger::~TtLogger()
{
  this->Close();
}

const std::string&
TtLogger::GetPath( void )
{
  return path_;
}

void
TtLogger::Open( void )
{
  if ( file_writer_ ) {
    throw TtInvalidOperationException( typeid( *this ) );
  }
  file_writer_.emplace( path_, true, false );
}

void
TtLogger::Close( void )
{
  if ( file_writer_ ) {
    file_writer_.reset();
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
  file_writer_->WriteString( tmp );
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
