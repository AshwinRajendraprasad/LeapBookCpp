#pragma once

#include "cinder/gl/gl.h"

#include "Leap.h"

using namespace ci;

class Paint
{
public:

  Paint()
    : mIs3D( false )
  {
  }

  void clear()
  {
    // �O�Ղ��N���A����
    mCompleteStrokes.clear();
    mCurrentStrokes.clear();
  }

  void update()
  {
    mCurrentFrame = mLeap.frame();

    mPointablePoints.clear();

    // �|�C���^�u���E�I�u�W�F�N�g�̍��W���擾����
    auto pointables = mCurrentFrame.pointables();
    for ( auto pointable : pointables ) {
      auto point = pointable.tipPosition();
      mPointablePoints.push_back( point );

      // �L���ȃ|�C���^�u���E�I�u�W�F�N�g�ł���΁A�����r���̋O�Ղɒǉ�(ID�ƍ��킹��)
      if ( isValid( pointable ) ) {
        mCurrentStrokes[pointable.id()].push_back( point );
      }
    }

    // �����r���̋O�Ղ𒲂ׂāA����̃t���[���ł̃|�C���^�u���E�I�u�W�F�N�g���Ȃ������ꍇ�A
    // �����̂��I������Ɣ��f���A���������O�ՂƂ���
    for ( auto it = mCurrentStrokes.begin(); it != mCurrentStrokes.end();  ) {
      auto pointable = mCurrentFrame.pointable( it->first );
      if ( isValid( pointable ) ) {
        ++it;
      }
      else {
        mCompleteStrokes.push_back( it->second );
        it = mCurrentStrokes.erase( it );
      }
    }
  }

  void draw()
  {
    gl::clear( Color( 0, 0, 0 ) ); 

    // ���݂̃|�C���^�u���E�I�u�W�F�N�g�̈ʒu��\������
    drawPoints( mPointablePoints, 15, Color( 1, 0, 0 ) );

    // ���������O�Ղ�\������
    for ( auto strokes : mCompleteStrokes ) {
      drawLineStrip( strokes );
    }

    // �����r���̋O�Ղ�\������
    for ( auto strokes : mCurrentStrokes ) {
      drawLineStrip( strokes.second );
    }
  }

  void set3DMode( bool is3d )
  {
    mIs3D = is3d;
  }

  bool get3DMode() const
  {
    return mIs3D;
  }

  static const int Width = 1280;
  static const int Height = 700;

private:

  bool isValid( Leap::Pointable pointable )
  {
    // �L���ȃ|�C���^(�I�u�W�F�N�g���L���ŁA�L�тĂ���)
    return pointable.isValid() &&
           pointable.isExtended();
  }

  // ��������
  void drawLineStrip(const std::vector<Leap::Vector>& strokes)
  {
    gl::lineWidth( 10 );
    setDiffuseColor( Color::white() );
    gl::begin( GL_LINE_STRIP );

    for ( auto position : strokes ) {
      drawVertex( position );
    }

    gl::end();

    // ���Ɛ��̕⊮
    drawPoints( strokes, 5, Color::white() );
  }

  // �_������
  void drawPoints( const std::vector<Leap::Vector>& points, int size, Color color )
  {
    glPointSize( size );
    setDiffuseColor( color );
    gl::begin( GL_POINTS );

    for ( auto point : points ) {
      drawVertex( point );
    }
    gl::end();
  }

  // ���W��ݒ肷��
  void drawVertex( Leap::Vector pointable )
  {
    // 3�����f�[�^�ŏ���
    if ( mIs3D ) {
      // 3����
      gl::vertex( toVec3f( pointable ) );
    }
    // 2�����f�[�^�ŏ���
    else {
      // ���W�ϊ�����(0-1�̒l)
      auto box = mCurrentFrame.interactionBox();
      auto point = box.normalizePoint( pointable );

      // �c(�ʂ�Leap Motion�ɑ΂��Đ���)
      gl::vertex( point.x * Width, Height  - (point.y * Height) );

      // ��(�ʂ�Leap Motion�ɑ΂��Đ���)
      //gl::vertex( point.x * Width, point.z * Height );
    }
  }

  // GL_LIGHT0�̐F��ς���
  void setDiffuseColor( ci::ColorA diffuseColor )
  {
    gl::color( diffuseColor );
    glLightfv( GL_LIGHT0 , GL_DIFFUSE, diffuseColor );
  }

  // Leap SDK��Vector��Cinder��Vec3f�ɕϊ�����
  Vec3f toVec3f( Leap::Vector vec )
  {
    return Vec3f( vec.x, vec.y, vec.z );
  }

private:

  bool mIs3D;

  // Leap Motion
  Leap::Controller mLeap;
  Leap::Frame mCurrentFrame;

  // �`��p�̍��W
  std::vector<std::vector<Leap::Vector>> mCompleteStrokes;
  std::map<int, std::vector<Leap::Vector>> mCurrentStrokes;
  std::vector<Leap::Vector> mPointablePoints;
};

