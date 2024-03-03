// tt_ini_file.cpp

#include "tt_windows_h_include.h"

#include "ttl_define.h"
#include "tt_exception.h"
#include "tt_path.h"
#include "tt_string.h"
#include "tt_utility.h"

#include "tt_ini_file.h"

#pragma comment(lib, "kernel32.lib")


namespace {
  std::string UTF16Serialize( const std::wstring& str ) {
    const unsigned char* c = reinterpret_cast<const unsigned char*>( str.c_str() );
    std::string result;
    for ( unsigned int i = 0; i < str.size() * sizeof( std::wstring::value_type ); ++i ) {
      char buf[16];
      sprintf_s( buf, sizeof( buf ), "%02x", *c );
      result.append( buf );
      ++c;
    }
    return result;
  }

  std::wstring UTF16Deserialize( const std::string& data ) {
    const char* c = data.c_str();
    auto target_buf = std::make_unique<unsigned char[]>( (data.size() / 2) + 2 );
    unsigned char* target = target_buf.get();
    for ( unsigned int i = 0; i < data.size() / 2; ++i ) {
      char buf[3] = {*c, *(c + 1), 0};
      unsigned int ret;
      if ( NOT( TtUtility::StringToInteger( buf, reinterpret_cast<int*>( &ret ), 16 ) ) ) {
        return L"";
      }
      *target = static_cast<unsigned char>( ret );
      ++target;
      c += 2;
    }
    target[0] = '\0';
    target[1] = '\0';
    return reinterpret_cast<wchar_t*>( target_buf.get() );
  }
}


// -- TtIniFile ----------------------------------------------------------
TtIniFile::TtIniFile( const std::string& filename, unsigned int buffer_size ) :
filename_( TtPath::IsRelative( filename ) ? TtPath::ExpandPath( filename ) : filename ),
buffer_( buffer_size )
{
}

TtIniFile::~TtIniFile()
{
  this->Flush();
}

const std::string&
TtIniFile::GetFileName( void ) const
{
  return filename_;
}

void
TtIniFile::SetFileName( const std::string& filename )
{
  filename_ = filename;
}

TtIniSection
TtIniFile::GetSection( const std::string& name )
{
  return TtIniSection( *this, name, buffer_ );
}

TtIniSection
TtIniFile::operator []( const std::string& name )
{
  return this->GetSection( name );
}


std::vector<TtIniSection>
TtIniFile::GetSections( void )
{
  const unsigned int max = 1024 * 1024;
  for (;;) {
    DWORD ret = ::GetPrivateProfileString(
      NULL, NULL, "", buffer_.GetPointer(), static_cast<DWORD>( buffer_.GetCapacity() ),
      filename_.c_str() );
    if ( buffer_.GetCapacity() >= max || ret != buffer_.GetCapacity() - 2 ) {
      break;
    }
    buffer_.ExtendCapacity( buffer_.GetCapacity() / 4 );
  }

  std::vector<TtIniSection> v;
  for ( std::string& name : TtString::ToVectorFromRangedString( buffer_.GetPointer(), buffer_.GetCapacity() ) ) {
    v.push_back( this->GetSection( name ) );
  }
  return v;
}


void
TtIniFile::Flush( void )
{
  ::WritePrivateProfileString( nullptr, nullptr, nullptr, filename_.c_str() );
}


// -- TtIniSection -------------------------------------------------------
TtIniSection::TtIniSection( TtIniFile& ini_file, const std::string& name, TtString::SharedString& buffer ) :
ini_file_( ini_file ),
name_( name ),
buffer_( buffer )
{
}

TtIniFile&
TtIniSection::GetIniFile( void ) const
{
  return ini_file_;
}

const std::string&
TtIniSection::GetName( void ) const
{
  return name_;
}


std::vector<std::string>
TtIniSection::GetKeys( void ) const
{
  const unsigned int max = 1024 * 1024;
  for (;;) {
    DWORD ret = ::GetPrivateProfileString(
      name_.c_str(), NULL, "", buffer_.GetPointer(), static_cast<DWORD>( buffer_.GetCapacity() ),
      ini_file_.GetFileName().c_str() );
    if ( buffer_.GetCapacity() >= max || ret != buffer_.GetCapacity() - 2 ) {
      break;
    }
    buffer_.ExtendCapacity( buffer_.GetCapacity() / 4 );
  }

  return TtString::ToVectorFromRangedString( buffer_.GetPointer(), buffer_.GetCapacity() );
}


bool
TtIniSection::GetBoolean( const std::string& key, bool default_value )
{
  return this->GetInteger( key, default_value ? 1u : 0u ) ? true : false;
}

void
TtIniSection::SetBoolean( const std::string& key, bool value )
{
  this->SetInteger( key, value ? 1u : 0u );
}


unsigned int
TtIniSection::GetInteger( const std::string& key, unsigned int default_value )
{
  return ::GetPrivateProfileInt( name_.c_str(), key.c_str(), default_value, ini_file_.GetFileName().c_str() );
}

void
TtIniSection::SetInteger( const std::string& key, unsigned int value )
{
  this->SetString( key, TtUtility::ToStringFrom( value ) );
}


double
TtIniSection::GetDouble( const std::string& key, double default_value )
{
  std::string tmp = this->GetString( key );
  double value;
  return TtUtility::StringToDouble( tmp, &value ) ? value : default_value;
}

void
TtIniSection::SetDouble( const std::string& key, double value )
{
  this->SetString( key, TtUtility::ToStringFrom( value ) );
}


std::string
TtIniSection::GetString( const std::string& key, const std::string& default_value )
{
  const unsigned int max = 1024 * 1024;
  for (;;) {
    DWORD ret = ::GetPrivateProfileString(
      name_.c_str(), key.c_str(), default_value.c_str(),
      buffer_.GetPointer(), static_cast<DWORD>( buffer_.GetCapacity() ), ini_file_.GetFileName().c_str() );
    if ( buffer_.GetCapacity() >= max || ret != buffer_.GetCapacity() - 1 ) {
      break;
    }
    buffer_.ExtendCapacity( buffer_.GetCapacity() / 4 );
  }
  return buffer_.GetPointer();
}

void
TtIniSection::SetString( const std::string& key, const std::string& value )
{
  TtUtility::CallWindowsSystemFunctionWithErrorHandling(
    [&] ( void ) {
      ::WritePrivateProfileString( name_.c_str(), key.c_str(), value.c_str(), ini_file_.GetFileName().c_str() );
    },
    [] ( void ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::WritePrivateProfileString ) );
    } );
}

std::wstring
TtIniSection::GetStringUTF16( const std::string& key, const std::wstring& default_value )
{
  return UTF16Deserialize( this->GetString( key, UTF16Serialize( default_value ) ) );
}

void
TtIniSection::SetStringUTF16( const std::string& key, const std::wstring& value )
{
  this->SetString( key, UTF16Serialize( value ) );
}


void
TtIniSection::RemoveKey( const std::string& key )
{
  TtUtility::CallWindowsSystemFunctionWithErrorHandling(
    [&] ( void ) {
      ::WritePrivateProfileString( name_.c_str(), key.c_str(), nullptr, ini_file_.GetFileName().c_str() );
    },
    [] ( void ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::WritePrivateProfileString ) );
    } );
}


void
TtIniSection::Flush( void )
{
  ini_file_.Flush();
}

void
TtIniSection::Clear( void )
{
  TtUtility::CallWindowsSystemFunctionWithErrorHandling(
    [&] ( void ) {
      ::WritePrivateProfileString( name_.c_str(), nullptr, nullptr, ini_file_.GetFileName().c_str() );
    },
    [] ( void ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::WritePrivateProfileString ) );
    } );
}
