#pragma		once

//Struct for save particle system file
struct stParticleEffect
{
	float mColorStartR;
	float mColorStartG;
	float mColorStartB;

	float mColorEndR;
	float mColorEndG;
	float mColorEndB;

	float mColorVarR;
	float mColorVarG;
	float mColorVarB;	

	float mAlphaStart;
	float mAlphaVar;
	float mAlphaEnd;

	float mSizeStart;
	float mSizeVar;
	float mSizeEnd;

	float mSpeed;
	float mSpeedVar;

	float mLife;
	float mLifeVar;

	unsigned int mTextureID;
	bool mSpark;
	unsigned int mParticlePerSec;
	float mSysLife;
	bool mIsAttractive;
	int mAttractStrength;

	float mGravityStart;
	float mGravityVar;
	float mGravityEnd;

	int	mEmiterType;
	bool mIsMoving;
	int mMovementType;
};