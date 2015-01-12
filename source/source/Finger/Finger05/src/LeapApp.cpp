﻿#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include "cinder/MayaCamUI.h"

#include "Leap.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class LeapApp : public AppNative {
public:

  void setup()
  {
    // ウィンドウの位置とサイズを設定
    setWindowPos( 50, 50 );
    setWindowSize( 1280, 700 );

    // 光源を追加する
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );

    // 表示フォントと領域の設定
    mFont = Font( "YuGothic", 20 );

    // カメラ(視点)の設定
    float y = 250;
    mCam.setEyePoint( Vec3f( 0.0f, y, 500.0f ) );
    mCam.lookAt( Vec3f( 0.0f, y, 0.0f ) );
    mCam.setPerspective( 45.0f, getWindowAspectRatio(), 5.0f, 3000.0f );

    mMayaCam.setCurrentCam(mCam);

    // 描画時に奥行きの考慮を有効にする
    gl::enableDepthRead();

    // Leap Motion関連のセットアップ
    setupLeapObject();
  }

  // マウスダウン
  void mouseDown( MouseEvent event )
  {
    mMayaCam.mouseDown( event.getPos() );
  }

  // マウスのドラッグ
  void mouseDrag( MouseEvent event )
  {
    mMayaCam.mouseDrag( event.getPos(), event.isLeftDown(),
      event.isMiddleDown(), event.isRightDown() );
  }

  // 更新処理
  void update()
  {
    // フレームの更新
    mLastFrame = mCurrentFrame;
    mCurrentFrame = mLeap.frame();

    renderFrameParameter();
  }

  // 描画処理
  void draw()
  {
    gl::clear( Color( 0, 0, 0 ) ); 

    drawLeapObject();
    drawTexture();
  }

  // Leap Motion関連のセットアップ
  void setupLeapObject()
  {
  }

  // フレーム情報の描画
  void renderFrameParameter()
  {
    stringstream ss;

    // 指の関節の長さと太さを表示する
    for ( auto finger : mCurrentFrame.fingers() ) {
      if ( finger.isExtended() ) {
        const Leap::Bone::Type boneType[] = {
          Leap::Bone::Type::TYPE_METACARPAL,
          Leap::Bone::Type::TYPE_PROXIMAL,
          Leap::Bone::Type::TYPE_INTERMEDIATE,
          Leap::Bone::Type::TYPE_DISTAL,
        };

        for ( auto type : boneType ) {
          auto bone = finger.bone( type );
          ss << bone.length() << ", " << bone.width() << std::endl;
        }
      }
    }

    // テキストボックスを作成する
    auto tbox = TextBox()
      .alignment( TextBox::LEFT )
      .font( mFont )
      .text ( ss.str() )
      .color(Color( 1.0f, 1.0f, 1.0f ))
      .backgroundColor( ColorA( 0, 0, 0, 0.5f ) );

    mTextTexture = gl::Texture( tbox.render() );
  }

  // Leap Motion関連の描画
  void drawLeapObject()
  {
    // 表示座標系の保持
    gl::pushMatrices();

    // カメラ位置を設定する
    gl::setMatrices( mMayaCam.getCamera() );

    // 指を表示する
    for ( auto finger : mCurrentFrame.fingers() ) {
      if ( finger.isExtended() ) {
        setDiffuseColor( ci::ColorA( 0, 1, 1 ) );
      }
      else {
        setDiffuseColor( ci::ColorA( 1, 1, 0 ) );
      }

      const Leap::Bone::Type boneType[] = {
        Leap::Bone::Type::TYPE_METACARPAL,
        Leap::Bone::Type::TYPE_PROXIMAL,
        Leap::Bone::Type::TYPE_INTERMEDIATE,
        Leap::Bone::Type::TYPE_DISTAL,
      };

      for ( auto type : boneType ) {
        drawBone( finger.bone( type ) );
      }
    }
    
    // 色を元に戻す
    setDiffuseColor( ci::ColorA( 0.8, 0.8, 0.8 ) );

    // 表示座標系を戻す
    gl::popMatrices();
  }

  // 指の関節座標をより詳細に表示する
  void drawBone( Leap::Bone bone )
  {
    // 中心
    setDiffuseColor( ci::ColorA( 1, 0, 0 ) );
    gl::drawSphere( toVec3f( bone.center() ), 5 );

    // てのひら側
    setDiffuseColor( ci::ColorA( 0, 1, 0 ) );
    gl::drawColorCube( toVec3f( bone.nextJoint() ), Vec3f( 7, 7, 7 ) );

    // 指先側
    setDiffuseColor( ci::ColorA( 0, 0, 1 ) );
    gl::drawSphere( toVec3f( bone.prevJoint() ), 5 );
  }

  // テクスチャの描画
  void drawTexture()
  {
    if( mTextTexture ) {
      gl::draw( mTextTexture );
    }
  }

  void setDiffuseColor( ci::ColorA diffuseColor )
  {
    gl::color( diffuseColor );
    glMaterialfv( GL_FRONT, GL_DIFFUSE, diffuseColor );
  }

  // Leap SDKのVectorをCinderのVec3fに変換する
  Vec3f toVec3f( Leap::Vector vec )
  {
    return Vec3f( vec.x, vec.y, vec.z );
  }

  // カメラ
  CameraPersp  mCam;
  MayaCamUI    mMayaCam;

  // パラメータ表示用のテクスチャ
  gl::Texture mTextTexture;
  Font mFont;

  // Leap Motion
  Leap::Controller mLeap;
  Leap::Frame mCurrentFrame;
  Leap::Frame mLastFrame;
};

CINDER_APP_NATIVE( LeapApp, RendererGl )
