// tt_logger.h

#pragma once

#include <stdio.h>
#include <string>


class TtLogger;

// -- TtLogFlusher -------------------------------------------------------
class TtLogFlusher {
private:
  friend class TtLogger;
  explicit TtLogFlusher( void ) {};
};

// -- TtLogger -----------------------------------------------------------
class TtLogger {
public:
  static TtLogFlusher FLUSH;

  explicit TtLogger( void );
  explicit TtLogger( const std::string& filename );
  virtual ~TtLogger();

  const std::string& GetFileName( void );

  void Open( void );
  void Close( void );

  template <class TYPE>
  TtLogger& operator <<( const TYPE& value ) {
    buffer_.append( TtUtility::ToStringFrom( value ) );
    return *this;
  }
  void operator <<( TtLogFlusher& flusher );

  virtual void Put( const std::string& value );

protected:
  virtual void Prefix( std::string& str );
  virtual void Postfix( std::string& str );

protected:
  std::string       filename_;
  FILE*             file_stream_;
  std::string       buffer_;
};


// -- TtLoggerStandard ---------------------------------------------------
class TtLoggerStandard : public TtLogger {
public:
  explicit TtLoggerStandard( void );
  explicit TtLoggerStandard( const std::string& filename );

protected:
  virtual void Prefix( std::string& str ) override;
  virtual void Postfix( std::string& str ) override;
};
