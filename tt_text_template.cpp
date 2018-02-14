// tt_text_template.cpp

#include <stdio.h>

#include "ttl_define.h"
#include "tt_exception.h"
#include "tt_utility.h"

#include "tt_text_template.h"

// -- Document -----------------------------------------------------------
TtTextTemplate::Document::Document( void ) :
replace_table_(),
document_table_(),
data_()
{
}

void
TtTextTemplate::Document::ResetAll( void )
{
  replace_table_.clear();
  document_table_.clear();
  data_.clear();
}

void
TtTextTemplate::Document::ResetRegistered( void )
{
  for ( auto& it : replace_table_ ) {
    it.second->clear();
  }
  for ( auto& it : document_table_ ) {
    it.second->result_buffer_->clear();
  }
}


void
TtTextTemplate::Document::ParseFile( const std::string& path )
{
  FILE* file;
  int error_number = fopen_s( &file, path.c_str(), "r" );
  if ( error_number != 0 ) {
    throw TtFileAccessException( path, error_number );
  }
  std::string template_text;
  {
    TtUtility::DestructorCall file_closer( [&file]( void ) { fclose( file ); } );

    for (;;) {
      int c = fgetc( file );
      if ( c == EOF ) {
        // ferror ÇÕê≥èÌÇÃèÍçá 0 Çï‘Ç∑
        if ( ferror( file ) == 0 && feof( file ) ) {
          break;
        }
        throw TT_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( fgetc ), 0 );
      }
      template_text.append( 1, static_cast<char>( c ) );
    }
  }
  this->ParseText( template_text );
}

void
TtTextTemplate::Document::ParseText( const std::string& template_text )
{
  data_.push_back( std::make_shared<std::string>() );
  for ( const char* cp = template_text.c_str(); *cp != '\0'; ++cp ) {
    switch ( *cp ) {
    case '@':
      if ( NOT( this->ParseAsReplace( cp ) ) ) {
        data_.back()->append( cp, 1 );
      }
      break;

    case '%':
      if ( NOT( this->ParseAsDocument( cp ) ) ) {
        data_.back()->append( cp, 1 );
      }
      break;

    default:
      data_.back()->append( cp, 1 );
    }
  }
}


bool
TtTextTemplate::Document::HasKey( const std::string& key )
{
  return ( replace_table_.find( key ) != replace_table_.end() ||
           document_table_.find( key ) != document_table_.end() );
}


bool
TtTextTemplate::Document::ParseAsReplace( const char*& cp )
{
  auto is_id_char = []( const char* cp ) -> bool { return isalnum( *cp ) || *cp == '_'; };

  if ( *(cp + 1) != '@' ) {
    return false;
  }
  if ( NOT( is_id_char( (cp + 2) ) ) ) {
    return false;
  }

  const char* id_start = cp + 2;
  for ( const char* tmp = id_start + 1; tmp != '\0'; ++tmp ) {
    if ( isalnum( *tmp ) || *tmp == '_' ) {
      continue;
    }
    if ( *tmp == '@' && *(tmp + 1) == '@' ) {
      std::string key( id_start, tmp - id_start );
      if ( replace_table_.find( key ) == replace_table_.end() ) {
        data_.push_back( std::make_shared<std::string>( "@@" + key + "@@" ) );
        replace_table_[key] = data_.back();
      }
      else {
        data_.push_back( replace_table_[key] );
      }

      cp = tmp + 1;
      data_.push_back( std::make_shared<std::string>() );
      return true;
    }
    else {
      return false;
    }
  }
  return false;
}

bool
TtTextTemplate::Document::ParseAsDocument( const char*& cp )
{
  auto is_id_char = []( const char* cp ) -> bool { return isalnum( *cp ) || *cp == '_'; };

  if ( *(cp + 1) != '%' ) {
    return false;
  }
  if ( NOT( is_id_char( (cp + 2) ) ) ) {
    return false;
  }
  const char* id_start = cp + 2;
  const char* tmp = id_start + 1;

  for (;; ++tmp ) {
    if ( is_id_char( tmp ) ) {
      continue;
    }
    if ( *tmp == '%' && *(tmp + 1) == '%' && *(tmp + 2) == '{' ) {
      break;
    }
    return false;
  }
  std::string key( id_start, tmp - id_start );
  const char* document_start = tmp + 3;
  std::string close_str( "}%%" + key + "%%" );

  auto is_closed = [&close_str]( const char* cp ) -> bool {
    for ( unsigned int i = 0; i < close_str.size(); ++i ) {
      if ( *(cp + i) != close_str[i] ) {
        return false;
      }
    }
    return true;
  };

  for ( ; tmp != '\0'; ++tmp ) {
    if ( is_closed( tmp ) ) {
      std::string document_text( document_start, tmp - document_start );
      data_.push_back( std::make_shared<std::string>() );
      document_table_[key] = std::shared_ptr<InternalDocument>( new InternalDocument( data_.back() ) );
      document_table_[key]->ParseText( document_text );

      cp = tmp + close_str.size() - 1;
      data_.push_back( std::make_shared<std::string>() );
      return true;
    }
  }
  return false;
}


void
TtTextTemplate::Document::RegisterAsReplace( const std::string& key, const std::string& replacement )
{
  auto it = replace_table_.find( key );
  if ( it == replace_table_.end() ) {
    throw KeyNotFoundException( key );
  }
  it->second->assign( replacement );
}

void
TtTextTemplate::Document::RegisterAsDocument( const std::string& key, Callback callback )
{
  auto it = document_table_.find( key );
  if ( it == document_table_.end() ) {
    throw KeyNotFoundException( key );
  }
  it->second->ResetRegistered();
  callback( *(it->second) );
  it->second->result_buffer_->append( it->second->MakeText() );
}


TtTextTemplate::Document::Registrar::Registrar( const std::string& key, Document& document ) :
key_( key ),
document_( document )
{
}

void
TtTextTemplate::Document::Registrar::operator =( const char* replacement )
{
  this->operator =( std::string( replacement ) );
}

void
TtTextTemplate::Document::Registrar::operator =( const std::string& replacement )
{
  document_.RegisterAsReplace( key_, replacement );
}

void
TtTextTemplate::Document::Registrar::operator =( TtTextTemplate::Document::Callback callback )
{
  document_.RegisterAsDocument( key_, callback );
}

void
TtTextTemplate::Document::Registrar::operator =( bool flag )
{
  if ( flag ) {
    document_.RegisterAsDocument( key_, [] ( InternalDocument& ) -> void {} );
  }
}


TtTextTemplate::Document::Registrar
TtTextTemplate::Document::operator []( const std::string& key )
{
  return TtTextTemplate::Document::Registrar( key, *this );
}


std::string
TtTextTemplate::Document::MakeText( void )
{
  std::string buf;
  for ( auto& it : data_ ) {
    buf.append( *it );
  }
  return buf;
}


// -- other --------------------------------------------------------------
TtTextTemplate::InternalDocument::InternalDocument( std::shared_ptr<std::string> result_buffer ) :
result_buffer_( result_buffer )
{
}


TtTextTemplate::FileDocument::FileDocument( const std::string& filename ) :
Document()
{
  this->ParseFile( filename );
}

// -- exception ----------------------------------------------------------
TtTextTemplate::KeyNotFoundException::KeyNotFoundException( const std::string& key ) :
key_( key )
{
}

const std::string&
TtTextTemplate::KeyNotFoundException::GetKey( void ) const
{
  return key_;
}


std::string
TtTextTemplate::KeyNotFoundException::Dump( void ) const
{
  std::ostringstream os;
  os << typeid( *this ).name() << " ; key : " << this->GetKey();
  return os.str();
}
