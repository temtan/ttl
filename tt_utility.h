// tt_utility.h

#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <memory>
#include <functional>
#include <algorithm>

#include "ttl_define.h"
#include "tt_windows_h_include.h"


namespace TtUtility {
  // ----- 文字列数値変換
  template <class TYPE>
  bool StringToInteger( const std::string& str, TYPE* ret, int base = 10 );

  bool StringToDouble( const std::string& str, double* ret );

  template <class TYPE>
  std::string ToStringFrom( const TYPE& value ) {
    std::ostringstream os;
    os << value;
    return os.str();
  }

  // -- vector の find と find_if の簡易化
  template <class TYPE>
  typename std::vector<TYPE>::const_iterator Find( const std::vector<TYPE>& v, const TYPE& value ) {
    return std::find( v.begin(), v.end(), value );
  }

  template <class TYPE, class FUNCTION>
  typename std::vector<TYPE>::const_iterator FindIf( const std::vector<TYPE>& v, FUNCTION f ) {
    return std::find_if( v.begin(), v.end(), f );
  }

  template <class TYPE, class FUNCTION>
  typename std::vector<TYPE>::const_iterator FindIfNot( const std::vector<TYPE>& v, FUNCTION f ) {
    return std::find_if_not( v.begin(), v.end(), f );
  }

  // -- DestructorCall ---------------------------------------------------
  class DestructorCall {
  public:
    using Function = std::function<void ( void )>;
    explicit DestructorCall( Function function ) : function_( function ) {}
    template <class TYPE> explicit DestructorCall( TYPE function ) : function_( function ) {}
    ~DestructorCall() { function_(); }
    DestructorCall( DestructorCall const & ) = delete ;
    void operator = ( DestructorCall const & ) = delete ;
  private:
    Function function_;
  };

  // -- SharedDestructorCall -----
  class SharedDestructorCall {
  public:
    using Function = std::function<void ( void )>;
    explicit SharedDestructorCall( Function function ) : shared_( nullptr, [function] ( void* ) { function(); } ) {}

  private:
    std::shared_ptr<void> shared_;
  };

  // 関数オブジェクトを nullptr でも安全に呼び出せるようにする。
  template <class... Args>
  auto Safing( std::function<void ( Args... )> function ) -> decltype( function ) {
    return function ? function : [] ( Args... ) {};
  }

  template <class RETURN_VALUE, class... Args>
  auto Safing( std::function<RETURN_VALUE ( Args... )> function, RETURN_VALUE default_value ) -> decltype( function ) {
    return function ? function : [default_value] ( Args... ) { return default_value; };
  }

  // ----- 一時的配列
  template <class TYPE>
  class UniqueArray {
  public:
    explicit UniqueArray( size_t capacity ) :
    capacity_( capacity ), pointer_( new TYPE[capacity] ) {}

    TYPE* GetPointer( void ) { return pointer_.get(); }
    size_t GetCapacity( void ) { return capacity_; }

  protected:
    size_t                capacity_;
    std::unique_ptr<TYPE> pointer_;
  };

  template <class TYPE>
  class SharedArray {
  public:
    explicit SharedArray( size_t capacity ) :
    capacity_( capacity ), pointer_( new TYPE[capacity], std::default_delete<TYPE[]>() ) {}

    TYPE* GetPointer( void ) { return pointer_.get(); }
    size_t GetCapacity( void ) { return capacity_; }

  protected:
    size_t                capacity_;
    std::shared_ptr<TYPE> pointer_;
  };

  // -- WindowsHandleHolder ----------------------------------------------
  template <class TYPE, TYPE null_value = nullptr>
  class WindowsHandleHolder : private std::shared_ptr<TYPE> {
  public:
    using DestroyFunction = std::function<void ( void )>;

    static TYPE GetNullValue( void ) { return null_value; }

    explicit WindowsHandleHolder( DestroyFunction destroy_function, TYPE handle, bool auto_destroy ) :
    handle_( handle ),
    deleter_( auto_destroy ? destroy_function : [] ( void ) {} ) {}

    TYPE GetHandle( void ) {
      return handle_;
    }

  protected:
    TYPE                 handle_;
    SharedDestructorCall deleter_;
  };

  // ----- 簡易シリアライズ
  template <class TYPE>
  std::string Serialize( TYPE& t ) {
    const unsigned char* c = reinterpret_cast<const unsigned char*>( &t );
    std::string result;
    for ( int i = 0; i < sizeof( TYPE ); ++i ) {
      char buf[16];
      sprintf_s( buf, sizeof( buf ), "%02x", *c );
      result.append( buf );
      ++c;
    }
    return result;
  }

  template <class TYPE>
  bool Deserialize( const std::string& data, TYPE& t ) {
    if ( data.size() / 2 != sizeof( TYPE ) ) {
      return false;
    }
    const char* c = data.c_str();
    unsigned char* target = reinterpret_cast<unsigned char*>( &t );
    for ( int i = 0; i < sizeof( TYPE ); ++i ) {
      char buf[3] = {*c, *(c + 1), 0};
      unsigned int ret;
      if ( NOT( TtUtility::StringToInteger( buf, reinterpret_cast<int*>( &ret ), 16 ) ) ) {
        return false;
      }
      *target = static_cast<unsigned char>( ret );
      ++target;
      c += 2;
    }
    return true;
  };

  // ----- 比較系
  template <class TYPE>
  TYPE& Bigger( TYPE& r, TYPE& l ) {
    return r > l ? r : l;
  }

  template <class TYPE>
  const TYPE& ConstBigger( const TYPE& r, const TYPE& l ) {
    return r > l ? r : l;
  }

  template <class TYPE>
  TYPE& Smaller( TYPE& r, TYPE& l ) {
    return r > l ? l : r;
  }

  template <class TYPE>
  const TYPE& ConstSmaller( const TYPE& r, const TYPE& l ) {
    return r > l ? l : r;
  }

  template <class TYPE>
  int OrderingToInt( TYPE ordering ) {
    return (ordering == 0) ? 0 : ((ordering > 0) ? 1 : -1);
  }

  // ----- システム系
  void CallWindowsSystemFunctionWithErrorHandling( std::function<void ( void )> function,
                                                   std::function<void ( void )> error_handling );
  std::string GetWindowsSystemErrorMessage( unsigned int error_number );

  std::string MicrosoftCompilerDateMacroToNormalString( const std::string& date );
#define TTL_DATE_STRING TtUtility::MicrosoftCompilerDateMacroToNormalString( __DATE__ )

  std::string GetANSIErrorMessage( errno_t error_number );

  std::string ExpandEnvironmentString( const std::string& str );

  void MessageBoxForDebug( const char* type, const char* obj_name, void* obj, const char* file, int line );
}
