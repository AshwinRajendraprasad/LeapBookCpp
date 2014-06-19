#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"

#include "Leap.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class InteractionBoxApp : public AppNative {
public:

  void setup()
  {
    setWindowPos( 50, 50 );
    setWindowSize( WindowWidth, WindowHeight );
    gl::enableAlphaBlending();
  }

  void draw()
  {
    gl::clear( Color( 1, 1, 1 ) );

    gl::pushMatrices();

    // �l�����w���擾����
    Leap::Finger finger = mLeap.frame()
                               .fingers()
                               .fingerType( Leap::Finger::Type::TYPE_INDEX )[0];
    if ( !finger.isValid() ) {
      return;
    }

    // InteractionBox�̍��W�ɕϊ�����
    Leap::InteractionBox iBox = mLeap.frame().interactionBox();
    Leap::Vector normalizedPosition =
                    iBox.normalizePoint( finger.stabilizedTipPosition() );

    // �E�B���h�E�̍��W�ɕϊ�����
    float x = normalizedPosition.x * WindowWidth;
    float y = WindowHeight - (normalizedPosition.y * WindowHeight);

    gl::color( 1, 0, 0, 1 );
    gl::drawSolidCircle( Vec2f( x, y ), 20 );

    // �w�̍��W��\������
    stringstream ss;
    ss << normalizedPosition.x << ", " << normalizedPosition.y;

    auto tbox = TextBox()
      .font( Font( "���S�V�b�N��", 20 ) )
      .text ( ss.str() );
    auto texture = gl::Texture( tbox.render() );

    auto textX = (normalizedPosition.x < 0.5) ?
                  x : x - texture.getBounds().getWidth();
    auto textY = (normalizedPosition.y > 0.5) ?
                  y : y - texture.getBounds().getHeight();

    gl::color( 0, 0, 0, 1 );
    gl::translate( textX, textY );
    gl::draw( texture );

    gl::popMatrices();
  }

  static const int WindowWidth = 640;
  static const int WindowHeight = 480;

  Leap::Controller mLeap;
};

CINDER_APP_NATIVE( InteractionBoxApp, RendererGl )
