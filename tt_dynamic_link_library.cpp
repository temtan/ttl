// tt_dynamic_link_library.cpp

#include "tt_windows_h_include.h"
#include <dbghelp.h>

#include "ttl_define.h"

#include "tt_dynamic_link_library.h"
#include "tt_string.h"

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "imagehlp.lib")


// -- TtDynamicLinkLibrary -----------------------------------------------
TtDynamicLinkLibrary
TtDynamicLinkLibrary::LoadLibrary( const std::string& filename )
{
  HINSTANCE handle = ::LoadLibrary( filename.c_str() );
  if ( handle == nullptr ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::LoadLibrary ) );
  }
  LPBYTE base = reinterpret_cast<LPBYTE>( handle );
  std::vector<std::string> function_name_list;

  unsigned long size;
  IMAGE_EXPORT_DIRECTORY* directory = static_cast<IMAGE_EXPORT_DIRECTORY*>(
    ::ImageDirectoryEntryToData( base, TRUE, IMAGE_DIRECTORY_ENTRY_EXPORT, &size ) );

  if ( directory ) {
    WORD* ordinals = reinterpret_cast<WORD*>( base + directory->AddressOfNameOrdinals );
    DWORD* names = reinterpret_cast<DWORD*>( base + directory->AddressOfNames );

    for ( unsigned int i = 0; i < directory->NumberOfFunctions; ++i ) {
      for ( unsigned int k = 0; k < directory->NumberOfNames; ++k ) {
        if ( ordinals[k] == i ) {
          function_name_list.push_back( reinterpret_cast<char*>( base + names[k] ) );
        }
      }
    }
  }

  return TtDynamicLinkLibrary( handle, true, function_name_list );
}


TtDynamicLinkLibrary::TtDynamicLinkLibrary( HINSTANCE handle, bool auto_destroy, const std::vector<std::string>& function_name_list ) :
TtUtility::WindowsHandleHolder<HINSTANCE>( [handle] ( void ) { ::FreeLibrary( handle ); }, handle, auto_destroy ),
function_name_list_( function_name_list )
{
}


const std::vector<std::string>&
TtDynamicLinkLibrary::GetFunctionNameList( void )
{
  return function_name_list_;
}


std::string
TtDynamicLinkLibrary::GetString( unsigned int id )
{
  size_t buf_size = 1024;
  for (;;) {
    TtString::UniqueString buf( buf_size );
    int ret = ::LoadString( handle_, id, buf.GetPointer(), static_cast<int>( buf_size ) );
    if ( ret == 0 ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::LoadString ) );
    }
    if ( ret < static_cast<int>( buf_size - 2 ) ) {
      return buf.ToString();
    }
    buf_size = static_cast<size_t>( buf_size * 1.5 );
  }
}


FARPROC
TtDynamicLinkLibrary::GetProcessAddress( const char* name )
{
  FARPROC tmp = ::GetProcAddress( handle_, name );
  if ( tmp == nullptr ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::GetProcAddress ) );
  }
  return tmp;
}


#ifdef TT_MAKE_TEMPLATE_INSTANCE_
using VOID_FUNC = void (*)( void );
template VOID_FUNC TtDynamicLinkLibrary::GetFunction<VOID_FUNC>( const std::string& );
template VOID_FUNC TtDynamicLinkLibrary::GetFunction<VOID_FUNC>( unsigned int );
#endif
