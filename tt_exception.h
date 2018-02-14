// tt_exception.h

#pragma once

#include <stdexcept>
#include <typeinfo>

#pragma comment(lib, "kernel32.lib")


// 多重継承用
// 原因クラス保持用
class TtWithCauseClassException
{
protected:
  explicit TtWithCauseClassException( const std::type_info& klass ) : klass_( klass ) {}
  const std::type_info& GetCauseClass( void ) const { return klass_; }
private:
  const std::type_info& klass_;
};


// -- TtException --------------------------------------------------------
class TtException : public std::domain_error {
public:
  explicit TtException( void );
  virtual ~TtException();
  virtual std::string Dump( void ) const;
};


// -- TtInternalException ------------------------------------------------
class TtInternalException : public TtException {
public:
  explicit TtInternalException( const char* file, unsigned int line );

  const char*  GetFile( void ) const;
  unsigned int GetLine( void ) const;

  virtual std::string Dump( void ) const override;

private:
  const char*  file_;
  unsigned int line_;
};

#define TT_INTERNAL_EXCEPTION TtInternalException( __FILE__, __LINE__ )


// -- TtSystemCallException ----------------------------------------------
class TtSystemCallException : public TtException {
public:
  explicit TtSystemCallException( const char* func_name, int error_number, const char* file, unsigned line );

  const char*  GetFile( void ) const;
  unsigned int GetLine( void ) const;
  const char*  GetFunctionName( void ) const;
  int          GetErrorNumber( void ) const;

  virtual std::string Dump( void ) const override;

private:
  const char*  file_;
  unsigned int line_;
  const char*  func_name_;
  int          error_number_;
};

#define TT_SYSTEM_CALL_EXCEPTION( f, n ) TtSystemCallException( f, n, __FILE__, __LINE__ );


// -- TtWindowsSystemCallException ---------------------------------------
class TtWindowsSystemCallException : public TtSystemCallException {
public:
  explicit TtWindowsSystemCallException( const char* func_name, unsigned int error_number, const char* file, unsigned line );

  std::string GetSystemErrorMessage( void ) const;

  virtual std::string Dump( void ) const override;
};

#define TT_WIN_SYSTEM_CALL_EXCEPTION( f ) TtWindowsSystemCallException( f, ::GetLastError(), __FILE__, __LINE__ );


// -- TtFileAccessException ----------------------------------------------
class TtFileAccessException : public TtException {
public:
  explicit TtFileAccessException( const std::string& filename, int error_number );

  const std::string& GetFileName( void ) const;
  int                GetErrorNumber( void ) const;

  virtual std::string Dump( void ) const override;

private:
  const std::string filename_;
  int               error_number_;
};


// -- TtInvalidOperationException ---------------------------------------------------
class TtInvalidOperationException : public TtException,
                                    public TtWithCauseClassException {
public:
  explicit TtInvalidOperationException( const std::type_info& klass );

  virtual std::string Dump( void ) const override;
};


// -- TtInvalidFileTypeException -----------------------------------------
class TtInvalidFileTypeException : public TtException,
                                   public TtWithCauseClassException {
public:
  explicit TtInvalidFileTypeException( const std::type_info& klass, const std::string& require_type );

  const std::string& GetRequireType( void ) const;

  virtual std::string Dump( void ) const override;

private:
  const std::string require_type_;
};
