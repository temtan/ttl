// tt_enum.h

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "ttl_define.h"


template <class TYPE> class TtEnum;

// -- EnumTable --------------------------------------------------------
template <class TYPE>
class TtEnumTable {
public:
  explicit TtEnumTable( void ) {}

  std::string ValueToName( TYPE value ) {
    auto it = value_to_name_.find( value );
    if ( it == value_to_name_.end() ) {
      return value_to_name_[TYPE( 0 )];
    }
    return it->second;
  }

  TYPE NameToValue( const std::string& name ) {
    auto it = name_to_value_.find( name );
    if ( it == name_to_value_.end() ) {
      return TYPE( 0 );
    }
    return it->second;
  }

  void Register( TYPE value, const std::string& name ) {
    value_to_name_[value] = name;
    name_to_value_[name] = value;
  }

  std::vector<TYPE> GetValues( void ) {
    std::vector<TYPE> v;
    for ( auto& one : value_to_name_ ) {
      v.push_back( one.first );
    }
    return v;
  }

  std::vector<std::string> GetNames( void ) {
    std::vector<std::string> v;
    for ( auto& one : name_to_value_ ) {
      v.push_back( one.first );
    }
    return v;
  }

private:
  std::unordered_map<TYPE, std::string> value_to_name_;
  std::unordered_map<std::string, TYPE> name_to_value_;
};

// -- TtEnumBase ---------------------------------------------------------
template <class TYPE>
class TtEnumBase {
private:
  static TtEnumTable<TYPE> TABLE;

public:
  TtEnumBase( void ) : value_( static_cast<TYPE>( 0 ) ) {}
  TtEnumBase( TYPE value ) : value_( value ) {}

  static TYPE Parse( const std::string& name ) {
    return TABLE.NameToValue( name );
  }

  static std::vector<TYPE> GetValues( void ) {
    return TABLE.GetValues();
  }

  static std::vector<std::string> GetNames( void ) {
    return TABLE.GetNames();
  }

  TYPE ToValue( void ) const {
    return value_;
  }

  int ToInteger( void ) const {
    return static_cast<int>( value_ );
  }

  operator int( void ) const {
    return static_cast<int>( value_ );
  }

  operator TYPE( void ) const {
    return this->ToValue();
  }

  std::string ToString( void ) const {
    return TABLE.ValueToName( value_ );
  }

  friend bool operator ==( TtEnum<TYPE> l, TtEnum<TYPE> r ) { return l.value_ == r.value_; }
  friend bool operator !=( TtEnum<TYPE> l, TtEnum<TYPE> r ) { return NOT( l == r ); }

  friend bool operator ==( TtEnum<TYPE> l, TYPE r ) { return l.value_ == r; }
  friend bool operator !=( TtEnum<TYPE> l, TYPE r ) { return NOT( l == r ); }

  friend bool operator ==( TYPE l, TtEnum<TYPE> r ) { return l == r.value_; }
  friend bool operator !=( TYPE l, TtEnum<TYPE> r ) { return NOT( l == r ); }

private:
  TYPE value_;
};

// -- TtEnum -------------------------------------------------------------
template <class TYPE>
class TtEnum : public TtEnumBase<TYPE> {
public:
  TtEnum<TYPE>( void ) : TtEnumBase<TYPE>() {}
  TtEnum<TYPE>( TYPE value ) : TtEnumBase<TYPE>( value ) {}
  explicit TtEnum<TYPE>( int value ) : TtEnumBase<TYPE>( static_cast<TYPE>( value ) ) {}
};


// // Usage
// enum class Foo {
//   VAL1,
//   VAL2,
//   VAL3,
// };
//
// // ŽÀ‘•‘¤
// template <>
// TtEnumTable<Foo>::TtEnumTable( void ) {
// #define REGISTER( NAME ) this->Register( Foo::NAME, #NAME )
//   REGISTER( VAL1 );
//   REGISTER( VAL2 );
//   REGISTER( VAL3 );
// #undef REGISTER
// }
// 
// TtEnumTable<Foo>
// TtEnumBase<Foo>::TABLE;
