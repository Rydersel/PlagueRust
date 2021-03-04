/**
 * <3 skit#2567
 *
 * don't make this into a shitty p2c
 */

#ifndef _AIMBOT_HPP
#define _AIMBOT_HPP

#pragma once
#include "../../classes/math/math.hpp"
class Aimbot
{
public:
	bool m_TargetExist;
	Vector3 prediction_pos;
private:
public:

	float distance_cursor(Vector2 vec);

	float to_radian(float degree)
	{
		return degree * 3.141592f / 180.f;
	}

	float to_degree(float radian)
	{
		return radian * 180.f / 3.141592f;
	}

	inline float Length(Vector3 v) {
		return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	}

	void Normalize(float& Yaw, float& Pitch) {
		if (Pitch < -89)
			Pitch = -89;

		else if (Pitch > 89)
			Pitch = 89;

		if (Yaw < -360)
			Yaw += 360;

		else if (Yaw > 360)
			Yaw -= 360;
	}

	std::map<std::string, float> BulletSpeeds = {
		/* For 5.56 Fed Weapons */
		{ std::string("ammo.rifle"), 1.0f },
		{ std::string("ammo.rifle.hv"), 1.2f },
		{ std::string("ammo.rifle.explosive"), 0.6f },
		{ std::string("ammo.rifle.incendiary"), 0.6f },

		/* For Pistol Ammunition Fed Weapons */
		{ std::string("ammo.pistol"), 1.0f },
		{ std::string("ammo.pistol.hv"), 1.33333f },
		{ std::string("ammo.pistol.incendiary"), 0.75f },

		/* For Weapons That Use Arrows */
		{ std::string("arrow.wooden"), 1.0f },
		{ std::string("arrow.hv"), 1.6f },
		{ std::string("arrow.fire"), 0.8f },
		{ std::string("arrow.bone"), 0.9f },

		/* For Shotguns */
		{ std::string("ammo.handmade.shell"), 1.0f },
		{ std::string("ammo.shotgun.slug"), 2.25f },
		{ std::string("ammo.shotgun.fire"), 1.0f },
		{ std::string("ammo.shotgun"), 2.25f },

		{ std::string("ammo.nailgun.nails"), 1.0f }
	};

	std::map<std::string, float> BulletGravity = {
		/* For 5.56 Fed Weapons */
		{ std::string("ammo.rifle"), 1.0f },
		{ std::string("ammo.rifle.hv"), 1.0f },
		{ std::string("ammo.rifle.explosive"), 1.25f },
		{ std::string("ammo.rifle.incendiary"), 1.0f },

		/* For Pistol Ammunition Fed Weapons */
		{ std::string("ammo.pistol"), 1.0f },
		{ std::string("ammo.pistol.hv"), 1.0f },
		{ std::string("ammo.pistol.incendiary"), 1.0f },

		/* For Weapons That Use Arrows */
		{ std::string("arrow.wooden"), 0.75f },
		{ std::string("arrow.hv"), 0.5f },
		{ std::string("arrow.fire"), 1.0f },
		{ std::string("arrow.bone"), 0.75f },

		/* For Shotguns */
		{ std::string("ammo.handmade.shell"), 1.0f },
		{ std::string("ammo.shotgun.slug"), 1.0f },
		{ std::string("ammo.shotgun.fire"), 1.0f },
		{ std::string("ammo.shotgun"), 1.0f },

		{ std::string("ammo.nailgun.nails"), 0.75f }
	};

	float BulletDrop(Vector3 v1, Vector3 v2, float BulletSpeed, float BulletGravity, float kin_deltatime);

	float ProjectileSpeed_Normal(uint64_t weapon, float Ammunition_Multiplier);

	void SmoothAim(Vector2& Angle, float smooth) {
		Angle.x /= smooth;
		Angle.y /= smooth;
	}
	Vector3 Prediction(Vector3 playerPos, Vector3 localPos);

	Vector2 CalcAngle(const Vector3& LocalPos, const Vector3& EnemyPos);

	void DoAimbot();

	void FindTarget();
};

extern Aimbot* aimbot;

#endif // _AIMBOT_HPP