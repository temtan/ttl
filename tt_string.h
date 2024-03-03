// tt_string.h

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <format>

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
      strncpy_s( ArrayType::pointer_.get(), ArrayType::capacity_, str.c_str(), str.size() + 1 );
    }

    explicit HeapString( size_t capacity, const std::string& str ) :
    ArrayType( std::max( capacity, str.size() ) + 1 ) {
      strncpy_s( ArrayType::pointer_.get(), ArrayType::capacity_, str.c_str(), str.size() + 1 );
    }

    void ExtendCapacity( size_t increasing ) {
      size_t new_capacity = ArrayType::capacity_ + increasing;
      decltype( ArrayType::pointer_ ) tmp( new char[new_capacity + 1] );
      memcpy( tmp.get(), ArrayType::pointer_.get(), ArrayType::capacity_ );
      ArrayType::capacity_  = new_capacity;
      ArrayType::pointer_ = std::move( tmp );
    }

    std::string ToString( void ) {
      return std::string( ArrayType::pointer_.get() );
    }
  };

  using UniqueString = HeapString<TtUtility::UniqueArray<char>>;
  using SharedString = HeapString<TtUtility::SharedArray<char>>;

  std::string ToRangedStringFromVector( const std::vector<std::string>& v );
  std::vector<std::string> ToVectorFromRangedString( const char* buf, size_t buf_size );

  // 文字列追加用
  template <class STRING_TYPE>
  class Appender {
  public:
    using CHAR_TYPE = STRING_TYPE::value_type;

  public:
    explicit Appender( STRING_TYPE& str ) : str_( str ) {}

    template <class TYPE>
    Appender& operator <<( const TYPE& value ) {
      str_.append( std::format( std::get<const CHAR_TYPE*>( std::make_tuple( "{}", L"{}" ) ), value ) );
      return *this;
    }

    template <> Appender& operator << <STRING_TYPE>( const STRING_TYPE& value ) {
      str_.append( value );
      return *this;
    }

    template <> Appender& operator << <const CHAR_TYPE*>( const CHAR_TYPE* const& value ) {
      str_.append( value );
      return *this;
    }

  private:
    STRING_TYPE& str_;
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

  std::wstring MultiByteToUTF16( UINT code_page, const std::string& str );
  std::string  UTF16ToMultiByte( UINT code_page, const std::wstring& str );

  std::wstring CP932ToUTF16( const std::string& str );
  std::string  UTF16ToCP932( const std::wstring& str );
  std::wstring UTF8ToUTF16( const std::string& str );
  std::string  UTF16ToUTF8( const std::wstring& str );

  std::string  UTF8ToCP932( const std::string& str );
  std::string  CP932ToUTF8( const std::string& str );
}
