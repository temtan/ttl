// tt_json.h

#pragma once

#include <vector>
#include <unordered_map>
#include <typeinfo>

#include "tt_exception.h"


namespace TtJson {
  // -- Exception --------------------------------------------------------
  class Exception : public TtException {
  public:
    explicit Exception( unsigned int line );

    unsigned int GetLine( void ) const;

  private:
    unsigned int line_;
  };

  // -- UnexpectedTokenException -----
  class UnexpectedTokenException : public Exception {
  public:
    explicit UnexpectedTokenException( unsigned int line, char token );

    char GetToken( void ) const;

  private:
    char token_;
  };

  // -- NumberFormatException -----
  class NumberFormatException : public Exception {
  public:
    explicit NumberFormatException( unsigned int line );
  };

  // -- UnicodeFormatException -----
  class UnicodeFormatException : public Exception {
  public:
    explicit UnicodeFormatException( unsigned int line );
  };

  // -- BadCastException -----
  class BadCastException : public Exception {
  public:
    explicit BadCastException( unsigned int line, const type_info& expected, const type_info& actual );

    const type_info& GetExpectedType( void ) const;
    const type_info& GetActualType( void ) const;

  private:
    const type_info& expected_;
    const type_info& actual_;
  };

  // -- Value ------------------------------------------------------------
  class Value {
    friend class Parser;

  public:
    explicit Value( unsigned int line );

    virtual ~Value( void );

    const type_info& GetType( void );

    template <class TYPE>
    bool Is( void ) {
      return dynamic_cast<TYPE*>( this ) != nullptr;
    }

    template <class TYPE>
    TYPE& CastTo( void ) {
      if ( this->Is<TYPE>() ) {
        return *(dynamic_cast<TYPE*>( this ));
      }
      throw BadCastException( line_, typeid( TYPE ), this->GetType() );
    }

    unsigned int       GetLine( void );
    const std::string& GetOriginalString( void );

  private:
    unsigned int line_;
    std::string  original_string_;
  };

  // -- String -----
  class String : public Value {
  public:
    explicit String( unsigned int line, const std::string& str );

    friend bool operator ==( const String& l, const String& r ) {
      return l.data_ == r.data_;
    }

    const std::string& GetData( void );

  private:
    std::string data_;
  };

  // -- Number -----
  class Number : public Value {
  public:
    explicit Number( unsigned int line, double number );

    double GetData( void );

  private:
    double data_;
  };

  // -- Boolean ----
  class Boolean : public Value {
  public:
    explicit Boolean( unsigned int line, bool flag );

    bool GetData( void );

  private:
    bool data_;
  };

  // -- Null -----
  class Null : public Value {
  public:
    explicit Null( unsigned int line );
  };

  // -- Array -----
  class Array : public Value {
  public:
    explicit Array( unsigned int line );

    void Append( std::shared_ptr<Value> value );

    const std::vector<std::shared_ptr<Value>>& GetData( void );

  private:
    std::vector<std::shared_ptr<Value>> data_;
  };

  // -- HashTable -----
  class HashTable : public Value {
  public:
    explicit HashTable( unsigned int line );

    void Append( const std::string& key, std::shared_ptr<Value> value );

    std::unordered_map<std::string, std::shared_ptr<Value>>& GetData( void );

    template <class TYPE>
    auto GetValueDataAs( const std::string& key ) {
      return data_[key]->CastTo<TYPE>().GetData();
    }

  private:
    std::unordered_map<std::string, std::shared_ptr<Value>> data_;
  };

  // -- Parser -----------------------------------------------------------
  class Parser {
  public:
    static std::shared_ptr<Value> Parse( const std::string& str );

  private:
    static const char BraceStart = '{';
    static const char BraceEnd   = '}';

  public:
    explicit Parser( void );

    std::shared_ptr<Value> ParseStart( const std::string& str );

  private:
    void SkipSpace( void );
    void CheckEndOfToken( void );
    bool CheckKeyword( const char* str, const std::string& word );

    std::shared_ptr<Value> ParseAsRoot( void );

    std::shared_ptr<Value> ParseAsAny( void );

    std::shared_ptr<Null>      ParseAsNull( void );
    std::shared_ptr<Boolean>   ParseAsBoolean( void );
    std::shared_ptr<String>    ParseAsString( void );
    std::shared_ptr<Number>    ParseAsNumber( void );
    std::shared_ptr<Array>     ParseAsArray( void );
    std::shared_ptr<HashTable> ParseAsHashTable( void );

    class UnicodeEscapeFormatError {};
    std::string ParseAsUnicodeInString( const char* escape_start, const char*& next_pointer );

  private:
    const char*  cp_;
    unsigned int current_line_;
  };
}
