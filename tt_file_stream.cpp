// tt_file_stream.cpp

#include "ttl_define.h"
#include "tt_exception.h"

#include "tt_file_stream.h"


// -- TtFileHandleHolder -------------------------------------------------
TtFileHandleHolder::TtFileHandleHolder( const std::string& path, const std::string& mode ) :
path_( path ),
file_( NULL )
{
  errno_t error_number = ::fopen_s( &file_, path.c_str(), mode.c_str() );
  if ( file_ == NULL ) {
    throw TtFileAccessException( path, error_number );
  }
}

TtFileHandleHolder::~TtFileHandleHolder( void )
{
  this->Close();
}

FILE*
TtFileHandleHolder::GetHandle( void )
{
  return file_;
}

bool
TtFileHandleHolder::IsEOF( void )
{
  return std::feof( file_ );
}

bool
TtFileHandleHolder::HasError( void )
{
  return std::ferror( file_ );
}


void
TtFileHandleHolder::Flush( void )
{
  auto ret = std::fflush( file_ );
  if ( ret == EOF ) {
    throw TtFileAccessException( path_, errno );
  }
}

void
TtFileHandleHolder::Close( void )
{
  if ( file_ ) {
    std::fclose( file_ );
    file_ = NULL;
  }
}

// -- TtFileReader -------------------------------------------------------
TtFileReader::TtFileReader( const std::string& path, bool binary_mode ) :
TtFileHandleHolder( path, binary_mode ? "rb" : "r" )
{
}

bool
TtFileReader::HasUTF8BOM( void )
{
  std::fpos_t pos;
  std::fgetpos( file_, &pos );
  std::fseek( file_, 0, SEEK_SET );

  unsigned char buf[3];
  size_t ret = std::fread( buf, 1, 3, file_ );

  std::fsetpos( file_, &pos );

  return (ret == 3) && (buf[0] == 0xEF) && (buf[1] == 0xBB) && (buf[2] == 0xBF);
}


std::optional<std::string>
TtFileReader::ReadLine( void )
{
  return this->ReadLineChompFlag( false );
}

std::optional<std::string>
TtFileReader::ReadLineWithChomp( void )
{
  return this->ReadLineChompFlag( true );
}

std::optional<std::string>
TtFileReader::ReadLineChompFlag( bool chomp )
{
  char buffer[1024];

  char* ret = std::fgets( buffer, sizeof( buffer ), file_ );
  if ( ret == NULL ) {
    if ( this->HasError() ) {
      throw TtFileAccessException( path_, errno );
    }
    return std::nullopt;
  }
  std::string tmp = buffer;

  for (;;) {
    if ( tmp.back() == '\n' || tmp.back() == '\r' ) {
      if ( chomp ) {
        tmp.pop_back();
        if ( tmp.back() == '\r' ) {
          tmp.pop_back();
        }
      }
      return tmp;
    }
    ret = std::fgets( buffer, sizeof( buffer ), file_ );
    if ( ret == NULL ) {
      if ( this->HasError() ) {
        throw TtFileAccessException( path_, errno );
      }
      return tmp;
    }
    tmp.append( buffer );
  }
}


std::string
TtFileReader::ReadAll( void )
{
  std::string sum;
  for (;;) {
    auto tmp = this->ReadLine();
    if ( NOT( tmp ) ) {
      break;
    }
    sum.append( *tmp );
  }
  return sum;
}


// -- TtFileWriter -------------------------------------------------------
TtFileWriter::TtFileWriter( const std::string& path, bool additonal_mode, bool binary_mode ) :
TtFileHandleHolder( path, additonal_mode ? (binary_mode ? "ab" : "a") : (binary_mode ? "wb" : "w") )
{
}

void
TtFileWriter::WriteString( const std::string& str )
{
  auto ret = std::fputs( str.c_str(), file_ );
  if ( ret == EOF ) {
    throw TtFileAccessException( path_, errno );
  }
};

void
TtFileWriter::Puts( const std::string& str )
{
  this->WriteString( str );
  this->WriteString( "\n" );
}



#ifdef TT_MAKE_TEMPLATE_INSTANCE_
template char  TtFileReader::ReadAs<char>( void );
template short TtFileReader::ReadAs<short>( void );
template int   TtFileReader::ReadAs<int>( void );

template void TtFileWriter::WriteAs<char>( char );
template void TtFileWriter::WriteAs<short>( short );
template void TtFileWriter::WriteAs<int>( int );
#endif
