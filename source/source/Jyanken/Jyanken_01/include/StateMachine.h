#pragma once 

#include "GameAssets.h"

// ��Ԃ̊��N���X
class State
{
protected:

  GameAssets* mAssets;
  int64_t mEnterTime;

public:

  State( GameAssets* assets )
    : mAssets( assets )
  {
    mEnterTime = mAssets->getFrame().timestamp();
  }

  virtual State* update()
  {
    return this;
  }

protected:

  // �w�̐�����A�|�[�Y�����߂�
  JyankenPose fingerCountToPose( Leap::Hand hand )
  {
    // �L�тĂ���w�̐��𐔂���
    auto count = std::count_if( hand.fingers().begin(), hand.fingers().end(),
      [](const Leap::Finger& finger) { return finger.isExtended(); } );
    if ( count <= 1 ) {
      return JyankenPose::Gu;
    }
    else if ( count <= 3 ) {
      return JyankenPose::Choki;
    }
      
    return JyankenPose::Pa;
  }
};

// �A�C�h��(�ҋ@)���
class IdleState : public State
{
public:

  IdleState( GameAssets* assets )
    : State( assets )
  {
  }

  State* update();
};

// �u����񂯂�v�̊|�������s�����
class JyankenState : public State
{
public:

  JyankenState( GameAssets* assets )
    : State( assets )
  {
  }

  State* update();
};

// �u�ۂ�v�̊|�����ƃ|�[�Y�̌���s�����
class PonState : public State
{
  JyankenPose mPlayer;
  JyankenPose mComputer;
  std::list<float> mTransrates;

public:

  PonState( GameAssets* assets )
    : State( assets )
  {
    // �R���s���[�^�[�̃|�[�Y�����߂�
    mComputer = (JyankenPose)(Rand::randUint() % 3);
  }

  State* update();
};

// ���ʂ̕\�����s�����
class ResultState : public State
{
  JyankenPose mPlayer;
  JyankenPose mComputer;

public:

  ResultState( GameAssets* assets, JyankenPose player, JyankenPose computer )
    : State( assets )
    , mPlayer( player )
    , mComputer( computer )
  {
  }

  State* update();
};

State* IdleState::update()
{
  mAssets->setComputerPose( JyankenPose::Gu );
  mAssets->setText( L"����o���Ă�" );

  // ������o������A���̏�Ԃ֑J�ڂ���
  auto frame = mAssets->getFrame();
  if ( frame.hands().count() == 0 ) {
    return this;
  }
  else {
    return new JyankenState( mAssets );
  }
}

State* JyankenState::update()
{
  auto hand = mAssets->getFrame().hands()[0];
  if ( hand.isValid() ) {
    mAssets->setPlayerPose( fingerCountToPose( hand ) );
  }

  mAssets->setComputerPose( JyankenPose::Gu );
  mAssets->setText( L"����񂯂�" );

  // ��莞�ԂŎ��̏�Ԃ֑J�ڂ���
  if ( (mAssets->getFrame().timestamp() - mEnterTime) < (2 * 1000 * 1000) ) {
    return this;
  }
  else {
    return new PonState( mAssets );
  }
}

State* PonState::update()
{
  mAssets->setComputerPose( mComputer );
  mAssets->setText( L"�ۂ�" );

  // �肪����������A������|�[�Y�ɂ���
  auto hand = mAssets->getFrame().hands()[0];
  if ( hand.isValid() ) {
    mPlayer = fingerCountToPose( hand );
    mAssets->setPlayerPose( mPlayer );
    return new ResultState( mAssets, mPlayer ,mComputer );
  }

  // ��莞�ԃ|�[�Y�����܂�Ȃ�������A�A�C�h����Ԃɂ��ǂ�
  if ( (mAssets->getFrame().timestamp() - mEnterTime) > (5 * 1000 * 1000) ) {
    return new IdleState( mAssets );
  }

  return this;
}

State* ResultState::update()
{
  // ���������𔻒肷��
  mAssets->setComputerPose( mComputer );
  mAssets->setPlayerPose( mPlayer );

  if ( mComputer == mPlayer ) {
    mAssets->setText( L"������" );
  }
  else if ( (mComputer == JyankenPose::Gu) && (mPlayer == JyankenPose::Pa) ) {
    mAssets->setText( L"���Ȃ��̏���" );
  }
  else if ( (mComputer == JyankenPose::Pa) && (mPlayer == JyankenPose::Choki) ) {
    mAssets->setText( L"���Ȃ��̏���" );
  }
  else if ( (mComputer == JyankenPose::Choki) && (mPlayer == JyankenPose::Gu) ) {
    mAssets->setText( L"���Ȃ��̏���" );
  }
  else {
    mAssets->setText( L"���Ȃ��̕���" );
  }

  // ��莞�ԂŃA�C�h����Ԃɖ߂�
  if ( (mAssets->getFrame().timestamp() - mEnterTime) < (2 * 1000 * 1000) ) {
    return this;
  }
  else {
    return new IdleState( mAssets );
  }
}

// ��ԑJ�ڂ̏���
class StateMachine
{
  State* state;

public:

  StateMachine()
    : state( 0 )
  {
  }

  void setup( GameAssets* assets )
  {
    state = new IdleState( assets );
  }

  void update()
  {
    if ( state != 0 ) {
      auto nextState = state->update();
      if ( state != nextState ) {
        delete state;

        state = nextState;
      }
    }
  }
};
