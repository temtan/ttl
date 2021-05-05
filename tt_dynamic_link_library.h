// tt_dynamic_link_library.h

#pragma once

#include <vector>

#include "tt_windows_h_include.h"

#include "tt_utility.h"
#include "tt_exception.h"

#pragma warning(push)
#pragma warning(disable : 4191)


// -- TtDynamicLinkLibrary -----------------------------------------------
class TtDynamicLinkLibrary : public TtUtility::WindowsHandleHolder<HINSTANCE> {
public:
  static TtDynamicLinkLibrary LoadLibrary( const std::string& filename );

public:
  explicit TtDynamicLinkLibrary( HINSTANCE handle, bool auto_destroy, const std::vector<std::string>& function_name_list );

  const std::vector<std::string>& GetFunctionNameList( void );

  std::string GetString( unsigned int id );

  template <class FUNCTION_TYPE>
  FUNCTION_TYPE GetFunction( const std::string& name ) {
    return reinterpret_cast<FUNCTION_TYPE>( this->GetProcessAddress( name.c_str() ) );
  }

  template <class FUNCTION_TYPE>
  FUNCTION_TYPE GetFunction( unsigned int index ) {
    if ( index >= function_name_list_.size() ) {
      throw TtInvalidOperationException( typeid( *this ) );
    }
    return reinterpret_cast<FUNCTION_TYPE>( this->GetProcessAddress( function_name_list_[index].c_str() ) );
  }

private:
  FARPROC GetProcessAddress( const char* name );

private:
  std::vector<std::string> function_name_list_;
};

#pragma warning(pop)
