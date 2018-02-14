// tt_file_dialog.h

#pragma once

#include <string>
#include <vector>

#include "tt_window.h"
#include "tt_string.h"

#pragma warning(push)
#pragma warning(disable : 4265)


// -- TtFileDialog -------------------------------------------------------
class TtFileDialog {
public:
  explicit TtFileDialog( unsigned int buffer_size );

  bool GetCheckFileExists( void ) const;
  void SetCheckFileExists( bool flag );

  bool GetCheckPathExists( void ) const;
  void SetCheckPathExists( bool flag );

  const std::string& GetDefaultExtension( void ) const;
  void SetDefaultExtension( const std::string& extension );

  bool GetDereferenceLinks( void ) const;
  void SetDereferenceLinks( bool flag );

  const std::string& GetFileName( void ) const;
  void SetFileName( const std::string& file_name );

  int GetFilterIndex( void ) const;
  void SetFilterIndex( int index );

  const std::string& GetInitialDirectory( void ) const;
  void SetInitialDirectory( const std::string& directory );

  bool GetMultiselect( void ) const;
  void SetMultiselect( bool flag );

  bool GetRestoreDirectory( void ) const;
  void SetRestoreDirectory( bool flag );

  bool GetShowHelp( void ) const;
  void SetShowHelp( bool flag );

  const std::string& GetTitle( void ) const;
  void SetTitle( const std::string& title );

  bool GetShowPlaceBar( void ) const;
  void SetShowPlaceBar( bool flag );

  const std::vector<std::string>& GetFileNames( void ) const;

  // -- Filter -----
  class Filter {
  public:
    explicit Filter( void );
    Filter( const std::string& description, const std::string& extension );

    const std::string& GetDescription( void ) const;
    void SetDescription( const std::string& description );

    std::vector<std::string>& GetExtensions( void );

  private:
    std::string description_;
    std::vector<std::string> extensions_;
  };

  std::vector<Filter>& GetFilters( void );

  virtual void Reset( void );
  bool ShowDialog( void );
  virtual bool ShowDialog( TtWindow& parent ) = 0;

protected:
  TtString::UniqueString& GetBuffer( void );
  std::string MakeFilterString( void );
  void MakeStructure( OPENFILENAME& ofn );
  void SetFileNamesFromBuffer( WORD offset );

private:
  bool                     check_file_exists_;
  bool                     check_path_exists_;
  std::string              default_extension_;
  bool                     dereference_links_;
  std::string              file_name_;
  int                      filter_index_;
  std::string              initial_directory_;
  bool                     multiselect_;
  bool                     restore_directory_;
  bool                     show_help_;
  bool                     show_place_bar_;
  std::string              title_;
  std::vector<std::string> file_names_;
  std::vector<Filter>      filters_;
  TtString::UniqueString   buffer_;
};

// -- TtOpenFileDialog ---------------------------------------------------
class TtOpenFileDialog : public TtFileDialog {
public:
  explicit TtOpenFileDialog( unsigned int buffer_size = MAX_PATH * 128 );

  bool GetReadOnlyChecked( void ) const;
  void SetReadOnlyChecked( bool flag );

  bool GetShowReadOnly( void ) const;
  void SetShowReadOnly( bool flag );

  virtual void Reset( void ) override;
  virtual bool ShowDialog( TtWindow& parent ) override;

private:
  bool read_only_checked_;
  bool show_read_only_;
};

// -- TtSaveFileDialog ---------------------------------------------------
class TtSaveFileDialog : public TtFileDialog {
public:
  explicit TtSaveFileDialog( void );

  bool GetCreatePrompt( void ) const;
  void SetCreatePrompt( bool flag );

  bool GetOverwritePrompt( void ) const;
  void SetOverwritePrompt( bool flag );

  virtual void Reset( void ) override;
  virtual bool ShowDialog( TtWindow& parent ) override;

private:
  bool create_prompt_;
  bool overwite_prompt_;
};

#pragma warning(pop)
