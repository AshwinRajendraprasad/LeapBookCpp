#include "Leap.h"

// �C�x���g���󂯎�邽�߂̃N���X
class SampleListener : public Leap::Listener
{
public:

    // �t���[���̍X�V�C�x���g
    void onFrame(const Leap::Controller& controller ) 
    {
      Leap::Frame frame = controller.frame();
      std::cout <<  frame.id() << std::endl;
    }
};

void main()
{
  // ���X�i�[��o�^����
  // ���X�i�[�Ƃ̂����͕ʃX���b�h�ɂčs����
  SampleListener listener;
  Leap::Controller leap;
  leap.addListener( listener );

  std::cout << "�I������ɂ͉����L�[�������Ă�������" << std::endl;
  std::cin.get();

  leap.removeListener( listener );
}
