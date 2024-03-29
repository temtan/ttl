// ttl_define.h

#pragma once

#ifndef NOT_USE
#define NOT_USE( var ) ((void)var)
#endif

#ifndef NOT
#define NOT( v ) (!(v))
#endif

#define REAL_FUNC_NAME_OF( f ) (#f)

#ifdef DEBUG
#  define FUNC_NAME_OF( f ) ((void)(&f), (*(&f)) == ((&f)), #f)
#else
#  define FUNC_NAME_OF( f ) (#f)
#endif

#define TO_STR_1( v ) #v
#define TO_STR_2( v ) TO_STR_1( v )
#define FILE_LINE     __FILE__ "(" TO_STR_2( __LINE__ ) ")"

#ifdef DEBUG
#include <stdio.h>
#include <string>
#  define PCD()            (printf( "%s(%d) : gone.\n", __FILE__, __LINE__ ), fflush( stdout ))
#  define PCA( type, obj ) (printf( "%s(%d) : %s = " type "\n", __FILE__, __LINE__, #obj, (obj) ), fflush( stdout ))
#  define PCC( c )         PCA( "%c", c )
#  define PCWC( c )        PCA( "%lc", c )
#  define PCI( i )         PCA( "%d", i )
#  define PCX( x )         PCA( "0x%X", x )
#  define PCP( p )         PCA( "%p", p )
#  define PCS( s )         PCS_BODY( s, #s, __FILE__, __LINE__ )
#  define PCF( f )         PCA( "%f", f )
#  define PCLL( l )        PCA( "%lld", l )
#  define PCLF( d )        PCA( "%lf", d )
#  define PCSS( s )        (printf( "%s(%d) : %s = %s\n", __FILE__, __LINE__, #s, (s).c_str() ), fflush( stdout ))
#  define PCB( b )         (printf( "%s(%d) : %s = %s\n", __FILE__, __LINE__, #b, (b) ? "true" : "false" ), fflush( stdout ))
inline void PCS_BODY( const char* s, const char* name, const char* f, unsigned int l ) {
  printf( "%s(%d) : %s = %s\n", f, l, name, s );
  fflush( stdout );
}
inline void PCS_BODY( const std::string& s, const char* name, const char* f, unsigned int l  ) { PCS_BODY( s.c_str(), name, f, l ); }

inline void PCS_BODY( const wchar_t* s, const char* name, const char* f, unsigned int l ) {
  wprintf( L"%hs(%d) : %hs = %s\n", f, l, name, s );
  fflush( stdout );
}
inline void PCS_BODY( const std::wstring& s, const char* name, const char* f, unsigned int l ) { PCS_BODY( s.c_str(), name, f, l ); }


#  include "tt_utility.h"
#  define PMD()            TtUtility::MessageBoxForDebug( "", "gone", 0, __FILE__, __LINE__ )
#  define PMA( type, obj ) TtUtility::MessageBoxForDebug( type, #obj, (void*)(obj), __FILE__, __LINE__ )
#  define PMC( c )         PMA( "%c", c )
#  define PMI( i )         PMA( "%d", i )
#  define PMX( x )         PMA( "0x%X", x )
#  define PMP( p )         PMA( "%p", p )
#  define PMS( s )         PMA( "%s", s )
#  define PMF( f )         PMA( "%f", f )
#  define PMLL( l )        PMA( "%lld", l )
#  define PMLF( d )        PMA( "%lf", d )
#  define PMSS( s )        TtUtility::MessageBoxForDebug( "%s", #s, (void*)((s).c_str()), __FILE__, __LINE__ )
#  define PMB( b )         TtUtility::MessageBoxForDebug( "%s", #b, (void*)((b) ? "true" : "false" ), __FILE__, __LINE__ )
#endif
