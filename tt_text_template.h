// tt_text_template.h

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

#include "tt_exception.h"


namespace TtTextTemplate {
  class InternalDocument;

  // -- Document ---------------------------------------------------------
  class Document {
  public:
    using Callback = std::function<void ( InternalDocument& )>;

    explicit Document( void );

    void ResetAll( void );
    void ResetRegistered( void );

    void ParseFile( const std::string& path );
    void ParseText( const std::string& template_text );

    bool HasKey( const std::string& key );
    bool HasReplaceKey( const std::string& key );
    bool HasDocumentKey( const std::string& key );

    std::vector<std::string> GetKeys( void );
    std::vector<std::string> GetReplaceKeys( void );
    std::vector<std::string> GetDocumentKeys( void );

  private:
    bool ParseAsReplace( const char*& cp );
    bool ParseAsDocument( const char*& cp );

  public:
    void RegisterAsReplace( const std::string& key, const std::string& replacement );
    void RegisterAsDocument( const std::string& key, Callback callback );
    void RegisterPostProcessing( std::function<void ( std::string& )> processing );

    // -- Registrar ------------------------------------------------------
    class Registrar {
      friend class Document;
    private:
      explicit Registrar( const std::string& key, Document& document );

    public:
      void operator =( const char* replacement );
      void operator =( const std::string& replacement );
      void operator =( Callback callback );
      void operator =( bool flag );

    private:
      const std::string& key_;
      Document& document_;
    };

    Registrar operator []( const std::string& key );

    std::string MakeText( void );

  private:
    std::unordered_map<std::string, std::shared_ptr<std::string>>      replace_table_;
    std::unordered_map<std::string, std::shared_ptr<InternalDocument>> document_table_;
    std::function<void ( std::string& )>                               post_processing_;
    std::vector<std::shared_ptr<std::string>>                          data_;
  };


  // -- InternalDocument -------------------------------------------------
  class InternalDocument : public Document {
    friend class Document;
  private:
    explicit InternalDocument( std::shared_ptr<std::string> result_buffer );

  private:
    std::shared_ptr<std::string> result_buffer_;
  };

  // -- FileDocument -----------------------------------------------------
  class FileDocument : public Document {
  public:
    explicit FileDocument( const std::string& filename );
  };

  template <class TYPE>
  class NameDefinedFileDocument : public FileDocument {
  public:
    explicit NameDefinedFileDocument( void ) : FileDocument( TYPE::FILENAME ) {}
  };

  // -- Exception --------------------------------------------------------
  class Exception : public TtException {};

  class KeyNotFoundException : public Exception {
  public:
    explicit KeyNotFoundException( const std::string& key );

    const std::string& GetKey( void ) const;

    virtual std::string Dump( void ) const override;

  private:
    const std::string key_;
  };
}
