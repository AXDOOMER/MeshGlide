// Copyright (C) 2014-2018 Alexandre-Xavier Labonté-Lamoureux
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// things.h
// Data structures (player, polygons A.K.A. "planes", etc. ) used in the World

#ifndef THINGS_H
#define THINGS_H

#include "texture.h"
#include "cache.h"
#include "vecmath.h"	// Custom library for vector math, collision with planes, etc.

#include <string>
#include <cmath>
#include <vector>

#include <cfloat>
#include <iostream>

using namespace std;

const float GRAVITY = 0.2f;
//const float PI = atan(1) * 4;
const int MAXOWNEDWEAPONS = 10;

class Plane
{
public:
	string Texture;
	bool Impassable = true;
	bool TwoSided = false;
	vector<Float3> Vertices;
	vector<Float2> UVs;
	float Xscale = 0;
	float Yscale = 0;
	float Xoff = 0;
	float Yoff = 0;
	float Light = 1;	// Must be between 0 (dark) and 1 (full bright)

	Float3 normal;
	Float3 centroid;
//	vector<Plane*> Neighbors;	// List of adjacent planes

	float Angle() const;

	float Max() const;
	float Min() const;

	void Process();		// Find centroid, find normal...

    float    maxx = -FLT_MAX;    // right
    float    maxy = -FLT_MAX;    // top
    float    minx = FLT_MAX;     // left
    float    miny = FLT_MAX;     // bottom

	void setbox()
	{
        for (unsigned int i = 0; i < Vertices.size(); i++)
        {
            if (Vertices[i].x < minx)
                minx = Vertices[i].x;
            if (Vertices[i].x > maxx)
                maxx = Vertices[i].x;
            
            if (Vertices[i].y < miny)
                miny = Vertices[i].y;
            if (Vertices[i].y > maxy)
                maxy = Vertices[i].y;
        }
	}

	bool CanWalk() const
	{
		const float WALL_ANGLE = 0.4f;	// '1' points up (floor) and '0' points to the side (wall)

		if (Impassable && normal.z < WALL_ANGLE && normal.z > -WALL_ANGLE)
			return false;
		return true;
	}
};

class Block
{
public:

    int dimension;  // size (a square, so width and height is the same)
    int x;
    int y;

    Block(int d, int cornerx, int cornery)
    {
        dimension = d;
        x = cornerx;
        y = cornery;
//        cout << "Bloc de grosseur "<< d << " cree avec le coin (" << x << ", " << y <<")" << endl;
    }
    
    bool touch(Plane* p)
    {
        if (x <= p->maxx && x + dimension >= p->minx && y >= p->miny && y - dimension <= p->maxy)
            return true;
        return false;
    }
    
    void addPoly(Plane* p) {polys.push_back(p); }
    
    vector<Plane*> polys;
    
    // Nombre d'elements qui est dans le bloc
    int Count() { return polys.size(); };
};

class Blockmap
{
public:

    float maxx, maxy, minx, miny;
    int columns = 0;
    int rows = 0;
    const int blocksize = 8;    // Pour changer la taille du bloc (peut etre change de 1 a 8 pour faire des tests)
    vector<vector<Block>> blocks;

    /* build a blockmap from polys */
    Blockmap(vector<Plane*> polys)
    {
        // block
        maxx = -FLT_MAX;    // right
        maxy = -FLT_MAX;    // top
        minx = FLT_MAX;     // left
        miny = FLT_MAX;     // bottom
        
        for (unsigned int i = 0; i < polys.size(); i++)
        {
            if (polys[i]->minx < minx)
                minx = polys[i]->minx;
            if (polys[i]->maxx > maxx)
                maxx = polys[i]->maxx;
            
            if (polys[i]->miny < miny)
                miny = polys[i]->miny;
            if (polys[i]->maxy > maxy)
                maxy = polys[i]->maxy;
        }
        
        cout << "Largeur du blockmap: " << maxx - minx << endl;
        cout << "Hauteur du blockmap: " << maxy - miny << endl;
        columns = ceil((maxx - minx) / blocksize);
        rows = ceil((maxy - miny) / blocksize);
        cout << "Colonnes du blockmap: " << columns << endl;
        cout << "Lignes du blockmap: " << rows << endl;
        
        // Creer les blocs. 
        for (int r = 0; r < rows; r++)
        {
            vector<Block> rowOfBlocks;
            for (int c = 0; c < columns; c++)
            {
                Block b = Block(blocksize, floor(minx + c*blocksize), floor(maxy - r*blocksize));
                for (unsigned int p = 0; p < polys.size(); p++)
                {
                    if (b.touch(polys[p]))
                    {
                        b.addPoly(polys[p]);
                    }
                }
                
                rowOfBlocks.push_back(b);
            }
            
            blocks.push_back(rowOfBlocks);
        }
    }

	vector<Plane*> getPlanes(float x, float y)
	{
		// use the player's position and return the planes touched
		int i = abs(x - minx) / blocksize;
		int j = abs(y - miny) / blocksize;

		j = rows-j-1;

		if (i < 0 || j < 0 || j >= blocks.size() || i >= blocks[0].size())
		{
			vector<Plane*> empty;
			cout << "Player is out of the map!" << endl;
			return empty;
		}

		cout << blocks[j][i].Count() << " polys in current block.   i:" << i << "   j:" << j << endl;

		vector<Plane*> myplanes;

		for(int m = -1; m < 2; m++) {
			for(int n = -1; n < 2; n++) {
				if (!(i+m < 0 || j+n < 0 || j+n >= blocks.size() || i+m >= blocks[0].size())) {

					for (int k = 0; k <  blocks[j+n][i+m].Count(); k++) {
						myplanes.push_back( blocks[j+n][i+m].polys[k] );
					}

				}
			}
		}

		cout << myplanes.size() << " polys in donut block and timbits.   i:" << i << "   j:" << j << endl;

/*
		for (int k = 0; k <  blocks[j][i].Count(); k++)
		{
			if (blocks[j][i].polys[k]->Light > 0.5f)
				blocks[j][i].polys[k]->Light = 0.0f;
			else
				blocks[j][i].polys[k]->Light = 1.0f;
		}
*/
		//return blocks[j][i].polys;
		return myplanes;
	}

	string moyenne()
	{
		// count non empty planes only
		int totalcount = 0;
		int totalplanes = 0;

cout <<  "i blocks	" << blocks.size() << endl; 
cout <<  "j blocks[i]	" << blocks[0].size() << endl; 

        for (unsigned int i = 0; i < blocks.size(); i++)
        {
            for (unsigned int j = 0; j < blocks[i].size(); j++)
            {
                if ( blocks[i][j].Count() > 0)
				{
					totalcount += blocks[i][j].Count();
					totalplanes++;
				}

            }
        }

		return to_string(totalcount / (float)totalplanes);
	}
    
    string toString()
    {
        string s = "";
        
        for (unsigned int i = 0; i < blocks.size(); i++)
        {
            for (unsigned int j = 0; j < blocks[i].size(); j++)
            {
                string n = to_string( blocks[i][j].Count() );
                //cout << s << endl;
                //s += c;

				s += n += string(5 - n.size(), ' ');
            }
            s += '\n';
        }
        
        return s;
    }
};

class TicCmd
{
public:
	TicCmd();
	void Reset();

	unsigned char id;
	signed char forward;
	signed char lateral;
	short rotation;
	short vertical;
	bool fire;
	bool quit;
	string chat;
};

class Thing
{
public:
	// Camera position
/*	virtual float CamX() const = 0;
	virtual float CamY() const = 0;
	virtual float CamZ() const = 0;
	// Camera orientation
	virtual float AimX() const = 0;
	virtual float AimY() const = 0;
	virtual float AimZ() const = 0;*/

	// Position
	virtual float PosX() const = 0;
	virtual float PosY() const = 0;
	virtual float PosZ() const = 0;

	virtual float Radius() const = 0;
	virtual float Height() const = 0;

	virtual Texture* GetSprite(Float3 CamPos) const = 0;

	virtual bool Update();	// Returns 'true' if still alive, 'false' if it needs to be deleted.

	// So the compiler doesn't warn on deleting an object of polymorphic class type
	// https://stackoverflow.com/questions/353817/should-every-class-have-a-virtual-destructor
	virtual ~Thing() = default;

//protected:
//	Texture* sprite;
	Float3 pos_;	// Position
	Float3 mom_;	// Momentum
	//float Radius;
	Plane* plane;
};

class Player: public Thing
{
public:
	TicCmd Cmd = TicCmd();

	// Object-oriented programming is a pain. Made it public so it's easily accessible.
	short Angle = 8192;	// Yaw
	float VerticalAim = 0;	// Pitch

//	Float3 pos_;		// Position of player's feet
	const float ViewZ = 1.8f;
	char MoX = 0;		// Speed vector (momentum)
	char MoY = 0;
	char MoZ = 0;		// Used by gravity
	const float MaxStep = 1.0f;
	const float Radius_ = 0.5f;
	const float Height_ = 2.0f;

	int AirTime = 0;	// When the player falls
	bool ShouldFire = false;

	// Weapons that are in the player's possession
	bool OwnedWeapons[MAXOWNEDWEAPONS];

	// Ammo types
	short Ammo = 0;
	short Shells = 0;
	short Rockets = 0;
	short Cells = 0;

	Player(); // We need a constructor for the weapons array
	~Player();	// Deletes the sprite
	void Reset();
	float PosX() const;
	float PosY() const;
	float PosZ() const;

	// Sprite names
	const vector<string> sprites_ = {"playa1.png", "playa2.png", "playa3.png", "playa4.png", "playa5.png", "playa6.png", "playa7.png", "playa8.png"};
	Texture* GetSprite(Float3 CamPos) const;

private:
	const float MaxWalkSpeed = 0.6f;
	const float MaxRunSpeed = 1.1f;

	int Kills = 0;		// For deathmatch
	int Deaths = 0;

public:
	// Executes the player's actions
	void ExecuteTicCmd();

	// Command transfer
	vector<unsigned char> CmdToNet() const;
	void NetToCmd(vector<unsigned char> v);

	float GetRadianAngle(short Angle) const;
	float Radius() const;
	float Height() const;

	void ForwardMove(int Thrust);
	void LateralMove(int Thrust);
	void AngleTurn(short AngleChange);
	void AngleLook(short AngleChange);
	short AmountToCenterLook();	// Angle

	// Camera position
	float CamX() const;
	float CamY() const;
	float CamZ() const;
	// Camera orientation
	float AimX() const;
	float AimY() const;
	float AimZ() const;

	// TODO: Add Pos() and Aim(). Aim is a normalized vector.
};

class Weapon: public Thing
{
public:
	Weapon(float x, float y, float z, string type);
	~Weapon();	// Deletes the sprite

	float PosX() const;
	float PosY() const;
	float PosZ() const;
	float Radius() const;
	float Height() const;

	string Type_;
	string Filename_;	// For the sprite
	float Radius_;
	float Height_;

	Texture* GetSprite(Float3 CamPos) const;
};

class Puff: public Thing
{
public:
	Puff(float x, float y, float z);
	~Puff();	// Deletes the sprite

	float PosX() const;
	float PosY() const;
	float PosZ() const;
	float Radius() const;
	float Height() const;

	string Type;
	int Age_;
	const int MAX_AGE = 16;

	// Sprite names
	const vector<string> sprites_ = {"puffa0.png", "puffb0.png", "puffc0.png", "puffd0.png"};
	Texture* GetSprite(Float3 CamPos) const;
	bool Update();
};

class Critter: public Thing
{
private:
	float Direction = 0;
public:
	void Move();
};

struct SpawnSpot
{
	Float3 pos_;
	short Angle;
};

#endif /* THINGS_H */
