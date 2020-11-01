// tt_path.h

#pragma once

#include <string>
#include <optional>


namespace TtPath {
  // パス文字列の判定・検証
  bool IsDirectory( const std::string& path );
  bool IsFileSpec(  const std::string& path );
  bool IsRelative(  const std::string& path );
  bool IsRoot(      const std::string& path );
  // ワイルドカードを使ったマッチテスト
  bool MatchSpec(   const std::string& path, const std::string& match );

  bool FileExists(  const std::string& path );

  bool AreTheseSamePath( const std::string& x, const std::string& y );

  // 検索など
  std::string FindFileName( const std::string& path );
  std::string FindExtension( const std::string& path );

  // 編集など
  std::string EditPath( const std::string& path, bool f_drive, bool f_dir, bool f_fname, bool f_ext );
  std::string DirName( const std::string& path );
  std::string BaseName( const std::string& path, const std::string& suffix = "" );
  std::string CompactPath( const std::string& src, unsigned int max );
  std::string QuoteIfHasSpaces( const std::string& path );
  std::string RemoveExtension( const std::string& path );
  std::string ChangeExtension( const std::string& path, const std::string& ext );
  std::optional<std::string> GetRelativePathTo( const std::string& from, bool from_is_directory, const std::string to, bool to_is_directory );
  std::optional<std::string> CanonicalizeFrom( const std::string& path );
  std::string Canonicalize( const std::string& path );

  // 取得など
  std::string GetExecutingFilePath( void );
  std::string GetExecutingFilePathWithoutExtension( void );
  std::string GetExecutingFilePathCustomExtension( const std::string& extension );
  std::string GetExecutingDirectoryPath( void );

  std::string ExpandPath( const std::string& path );
}
