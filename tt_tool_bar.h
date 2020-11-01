// tt_tool_bar.h

#pragma once

#include <unordered_map>

#include "tt_window.h"
#include "tt_window_utility.h"
#include "tt_panel.h"

#include "tt_bmp_image.h"


// -- TtToolBar ----------------------------------------------------------
class TtToolBar : public BasedOnTtWindow<WS_TABSTOP, WS_EX_CLIENTEDGE, TtWindowClassName::ToolBar> {
public:
  // -- Button -----------------------------------------------------------
  class Button {
  public:
    struct State {
      static const BYTE Checked       = TBSTATE_CHECKED;
      static const BYTE Pressed       = TBSTATE_PRESSED;
      static const BYTE Enabled       = TBSTATE_ENABLED;
      static const BYTE Hidden        = TBSTATE_HIDDEN;
      static const BYTE Indeterminate = TBSTATE_INDETERMINATE;
      static const BYTE Wrap          = TBSTATE_WRAP;
      static const BYTE Ellipses      = TBSTATE_ELLIPSES;
      static const BYTE Marked        = TBSTATE_MARKED;
    };

    struct Style {
      static const int Standard      = BTNS_BUTTON;
      static const int Separator     = BTNS_SEP;
      static const int Check         = BTNS_CHECK;
      static const int Group         = BTNS_GROUP;
      static const int CheckGroup    = BTNS_CHECKGROUP;
      static const int DropDown      = BTNS_DROPDOWN;
      static const int AutoSize      = BTNS_AUTOSIZE;
      static const int NoPrefix      = BTNS_NOPREFIX;
      static const int ShowText      = BTNS_SHOWTEXT;
      static const int WholeDropDown = BTNS_WHOLEDROPDOWN;
    };

    explicit Button( TtToolBar* tool_bar, int command_id );

    void SetState( BYTE state );
    // SetBmpImageIndex はセパレータに対してだと幅変更になる。
    void SetBmpImageIndex( int index );
    void SetCommandID( int command_id );

    int GetCommandID( void );
    int GetIndex( void );
    int GetBmpImageIndex( void );
    RECT GetRectangle( void );

    bool IsChecked( void );
    bool IsEnabled( void );
    bool IsHidden( void );
    bool IsIndeterminate( void );
    bool IsPressed( void );

    void SetCheck( bool flag );
    void Check( void );
    void Uncheck( void );
    void SetEnable( bool flag );
    void Enable( void );
    void Disable( void );
    void Press( void );
    void Pull( void );

    void Delete( void );

  private:
    TtToolBar* tool_bar_;
    int        command_id_;
  };
  // -- End Button -------------------------------------------------------

public:
  struct Style {
    static const int Top           = CCS_TOP;
    static const int Bottom        = CCS_BOTTOM;
    static const int Vertical      = CCS_VERT;
    static const int Left          = CCS_LEFT;
    static const int Right         = CCS_RIGHT;
    static const int NoMoveX       = CCS_NOMOVEX;
    static const int NoMoveY       = CCS_NOMOVEY;
    static const int NoResize      = CCS_NORESIZE;
    static const int NoParentAlign = CCS_NOPARENTALIGN;
    static const int Adjustable    = CCS_ADJUSTABLE;
    static const int NoDivider     = CCS_NODIVIDER;
    static const int WithToolTips  = TBSTYLE_TOOLTIPS;
    static const int Wrapable      = TBSTYLE_WRAPABLE;
    static const int CanAltDrag    = TBSTYLE_ALTDRAG;
    static const int Flat          = TBSTYLE_FLAT;
    static const int List          = TBSTYLE_LIST;
    static const int CustomErase   = TBSTYLE_CUSTOMERASE;
    static const int RegisterDrop  = TBSTYLE_REGISTERDROP;
    static const int TransParent   = TBSTYLE_TRANSPARENT;
  };

  struct ExtendedStyle {
    static const int DrawDropDwonArrows = TBSTYLE_EX_DRAWDDARROWS;
    static const int MixedButtons       = TBSTYLE_EX_MIXEDBUTTONS;
  };

  class StandardBmpImageAppointer {
  public:
    explicit StandardBmpImageAppointer( int base ) : base_( base ) {}
    int Cut( void )          { return base_ + STD_CUT; }
    int Copy( void )         { return base_ + STD_COPY; }
    int Paste( void )        { return base_ + STD_PASTE; }
    int Undo( void )         { return base_ + STD_UNDO; }
    int Redo( void )         { return base_ + STD_REDOW; }
    int Delete( void )       { return base_ + STD_DELETE; }
    int FileNew( void )      { return base_ + STD_FILENEW; }
    int FileOpen( void )     { return base_ + STD_FILEOPEN; }
    int FileSave( void )     { return base_ + STD_FILESAVE; }
    int PrintPreview( void ) { return base_ + STD_PRINTPRE; }
    int Properties( void )   { return base_ + STD_PROPERTIES; }
    int Help( void )         { return base_ + STD_HELP; }
    int Find( void )         { return base_ + STD_FIND; }
    int Replace( void )      { return base_ + STD_REPLACE; }
    int Print( void )        { return base_ + STD_PRINT; }
  private:
    int base_;
  };

  class ViewBmpImageAppointer {
  public:
    explicit ViewBmpImageAppointer( int base ) : base_( base ) {}
    int LargeIcons( void )    { return base_ + VIEW_LARGEICONS; }
    int SmallIcons( void )    { return base_ + VIEW_SMALLICONS; }
    int List( void )          { return base_ + VIEW_LIST; }
    int Details( void )       { return base_ + VIEW_DETAILS; }
    int SortName( void )      { return base_ + VIEW_SORTNAME; }
    int SortSize( void )      { return base_ + VIEW_SORTSIZE; }
    int SortData( void )      { return base_ + VIEW_SORTDATE; }
    int SortType( void )      { return base_ + VIEW_SORTTYPE; }
    int ParentFolder( void )  { return base_ + VIEW_PARENTFOLDER; }
    int NetConnect( void )    { return base_ + VIEW_NETCONNECT; }
    int NetDisconnect( void ) { return base_ + VIEW_NETDISCONNECT; }
    int NewFolder( void )     { return base_ + VIEW_NEWFOLDER; }
  private:
    int base_;
  };

  virtual bool CreatedInternal( void ) override;

  void SetExtendedStyle( int style );

  void SetButtonSize( int x, int y );
  void SetImageSize( int x, int y );

  int AddStandardSmallBmpImage( void );
  int AddStandardLargeBmpImage( void );
  int AddViewSmallBmpImage( void );
  int AddViewLargeBmpImage( void );

  // SetImageList とは併用しないこと
  int AddBmpImage( int image_count, TtBmpImage& bmp_image );
  int AddBmpImage( int image_count, HINSTANCE h_instance, UINT resource_id );

  // AddBmpImage とは併用しないこと
  void SetImageList( TtImageList& image_list );

  int AddString( const std::string& str );
  int AddStrings( const std::vector<std::string> v );

  int GetButtonCount( void );

  RECT SetRowsCount( int rows, bool force = false );
  int GetRowsCount( void );

  void ResizeButtonAuto( void );

  Button AddButton( int command_id,
                    int bmp_index,
                    int string_index = 0,
                    int style        = Button::Style::Standard );
  Button AddButtonWithString( int command_id,
                              int bmp_index,
                              const std::string& string,
                              int style = Button::Style::Standard );
  void AddSeparator( int command_id = 0 );

  TtPanel::NotifyHandler MakeNotifyHandlerForToolTipByStringTable( void );

  Button GetButton( int command_id );

private:
  std::unordered_map<int, std::string> string_table_;
};
