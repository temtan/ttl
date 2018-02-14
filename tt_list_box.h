// tt_list_box.h

#pragma once

#include "tt_window.h"


// -- TtListBox ----------------------------------------------------------
class TtListBox : public BasedOnTtWindow<WS_TABSTOP | LBS_NOTIFY, WS_EX_CLIENTEDGE, TtWindowClassName::ListBox> {
public:
  int GetItemHeight( void );
  void SetItemHeight( int height );

  int GetCount( void );
  unsigned int GetTextLengthAt( unsigned int index );
  std::string GetTextAt( unsigned int index );
  std::string GetCurrentText( void );

  // ñ¢ëIëÇÃèÍçá LB_ERR
  int GetCurrent( void );
  void SetCurrent( int index );
  void UnsetCurrent( void );

  int Find( unsigned int start, const std::string& str );

  int Push( const std::string& str );
  int InsertAt( unsigned int index, const std::string& str );

  int DeleteAt( unsigned int index );
  void Clear( void );

  int LockDraw( void );
  int UnlockDraw( void );
};

// -- TtListBoxWithStyle ----------------------------------------------------
template <DWORD style>
using TtListBoxWithStyle = TtWindowWithStyle<TtListBox, style>;


// -- TtListBoxMultiSelect --------------------------------------------------
class TtListBoxMultiSelect : public TtListBoxWithStyle<LBS_MULTIPLESEL> {
public:
  int  GetSelectedCount( void );
  bool IsSelect( unsigned int index );
  int  SetSelect( unsigned int index );
  int  UnsetSelect( unsigned int index );
};

// -- TtListBoxMultiSelectWithStyle --------------------------------------
template <DWORD style>
using TtListBoxMultiSelectWithStyle = TtWindowWithStyle<TtListBoxMultiSelect, style>;
