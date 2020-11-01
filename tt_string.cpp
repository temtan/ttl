// tt_string.cpp

#include <algorithm>

#include "ttl_define.h"
#include "tt_exception.h"

#include "tt_string.h"


std::string
TtString::ToRangedStringFromVector( const std::vector<std::string>& v )
{
  std::string buf;
  for ( auto& str : v ) {
    buf.append( str );
    buf.append( 1, '\0' );
  }
  buf.append( 1, '\0' );
  return buf;
}

std::vector<std::string>
TtString::ToVectorFromRangedString( const char* buf, size_t buf_size )
{
  std::vector<std::string> v;
  unsigned int string_start = 0;
  int length = 0;
  for ( unsigned int i = 0; ; ++i, ++length ) {
    if ( i >= buf_size ) {
      if ( length != 0 ) {
        v.push_back( std::string( buf + string_start, length ) );
      }
      break;
    }
    if ( buf[i] == '\0' ) {
      if ( length == 0 ) {
        break;
      }
      v.push_back( std::string( buf + string_start, length ) );
      string_start = i + 1;
      length = -1;
    }
  }
  return v;
}


TtString::Appender::Appender( std::string& str ) :
str_( str )
{
}


std::string
TtString::Replace( const std::string& source, const std::string& pattern, const std::string& replacement )
{
  size_t pos = 0;
  std::string tmp;
  for (;;) {
    size_t result = source.find( pattern, pos );
    if ( result == std::string::npos ) {
      break;
    }
    tmp.append( source, pos, result - pos );
    tmp.append( replacement );
    pos = result + pattern.size();
  }
  tmp.append( source, pos, source.size() - pos );
  return tmp;
}


namespace {
  bool is_space( char c ) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
  }
}

std::string
TtString::Strip( const std::string& source, bool left, bool right )
{
  std::string tmp( source );
  TtString::StripDirect( tmp, left, right );
  return tmp;
}

void
TtString::StripDirect( std::string& source, bool left, bool right )
{
  if ( left && NOT( source.empty() ) && is_space( *source.begin() ) ) {
    std::string::iterator end = source.begin();
    while ( is_space( *(end + 1) ) ) {
      end += 1;
    }
    source.erase( source.begin(), end + 1 );
  }
  if ( right && NOT( source.empty() ) && is_space( *( source.end() - 1 ) ) ) {
    std::string::iterator begin = source.end() - 1;
    while ( begin != source.begin() && is_space( *(begin - 1) ) ) {
      begin -= 1;
    }
    source.erase( begin, source.end() );
  }
}


bool
TtString::StartWith( const std::string& source, const std::string& pattern )
{
  return TtString::StartWith( source, 0, pattern );
}

bool
TtString::StartWith( const std::string& source, unsigned int start, const std::string& pattern )
{
  std::string::const_iterator source_it = source.begin() + start;
  std::string::const_iterator pattern_it = pattern.begin();

  while ( pattern_it != pattern.end() ) {
    if ( *source_it != *pattern_it ) {
      return false;
    }
    source_it++;
    pattern_it++;
    if ( source_it == source.end() ) {
      return false;
    }
  }
  return true;
}


bool
TtString::EndWith( const std::string& source, const std::string& pattern )
{
  std::string::const_reverse_iterator source_it = source.rbegin();
  std::string::const_reverse_iterator pattern_it = pattern.rbegin();

  while ( pattern_it != pattern.rend() ) {
    if ( *source_it != *pattern_it ) {
      return false;
    }
    source_it++;
    pattern_it++;
    if ( source_it == source.rend() ) {
      return false;
    }
  }
  return true;
}


std::string
TtString::SubstringFromTo( const std::string& source, size_t from, size_t to )
{
  if ( to + 1 == from ) {
    return "";
  }
  else if ( from > to ) {
    throw TT_INTERNAL_EXCEPTION;
  }
  return source.substr( from, to - from + 1 );
}


std::string
TtString::ToUpper( const std::string& source )
{
  std::string str = source;
  std::transform( str.begin(), str.end(), str.begin(), toupper ); 
  return str;
}

std::string
TtString::ToLower( const std::string& source )
{
  std::string str = source;
  std::transform( str.begin(), str.end(), str.begin(), tolower ); 
  return str;
}


std::string
TtString::RemoveCR( const std::string& source )
{
  auto buffer = std::make_unique<char[]>( source.size() + 1 );
  char* c = buffer.get();
  const char* s = source.c_str();
  while ( *s != '\0' ) {
    *c = *s;
    if ( *s != '\r' ) {
      ++c;
    }
    ++s;
  }
  *c = *s;
  return buffer.get();
}

std::string
TtString::AddCR( const std::string& source )
{
  auto buffer = std::make_unique<char[]>( source.size() * 2 + 1 );
  char* c = buffer.get() + source.size() * 2;
  const char* s = source.c_str() + source.size();
  *c = *s;
  while ( s != source.c_str() ) {
    if ( *s == '\n' && s[-1] != '\r' ) {
      --c;
      *c = '\r';
    }
    --c;
    --s;
    *c = *s;
  }
  return c;
}


std::vector<std::string>
TtString::Split( const std::string& source, char delimiter )
{
  std::vector<std::string> v;
  size_t current = 0;
  for (;;) {
    size_t point = source.find( delimiter, current );
    if ( point == std::string::npos ) {
      v.push_back( TtString::SubstringFromTo( source, current, source.size() - 1 ) );
      return v;
    }
    v.push_back( TtString::SubstringFromTo( source, current, point - 1 ) );
    current = point + 1;
    if ( current == source.size() ) {
      v.push_back( "" );
      return v;
    }
  }
}


int
TtString::Compare( const std::string& x, const std::string& y )
{
  return ::strcmp( x.c_str(), y.c_str() );
}


#ifdef TT_MAKE_TEMPLATE_INSTANCE_
template class TtString::HeapString<TtUtility::UniqueArray<char>>;
template class TtString::HeapString<TtUtility::SharedArray<char>>;

template TtString::Appender& TtString::Appender::operator << <const char*const>( const char*const& );
#endif
