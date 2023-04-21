#ifndef IPARTICLEMAN_H
#define IPARTICLEMAN_H

#include "hlsdk/particleman/pman_triangleffect.h"

#ifdef _WIN32
#define PARTICLEMAN_DLLNAME "cl_dlls/particleman.dll"
#elif defined(OSX)
#define PARTICLEMAN_DLLNAME "cl_dlls/particleman.dylib"
#elif defined(LINUX)
#define PARTICLEMAN_DLLNAME "cl_dlls/particleman.so"
#else
#error
#endif

class CBaseParticle;

class IParticleMan : public IBaseInterface
{
protected:
	virtual			~IParticleMan() {}

public:

	virtual void SetUp(cl_enginefunc_t* pEnginefuncs) = 0;
	virtual void Update(void) = 0;
	virtual void SetVariables(float flGravity, Vector vViewAngles) = 0;
	virtual void ResetParticles(void) = 0;
	virtual void ApplyForce(Vector vOrigin, Vector vDirection, float flRadius, float flStrength, float flDuration) = 0;
	virtual void AddCustomParticleClassSize(unsigned long lSize) = 0;

	//Use this if you want to create a new particle without any overloaded functions, Think, Touch, etc.
	//Just call this function, set the particle's behavior and let it rip.
	virtual CBaseParticle* CreateParticle(Vector org, Vector normal, model_t* sprite, float size, float brightness, const char* classname) = 0;

	//Use this to take a block from the mempool for custom particles ( used in new ).
	virtual char* RequestNewMemBlock(int iSize) = 0;

	//These ones are used along a custom Create for new particles you want to override their behavior.
	//You can call these whenever you want, but they are mainly used by CBaseParticle.
	virtual void CoreInitializeSprite(CCoreTriangleEffect* pParticle, Vector org, Vector normal, model_t* sprite, float size, float brightness) = 0; //Only use this for TrianglePlanes
	virtual void CoreThink(CCoreTriangleEffect* pParticle, float time) = 0;
	virtual void CoreDraw(CCoreTriangleEffect* pParticle) = 0;
	virtual void CoreAnimate(CCoreTriangleEffect* pParticle, float time) = 0;
	virtual void CoreAnimateAndDie(CCoreTriangleEffect* pParticle, float time) = 0;
	virtual void CoreExpand(CCoreTriangleEffect* pParticle, float time) = 0;
	virtual void CoreContract(CCoreTriangleEffect* pParticle, float time) = 0;
	virtual void CoreFade(CCoreTriangleEffect* pParticle, float time) = 0;
	virtual void CoreSpin(CCoreTriangleEffect* pParticle, float time) = 0;
	virtual void CoreCalculateVelocity(CCoreTriangleEffect* pParticle, float time) = 0;
	virtual void CoreCheckCollision(CCoreTriangleEffect* pParticle, float time) = 0;
	virtual void CoreTouch(CCoreTriangleEffect* pParticle, Vector pos, Vector normal, int index) = 0;
	virtual void CoreDie(CCoreTriangleEffect* pParticle) = 0;
	virtual void CoreForce(CCoreTriangleEffect* pParticle) = 0;
	virtual bool CoreCheckVisibility(CCoreTriangleEffect* pParticle) = 0;
	virtual void SetRender(int iRender) = 0;
};

extern IParticleMan* g_pParticleMan;

#define PARTICLEMAN_INTERFACE_VERSION "create_particleman"

#endif //PARTICLEMAN_H