// tt_json.cpp

#include <bitset>

#include "tt_utility.h"

#include "tt_json.h"


// -- Exception ----------------------------------------------------------
TtJson::Exception::Exception( unsigned int line ) :
line_( line )
{
}

unsigned int
TtJson::Exception::GetLine( void ) const
{
  return line_;
}

// -- UnexpectedTokenException -----
TtJson::UnexpectedTokenException::UnexpectedTokenException( unsigned int line, char token ) :
Exception( line ),
token_( token )
{
}


char
TtJson::UnexpectedTokenException::GetToken( void ) const
{
  return token_;
}

// -- NumberFormatException -----
TtJson::NumberFormatException::NumberFormatException( unsigned int line ) :
Exception( line )
{
}

// -- UnicodeFormatException -----
TtJson::UnicodeFormatException::UnicodeFormatException( unsigned int line ) :
Exception( line )
{
}

// -- BadCastException -----
TtJson::BadCastException::BadCastException( unsigned int line, const type_info& expected, const type_info& actual ) :
Exception( line ),
expected_( expected ),
actual_( actual )
{
}

const type_info&
TtJson::BadCastException::GetExpectedType( void ) const
{
  return expected_;
}

const type_info&
TtJson::BadCastException::GetActualType( void ) const
{
  return actual_;
}


// -- TtJson::Value ------------------------------------------------------
TtJson::Value::Value( unsigned int line ) :
line_( line ),
original_string_()
{
}

TtJson::Value::~Value( void )
{
}

const type_info&
TtJson::Value::GetType( void )
{
  return typeid( *this );
}

unsigned int
TtJson::Value::GetLine( void )
{
  return line_;
}

const std::string&
TtJson::Value::GetOriginalString( void )
{
  return original_string_;
}


// -- String -----
TtJson::String::String( unsigned int line, const std::string& str ) :
Value( line ),
data_( str )
{
}

const std::string&
TtJson::String::GetData( void )
{
  return data_;
}


// -- Number -----
TtJson::Number::Number( unsigned int line, double number ) :
Value( line ),
data_( number )
{
}

double
TtJson::Number::GetData( void )
{
  return data_;
}


// -- Boolean ----
TtJson::Boolean::Boolean( unsigned int line, bool flag ) :
Value( line ),
data_( flag )
{
}

bool
TtJson::Boolean::GetData( void )
{
  return data_;
}


// -- Null -----
TtJson::Null::Null( unsigned int line ) :
Value( line )
{
}


// -- Array -----
TtJson::Array::Array( unsigned int line ) :
Value( line )
{
}

void
TtJson::Array::Append( std::shared_ptr<Value> value )
{
  data_.push_back( value );
}

const std::vector<std::shared_ptr<TtJson::Value>>&
TtJson::Array::GetData( void )
{
  return data_;
}


// -- HashTable -----
TtJson::HashTable::HashTable( unsigned int line ) :
Value( line )
{
}

void
TtJson::HashTable::Append( const std::string& key, std::shared_ptr<Value> value )
{
  data_.insert_or_assign( key, value );
}

std::unordered_map<std::string, std::shared_ptr<TtJson::Value>>&
TtJson::HashTable::GetData( void )
{
  return data_;
}


// -- TtJson::Parser -----------------------------------------------------
std::shared_ptr<TtJson::Value>
TtJson::Parser::Parse( const std::string& str )
{
  Parser parser;
  return parser.ParseStart( str );
}


TtJson::Parser::Parser( void ) :
cp_( nullptr ),
current_line_( 0 )
{
}

std::shared_ptr<TtJson::Value>
TtJson::Parser::ParseStart( const std::string& str )
{
  cp_ = str.c_str();
  current_line_ = 1;
  return this->ParseAsRoot();
}

void
TtJson::Parser::SkipSpace( void )
{
  for (;; cp_++ ) {
    switch( *cp_ ) {
    case '\n':
      current_line_ += 1;
    case '\r':
    case ' ':
    case '\t':
      break;

    default:
      return;
    }
  }
}

void
TtJson::Parser::CheckEndOfToken( void )
{
  switch( *cp_ ) {
  case '\0':

  case '\n':
  case '\r':
  case ' ':
  case '\t':

  case ':':
  case ',':
  case Parser::BraceEnd:
  case ']':
    break;

  default:
    throw UnexpectedTokenException( current_line_, *cp_ );
  }
}

bool
TtJson::Parser::CheckKeyword( const char* str, const std::string& word )
{
  for ( unsigned int i = 0; i < word.size(); ++i ) {
    if ( str[i] != word.c_str()[i] ) {
      return false;
    }
  }
  return true;
}


std::shared_ptr<TtJson::Value>
TtJson::Parser::ParseAsRoot( void )
{
  auto tmp = this->ParseAsAny();
  this->SkipSpace();
  if ( *cp_ != '\0' ) {
    throw UnexpectedTokenException( current_line_, *cp_ );
  }
  return tmp;
}

std::shared_ptr<TtJson::Value>
TtJson::Parser::ParseAsAny( void )
{
  this->SkipSpace();

  switch ( *cp_ ) {
  case Parser::BraceStart:
    return this->ParseAsHashTable();
    break;

  case '[':
    return this->ParseAsArray();
    break;

  case '\"':
    return this->ParseAsString();
    break;

  case 't':
  case 'f':
    return this->ParseAsBoolean();
    break;

  case 'n':
    return this->ParseAsNull();
    break;

  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
  case '-':
    return this->ParseAsNumber();
    break;

  default:
    throw UnexpectedTokenException( current_line_, *cp_ );
  }
}


std::shared_ptr<TtJson::Null>
TtJson::Parser::ParseAsNull( void )
{
  this->SkipSpace();
  unsigned int start_line = current_line_;

  if ( this->CheckKeyword( cp_, "null" ) ) {
    cp_ += 4;
  }
  else {
    throw UnexpectedTokenException( start_line, *cp_ );
  }

  this->CheckEndOfToken();
  auto tmp = std::make_shared<TtJson::Null>( start_line );
  tmp->original_string_ = "null";
  return tmp;
}


std::shared_ptr<TtJson::Boolean>
TtJson::Parser::ParseAsBoolean( void )
{
  this->SkipSpace();
  unsigned int start_line = current_line_;

  bool flag;
  if ( this->CheckKeyword( cp_, "true" ) ) {
    cp_ += 4;
    flag = true;
  }
  else if ( this->CheckKeyword( cp_, "false" ) ) {
    cp_ += 5;
    flag = false;
  }
  else {
    throw UnexpectedTokenException( start_line, *cp_ );
  }

  this->CheckEndOfToken();
  auto tmp = std::make_shared<TtJson::Boolean>( start_line, flag );
  tmp->original_string_ = flag ? "true" : "false";
  return tmp;
}


std::shared_ptr<TtJson::String>
TtJson::Parser::ParseAsString( void )
{
  this->SkipSpace();
  unsigned int start_line = current_line_;

  if ( *cp_ != '\"' ) {
    throw UnexpectedTokenException( start_line, *cp_ );
  }
  const char* start = cp_;

  std::string out;
  for ( const char* current = cp_ + 1; ; ++current ) {
    if ( *current == '\0' || *current == '\n' ) {
      throw UnexpectedTokenException( start_line, *current );
    }

    if ( *current == '\"' ) {
      cp_ = current + 1;
      break;
    }
    if ( *current == '\\' ) {
      ++current;
      switch ( *current ) {
      case '\0':
      case '\n':
        throw UnexpectedTokenException( start_line, *current );

      case 'u':
        try {
          ++current;
          const char* next_pointer;
          out.append( this->ParseAsUnicodeInString( current, next_pointer ) );
          current = next_pointer - 1;
        }
        catch ( UnicodeEscapeFormatError ) {
          throw UnicodeFormatException( start_line );
        }
        break;

      case 'n': out.append( 1, '\n' ); break;
      case 'r': out.append( 1, '\r' ); break;
      case 't': out.append( 1, '\t' ); break;
      case 'b': out.append( 1, '\b' ); break;
      case 'f': out.append( 1, '\f' ); break;
      default :
        out.append( 1, *current );
        break;
      }
    }
    else {
      out.append( 1, *current );
    }
  }
  auto tmp = std::make_shared<TtJson::String>( start_line, out );
  tmp->original_string_ = std::string( start, cp_ - start );
  return tmp;
}


std::shared_ptr<TtJson::Number>
TtJson::Parser::ParseAsNumber( void )
{
  this->SkipSpace();
  unsigned int start_line = current_line_;

  auto is_digit = [] ( char c ) -> bool {
    return '0' <= c && c <= '9';
  };
  auto is_not_digit = [&is_digit] ( char c ) -> bool {
    return !(is_digit( c ));
  };

  const char* start = cp_;
  const char* current = cp_;
  if ( *current == '-' ) {
    ++current;
  }

  if ( *current == '0' ) {
    ++current;
  }
  else {
    if ( is_not_digit( *current ) ) {
      throw NumberFormatException( start_line );
    }
    while ( is_digit( *current ) ) {
      ++current;
    }
  }

  if ( *current == '.' ) {
    ++current;
    if ( is_not_digit( *current ) ) {
      throw NumberFormatException( start_line );
    }
    while ( is_digit( *current ) ) {
      ++current;
    }
  }
  if ( *current == 'e' || *current == 'E' ) {
    ++current;
    if ( *current == '-' || *current == '+' ) {
      ++current;
    }
    if ( is_not_digit( *current ) ) {
      throw NumberFormatException( start_line );
    }
    while ( is_digit( *current ) ) {
      ++current;
    }
  }

  cp_ = current;
  std::string original_string( start, current - start );
  double num;
  if ( NOT( TtUtility::StringToDouble( original_string, &num ) ) ) {
    throw NumberFormatException( start_line );
  }
  auto tmp = std::make_shared<Number>( start_line, num );
  tmp->original_string_ = original_string;
  return tmp;
}


std::shared_ptr<TtJson::Array>
TtJson::Parser::ParseAsArray( void )
{
  this->SkipSpace();

  if ( *cp_ != '[' ) {
    throw UnexpectedTokenException( current_line_, *cp_ );
  }
  const char* start = cp_;
  ++cp_;
  auto tmp = std::make_shared<Array>( current_line_ );

  for (;;) {
    this->SkipSpace();
    if ( *cp_ == ']' ) {
      ++cp_;
      break;
    }

    tmp->Append( this->ParseAsAny() );

    this->SkipSpace();
    if ( *cp_ == ']' ) {
      ++cp_;
      break;
    }
    if ( *cp_ != ',' ) {
      throw UnexpectedTokenException( current_line_, *cp_ );
    }
    ++cp_;
    this->SkipSpace();
  }
  tmp->original_string_ = std::string( start, cp_ - start );
  return tmp;
}


std::shared_ptr<TtJson::HashTable>
TtJson::Parser::ParseAsHashTable( void )
{
  this->SkipSpace();

  if ( *cp_ != Parser::BraceStart ) {
    throw UnexpectedTokenException( current_line_, *cp_ );
  }
  const char* start = cp_;
  ++cp_;
  auto table = std::make_shared<HashTable>( current_line_ );

  for (;;) {
    this->SkipSpace();
    if ( *cp_ == Parser::BraceEnd ) {
      ++cp_;
      break;
    }

    auto key = this->ParseAsString();

    this->SkipSpace();
    if ( *cp_ != ':' ) {
      throw UnexpectedTokenException( current_line_, *cp_ );
    }
    ++cp_;

    this->SkipSpace();
    auto value = this->ParseAsAny();
    table->Append( key->GetData(), value );

    this->SkipSpace();
    if ( *cp_ == Parser::BraceEnd ) {
      ++cp_;
      break;
    }
    if ( *cp_ != ',' ) {
      throw UnexpectedTokenException( current_line_, *cp_ );
    }
    ++cp_;
    this->SkipSpace();
  }
  table->original_string_ = std::string( start, cp_ - start );
  return table;
}


std::string
TtJson::Parser::ParseAsUnicodeInString( const char* escape_start, const char*& next_pointer )
{
  using Bitset16 = std::bitset<16>;

  auto is_hex_and_get_bit = [] ( char c, Bitset16& out ) -> bool {
    if ( '0' <= c && c <= '9' ) {
      out = Bitset16( c - '0' );
      return true;
    }
    if ( 'a' <= c && c <= 'f' ) {
      out = Bitset16( c - 'a' + 0x000A );
      return true;
    }
    if ( 'A' <= c && c <= 'F' ) {
      out = Bitset16( c - 'A' + 0x000A );
      return true;
    }
    return false;
  };

  auto hexchar4_to_bitset16 = [&is_hex_and_get_bit] ( const char* str ) -> Bitset16 {
    Bitset16 out;
    for ( unsigned int i = 0; i < 4; ++i ) {
      Bitset16 tmp;
      if ( NOT( is_hex_and_get_bit( str[i], tmp ) ) ) {
        throw UnicodeEscapeFormatError();
      }
      out = (out << 4) | tmp;
    }
    return out;
  };

  auto is_first_surrogate_pair = [] ( const Bitset16& tmp ) {
    return 0xD800 <= tmp.to_ulong() && tmp.to_ulong() <= 0xDBFF;
  };
  auto is_second_surrogate_pair = [] ( const Bitset16& tmp ) {
    return 0xDC00 <= tmp.to_ulong() && tmp.to_ulong() <= 0xDFFF;
  };

  auto unicode_to_1byte = [] ( auto unicode, const char* header, const char* use_bit, unsigned int shift ) -> char {
    using TYPE = decltype( unicode );
    return static_cast<char>( (TYPE( header ) | ((TYPE( use_bit ) & unicode) >> shift)).to_ulong() );
  };


  Bitset16 first = hexchar4_to_bitset16( escape_start );
  if ( is_second_surrogate_pair( first ) ) {
    throw UnicodeEscapeFormatError();
  }
  if ( is_first_surrogate_pair( first ) ) {
    if ( escape_start[4] != '\\' || escape_start[5] != 'u' ) {
      next_pointer = escape_start + 4;
      return "?";
    }
    Bitset16 second = hexchar4_to_bitset16( escape_start + 6 );
    if ( NOT( is_second_surrogate_pair( second ) ) ) {
      throw UnicodeEscapeFormatError();
    }

    next_pointer = escape_start + 10;

    std::bitset<32> unicode;
    unicode = ((first.to_ulong() - 0xD800) << 10) | ((second.to_ulong() - 0xDC00) & 0x3FF);
    unicode = unicode.to_ulong() + 0x10000;

    std::string out;
    out.append( 1, unicode_to_1byte( unicode, "11110000", "11000000000000000000", 18 ) );
    out.append( 1, unicode_to_1byte( unicode, "10000000", "00111111000000000000", 12 ) );
    out.append( 1, unicode_to_1byte( unicode, "10000000", "00000000111111000000",  6 ) );
    out.append( 1, unicode_to_1byte( unicode, "10000000", "00000000000000111111",  0 ) );
    return out;
  }

  next_pointer = escape_start + 4;

  Bitset16 unicode = first;
  if ( unicode.to_ulong() < 0x0080 ) {
    return std::string( 1, static_cast<char>( unicode.to_ulong() ) );
  }

  if ( unicode.to_ulong() < 0x0800 ) {
    std::string out;
    out.append( 1, unicode_to_1byte( unicode, "11000000", "0000011111000000", 6 ) );
    out.append( 1, unicode_to_1byte( unicode, "10000000", "0000000000111111", 0 ) );
    return out;
  }
  else {
    std::string out;
    out.append( 1, unicode_to_1byte( unicode, "11100000", "1111000000000000", 12 ) );
    out.append( 1, unicode_to_1byte( unicode, "10000000", "0000111111000000",  6 ) );
    out.append( 1, unicode_to_1byte( unicode, "10000000", "0000000000111111",  0 ) );
    return out;
  }
}
