// tt_utility.cpp

#include <errno.h>
#include <stdio.h>

#include "tt_windows_h_include.h"

#include "ttl_define.h"
#include "tt_string.h"

#include "tt_utility.h"

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")


template <class TYPE>
bool
TtUtility::StringToInteger( const std::string& str, TYPE* ret, int base )
{
  if ( str.empty() ) {
    return false;
  }
  char* endptr;
  errno = 0;
  *ret = static_cast<TYPE>( strtoll( str.c_str(), &endptr, base ) );
  return *endptr == '\0' && errno == 0;
}

template bool TtUtility::StringToInteger<int>( const std::string&, int*, int );
template bool TtUtility::StringToInteger<unsigned int>( const std::string&, unsigned int*, int );

bool
TtUtility::StringToDouble( const std::string& str, double* ret )
{
  if ( str.empty() ) {
    return false;
  }
  char* endptr;
  errno = 0;
  *ret = strtod( str.c_str(), &endptr );
  return *endptr == '\0' && NOT( *ret == 0.0 && endptr != str.c_str() ) && errno == 0;
}

template <>
std::string
TtUtility::ToStringFrom<std::string>( const std::string& value ) {
  return value;
}

template <>
std::string
TtUtility::ToStringFrom<const char*>( const char* const& value ) {
  return value;
}


// ƒVƒXƒeƒ€Œn
void
TtUtility::CallWindowsSystemFunctionWithErrorHandling( std::function<void ( void )> function,
                                                       std::function<void ( void )> error_handling )
{
  ::SetLastError( NO_ERROR );
  function();
  if ( ::GetLastError() != NO_ERROR ) {
    error_handling();
  }
}


std::string
TtUtility::GetWindowsSystemErrorMessage( unsigned int error_number )
{
  char buf[1024 * 16];
  DWORD ret = ::FormatMessage(
    FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
    nullptr, error_number, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
    buf, sizeof( buf ), nullptr );
  if ( ret == 0 ) {
    return "FormatMessage error.";
  }
  return std::string( buf );
}


std::string
TtUtility::MicrosoftCompilerDateMacroToNormalString( const std::string& date )
{
  std::string tmp;
  tmp.append( date.substr( date.size() - 4, 4 ) );

  tmp.append( "/" );

  std::string month = date.substr( 0, 3 );
  tmp.append( month == "Jan" ? "01" :
              month == "Feb" ? "02" :
              month == "Mar" ? "03" :
              month == "Apr" ? "04" :
              month == "May" ? "05" :
              month == "Jun" ? "06" :
              month == "Jul" ? "07" :
              month == "Aug" ? "08" :
              month == "Sep" ? "09" :
              month == "Oct" ? "10" :
              month == "Nov" ? "11" :
              month == "Dec" ? "12" : "??" );

  tmp.append( "/" );

  std::string day = date.substr( 4, 2 );
  if ( day[0] == ' ' ) {
    day[0] = '0';
  }
  tmp.append( day );
  return tmp;
}

std::string
TtUtility::GetANSIErrorMessage( errno_t error_number )
{
  char buf[1024];
  ::strerror_s( buf, sizeof( buf ), error_number );
  return buf;
}


std::string
TtUtility::ExpandEnvironmentString( const std::string& str )
{
  char buf[1024 * 4];
  auto ret = ::ExpandEnvironmentStrings( str.c_str(), buf, sizeof( buf ) );
  if ( ret <= sizeof( buf ) ) {
    return buf;
  }
  TtString::UniqueString heap_buf( ret );
  ::ExpandEnvironmentStrings( str.c_str(), heap_buf.GetPointer(), static_cast<DWORD>( heap_buf.GetCapacity() ) );
  return heap_buf.ToString();
}



void
TtUtility::MessageBoxForDebug( const char* type, const char* obj_name, void* obj, const char* file, int line )
{
  char fmt[256];
  char buf[1024 * 16];
  sprintf_s( fmt, sizeof( fmt ), "%%s(%%d) : %%s = %s\n", type );
  sprintf_s( buf, sizeof( buf ), fmt, file, line, obj_name, obj );
  ::MessageBox( nullptr, buf, "DEBUG", 0 );
}


#ifdef TT_MAKE_TEMPLATE_INSTANCE_
template std::string TtUtility::ToStringFrom( const int& );

template TtUtility::DestructorCall::DestructorCall( void (*)( void ) );

template class TtUtility::UniqueArray<int>;
template class TtUtility::SharedArray<int>;

template class TtUtility::Serialize<int>;
template class TtUtility::Deserialize<int>;

template int&       TtUtility::Bigger( int&, int& );
template const int& TtUtility::ConstBigger( const int&, const int& );
template int&       TtUtility::Smaller( int&, int& );
template const int& TtUtility::ConstSmaller( const int&, const int& );

#include "tt_message_queue.h"
template class TtMessageQueue<int>;

#include "tt_enum.h"
enum class Foo { FOO };
template class TtEnum<Foo>;

#endif
