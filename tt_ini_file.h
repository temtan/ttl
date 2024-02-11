// tt_ini_file.h

#pragma once

#include <string>
#include <vector>

#include "tt_path.h"
#include "tt_string.h"
#include "tt_enum.h"


class TtIniSection;

// -- TtIniFile ----------------------------------------------------------
class TtIniFile {
public:
  static const int DEFAULT_BUFFER_SIZE = 65536;

  // デフォルトは exe と同じファイル名の .ini のファイル
  explicit TtIniFile(
    const std::string& filename    = TtPath::GetExecutingFilePathCustomExtension( "ini" ),
    unsigned int       buffer_size = DEFAULT_BUFFER_SIZE );

  ~TtIniFile();

  const std::string& GetFileName( void ) const;
  void SetFileName( const std::string& filename );

  TtIniSection GetSection( const std::string& name );
  TtIniSection operator []( const std::string& name );

  std::vector<TtIniSection> GetSections( void );

  void Flush( void );

private:
  std::string            filename_;
  TtString::SharedString buffer_;
};


// -- TtIniSection -------------------------------------------------------
class TtIniSection {
  friend class TtIniFile;

private:
  explicit TtIniSection( TtIniFile& ini_file, const std::string& name, TtString::SharedString& buffer );

public:
  TtIniFile& GetIniFile( void ) const;

  const std::string& GetName( void ) const;

  std::vector<std::string> GetKeys( void ) const;

  bool GetBoolean( const std::string& key, bool default_value = false );
  void SetBoolean( const std::string& key, bool value );

  unsigned int GetInteger( const std::string& key, unsigned int default_value = 0 );
  void         SetInteger( const std::string& key, unsigned int value );

  double GetDouble( const std::string& key, double default_value = 0.0 );
  void   SetDouble( const std::string& key, double value );

  std::string GetString( const std::string& key, const std::string& default_value = "" );
  void        SetString( const std::string& key, const std::string& value );

  std::wstring GetStringUTF16( const std::string& key, const std::wstring& default_value = L"" );
  void         SetStringUTF16( const std::string& key, const std::wstring& value );

  template <class TYPE>
  TYPE GetEnum( const std::string& key, TYPE default_value ) {
    std::string tmp = this->GetString( key );
    return tmp.empty() ? default_value : TtEnum<TYPE>::Parse( tmp );
  }

  template <class TYPE>
  void SetEnum( const std::string& key, TYPE value ) {
    this->SetString( key, TtEnum<TYPE>( value ).ToString() );
  }

  void RemoveKey( const std::string& key );

  void Flush( void );
  void Clear( void );

private:
  TtIniFile&              ini_file_;
  std::string             name_;
  TtString::SharedString& buffer_;
};
