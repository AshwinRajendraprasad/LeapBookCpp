#pragma once 

#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include "cinder/MayaCamUI.h"

#include "Leap.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Motion
{
public:

  // �ړ��ʁA��]�ʁA�g�嗦
  Leap::Vector mTranslation;
  Leap::Matrix mRotationMatrix;
  float mScale;

  Motion()
  {
    mRotationMatrix = Leap::Matrix::identity();
    mTranslation = Leap::Vector::zero();
    mScale = 1.0f;
  }
  
  template<typename Object>
  static Motion getCurrentMotion( Object object, Leap::Frame lastFrame )
  {
    Motion motion;

    // �O�̃t���[������̈ړ���
    if ( object.translationProbability( lastFrame ) > 0.4 ) {
      // ��]���l�����Ĉړ�����
      motion.mTranslation = object.translation( lastFrame );
    }

    // �O�̃t���[������̉�]��
    if ( object.rotationProbability( lastFrame ) > 0.4 ) {
      motion.mRotationMatrix = object.rotationMatrix( lastFrame );
    }

    // �O�̃t���[������̊g��k��
    if ( object.scaleProbability( lastFrame ) > 0.4 ) {
      motion.mScale = object.scaleFactor( lastFrame );
    }

    return motion;
  }

  void update( const Motion& motion )
  {
    // �O�̃t���[������̈ړ���
    // ��]���l�����Ĉړ�����
    mTranslation += mRotationMatrix
                    .rigidInverse()
                    .transformDirection( motion.mTranslation );

    // �O�̃t���[������̉�]��
    mRotationMatrix *= motion.mRotationMatrix;
    
    // �O�̃t���[������̊g��k��
    mScale *= motion.mScale;
    if ( mScale < 0.1f ) {
      mScale = 0.1f;
    }
  }

  template<typename Object>
  void update( Object object, Leap::Frame lastFrame )
  {
    update( Motion::getCurrentMotion( object, lastFrame ) );
  }

  void draw( int offsetX )
  {
    gl::pushMatrices();

    // �\������
    glTranslatef( mTranslation.x,
                  mTranslation.y,
                  mTranslation.z );
    glMultMatrixf( mRotationMatrix.toArray4x4() );
    glScalef( mScale, mScale, mScale );
    gl::drawColorCube( Vec3f( offsetX, 0, 0 ),
                        Vec3f( 100, 100, 100 ) );

    gl::popMatrices();
  }
};
