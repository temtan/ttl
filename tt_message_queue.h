// tt_message_queue.h

#pragma once

#include <queue>

#include <limits.h>
#include "tt_windows_h_include.h"

#include "ttl_define.h"
#include "tt_exception.h"
#include "tt_critical_section.h"

#pragma comment(lib, "kernel32.lib")


// -- TtMessageQueue -----------------------------------------------------
template <class TYPE>
class TtMessageQueue {
public:
  explicit TtMessageQueue() :
  queue_(),
  lock_(),
  get_semaphoer_() {
    get_semaphoer_ = ::CreateSemaphore( nullptr, queue_.size(), INT_MAX, nullptr );
    if ( get_semaphoer_ == nullptr ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::CreateSemaphore ) );
    }
  }

  ~TtMessageQueue() {
    ::CloseHandle( get_semaphoer_ );
  }

  void PostMessage( TYPE data ) {
    lock_.EnterExecute( [this, &data] ( void ) {
      queue_.push( data );
      if ( ::ReleaseSemaphore( get_semaphoer_, 1, nullptr ) == 0 ) {
        throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::ReleaseSemaphore ) );
      }
    } );
  }

  bool HasMessage( void ) const {
    return NOT( queue_.empty() );
  }

  TYPE GetMessage( void ) {
    if ( ::WaitForSingleObject( get_semaphoer_, INFINITE ) != WAIT_OBJECT_0 ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::WaitForSingleObject ) );
    }
    lock_.Enter();
    TYPE data = queue_.front();
    queue_.pop();
    lock_.Leave();
    return data;
  }

  void ClearMessage( void ) {
    lock_.EnterExecute( [this] ( void ) {
      while ( NOT( queue_.empty() ) ) {
        queue_.pop();
      }
    } );
  }

private:
  std::queue<TYPE>  queue_;
  TtCriticalSection lock_;
  HANDLE            get_semaphoer_;
};
