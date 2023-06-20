// tt_path.cpp

#include <stdio.h>

#pragma warning(push)
#  pragma warning( disable: 4005 )
#  include <shlwapi.h>
#pragma warning(pop)

#include <memory>

#include "ttl_define.h"
#include "tt_exception.h"
#include "tt_string.h"

#include "tt_path.h"

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "shlwapi.lib")

#pragma warning(disable : 4800)


// パス文字列の判定・検証
bool
TtPath::IsDirectory( const std::string& path )
{
  return ::PathIsDirectory( path.c_str() );
}

bool
TtPath::IsFileSpec( const std::string& path )
{
  return ::PathIsFileSpec( path.c_str() );
}

bool
TtPath::IsRelative( const std::string& path )
{
  return ::PathIsRelative( path.c_str() );
}

bool
TtPath::IsRoot( const std::string& path )
{
  return ::PathIsRoot( path.c_str() );
}

bool
TtPath::MatchSpec( const std::string& path, const std::string& match )
{
  return ::PathMatchSpec( path.c_str(), match.c_str() );
}

bool
TtPath::FileExists( const std::string& path )
{
  return ::PathFileExists( path.c_str() );
}

bool
TtPath::AreTheseSamePath( const std::string& x, const std::string& y )
{
  auto f = [] ( const std::string& path ) -> std::string {
    if ( TtPath::IsRelative( path ) ) {
      return TtPath::Canonicalize( TtPath::ExpandPath( path ) );
    }
    return TtPath::Canonicalize( path );
  };
  return f( x ) == f( y );
}


// 検索など
std::string
TtPath::FindFileName( const std::string& path )
{
  return ::PathFindFileName( path.c_str() );
}

std::string
TtPath::FindExtension( const std::string& path )
{
  return ::PathFindExtension( path.c_str() );
}

// 編集など
std::string
TtPath::EditPath( const std::string& path, bool f_drive, bool f_dir, bool f_fname, bool f_ext )
{
  char drive[MAX_PATH], dir[MAX_PATH], fname[MAX_PATH], ext[MAX_PATH];
  _splitpath_s( path.c_str(), drive, sizeof( drive ), dir, sizeof( dir ), fname, sizeof( fname ), ext, sizeof( ext ) );

  std::string tmp;
  tmp.append( f_drive ? drive : "" );
  tmp.append( f_dir   ? dir   : "" );
  tmp.append( f_fname ? fname : "" );
  tmp.append( f_ext   ? ext   : "" );
  return tmp;
}

std::string
TtPath::DirName( const std::string& path )
{
  std::string tmp = TtPath::EditPath( path, true, true, false, false );
  if ( tmp.size() == 0 ) {
    tmp = ".";
  }
  else if ( tmp[tmp.size() - 1] == '\\' || tmp[tmp.size() - 1] == '/' ) {
    tmp.erase( tmp.size() - 1 );
  }
  return tmp;
}

std::string
TtPath::BaseName( const std::string& path, const std::string& suffix )
{
  return TtPath::EditPath( path, false, false, true, (suffix.empty() || NOT( TtString::EndWith( path, suffix ) )) );
}

std::string
TtPath::CompactPath( const std::string& src, unsigned int max )
{
  TtString::UniqueString buf( max + 1 );
  if ( ::PathCompactPathEx( buf.GetPointer(), src.c_str(), max, '\\' ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::PathCompactPathEx ) );
  }
  return buf.ToString();
}

std::string
TtPath::QuoteIfHasSpaces( const std::string& path )
{
  TtString::UniqueString buf( ( path.size() > MAX_PATH ? path.size() : MAX_PATH ) + 10 );
  path.copy( buf.GetPointer(), path.size() );
  buf.GetPointer()[path.size()] = '\0';
  ::PathQuoteSpaces( buf.GetPointer() );
  return buf.ToString();
}

std::string
TtPath::RemoveExtension( const std::string& path )
{
  return TtPath::EditPath( path, true, true, true, false );
}

std::string
TtPath::ChangeExtension( const std::string& path, const std::string& ext )
{
  return TtPath::RemoveExtension( path ) + "." + ext;
}


std::optional<std::string>
TtPath::GetRelativePathTo( const std::string& from, bool from_is_directory, const std::string to, bool to_is_directory )
{
  TtString::UniqueString buf( MAX_PATH + 10 );
  DWORD from_attr = from_is_directory ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL;
  DWORD to_attr = to_is_directory ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL;
  bool ret = ::PathRelativePathTo( buf.GetPointer(), from.c_str(), from_attr, to.c_str(), to_attr  );
  return ret ? std::optional<std::string>( buf.ToString() ) : std::nullopt;
}

std::optional<std::string>
TtPath::CanonicalizeFrom( const std::string& path )
{
  TtString::UniqueString buf( MAX_PATH + 10 );
  bool ret = ::PathCanonicalize( buf.GetPointer(), path.c_str() );
  return ret ? std::optional<std::string>( buf.GetPointer() ) : std::nullopt;
}

std::string
TtPath::Canonicalize( const std::string& path )
{
  auto result = TtPath::CanonicalizeFrom( path );
  return result ? result.value() : path;
}


// 取得など
std::string
TtPath::GetExecutingFilePath( void )
{
  char buf[MAX_PATH + 1];
  ::GetModuleFileName( nullptr, buf, sizeof( buf ) );
  return std::string( buf );
}

std::string
TtPath::GetExecutingFilePathWithoutExtension( void )
{
  return TtPath::RemoveExtension( TtPath::GetExecutingFilePath() );
}

std::string
TtPath::GetExecutingFilePathCustomExtension( const std::string& extension )
{
  std::string buf = TtPath::GetExecutingFilePathWithoutExtension();
  buf.append( "." );
  buf.append( extension );
  return buf;
}

std::string
TtPath::GetExecutingDirectoryPath( void )
{
  return TtPath::DirName( TtPath::GetExecutingFilePath() );
}


std::string
TtPath::ExpandPath( const std::string& path )
{
  TtString::UniqueString buf( ( path.size() > MAX_PATH ? path.size() : MAX_PATH ) + 1 );
  ::PathSearchAndQualify( path.c_str(), buf.GetPointer(), static_cast<UINT>( buf.GetCapacity() ) );
  return buf.ToString();
}


std::string
TtPath::GetFileNotExistPathFrom( const std::string& path )
{
  if ( NOT( TtPath::FileExists( path ) ) ) {
    return path;
  }
  for ( unsigned int i = 1; i < std::numeric_limits<unsigned int>::max(); ++i ) {
    auto new_path = TtPath::ChangeExtension( path, TtUtility::ToStringFrom( i ) ) + TtPath::FindExtension( path );
    if ( NOT( TtPath::FileExists( new_path ) ) ) {
      return new_path;
    }
  }
  throw TT_INTERNAL_EXCEPTION;
}


std::string
TtPath::RemoveCanNotUseCharacterAsFilePathFrom( const std::string& path )
{
  auto buffer = std::make_unique<char[]>( path.size() + 1 );
  char* c = buffer.get();
  const char* s = path.c_str();
  if ( NOT( TtPath::IsRelative( path ) ) ) {
    *c = *s;
    ++c;
    ++s;
    *c = *s;
    ++c;
    ++s;
  }
  while ( *s != '\0' ) {
    *c = *s;
    switch ( *s ) {
    case '/':
    case ':':
    case '*':
    case '?':
    case '"':
    case '<':
    case '>':
      break;

    case '|':
      if ( s != path.c_str() && (static_cast<unsigned char>( s[-1] ) >= 0x81 && static_cast<unsigned char>( s[-1] ) <= 0x9F) ) {
        ++c;
      }
      break;

    default:
      ++c;
      break;
    }
    ++s;
  }
  *c = *s;
  return buffer.get();
}
