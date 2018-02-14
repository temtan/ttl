// tt_exception.cpp
#include <sstream>
#include <typeinfo>

#include "ttl_define.h"
#include "tt_utility.h"

#include "tt_exception.h"


// -- TtException --------------------------------------------------------
TtException::TtException( void ) :
std::domain_error( "TtException" )
{
}

TtException::~TtException()
{
}


std::string
TtException::Dump( void ) const
{
  return typeid( *this ).name();
}

// -- TtInternalException ------------------------------------------------
TtInternalException::TtInternalException( const char* file, unsigned int line ) :
file_( file ),
line_( line )
{
}

const char*
TtInternalException::GetFile( void ) const
{
  return file_;
}

unsigned int
TtInternalException::GetLine( void ) const
{
  return line_;
}

std::string
TtInternalException::Dump( void ) const
{
  std::ostringstream os;
  os << typeid( *this ).name() << " : " << file_ << "(" << line_ << ")";
  return os.str();
}

// -- TtSystemCallException ----------------------------------------------
TtSystemCallException::TtSystemCallException( const char* func_name, int error_number, const char* file, unsigned line ) :
file_( file ),
line_( line ),
func_name_( func_name ),
error_number_( error_number )
{
}

const char*
TtSystemCallException::GetFile( void ) const
{
  return file_;
}

unsigned int
TtSystemCallException::GetLine( void ) const
{
  return line_;
}


const char*
TtSystemCallException::GetFunctionName( void ) const
{
  return func_name_;
}

int
TtSystemCallException::GetErrorNumber( void ) const
{
  return error_number_;
}

std::string
TtSystemCallException::Dump( void ) const
{
  std::ostringstream os;
  os << typeid( *this ).name() << " : " << file_ << "(" << line_ << ")," <<
    func_name_ << "(" << error_number_ << ")";
  return os.str();
}

// -- TtWindowsSystemCallException ---------------------------------------
TtWindowsSystemCallException::TtWindowsSystemCallException( const char* func_name, unsigned int error_number, const char* file, unsigned line ) :
TtSystemCallException( func_name, error_number, file, line )
{
}

std::string
TtWindowsSystemCallException::GetSystemErrorMessage( void ) const
{
  return TtUtility::GetWindowsSystemErrorMessage( this->GetErrorNumber() );
}

std::string
TtWindowsSystemCallException::Dump( void ) const
{
  std::string tmp = this->TtSystemCallException::Dump();
  tmp.append( "," );
  tmp.append( this->GetSystemErrorMessage() );
  return tmp;
}

// -- TtFileAccessException ----------------------------------------------
TtFileAccessException::TtFileAccessException( const std::string& filename, int error_number ) :
filename_( filename ),
error_number_( error_number )
{
}

const std::string&
TtFileAccessException::GetFileName( void ) const
{
  return filename_;
}


int
TtFileAccessException::GetErrorNumber( void ) const
{
  return error_number_;
}

std::string
TtFileAccessException::Dump( void ) const
{
  std::ostringstream os;
  os << typeid( *this ).name() << " : " << filename_ << "(errno = " << error_number_ << ")";
  return os.str();
}

// -- TtInvalidOperationException ----------------------------------------
TtInvalidOperationException::TtInvalidOperationException( const std::type_info& klass ) :
TtWithCauseClassException( klass )
{
}

std::string
TtInvalidOperationException::Dump( void ) const
{
  std::ostringstream os;
  os << typeid( *this ).name() << " : " << this->GetCauseClass().name();
  return os.str();
}

// -- TtInvalidFileTypeException -----------------------------------------
TtInvalidFileTypeException::TtInvalidFileTypeException( const std::type_info& klass, const std::string& require_type ) :
TtWithCauseClassException( klass ),
require_type_( require_type )
{
}

const std::string&
TtInvalidFileTypeException::GetRequireType( void ) const
{
  return require_type_;
}

std::string
TtInvalidFileTypeException::Dump( void ) const
{
  std::ostringstream os;
  os << typeid( *this ).name() << " : " << require_type_;
  return os.str();
}
