// tt_directory.h

#pragma once

#include <string>
#include <vector>
#include <functional>


// -- TtDirectory --------------------------------------------------------
class TtDirectory {
public:
  explicit TtDirectory( const std::string& path );

  const std::string& GetPath( void ) const;

  using Filter = std::function<bool ( const std::string& path )>;
  static Filter All;
  static Filter FileOnly;
  static Filter DirectoryOnly;

  std::vector<std::string> GetEntries( const std::string& search, const Filter& filter = All ) const;
  std::vector<std::string> GetEntriesRecursive( const std::string& search, const Filter& filter = All ) const;

  template <class TYPE>
  std::vector<std::string> GetEntries( const std::string& search, TYPE obj ) const {
    return this->GetEntries( search, Filter( obj ) );
  }
  template <class TYPE>
  std::vector<std::string> GetEntriesRecursive( const std::string& search, TYPE obj ) const {
    return this->GetEntriesRecursive( search, Filter( obj ) );
  }

private:
  std::string self_path_;
};
