// tt_command_line_analyzer.h

#pragma once

#include <string>
#include <vector>
#include <set>
#include <memory>

#include "tt_exception.h"


namespace TtCommandLine {
  class Option;

  // -- Analyzer ---------------------------------------------------------
  class Analyzer {
  public:
    explicit Analyzer( void );
    ~Analyzer();

    Option& MakeOption( const std::string& main_option,
                        unsigned int       operand_count    = 0,
                        bool               can_duplicate    = false,
                        bool               is_deniable      = false,
                        bool               is_indispensable = false );

    Option& MakeOptionBoolean( const std::string& main_option,
                               bool               can_duplicate    = false,
                               bool               is_deniable      = false,
                               bool               is_indispensable = false );

    Option& MakeOptionSingleOperand( const std::string& main_option,
                                     bool               can_duplicate    = false,
                                     bool               is_deniable      = false,
                                     bool               is_indispensable = false );

    std::vector<std::string> AnalyzeForConsole( unsigned int argc, const char* const* argv );
    std::vector<std::string> AnalyzeForWindows( const std::string& args_string );

  private:
    std::vector<std::string> Analyze( const std::vector<std::string> args );

  private:
    std::set<std::shared_ptr<Option>> options_;
  };

  // -- Option -------------------------------------------------------
  class Option {
    friend class Analyzer;

  private:
    explicit Option( const std::string& main_option,
                     unsigned int       operand_count,
                     bool               can_duplicate,
                     bool               is_deniable,
                     bool               is_indispensable);

  public:
    const std::string& GetMainOption( void ) const;
    void AddOptionString( const std::string& option );
    const std::set<std::string>& GetOptionStrings( void ) const;

    void SetCanDuplicate( bool flag );
    bool CanDuplicate( void ) const;

    void SetDeniable( bool flag );
    bool IsDeniable( void ) const;

    void SetIndispensable( bool flag );
    bool IsIndispensable( void ) const;

    class Result {
      friend class Analyzer;
    public:
      explicit Result( void );

      bool IsSpecified( void ) const;
      bool IsDenied( void ) const;
      const std::vector<std::string>& GetOperand( void ) const;

    private:
      bool                     is_specified_;
      bool                     is_denied_;
      std::vector<std::string> operand_;
    };

    const Result& GetResult( void ) const;

  private:
    const std::string     main_option_;
    std::set<std::string> option_strings_;
    unsigned int          operand_count_;
    bool                  can_duplicate_;
    bool                  is_deniable_;
    bool                  is_indispensable_;
    Result                result_;
  };

  int ConvertToInteger( const Option& option,
                        const std::string& str,
                        int min = std::numeric_limits<int>::min(),
                        int max = std::numeric_limits<int>::max() );
  double ConvertToDouble( const Option& option,
                          const std::string& str,
                          double min = std::numeric_limits<double>::min(),
                          double max = std::numeric_limits<double>::max() );

  // -- Exception --------------------------------------------------------
  class Exception : public TtException {};

  // 多重継承用
  // オプション文字列保持用
  class WithOptionString {
  protected:
    explicit WithOptionString( const std::string& option ) : option_( option ) {}
    const std::string& GetOption( void ) const { return option_; }
  private:
    const std::string option_;
  };

  // オペランド文字列保持用
  class WithOperandString {
  protected:
    explicit WithOperandString( const std::string& operand ) : operand_( operand ) {}
    const std::string& GetOperand( void ) const { return operand_; }
  private:
    const std::string operand_;
  };

  // -- OperandCountException --------------------------------------------
  class OperandCountException : public Exception,
                                public WithOptionString {
  public:
    explicit OperandCountException( const std::string& option );

    using WithOptionString::GetOption;

    virtual std::string Dump( void ) const override;
  };

  // -- OptionDuplicateDefinitionException -------------------------------------
  class OptionDuplicateDefinitionException : public Exception,
                                             public WithOptionString {
  public:
    explicit OptionDuplicateDefinitionException( const std::string& option );

    using WithOptionString::GetOption;

    virtual std::string Dump( void ) const override;
  };

  // -- IndispensableOptionIsNoneException -------------------------------
  class IndispensableOptionIsNoneException : public Exception,
                                             public WithOptionString {
  public:
    explicit IndispensableOptionIsNoneException( const std::string& option );

    using WithOptionString::GetOption;

    virtual std::string Dump( void ) const override;
  };

  // -- UnknownOptionSpecifiedException ----------------------------------
  class UnknownOptionSpecifiedException : public Exception {
  public:
    explicit UnknownOptionSpecifiedException( const std::string& specified_string );

    const std::string& GetSpecifiedString( void ) const;

    virtual std::string Dump( void ) const override;

  private:
    const std::string specified_string_;
  };

  // -- OptionDuplicatedSpecifyException ---------------------------------------
  class OptionDuplicatedSpecifyException : public Exception,
                                           public WithOptionString {
  public:
    explicit OptionDuplicatedSpecifyException( const std::string& option );

    using WithOptionString::GetOption;

    virtual std::string Dump( void ) const override;
  };


  // -- OperandTypeException ---------------------------------------------
  class OperandTypeException : public Exception,
                               public WithOptionString,
                               public WithOperandString {
  public:
    explicit OperandTypeException( const std::string& option,
                                   const std::string& operand,
                                   const std::string& require_type );

    using WithOptionString::GetOption;
    using WithOperandString::GetOperand;
    const std::string& GetRequireType( void ) const;

    virtual std::string Dump( void ) const override;

  private:
    const std::string require_type_;
  };

  // -- OperandRangeException --------------------------------------------
  template <class TYPE>
  class OperandRangeException : public Exception,
                                public WithOptionString,
                                public WithOperandString {
  public:
    explicit OperandRangeException( const std::string& option,
                                    const std::string& operand,
                                    TYPE min, TYPE max ) :
    WithOptionString( option ),
    WithOperandString( operand ),
    min_( min ), max_( max ) {}

    using WithOptionString::GetOption;
    using WithOperandString::GetOperand;

    const TYPE& GetMinimum( void ) const { return min_; }
    const TYPE& GetMaximum( void ) const { return max_; }

    virtual std::string Dump( void ) const override {
      std::ostringstream os;
      os << typeid( *this ).name();
      os << " ; option : "       << this->GetOption();
      os << " ; operand : "      << this->GetOperand();
      os << " ; max : " << this->GetMaximum();
      os << " ; min : " << this->GetMinimum();
      return os.str();
    }

  private:
    TYPE min_;
    TYPE max_;
  };
}
