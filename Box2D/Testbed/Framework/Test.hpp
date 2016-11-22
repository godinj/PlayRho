/*
* Original work Copyright (c) 2006-2009 Erin Catto http://www.box2d.org
* Modified work Copyright (c) 2016 Louis Langholtz https://github.com/louis-langholtz/Box2D
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef TEST_H
#define TEST_H

#include <Box2D/Box2D.h>
#include <cstdlib>
#include "Drawer.hpp"
#include <Box2D/Collision/CollideShapes.hpp>

namespace box2d {

class Test;
struct Settings;

typedef Test* TestCreateFcn();

#define	RAND_LIMIT	32767
#define DRAW_STRING_NEW_LINE 16

/// Random number in range [-1,1]
inline float_t RandomFloat()
{
	auto r = static_cast<float_t>(std::rand() & (RAND_LIMIT));
	r /= RAND_LIMIT;
	r = 2.0f * r - 1.0f;
	return r;
}

/// Random floating point number in range [lo, hi]
inline float_t RandomFloat(float_t lo, float_t hi)
{
	auto r = static_cast<float_t>(std::rand() & (RAND_LIMIT));
	r /= RAND_LIMIT;
	r = (hi - lo) * r + lo;
	return r;
}

/// Test settings. Some can be controlled in the GUI.
struct Settings
{
	float_t hz = float_t(60);
	int32 velocityIterations = 8;
	int32 positionIterations = 3;
	bool drawShapes = true;
	bool drawJoints = true;
	bool drawAABBs = false;
	bool drawContactPoints = false;
	bool drawContactNormals = false;
	bool drawContactImpulse = false;
	bool drawFrictionImpulse = false;
	bool drawCOMs = false;
	bool drawStats = false;
	bool drawProfile = false;
	bool enableWarmStarting = true;
	bool enableContinuous = true;
	bool enableSubStepping = false;
	bool enableSleep = true;
	bool pause = false;
	bool singleStep = false;
};

struct TestEntry
{
	const char *name;
	TestCreateFcn *createFcn;
};

extern const TestEntry g_testEntries[];

// This is called when a joint in the world is implicitly destroyed
// because an attached body is destroyed. This gives us a chance to
// nullify the mouse joint.
class TestDestructionListener : public DestructionListener
{
public:
	void SayGoodbye(Fixture& fixture) { BOX2D_NOT_USED(fixture); }
	void SayGoodbye(Joint& joint);

	Test* test;
};

const int32 k_maxContactPoints = 2048;

struct ContactPoint
{
	Fixture* fixtureA;
	Fixture* fixtureB;
	Vec2 normal;
	Vec2 position;
	PointState state;
	float_t normalImpulse;
	float_t tangentImpulse;
	float_t separation;
};

class Test : public ContactListener
{
public:
	enum Key {
		Key_Comma, Key_Period,
		Key_0, Key_1, Key_2, Key_3, Key_4, Key_5, Key_6, Key_7, Key_8, Key_9,
		Key_A, Key_B, Key_C, Key_D, Key_E, Key_F, Key_G, Key_H, Key_I, Key_J, Key_K, Key_L, Key_M,
		Key_N, Key_O, Key_P, Key_Q, Key_R, Key_S, Key_T, Key_U, Key_V, Key_W, Key_X, Key_Y, Key_Z,
		Key_Unknown
	};

	Test();
	virtual ~Test();

	void DrawTitle(Drawer& drawer, const char *string);
	virtual void Step(Settings& settings, Drawer& drawer);
	virtual void Keyboard(Key key) { BOX2D_NOT_USED(key); }
	virtual void KeyboardUp(Key key) { BOX2D_NOT_USED(key); }
	void ShiftMouseDown(const Vec2& p);
	virtual void MouseDown(const Vec2& p);
	virtual void MouseUp(const Vec2& p);
	void MouseMove(const Vec2& p);
	void LaunchBomb();
	void LaunchBomb(const Vec2& position, const Vec2& velocity);
	
	void SpawnBomb(const Vec2& worldPt);
	void CompleteBombSpawn(const Vec2& p);

	// Let derived tests know that a joint was destroyed.
	virtual void JointDestroyed(Joint* joint) { BOX2D_NOT_USED(joint); }

	// Callbacks for derived classes.
	virtual void BeginContact(Contact& contact) override { BOX2D_NOT_USED(contact); }
	virtual void EndContact(Contact& contact) override { BOX2D_NOT_USED(contact); }
	virtual void PreSolve(Contact& contact, const Manifold& oldManifold) override;
	virtual void PostSolve(Contact& contact, const ContactImpulse& impulse, ContactListener::iteration_type solved) override
	{
		BOX2D_NOT_USED(contact);
		BOX2D_NOT_USED(impulse);
		BOX2D_NOT_USED(solved);
	}

	void ShiftOrigin(const Vec2& newOrigin);

protected:
	friend class TestDestructionListener;
	friend class BoundaryListener;
	friend class ContactListener;

	Body* m_groundBody;
	AABB m_worldAABB;
	ContactPoint m_points[k_maxContactPoints];
	int32 m_pointCount;
	TestDestructionListener m_destructionListener;
	int32 m_textLine;
	World* m_world;
	Body* m_bomb;
	MouseJoint* m_mouseJoint;
	Vec2 m_bombSpawnPoint;
	bool m_bombSpawning;
	Vec2 m_mouseWorld;
	int32 m_stepCount;

	Profile m_maxProfile;
	Profile m_totalProfile;
};

} // namespace box2d

#endif