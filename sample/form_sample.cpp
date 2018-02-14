#include "ttl_define.h"
#include "tt_window.h"
#include "tt_form.h"

int WINAPI WinMain( HINSTANCE h_instance,
                    HINSTANCE h_prev_instance,
                    PSTR lp_cmd_line,
                    int n_cmd_show )
{
  NOT_USE( h_instance );
  NOT_USE( h_prev_instance );
  NOT_USE( lp_cmd_line );
  NOT_USE( n_cmd_show );

  // オブジェクトの用意
  TtForm form;

  // ウィンドウの作成
  form.Create();

  // ウィンドウサイズと位置を調整
  form.SetPositionSize( 100, 100, 200, 200 );

  // タイトルバーの設定
  form.SetText( "タイトルバーの設定" );

  // デフォルトでは非表示なので表示する
  form.Show();

  // メッセージ待ち受け
  return TtForm::LoopDispatchMessage();
}
