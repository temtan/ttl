// tt_command_line_analyzer.cpp

#include <unordered_map>

#include "ttl_define.h"
#include "tt_utility.h"
#include "tt_exception.h"

#include "tt_command_line_analyzer.h"


// -- TtCommandLine::Analyzer --------------------------------------------
TtCommandLine::Analyzer::Analyzer( void ) :
options_()
{
}

TtCommandLine::Analyzer::~Analyzer()
{
}

TtCommandLine::Option&
TtCommandLine::Analyzer::MakeOption( const std::string& main_option,
                                     unsigned int       operand_count,
                                     bool               can_duplicate,
                                     bool               is_deniable,
                                     bool               is_indispensable  )
{
  std::shared_ptr<Option> option( new Option( main_option,
                                              operand_count,
                                              can_duplicate,
                                              is_deniable,
                                              is_indispensable ));
  options_.insert( option );
  return *(option.get());
}


TtCommandLine::Option&
TtCommandLine::Analyzer::MakeOptionBoolean( const std::string& main_option,
                                            bool               can_duplicate,
                                            bool               is_deniable,
                                            bool               is_indispensable  )
{
  return this->MakeOption( main_option, 0, can_duplicate, is_deniable, is_indispensable );
}

TtCommandLine::Option&
TtCommandLine::Analyzer::MakeOptionSingleOperand( const std::string& main_option,
                                                  bool               can_duplicate,
                                                  bool               is_deniable,
                                                  bool               is_indispensable  )
{
  return this->MakeOption( main_option, 1, can_duplicate, is_deniable, is_indispensable );
}


std::vector<std::string>
TtCommandLine::Analyzer::AnalyzeForConsole( unsigned int argc, const char* const* argv )
{
  std::vector<std::string> args;
  for ( unsigned int i = 0; i < argc; ++i ) {
    args.push_back( argv[i] );
  }
  return this->Analyze( args );
}

std::vector<std::string>
TtCommandLine::Analyzer::AnalyzeForWindows( const std::string& args_string )
{
  std::vector<std::string> args;
  std::string tmp;
  enum {
    DEFAULT,
    IN_DOUBLE_QUOTE,
    SEPARATOR,
  } status = DEFAULT;

  for ( unsigned int i = 0; i < args_string.size(); ++i ) {
    const char c = args_string[i];
    switch ( status ) {
    case DEFAULT:
      switch ( c ) {
      case ' ':
      case '\t':
        status = SEPARATOR;
        args.push_back( tmp );
        tmp.clear();
        break;

      case '\"':
        status = IN_DOUBLE_QUOTE;
        break;

      case '\\':
        if ( i + 1 < args_string.size() && args_string[i + 1] == '\"' ) {
          tmp.append( 1, '\"' );
          i += 1;
        }
        else {
          tmp.append( 1, '\\' );
        }
        break;

      default:
        tmp.append( 1, c );
        break;
      }
      break;

    case IN_DOUBLE_QUOTE:
      switch ( c ) {
      case '\"':
        status = DEFAULT;
        break;

      case '\\':
        if ( i + 1 < args_string.size() && args_string[i + 1] == '\"' ) {
          tmp.append( 1, '\"' );
          i += 1;
        }
        else {
          tmp.append( 1, '\\' );
        }
        break;

      default:
        tmp.append( 1, c );
        break;
      }
      break;

    case SEPARATOR:
      switch ( c ) {
      case ' ':
      case '\t':
        break;

      default:
        --i;
        status = DEFAULT;
        break;
      }
      break;
    }
  }

  if ( NOT( tmp.empty() ) ) {
    args.push_back( tmp );
  }
  return this->Analyze( args );
}


std::vector<std::string>
TtCommandLine::Analyzer::Analyze( const std::vector<std::string> args )
{
  std::unordered_map<std::string, std::shared_ptr<TtCommandLine::Option>> table;
  for ( auto& option : options_ ) {
    for ( auto& key : option->GetOptionStrings() ) {
      if ( table.find( key ) != table.end() ) {
        throw TtCommandLine::OptionDuplicateDefinitionException( key );
      }
      table[key] = option;
      if ( option->IsDeniable() ) {
        std::string deny_key = (key) + "-";
        if ( table.find( deny_key ) != table.end() ) {
          throw TtCommandLine::OptionDuplicateDefinitionException( deny_key );
        }
        table[deny_key] = option;
      }
    }
  }

  std::vector<std::string> rest;
  for ( auto it = args.begin(); it != args.end(); ++it ) {
    if ( (*it)[0] == '-' ) {
      std::string option_string = it->substr( 1 );
      if ( table.find( option_string ) == table.end() ) {
        throw TtCommandLine::UnknownOptionSpecifiedException( (*it) );
      }
      TtCommandLine::Option& option = *table[option_string];

      if ( NOT( option.can_duplicate_ ) && option.result_.is_specified_ ) {
        throw TtCommandLine::OptionDuplicatedSpecifyException( option_string );
      }
      option.result_.is_specified_ = true;
      option.result_.is_denied_ = option_string[option_string.size() - 1] == '-';

      option.result_.operand_.clear();
      for ( unsigned int i = 0; i < option.operand_count_; ++i ) {
        ++it;
        if ( it == args.end() || (*it)[0] == '-' ) {
          throw TtCommandLine::OperandCountException( option_string );
        }
        option.result_.operand_.push_back( *it );
      }
    }
    else {
      rest.push_back( *it );
    }
  }

  for ( auto& it : table ) {
    if ( it.second->is_indispensable_ && NOT( it.second->result_.is_specified_ ) ) {
      throw TtCommandLine::IndispensableOptionIsNoneException( it.second->GetMainOption() );
    }
  }
  return rest;
}

// -- TtCommandLine::Option ----------------------------------------------
TtCommandLine::Option::Option( const std::string& main_option,
                               unsigned int       operand_count,
                               bool               can_duplicate,
                               bool               is_deniable,
                               bool               is_indispensable ) :
main_option_( main_option ),
option_strings_(),
operand_count_( operand_count ),
can_duplicate_( can_duplicate ),
is_deniable_( is_deniable ),
is_indispensable_( is_indispensable ),
result_()
{
}


const std::string&
TtCommandLine::Option::GetMainOption( void ) const
{
  return main_option_;
}


void
TtCommandLine::Option::AddOptionString( const std::string& option )
{
  option_strings_.insert( option );
}

const std::set<std::string>&
TtCommandLine::Option::GetOptionStrings( void ) const
{
  return option_strings_;
}

void
TtCommandLine::Option::SetCanDuplicate( bool flag )
{
  can_duplicate_ = flag;
}

bool
TtCommandLine::Option::CanDuplicate( void ) const
{
  return can_duplicate_;
}

void
TtCommandLine::Option::SetDeniable( bool flag )
{
  is_deniable_ = flag;
}

bool
TtCommandLine::Option::IsDeniable( void ) const
{
  return is_deniable_;
}

void
TtCommandLine::Option::SetIndispensable( bool flag )
{
  is_indispensable_ = flag;
}

bool
TtCommandLine::Option::IsIndispensable( void ) const
{
  return is_indispensable_;
}

const TtCommandLine::Option::Result&
TtCommandLine::Option::GetResult( void ) const
{
  return result_;
}

TtCommandLine::Option::Result::Result( void ) :
is_specified_( false ),
is_denied_( false ),
operand_()
{
}

bool
TtCommandLine::Option::Result::IsSpecified( void ) const
{
  return is_specified_;
}

bool
TtCommandLine::Option::Result::IsDenied( void ) const
{
  return is_denied_;
}

const std::vector<std::string>&
TtCommandLine::Option::Result::GetOperand( void ) const
{
  return operand_;
}


int
TtCommandLine::ConvertToInteger( const Option& option, const std::string& str, int min, int max )
{
  int tmp;
  if ( NOT( TtUtility::StringToInteger( str, &tmp, 10 ) ) ) {
    throw TtCommandLine::OperandTypeException( option.GetMainOption(), str, typeid( int ).name() );
  }
  if ( tmp < min || tmp > max ) {
    throw TtCommandLine::OperandRangeException<int>( option.GetMainOption(), str, min, max );
  }
  return tmp;
}

double
TtCommandLine::ConvertToDouble( const Option& option, const std::string& str, double min, double max )
{
  double tmp;
  if ( NOT( TtUtility::StringToDouble( str, &tmp ) ) ) {
    throw TtCommandLine::OperandTypeException( option.GetMainOption(), str, typeid( int ).name() );
  }
  if ( tmp < min || tmp > max ) {
    throw TtCommandLine::OperandRangeException<double>( option.GetMainOption(), str, min, max );
  }
  return tmp;
}


// -- TtCommandLine::OperandCountException -------------------------------
TtCommandLine::OperandCountException::OperandCountException( const std::string& option ) :
WithOptionString( option )
{
}

std::string
TtCommandLine::OperandCountException::Dump( void ) const
{
  std::ostringstream os;
  os << typeid( *this ).name() << " ; option : " << this->GetOption();
  return os.str();
}

// -- TtCommandLine::OptionDuplicateDefinitionException ------------------
TtCommandLine::OptionDuplicateDefinitionException::OptionDuplicateDefinitionException( const std::string& option ) :
WithOptionString( option )
{
}

std::string
TtCommandLine::OptionDuplicateDefinitionException::Dump( void ) const
{
  std::ostringstream os;
  os << typeid( *this ).name() << " ; option : " << this->GetOption();
  return os.str();
}

// -- TtCommandLine::IndispensableOptionIsNoneException ------------------
TtCommandLine::IndispensableOptionIsNoneException::IndispensableOptionIsNoneException( const std::string& option ) :
WithOptionString( option )
{
}

std::string
TtCommandLine::IndispensableOptionIsNoneException::Dump( void ) const
{
  std::ostringstream os;
  os << typeid( *this ).name() << " ; option : " << this->GetOption();
  return os.str();
}

// -- TtCommandLine::UnknownOptionSpecifiedException ---------------------
TtCommandLine::UnknownOptionSpecifiedException::UnknownOptionSpecifiedException( const std::string& specified_string ) :
specified_string_( specified_string )
{
}

const std::string&
TtCommandLine::UnknownOptionSpecifiedException::GetSpecifiedString( void ) const
{
  return specified_string_;
}

std::string
TtCommandLine::UnknownOptionSpecifiedException::Dump( void ) const
{
  std::ostringstream os;
  os << typeid( *this ).name() << " ; specified string : " << this->GetSpecifiedString();
  return os.str();
}

// -- TtCommandLine::OptionDuplicatedSpecifyException --------------------
TtCommandLine::OptionDuplicatedSpecifyException::OptionDuplicatedSpecifyException( const std::string& option ) :
WithOptionString( option )
{
}

std::string
TtCommandLine::OptionDuplicatedSpecifyException::Dump( void ) const
{
  std::ostringstream os;
  os << typeid( *this ).name() << " ; option : " << this->GetOption();
  return os.str();
}

// -- TtCommandLine::OperandTypeException --------------------------------
TtCommandLine::OperandTypeException::OperandTypeException( const std::string& option,
                                                           const std::string& operand,
                                                           const std::string& require_type ) :
WithOptionString( option ),
WithOperandString( operand ),
require_type_( require_type )
{
}

const std::string&
TtCommandLine::OperandTypeException::GetRequireType( void ) const
{
  return require_type_;
}

std::string
TtCommandLine::OperandTypeException::Dump( void ) const
{
  std::ostringstream os;
  os << typeid( *this ).name();
  os << " ; option : "       << this->GetOption();
  os << " ; operand : "      << this->GetOperand();
  os << " ; require type : " << this->GetRequireType();
  return os.str();
}

#ifdef TT_MAKE_TEMPLATE_INSTANCE_
template class TtCommandLine::OperandRangeException<int>;
#endif
