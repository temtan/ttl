// tt_form.cpp

#include "ttl_define.h"
#include "tt_exception.h"
#include "tt_utility.h"

#include "tt_form.h"

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")

// -- TtForm::AcceleratorMap::ShortcutKey --------------------------------
TtForm::AcceleratorMap::ShortcutKey
TtForm::AcceleratorMap::ShortcutKey::GetMdifierFromKeyboard( WORD key )
{
  return ShortcutKey(
    (::GetKeyState( VK_SHIFT )   & 0x80) ? true : false,
    (::GetKeyState( VK_CONTROL ) & 0x80) ? true : false,
    (::GetKeyState( VK_MENU )    & 0x80) ? true : false,
    key );
}

TtForm::AcceleratorMap::ShortcutKey::ShortcutKey( void ) :
ShortcutKey( 0 )
{
}

TtForm::AcceleratorMap::ShortcutKey::ShortcutKey( bool shift, bool control, bool alt, WORD key ) :
ShortcutKey( (shift ? Modifier::Shift : 0) | (control ? Modifier::Control : 0) | (alt ? Modifier::Alt : 0), key )
{
}

TtForm::AcceleratorMap::ShortcutKey::ShortcutKey( WORD key ) :
ShortcutKey( false, false, false, key )
{
}

TtForm::AcceleratorMap::ShortcutKey::ShortcutKey( BYTE modifier, WORD key ) :
modifier_( FVIRTKEY | modifier ),
key_( key )
{
}

bool
TtForm::AcceleratorMap::ShortcutKey::WithShift( void ) const
{
  return (modifier_ & FSHIFT) ? true : false;
}

bool
TtForm::AcceleratorMap::ShortcutKey::WithControl( void ) const
{
  return (modifier_ & FCONTROL) ? true : false;
}

bool
TtForm::AcceleratorMap::ShortcutKey::WithAlt( void ) const
{
  return (modifier_ & FALT) ? true : false;
}

BYTE
TtForm::AcceleratorMap::ShortcutKey::GetModifier( void ) const
{
  return modifier_;
}

WORD
TtForm::AcceleratorMap::ShortcutKey::GetVirtualKey( void ) const
{
  return key_;
}

std::string
TtForm::AcceleratorMap::ShortcutKey::GetText( void ) const
{
  if ( key_ == 0x00 ) {
    return "";
  }
  UINT scan_code = ::MapVirtualKey( key_, 0 );
  if ( scan_code == 0 ) {
    return "";
  }
  std::string tmp;
  tmp.append( this->WithShift()   ? "Shift+" : "" );
  tmp.append( this->WithControl() ? "Ctrl+"  : "" );
  tmp.append( this->WithAlt()     ? "Alt+"   : "" );
  char buf[256];
  if ( ::GetKeyNameText( (scan_code << 16), buf, sizeof( buf ) ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::GetKeyNameText ) );
  }
  tmp.append( buf );
  return tmp;
}


std::string
TtForm::AcceleratorMap::ShortcutKey::Serialize( void ) const
{
  return TtUtility::Serialize( *this );
}

TtForm::AcceleratorMap::ShortcutKey
TtForm::AcceleratorMap::ShortcutKey::Deserialize( const std::string& data )
{
  ShortcutKey key( 0 );
  if ( NOT( TtUtility::Deserialize( data, key ) ) ) {
    return ShortcutKey( 0 );
  }
  return key;
}


bool
TtForm::AcceleratorMap::ShortcutKey::operator < ( const ShortcutKey& other ) const
{
  if ( this->modifier_ != other.modifier_ ) {
    return this->modifier_ < other.modifier_;
  }
  return this->key_ < other.key_;
}


// -- TtForm::AcceleratorMap ---------------------------------------------
TtForm::AcceleratorMap::AcceleratorMap( void ) :
table_()
{
}

void
TtForm::AcceleratorMap::Register( ShortcutKey key, WORD command_id )
{
  auto it = table_.find( key );
  if ( it != table_.end() ) {
    it->second = command_id;
  }
  else {
    table_.insert( {key, command_id} );
  }
}



// -- TtForm::AcceleratorTable -------------------------------------------
TtForm::AcceleratorTable::AcceleratorTable( HACCEL handle, TtForm& form ) :
handle_( handle ),
form_( form )
{
}

bool
TtForm::AcceleratorTable::Translate( MSG* msg )
{
  return (::TranslateAccelerator( form_.handle_, handle_, msg ) ? true : false);
}

void
TtForm::AcceleratorTable::Destroy( void )
{
  if ( handle_ != nullptr ) {
    ::DestroyAcceleratorTable( handle_ );
    handle_ = nullptr;
  }
}


// -- TtForm -------------------------------------------------------------
std::unordered_map<HWND, TtForm*>
TtForm::FORM_TABLE;

TtForm::AcceleratorTable*
TtForm::ACCELERATOR_TABLE( nullptr );


WNDCLASS
TtForm::MakeDefaultWindowClass( HINSTANCE h_instance, const char* class_name )
{
  WNDCLASS window_class;
  window_class.style         = CS_HREDRAW | CS_VREDRAW;
  window_class.lpfnWndProc   = TtWindow::WindowProcedureForTTL;
  window_class.cbClsExtra    = 0;
  window_class.cbWndExtra    = 0;
  window_class.hInstance     = h_instance;
  window_class.hIcon         = ::LoadIcon( nullptr , IDI_APPLICATION );
  window_class.hCursor       = ::LoadCursor( nullptr, IDC_ARROW );
  window_class.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
  window_class.lpszMenuName  = nullptr;
  window_class.lpszClassName = class_name;
  return window_class;
}

void
TtForm::RegisterAccelerator( TtForm& form, AcceleratorMap& map )
{
  TtUtility::UniqueArray<ACCEL> array( map.table_.size() );

  int i = 0;
  for ( auto& entry : map.table_ ) {
    array.GetPointer()[i].fVirt = entry.first.GetModifier();
    array.GetPointer()[i].key   = entry.first.GetVirtualKey();
    array.GetPointer()[i].cmd   = entry.second;
    ++i;
  }

  HACCEL handle = ::CreateAcceleratorTable( array.GetPointer(), static_cast<int>( array.GetCapacity() ) );
  if ( handle == NULL ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::CreateAcceleratorTable ) );
  }
  if ( ACCELERATOR_TABLE ) {
    ACCELERATOR_TABLE->Destroy();
    delete ACCELERATOR_TABLE;
  }
  ACCELERATOR_TABLE = new AcceleratorTable( handle, form );
}

int
TtForm::LoopDispatchMessage( void )
{
  for (;;) {
    MSG msg;
    switch ( ::GetMessage( &msg, nullptr, 0, 0 ) ) {
    case -1:
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::GetMessage ) );

    case 0:
      return static_cast<int>( msg.wParam );

    default:
      TtForm::DispatchOneMessage( msg );
      break;
    }
  }
}


void
TtForm::DispatchOneMessage( MSG& msg )
{
  if ( ACCELERATOR_TABLE && ACCELERATOR_TABLE->Translate( &msg ) ) {
    return;
  }
  auto it = FORM_TABLE.find( msg.hwnd );
  if ( it != FORM_TABLE.end() ) {
    if ( it->second->IsDialogMessage( &msg ) ) {
      return;
    }
  }
  ::TranslateMessage( &msg );
  ::DispatchMessage( &msg );
}

void
TtForm::DoEvent( void )
{
  MSG msg;
  while ( ::PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) ) {
    TtForm::DispatchOneMessage( msg );
  }
}


bool
TtForm::IsDialogMessage( MSG* msg )
{
  if ( handle_ == nullptr ) {
    return false;
  }
  return (::IsDialogMessage( handle_, msg ) ? true : false);
}


DWORD
TtForm::GetStyle( void )
{
  return WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
}

DWORD
TtForm::GetExtendedStyle( void )
{
  return WS_EX_STATICEDGE;
}

bool
TtForm::CreatedInternal( void )
{
  this->TtPanel::CreatedInternal();
  FORM_TABLE.insert( {handle_, this} );
  this->RegisterWMDestroy( [this] () -> WMResult {
    ::PostQuitMessage( exit_code_ );
    return {WMResult::Done};
  } );
  return this->Created();
}


bool
TtForm::Created( void )
{
  return true;
}


TtForm::TtForm( void ) :
exit_code_( 0 )
{
  TtPanel::Initialize();
}

TtForm::~TtForm()
{
  if ( FORM_TABLE.find( handle_ ) != FORM_TABLE.end() ) {
    FORM_TABLE.erase( handle_ );
  }
}


void
TtForm::SetMenu( TtMenuBar& menu )
{
  if ( ::SetMenu( handle_, menu.GetHandle() ) == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::SetMenu ) );
  }
}


TtSubMenu
TtForm::GetSystemMenu( void )
{
  return TtSubMenu( ::GetSystemMenu( handle_, FALSE ), false );
}



void
TtForm::RegisterWMClose( WMCloseHandler handler, bool do_override )
{
  this->RegisterSingleHandler( WM_CLOSE, [handler] ( WPARAM, LPARAM ) {
    return handler();
  }, do_override );
}

void
TtForm::RegisterWMDestroy( WMDestroyHandler handler, bool do_override )
{
  this->RegisterSingleHandler( WM_DESTROY, [handler] ( WPARAM, LPARAM ) {
    return handler();
  }, do_override );
}

void
TtForm::RegisterWMMouseMove( WMMouseMoveHandler handler, bool do_override )
{
  this->RegisterSingleHandler( WM_MOUSEMOVE, [handler] ( WPARAM w_param, LPARAM l_param ) {
    return handler( static_cast<int>( w_param ), TtWindow::GetXOfLParam( l_param ), TtWindow::GetYOfLParam( l_param ) );
  }, do_override );
}

void
TtForm::RegisterWMDropFiles( WMDropFilesHandler handler, bool do_override )
{
  this->RegisterSingleHandler( WM_DROPFILES, [handler] ( WPARAM w_param, LPARAM ) {
    HDROP drop = reinterpret_cast<HDROP>( w_param );
#ifdef _WIN64
    {
      LONG_PTR p = (LONG_PTR)::GlobalAlloc(GMEM_MOVEABLE, 0);
      drop = (HDROP)((0xFFFFFFFF00000000 & p) | (0x00000000FFFFFFFF & ((LONG_PTR)drop)));
      ::GlobalFree( (HGLOBAL)p );
    }
#endif
    return handler( drop );
  }, do_override );
}
