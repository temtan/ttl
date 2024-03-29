// tt_text_template.cpp

#include <stdio.h>
#include <optional>

#include "ttl_define.h"
#include "tt_exception.h"
#include "tt_utility.h"
#include "tt_file_stream.h"

#include "tt_text_template.h"

// -- Document -----------------------------------------------------------
TtTextTemplate::Document::Document( void ) :
replace_table_(),
document_table_(),
post_processing_( nullptr ),
data_()
{
}

void
TtTextTemplate::Document::ResetAll( void )
{
  replace_table_.clear();
  document_table_.clear();
  post_processing_ = nullptr;
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
  post_processing_ = nullptr;
}


void
TtTextTemplate::Document::ParseFile( const std::string& path )
{
  std::string template_text = [&] ( void ) {
    TtFileReader reader( path, false );
    return reader.ReadAll();
  }();
  this->ParseText( template_text );
}

void
TtTextTemplate::Document::ParseText( const std::string& template_text )
{
  this->ParseTextWithClosedCheck( template_text.c_str(), [] ( const char* cp ) { return *cp == '\0'; } );
}

void
TtTextTemplate::Document::ParseTextWithClosedCheck( const char* text_start, ClosedCheckFunction check_function )
{
  data_.push_back( std::make_shared<std::string>() );
  for ( const char* cp = text_start; NOT( check_function( cp ) ); ++cp ) {
    switch ( *cp ) {
    case '@':
      if ( this->ParseAsReplace( cp ) ) {
        data_.push_back( std::make_shared<std::string>() );
      }
      else {
        data_.back()->append( cp, 1 );
      }
      break;

    case '%':
      if ( this->ParseAsDocument( cp ) ) {
        data_.push_back( std::make_shared<std::string>() );
      }
      else {
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
  return this->HasReplaceKey( key ) || this->HasDocumentKey( key );
}

bool
TtTextTemplate::Document::HasReplaceKey( const std::string& key )
{
  return replace_table_.contains( key );
}

bool
TtTextTemplate::Document::HasDocumentKey( const std::string& key )
{
  return document_table_.contains( key );
}


std::vector<std::string>
TtTextTemplate::Document::GetKeys( void )
{
  std::vector<std::string> v;
  for ( auto& one : replace_table_ ) {
    v.push_back( one.first );
  }
  for ( auto& one : document_table_ ) {
    v.push_back( one.first );
  }
  return v;
}

std::vector<std::string>
TtTextTemplate::Document::GetReplaceKeys( void )
{
  std::vector<std::string> v;
  for ( auto& one : replace_table_ ) {
    v.push_back( one.first );
  }
  return v;
}

std::vector<std::string>
TtTextTemplate::Document::GetDocumentKeys( void )
{
  std::vector<std::string> v;
  for ( auto& one : document_table_ ) {
    v.push_back( one.first );
  }
  return v;
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
  for ( const char* tmp = id_start + 1; *tmp != '\0'; ++tmp ) {
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
  auto is_id_char = [] ( const char* cp ) -> bool { return isalnum( *cp ) || *cp == '_'; };
  auto start_with = [] ( const char* cp, const std::string& str ) {
    for ( unsigned int i = 0; i < str.size(); ++i ) {
      if ( *(cp + i) == '\0' || *(cp + i) != str[i] ) {
        return false;
      }
    }
    return true;
  };

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

  class IllegalCloseException {};
  const char* document_restart = nullptr;
  auto closed_check = [&start_with, &key, &document_restart] ( const char* cp ) {
    if ( *cp == '\0' ) {
      throw IllegalCloseException();
    }
    if ( start_with( cp, "}%%" ) ) {
      std::string close_str( "}%%" + key + "%%" );
      document_restart = cp + (start_with( cp, close_str ) ? close_str.size() : 3);
      return true;
    }
    return false;
  };

  auto result_buffer = std::make_shared<std::string>();
  auto internal_document = std::shared_ptr<InternalDocument>( new InternalDocument( result_buffer ) );
  try {
    internal_document->ParseTextWithClosedCheck( document_start, closed_check );
  }
  catch ( IllegalCloseException ) {
    return false;
  }
  document_table_[key] = internal_document;
  cp = document_restart - 1;
  data_.push_back( result_buffer );
  return true;
  /*
  auto is_closed = [&start_with, &key] ( const char* cp ) -> std::optional<unsigned int> {
    if ( start_with( cp, "}%%" ) ) {
      std::string close_str( "}%%" + key + "%%" );
      return start_with( cp, close_str ) ? static_cast<unsigned int>( close_str.size() ) : 3;
    }
    return std::nullopt;
  };

  for ( ; (*tmp) != '\0'; ++tmp ) {
    if ( auto enclosed_size = is_closed( tmp ); enclosed_size ) {
      std::string document_text( document_start, tmp - document_start );
      data_.push_back( std::make_shared<std::string>() );
      document_table_[key] = std::shared_ptr<InternalDocument>( new InternalDocument( data_.back() ) );
      document_table_[key]->ParseText( document_text );

      cp = tmp + enclosed_size.value() - 1;
      data_.push_back( std::make_shared<std::string>() );
      return true;
    }
  }
  return false;
   */
}


void
TtTextTemplate::Document::RegisterPostProcessing( std::function<void ( std::string& )> processing )
{
  post_processing_ = processing;
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
  if ( post_processing_ ) {
    post_processing_( buf );
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
