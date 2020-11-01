// tt_string.h

#pragma once

#include <string>
#include <memory>
#include <vector>

#include "tt_utility.h"


namespace TtString {
  // 一時的文字列バッファ
  template <class ArrayType>
  class HeapString : public ArrayType {
  public:
    explicit HeapString( size_t capacity ) :
    ArrayType( capacity + 1 ) {}

    explicit HeapString( const std::string& str ) :
    HeapString( str.size() + 1 ) {
      strncpy_s( pointer_.get(), capacity_, str.c_str(), str.size() + 1 );
    }

    explicit HeapString( unsigned int capacity, const std::string& str ) :
    ArrayType( std::max( capacity, str.size() ) + 1 ) {
      strncpy_s( pointer_.get(), capacity_, str.c_str(), str.size() + 1 );
    }

    void ExtendCapacity( size_t increasing ) {
      size_t new_capacity = capacity_ + increasing;
      decltype( pointer_ ) tmp( new char[new_capacity + 1] );
      memcpy( tmp.get(), pointer_.get(), capacity_ );
      capacity_  = new_capacity;
      pointer_ = std::move( tmp );
    }

    std::string ToString( void ) {
      return std::string( pointer_.get() );
    }
  };

  using UniqueString = HeapString<TtUtility::UniqueArray<char>>;
  using SharedString = HeapString<TtUtility::SharedArray<char>>;

  std::string ToRangedStringFromVector( const std::vector<std::string>& v );
  std::vector<std::string> ToVectorFromRangedString( const char* buf, size_t buf_size );

  // 文字列追加用
  class Appender {
  public:
    explicit Appender( std::string& str );

    template <class TYPE>
    Appender& operator <<( const TYPE& value ) {
      str_.append( TtUtility::ToStringFrom( value ) );
      return *this;
    }

    template <> Appender& operator << <std::string>( const std::string& value );
    template <> Appender& operator << <const char*>( const char* const& value );

  private:
    std::string& str_;
  };

  std::string Replace( const std::string& source, const std::string& pattern, const std::string& replacement );

  std::string Strip( const std::string& source, bool left = true, bool right = true );
  void StripDirect( std::string& source, bool left = true, bool right = true );

  bool StartWith( const std::string& source, const std::string& pattern );
  bool StartWith( const std::string& source, unsigned int start, const std::string& pattern );
  bool EndWith( const std::string& source, const std::string& pattern );

  std::string SubstringFromTo( const std::string& source, size_t from, size_t to );
  std::string ToUpper( const std::string& source );
  std::string ToLower( const std::string& source );

  std::string RemoveCR( const std::string& source );
  std::string AddCR( const std::string& source );

    std::vector<std::string> Split( const std::string& source, char delimiter );

  int Compare( const std::string& x, const std::string& y );
}

// -- 特殊化の実装 -------------------------------------------------------
template <>
TtString::Appender&
TtString::Appender::operator << <std::string>( const std::string& value )
{
  str_.append( value );
  return *this;
}

template <>
TtString::Appender&
TtString::Appender::operator << <const char*>( const char* const& value )
{
  str_.append( value );
  return *this;
}
