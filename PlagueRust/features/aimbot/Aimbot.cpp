/**
 * Special thanks to skit#2567 for help with this section
 *
 *  WORK IN PROGRESS
 */

#include "../../include.hpp"
#include "../../classes/math/Math.hpp"
#include "../../classes/Player.hpp"
#include "../../globals/globals.hpp"
#include "Aimbot.hpp"
#include "../../classes/GOM.hpp"

/**
 * Calculates The Angle
 */
Vector2 Aimbot::CalcAngle(const Vector3& LocalPos, const Vector3& EnemyPos)
{
	Vector3 dir = Vector3(LocalPos.x - EnemyPos.x, LocalPos.y - EnemyPos.y, LocalPos.z - EnemyPos.z);

	float Pitch = to_degree(asin(dir.y / Length(dir)));
	float Yaw = to_degree(-atan2(dir.x, -dir.z));

	return { Pitch, Yaw };
}

/**
 * Gets the Distance From The Cursor
 */
float Aimbot::distance_cursor(Vector2 vec)
{
	POINT p;
	if (GetCursorPos(&p))
	{
		float ydist = (vec.y - p.y);
		float xdist = (vec.x - p.x);
		float ret = sqrt(pow(ydist, 2) + pow(xdist, 2));
		return ret;
	}
}

/**
 * Gets Projectile Speed For A Weapon Using Normal Ammunition
 */
float Aimbot::ProjectileSpeed_Normal(uint64_t weapon, float Ammunition_Multiplier)
{
	/* Read held item to a string value */
	std::string held_item = GetItemName(weapon);

	/* Return some bogus value if it isn't held just a safety check if other functions don't work */
	if (held_item == "No Item")                 return 300.0f;

	/*
	* Decide weapons velocity based off name (( DEFAULT BULLET TYPE ))
	*/

	/* Normal 556 rifle ammunition */
	if (held_item == "rifle.ak")                return 375.0f * Ammunition_Multiplier;
	if (held_item == "rifle.lr300")             return 375.0f * Ammunition_Multiplier;
	if (held_item == "rifle.bolt")              return 656.0f * Ammunition_Multiplier;
	if (held_item == "rifle.l96")               return 1125.0f * Ammunition_Multiplier;
	if (held_item == "rifle.m39")               return 469.0f * Ammunition_Multiplier;
	if (held_item == "rifle.semiauto")          return 375.0f * Ammunition_Multiplier;

	/* Default 556 ammunition */
	if (held_item == "lmg.m249")                return 488.0f * Ammunition_Multiplier;

	/* Normal pistol bullet ammunition */
	if (held_item == "smg.thompson")            return 300.0f * Ammunition_Multiplier;
	if (held_item == "smg.custom")              return 240.0f * Ammunition_Multiplier;
	if (held_item == "smg.mp5")                 return 240.0f * Ammunition_Multiplier;

	/* Normal pistol bullet ammunition, hand made shell for Eoka  */
	if (held_item == "pistol.python")           return 300.0f * Ammunition_Multiplier;
	if (held_item == "pistol.semi")             return 300.0f * Ammunition_Multiplier;
	if (held_item == "pistol.revolver")         return 300.0f * Ammunition_Multiplier;
	if (held_item == "pistol.m92")              return 300.0f * Ammunition_Multiplier;
	if (held_item == "pistol.eoka")             return 100.0f * Ammunition_Multiplier;
	if (held_item == "pistol.nailgun")          return 50.0f;

	/* Plain arrow */
	if (held_item == "crossbow")               return 75.0f * Ammunition_Multiplier;
	if (held_item == "bow.compound")            return 100.0f * Ammunition_Multiplier;
	if (held_item == "bow.hunting")             return 50.0f * Ammunition_Multiplier;

	/* Hand made shell */
	if (held_item == "shotgun.pump")            return 100.0f * Ammunition_Multiplier;
	if (held_item == "shotgun.spas12")          return 100.0f * Ammunition_Multiplier;
	if (held_item == "shotgun.waterpipe")       return 100.0f * Ammunition_Multiplier;
	if (held_item == "shotgun.doublebarrel")    return 100.0f * Ammunition_Multiplier;

	/* If all fails then return a zero value to void all post calculations */
	return 300.0f;
}

/**
 * Calculates Bullet Drop For A Weapon
 */
float Aimbot::BulletDrop(Vector3 v1, Vector3 v2, float BulletSpeed, float BulletGravity, float kin_deltatime)
{
	//TODO
	return 0.f;
}

/**
 * Calculates Prediction For A Weapon
 */
Vector3 Aimbot::Prediction(Vector3 playerPos, Vector3 localPos)
{
	//TODO
	return Vector3(0, 0, 0);
}

void Aimbot::DoAimbot()
{
	/* Ensure The Target Is Still Networked */
	bool net = static_cast<bool>(mem->Read<uint64_t>(pTarget.bn + 0x50));
	if (!net)
		return;

	/* Get Velocity Of The Bullet */
	int this_vel = aimbot->ProjectileSpeed_Normal(globals->local_player.GetHeldItem().ent, aimbot->BulletSpeeds[globals->local_player.GetHeldItem().GetAmmoType()]);

	/* Grab The Target's Position */
	Vector3 first_pos = mem->Read<Vector3>(pTarget.transform + 0x90);

	/* Get Recoil Angles */
	Vector2 recoil_angles = Vector2{ globals->local_player.GetRecoilAngles().x, globals->local_player.GetRecoilAngles().y };

	/* Get The Camera's Transform */
	uint64_t camera_transform = mem->ReadChain<uint64_t>(gom->GetCamera(), { 0x30, 0x30, 0x8, 0x38 });

	/* Get The LocalPlayer's Head Position */
	Vector3 head_local = mem->Read<Vector3>(camera_transform + 0x90);

	/* Get The Target's Head Positon */
	Vector3 head_target = pTarget.bones[(Bones)settings->aimbot.bone_target];

	if (settings->aimbot.prediction)
	{
		
	}

	aimbot->prediction_pos = head_target;

	/* Calculate The Angle */
	Vector2 v2_aim = aimbot->CalcAngle(head_local, head_target);
	aimbot->Normalize(v2_aim.y, v2_aim.x);

	if (settings->aimbot.smooth)
	{
		v2_aim -= Vector2{ globals->local_player.GetViewAngles().x, globals->local_player.GetViewAngles().y };

		/* Smooth Aimbot */
		aimbot->SmoothAim(v2_aim, settings->aimbot.smoothing);

		v2_aim += Vector2{ globals->local_player.GetViewAngles().x, globals->local_player.GetViewAngles().y };
	}

	if (settings->aimbot.compensate_recoil)
	{
		/* Compensate Recoil */
		v2_aim -= recoil_angles;
	}

	/* Set View Angles*/
	globals->local_player.SetViewAngles({ v2_aim.x, v2_aim.y, 0 });
}

void Aimbot::FindTarget()
{

}

Aimbot* aimbot = new Aimbot();
