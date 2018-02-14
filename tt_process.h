// tt_process.h

#pragma once

#include <string>
#include "tt_windows_h_include.h"

#include "tt_enum.h"
#include "tt_window.h"


// -- TtPipe -------------------------------------------------------------
class TtPipe {
public:
  class Handle {
    friend TtPipe;
  public:
    static Handle GetStandardInputHandle( void );
    static Handle GetStandardOutputHandle( void );
    static Handle GetStandardErrorHandle( void );

    explicit Handle( HANDLE handle );

    HANDLE GetWindowsHandle( void );
    void Close( void );

    void SetInherit( bool inherit );

    Handle Duplicate( bool inherit );

  private:
    HANDLE handle_;
  };

  explicit TtPipe( bool inherit_handle = true, unsigned int buffer_size = 0 );

  Handle& GetReadPipeHandle( void );
  Handle& GetWritePipeHandle( void );

private:
  Handle read_pipe_;
  Handle write_pipe_;
};


// -- TtProcess ----------------------------------------------------------
class TtProcess {
public:
  // -- Priority ---------------------------------------------------------
  enum class Priority {
    NORMAL       = NORMAL_PRIORITY_CLASS,
    IDLE         = IDLE_PRIORITY_CLASS,
    HIGH         = HIGH_PRIORITY_CLASS,
    REALTIME     = REALTIME_PRIORITY_CLASS,
    BELOW_NORMAL = BELOW_NORMAL_PRIORITY_CLASS,
    ABOVE_NORMAL = ABOVE_NORMAL_PRIORITY_CLASS,
  };

  // -- CreateInfo --------------------------------------------------------
  class CreateInfo {
    friend class TtProcess;
  public:
    explicit CreateInfo( void );
    explicit CreateInfo( const std::string& filename );

    void SetFileName( const std::string& filename );
    const std::string& GetFileName( void ) const;

    void SetArguments( const std::string& arg );
    const std::string& GetArguments( void ) const;

    void SetCurrentDirectory( const std::string& path );
    const std::string& GetCurrentDirectory( void ) const;

    void SetInheritHandles( bool flag );
    bool GetInheritHandles( void ) const;

    void SetStandardInput( TtPipe::Handle handle );
    TtPipe::Handle GetStandardInput( void ) const;

    void SetStandardOutput( TtPipe::Handle handle );
    TtPipe::Handle GetStandardOutput( void ) const;

    void SetStandardError( TtPipe::Handle handle );
    TtPipe::Handle GetStandardError( void ) const;

    void SetCreateNewProcessGroup( bool flag );
    bool GetCreateNewProcessGroup( void ) const;

    void SetPriority( TtEnum<Priority> priority );
    TtEnum<Priority> GetPriority( void ) const;

    void SetShowState( TtEnum<TtWindow::ShowState> state );
    TtEnum<TtWindow::ShowState> GetShowState( void ) const;

    void SetUseSearchPath( bool flag );
    bool GetUseSearchPath( void ) const;

    void SetCreateSuspended( bool flag );
    bool GetCreateSuspended( void ) const;

  private:
    std::string                 filename_;
    std::string                 arguments_;
    std::string                 current_directory_;
    bool                        inherit_handles_;
    TtPipe::Handle              standard_input_;
    TtPipe::Handle              standard_output_;
    TtPipe::Handle              standard_error_;
    bool                        create_new_process_group_;
    TtEnum<Priority>            priority_;
    TtEnum<TtWindow::ShowState> show_state_;
    bool                        use_search_path_;
    bool                        create_suspended_;
  };

  // -- TtProcess --------------------------------------------------------
  static DWORD GetCurrentProcessID( void );

  TtProcess( void );
  ~TtProcess();


  void Create( CreateInfo& info );
  void Start( void );
  void Wait( void );
  void Terminate( UINT exit_code );

  bool IsCreated( void ) const;
  bool HasExited( void ) const;

  DWORD GetExitCode( void ) const;

  HANDLE GetProcessHandle( void ) const;
  DWORD  GetProcessID( void ) const;
  HANDLE GetThreadHandle( void ) const;
  DWORD  GetThreadID( void ) const;

private:
  HANDLE process_handle_;
  DWORD  process_id_;
  HANDLE thread_handle_;
  DWORD  thread_id_;
};
