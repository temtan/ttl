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

  // �I�u�W�F�N�g�̗p��
  TtForm form;

  // �E�B���h�E�̍쐬
  form.Create();

  // �E�B���h�E�T�C�Y�ƈʒu�𒲐�
  form.SetPositionSize( 100, 100, 200, 200 );

  // �^�C�g���o�[�̐ݒ�
  form.SetText( "�^�C�g���o�[�̐ݒ�" );

  // �f�t�H���g�ł͔�\���Ȃ̂ŕ\������
  form.Show();

  // ���b�Z�[�W�҂���
  return TtForm::LoopDispatchMessage();
}
