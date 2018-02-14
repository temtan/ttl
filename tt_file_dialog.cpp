// tt_file_dialog.cpp

#include "ttl_define.h"
#include "tt_exception.h"
#include "tt_window_controls.h"

#include "tt_file_dialog.h"

#pragma comment(lib, "comdlg32.lib")


// -- TtFileDialog -------------------------------------------------------
TtFileDialog::TtFileDialog( unsigned int buffer_size ) :
check_file_exists_( false ),
check_path_exists_( true ),
default_extension_( "" ),
dereference_links_( true ),
file_name_( "" ),
filter_index_( 1 ),
initial_directory_( "" ),
multiselect_( false ),
restore_directory_( false ),
show_help_( false ),
show_place_bar_( true ),
title_( "" ),
file_names_(),
filters_(),
buffer_( buffer_size )
{
}


bool
TtFileDialog::GetCheckFileExists( void ) const
{
  return check_file_exists_;
}

void
TtFileDialog::SetCheckFileExists( bool flag )
{
  check_file_exists_ = flag;
}

bool
TtFileDialog::GetCheckPathExists( void ) const
{
  return check_path_exists_;
}

void
TtFileDialog::SetCheckPathExists( bool flag )
{
  check_path_exists_ = flag;
}

const std::string&
TtFileDialog::GetDefaultExtension( void ) const
{
  return default_extension_;
}

void
TtFileDialog::SetDefaultExtension( const std::string& extension )
{
  default_extension_ = extension;
}

bool
TtFileDialog::GetDereferenceLinks( void ) const
{
  return dereference_links_;
}

void
TtFileDialog::SetDereferenceLinks( bool flag )
{
  dereference_links_ = flag;
}

const std::string&
TtFileDialog::GetFileName( void ) const
{
  return file_name_;
}

void
TtFileDialog::SetFileName( const std::string& file_name )
{
  file_name_ = file_name;
}

int
TtFileDialog::GetFilterIndex( void ) const
{
  return filter_index_;
}

void
TtFileDialog::SetFilterIndex( int index )
{
  filter_index_ = index;
}

const std::string&
TtFileDialog::GetInitialDirectory( void ) const
{
  return initial_directory_;
}

void
TtFileDialog::SetInitialDirectory( const std::string& directory )
{
  initial_directory_ = directory;
}

bool
TtFileDialog::GetMultiselect( void ) const
{
  return multiselect_;
}

void
TtFileDialog::SetMultiselect( bool flag )
{
  multiselect_ = flag;
}

bool
TtFileDialog::GetRestoreDirectory( void ) const
{
  return restore_directory_;
}

void
TtFileDialog::SetRestoreDirectory( bool flag )
{
  restore_directory_ = flag;
}


bool
TtFileDialog::GetShowHelp( void ) const
{
  return show_help_;
}

void
TtFileDialog::SetShowHelp( bool flag )
{
  show_help_ = flag;
}

const std::string&
TtFileDialog::GetTitle( void ) const
{
  return title_;
}

void
TtFileDialog::SetTitle( const std::string& title )
{
  title_ = title;
}

bool
TtFileDialog::GetShowPlaceBar( void ) const
{
  return show_place_bar_;
}

void
TtFileDialog::SetShowPlaceBar( bool flag )
{
  show_place_bar_ = flag;
}

const std::vector<std::string>&
TtFileDialog::GetFileNames( void ) const
{
  return file_names_;
}

// -- TtFileDialog::Filter -----------------------------------------------
TtFileDialog::Filter::Filter( void ) :
description_(),
extensions_()
{
}

TtFileDialog::Filter::Filter( const std::string& description, const std::string& extension ) :
description_( description ),
extensions_()
{
  extensions_.push_back( extension );
}

const std::string&
TtFileDialog::Filter::GetDescription( void ) const
{
  return description_;
}

void
TtFileDialog::Filter::SetDescription( const std::string& description )
{
  description_ = description;
}

std::vector<std::string>&
TtFileDialog::Filter::GetExtensions( void )
{
  return extensions_;
}


std::vector<TtFileDialog::Filter>&
TtFileDialog::GetFilters( void )
{
  return filters_;
}

void
TtFileDialog::Reset( void )
{
  check_file_exists_ = false;
  check_path_exists_ = true;
  default_extension_ = "";
  dereference_links_ = true;
  file_name_         = "";
  filter_index_      = 1;
  initial_directory_ = "";
  multiselect_       = false;
  restore_directory_ = false;
  show_help_         = false;
  title_             = "";
  file_names_.clear();
  filters_.clear();
}

bool
TtFileDialog::ShowDialog( void )
{
  TtStatic dummy;
  return this->ShowDialog( dummy );
}


TtString::UniqueString&
TtFileDialog::GetBuffer( void )
{
  return buffer_;
}

std::string
TtFileDialog::MakeFilterString( void )
{
  std::string str;
  for ( auto& filter : filters_ ) {
    str.append( filter.GetDescription() );
    str.append( 1, '\0' );
    for ( auto& ext : filter.GetExtensions() ) {
      str.append( ext );
      str.append( ";" );
    }
    str.erase( str.size() - 1 );
    str.append( 1, '\0' );
  }
  str.append( 1, '\0' );
  return str;
}

void
TtFileDialog::MakeStructure( OPENFILENAME& ofn )
{
  memset( &ofn, 0, sizeof( OPENFILENAME ) );

  ofn.lStructSize = sizeof( OPENFILENAME );

  file_name_.copy( buffer_.GetPointer(), buffer_.GetCapacity() );
  buffer_.GetPointer()[file_name_.size() < buffer_.GetCapacity() ? file_name_.size() : buffer_.GetCapacity()] = '\0';
  ofn.lpstrFile = buffer_.GetPointer();
  ofn.nMaxFile = buffer_.GetCapacity();

  if ( NOT( initial_directory_.empty() ) ) {
    ofn.lpstrInitialDir = initial_directory_.c_str();
  }

  if ( NOT( title_.empty() ) ) {
    ofn.lpstrTitle = title_.c_str();
  }

  if ( NOT( default_extension_.empty() ) ) {
    ofn.lpstrDefExt = default_extension_.c_str();
  }

  ofn.Flags |= check_file_exists_ ? OFN_FILEMUSTEXIST : 0;
  ofn.Flags |= check_path_exists_ ? OFN_PATHMUSTEXIST : 0;
  ofn.Flags |= dereference_links_ ? 0 : OFN_NODEREFERENCELINKS;
  ofn.Flags |= restore_directory_ ? OFN_NOCHANGEDIR : 0;
  ofn.Flags |= show_help_         ? OFN_SHOWHELP : 0;
  ofn.Flags |= multiselect_       ? OFN_ALLOWMULTISELECT : 0;
  ofn.Flags |= OFN_LONGNAMES;
  ofn.Flags |= OFN_EXPLORER;

  ofn.FlagsEx |= this->GetShowPlaceBar() ? 0 : OFN_EX_NOPLACESBAR;
}

void
TtFileDialog::SetFileNamesFromBuffer( WORD offset )
{
  file_name_.clear();

  if ( strlen( buffer_.GetPointer() ) > offset ) {
    // 単一選択
    file_name_ = buffer_.GetPointer();
    file_names_.push_back( file_name_ );
    return;
  }

  // 複数選択
  auto tmp = TtString::ToVectorFromRangedString( buffer_.GetPointer(), buffer_.GetCapacity() );
  if ( NOT( tmp.empty() ) ) {
    std::string base = tmp.front();
    tmp.erase( tmp.begin() );

    if ( tmp.empty() ) {
      file_names_.push_back( base );
    }
    else {
      for ( auto& one : tmp ) {
        file_names_.push_back( base + "\\" + one );
      }
    }
  }
  file_name_ = file_names_.empty() ? "" : file_names_.front();
}


// -- TtOpenFileDialog ---------------------------------------------------
TtOpenFileDialog::TtOpenFileDialog( unsigned int buffer_size ) :
TtFileDialog( buffer_size ),
read_only_checked_( false ),
show_read_only_( false )
{
  this->SetCheckFileExists( true );
}


bool
TtOpenFileDialog::GetReadOnlyChecked( void ) const
{
  return read_only_checked_;
}

void
TtOpenFileDialog::SetReadOnlyChecked( bool flag )
{
  read_only_checked_ = flag;
}

bool
TtOpenFileDialog::GetShowReadOnly( void ) const
{
  return show_read_only_;
}

void
TtOpenFileDialog::SetShowReadOnly( bool flag )
{
  show_read_only_ = flag;
}

void
TtOpenFileDialog::Reset( void )
{
  this->TtFileDialog::Reset();
  read_only_checked_ = false;
  show_read_only_    = false;
  this->SetCheckFileExists( true );
}

bool
TtOpenFileDialog::ShowDialog( TtWindow& parent )
{
  OPENFILENAME ofn;
  this->MakeStructure( ofn );

  ofn.hwndOwner = parent.GetHandle();

  // '\0' も含めてコピーする必要が有るため
  std::string filter_str = this->MakeFilterString();
  TtString::UniqueString filter_buf( filter_str.size() + 1 );
  filter_str.copy( filter_buf.GetPointer(), filter_str.size() );
  filter_buf.GetPointer()[filter_buf.GetCapacity() - 1] = '\0';

  if ( NOT( this->GetFilters().empty() ) ) {
    ofn.lpstrFilter  = filter_buf.GetPointer();
    ofn.nFilterIndex = this->GetFilterIndex();
  }

  ofn.Flags |= read_only_checked_ ? OFN_READONLY : 0;
  ofn.Flags |= show_read_only_    ? 0 : OFN_HIDEREADONLY;

  bool ret = ::GetOpenFileName( &ofn ) ? true : false;

  this->SetFileNamesFromBuffer( ofn.nFileOffset );
  return ret;
}

// -- TtSaveFileDialog ---------------------------------------------------
TtSaveFileDialog::TtSaveFileDialog( void ) :
TtFileDialog( MAX_PATH ),
create_prompt_( false ),
overwite_prompt_( true )
{
}


bool
TtSaveFileDialog::GetCreatePrompt( void ) const
{
  return create_prompt_;
}

void
TtSaveFileDialog::SetCreatePrompt( bool flag )
{
  create_prompt_ = flag;
}

bool
TtSaveFileDialog::GetOverwritePrompt( void ) const
{
  return overwite_prompt_;
}

void
TtSaveFileDialog::SetOverwritePrompt( bool flag )
{
  overwite_prompt_ = flag;
}

void
TtSaveFileDialog::Reset( void )
{
  this->TtFileDialog::Reset();
  create_prompt_   = false;
  overwite_prompt_ = true;
}

bool
TtSaveFileDialog::ShowDialog( TtWindow& parent )
{
  OPENFILENAME ofn;
  this->MakeStructure( ofn );

  ofn.hwndOwner = parent.GetHandle();

  // '\0' も含めてコピーする必要が有るため
  std::string filter_str = this->MakeFilterString();
  TtString::UniqueString filter_buf( filter_str.size() + 1 );
  filter_str.copy( filter_buf.GetPointer(), filter_str.size() );
  filter_buf.GetPointer()[filter_buf.GetCapacity() - 1] = '\0';

  if ( NOT( this->GetFilters().empty() ) ) {
    ofn.lpstrFilter  = filter_buf.GetPointer();
    ofn.nFilterIndex = this->GetFilterIndex();
  }

  ofn.Flags |= create_prompt_   ? OFN_CREATEPROMPT : 0;
  ofn.Flags |= overwite_prompt_ ? OFN_OVERWRITEPROMPT : 0;

  bool ret = ::GetSaveFileName( &ofn ) ? true : false;

  this->SetFileNamesFromBuffer( ofn.nFileOffset );
  return ret;
}
