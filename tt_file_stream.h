// tt_file_stream.h

#pragma once

#include <string>
#include <optional>


// -- TtFileHandleHolder -------------------------------------------------
class TtFileHandleHolder {
public:
  explicit TtFileHandleHolder( const std::string& path, const std::string& mode );
  virtual ~TtFileHandleHolder( void );

  FILE* GetHandle( void );

  bool IsEOF( void );
  bool HasError( void );

  void Flush( void );

  void Close( void );

protected:
  const std::string path_;
  FILE* file_;
};


// -- TtFileReader -------------------------------------------------------
class TtFileReader : public TtFileHandleHolder {
public:
  explicit TtFileReader( const std::string& path, bool binary_mode );

  bool HasUTF8BOM( void );

  std::optional<std::string> ReadLine( void );
  std::optional<std::string> ReadLineWithChomp( void );
  std::optional<std::string> ReadLineChompFlag( bool chomp );

  std::string ReadAll( void );

  // binary
  template <class TYPE>
  TYPE ReadAs( void ) {
    TYPE buf = 0;
    if ( std::fread( reinterpret_cast<char*>( &buf ), sizeof( TYPE ), 1, file_ ) != 1 ) {
      throw TtFileAccessException( path_, errno );
    }
    return buf;
  }
};

// -- TtFileWriter -------------------------------------------------------
class TtFileWriter : public TtFileHandleHolder {
public:
  explicit TtFileWriter( const std::string& path, bool additonal_mode, bool binary_mode );

  void WriteString( const std::string& str );
  void Puts( const std::string& str );

  // binary
  template <class TYPE>
  void WriteAs( TYPE data ) {
    auto ret = std::fwrite( &data, sizeof( data ), 1, file_ );
    if ( ret != 1 ) {
      throw TtFileAccessException( path_, errno );
    }
  }
};
