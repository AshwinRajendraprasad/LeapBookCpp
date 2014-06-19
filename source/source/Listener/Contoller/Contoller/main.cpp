#include "Leap.h"

class SampleListener : public Leap::Listener
{
public:

    void onFrame(const Leap::Controller&) 
    {
      std::cout << __FUNCTION__ << std::endl;
    }

    void onFocusGained(const Leap::Controller&) 
    {
      std::cout << __FUNCTION__ << std::endl;
    }

    void onFocusLost(const Leap::Controller&) 
    {
      std::cout << __FUNCTION__ << std::endl;
    }
};

void main()
{
  // ���X�i�[��o�^����
  // ���X�i�[�Ƃ̂����͕ʃX���b�h�ɂčs����
  SampleListener listener;
  Leap::Controller leap;
  leap.addListener( listener );

  leap.setPolicyFlags( Leap::Controller::PolicyFlag::POLICY_BACKGROUND_FRAMES );

  std::cout << "�I������ɂ͉����L�[�������Ă�������" << std::endl;
  std::cin.get();

  leap.removeListener( listener );
}
