/* GemRB - Infinity Engine Emulator
 * Copyright (C) 2003 The GemRB Project
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Header: /data/gemrb/cvs2svn/gemrb/gemrb/gemrb/plugins/Core/CharAnimations.h,v 1.16 2003/12/09 19:01:54 balrog994 Exp $
 *
 */

#ifndef CHARANIMATIONS_H
#define CHARANIMATIONS_H

#include "Animation.h"
#include "../../includes/RGBAColor.h"
#include "TableMgr.h"
#include <vector>

#ifdef WIN32

#ifdef GEM_BUILD_DLL
#define GEM_EXPORT __declspec(dllexport)
#else
#define GEM_EXPORT __declspec(dllimport)
#endif

#else
#define GEM_EXPORT
#endif

#define IE_ANI_ATTACK			0
#define IE_ANI_AWAKE			1
#define IE_ANI_CAST				2
#define IE_ANI_CONJURE			3
#define IE_ANI_DAMAGE			4
#define IE_ANI_DIE				5
#define IE_ANI_HEAD_TURN		6
#define IE_ANI_READY			7
#define IE_ANI_SHOOT			8
#define IE_ANI_TWITCH			9
#define IE_ANI_WALK				10
#define IE_ANI_ATTACK_SLASH		11
#define IE_ANI_ATTACK_BACKSLASH	12
#define IE_ANI_ATTACK_JAB		13
#define IE_ANI_EMERGE			14
#define IE_ANI_HIDE				15
#define IE_ANI_SLEEP			16

#define IE_ANI_CODE_MIRROR		0
#define IE_ANI_ONE_FILE			1
#define IE_ANI_TWO_FILES		2
#define IE_ANI_FOUR_FILES		3
#define IE_ANI_CODE_MIRROR_2	4
#define IE_ANI_ONE_FILE_2		5
#define IE_ANI_TWO_FILES_2		6
#define IE_ANI_CODE_MIRROR_3	7
#define IE_ANI_ONE_FILE_3		8
#define IE_ANI_TWO_FILES_3		9
#define IE_ANI_PST_ANIMATION_1	10
#define IE_ANI_PST_GHOST		11

#define IE_ANI_NO_ARMOR			0
#define IE_ANI_LIGHT_ARMOR		1
#define IE_ANI_MEDIUM_ARMOR		2
#define IE_ANI_HEAVY_ARMOR		3

#define IE_ANI_WEAPON_1H		0
#define IE_ANI_WEAPON_2H		1
#define IE_ANI_WEAPON_2W		2

#define IE_ANI_RANGED_BOW		0
#define IE_ANI_RANGED_XBOW		1
#define IE_ANI_RANGED_THROW		2


class GEM_EXPORT CharAnimations
{
private:
	Animation * Anims[18][16];
public:
	Color Palette[256];
	unsigned long LoadedFlag, RowIndex;
	unsigned char OrientCount, MirrorType;
	unsigned char ArmorType, WeaponType, RangedType;
	unsigned char CircleSize;
	bool DrawCircle;
	char * ResRef;
	TableMgr * Avatars;
	bool UsePalette;
public:
	CharAnimations(char * BaseResRef, unsigned char OrientCount, unsigned char MirrorType, int RowIndex);
	~CharAnimations(void);
	Animation * GetAnimation(unsigned char AnimID, unsigned char Orient);
	void SetNewPalette(Color * Pal)
	{
		memcpy(Palette, Pal, 256*sizeof(Color));
		for(int i = 0; i < 18; i++) {
			for(int j = 0; j < 16; j++) {
				if(Anims[i][j])
					Anims[i][j]->ChangePalette = true;
			}
		}
	}
private:
	void AddVHRSuffix(char * ResRef, unsigned char AnimID, unsigned char &Cycle, unsigned char Orient);
	void AddMHRSuffix(char * ResRef, unsigned char AnimID, unsigned char &Cycle, unsigned char Orient);
	void AddMMRSuffix(char * ResRef, unsigned char AnimID, unsigned char &Cycle, unsigned char Orient);
	void GetAnimResRef(unsigned char AnimID, unsigned char Orient, char * ResRef, unsigned char & Cycle);
};

#endif
