// tt_directory.cpp

#include <errno.h>

#include "tt_windows_h_include.h"

#include "ttl_define.h"
#include "tt_exception.h"
#include "tt_string.h"
#include "tt_path.h"

#include "tt_directory.h"

#pragma comment(lib, "kernel32.lib")


TtDirectory::TtDirectory( const std::string& path ) :
self_path_( path )
{
  if ( NOT( TtPath::IsDirectory( path ) ) ) {
    throw TtFileAccessException( path, ENOENT );
  }
}

const std::string&
TtDirectory::GetPath( void ) const
{
  return self_path_;
}

static void
get_entries_primitive( const std::string&         current,
                       const std::string&         search,
                       std::vector<std::string>&  array,
                       const TtDirectory::Filter& filter,
                       bool                       is_recursive )
{
  WIN32_FIND_DATA find_data;

  ::SetLastError( NO_ERROR );
  HANDLE search_handle = ::FindFirstFileEx(
    (current + "\\" + search).c_str(), FindExInfoStandard, &find_data, FindExSearchLimitToDirectories, nullptr, 0 );
  if ( search_handle == INVALID_HANDLE_VALUE ) {
    if ( ::GetLastError() != ERROR_FILE_NOT_FOUND && ::GetLastError() != ERROR_ACCESS_DENIED ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::FindFirstFileEx ) );
    }
    return;
  }
  TtUtility::DestructorCall closer( [search_handle] ( void ) { ::FindClose( search_handle ); } );

  do {
    std::string tmp = current + "\\" + find_data.cFileName;
    if ( filter( tmp ) ) {
      array.push_back( tmp );
    }
    if ( is_recursive ) {
      if ( (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
           NOT( TtString::EndWith( tmp, "\\." ) ) && NOT( TtString::EndWith( tmp, "\\.." ) ) ) {
        get_entries_primitive( tmp, search, array, filter, is_recursive );
      }
    }
    ::SetLastError( NO_ERROR );
  }
  while ( ::FindNextFile( search_handle, &find_data ) != 0 );

  if ( ::GetLastError() != ERROR_NO_MORE_FILES ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::FindNextFile ) );
  }
}


TtDirectory::Filter TtDirectory::All( [] ( const std::string& ) { return true; } );
TtDirectory::Filter TtDirectory::FileOnly( [] ( const std::string& path ) { return NOT( TtPath::IsDirectory( path ) ); } );
TtDirectory::Filter TtDirectory::DirectoryOnly( TtPath::IsDirectory );

std::vector<std::string>
TtDirectory::GetEntries( const std::string& search, const Filter& filter ) const
{
  std::vector<std::string> array;
  get_entries_primitive( self_path_, search, array, filter, false );
  return array;
}

std::vector<std::string>
TtDirectory::GetEntriesRecursive( const std::string& search, const Filter& filter ) const
{
  std::vector<std::string> array;
  get_entries_primitive( self_path_, search, array, filter, true );
  return array;
}

#ifdef TT_MAKE_TEMPLATE_INSTANCE_
template std::vector<std::string> TtDirectory::GetEntries( const std::string&, bool ( const std::string& ) ) const;
#endif
