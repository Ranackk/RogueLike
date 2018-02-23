#pragma once
#include "stdafx.h"
#include "BoxCollider.h"
#include "CircleCollider.h"

class CollisionLayer {
public:
	static const unsigned char NONE				= 0100000000;
	static const unsigned char MAP_GEOMETRY		= 0b00000000;
	static const unsigned char FRIENDLY_UNITS	= 0b00000001;
	static const unsigned char HOSTILE_UNITS	= 0b00000010;
};

class PhysicsEngine
{
public:

	static bool collides(BoxCollider &c1, BoxCollider &c2);
	static bool collides(BoxCollider &c1, CircleCollider &c2);
	static bool collides(CircleCollider &c1, CircleCollider &c2);


private:

	/* ======================================================================== *
	* Expand comment to find out how the collision layer mask flag field works *
	* ======================================================================== */
	/* Collision Layers:
	*
	* This engine uses a layer system to pre-eliminate collisions between objects that should not collide.
	*
	* There is a GLOBAL LayerMaskFlagField that is true for the whole Engine.
	* Each bit in this field acts as a FLAG that defines if two colliders can collide.
	* Each collider is set to a COLLISION LAYER (which is the layers number as an int)
	*
	* The LayerMaskFlagField is built like this & supports FIVE (5) layers:
	*					
	*					0			1				2
	*					1			2				4
	*					Map			PlayerComponent	EnemyComponent
	* Map				n			y				y
	* PlayerComponent				n				y
	* EnemyComponent								n
	*
	* y = Layers collide
	* n = Layers dont collide
	*
	* And it is stored in a simple unsigned int variable.
	* the last FIVE bits of the field are the		first row of the field in reversed order.
	* the last FOUR bits that then follow are the	second row of the field in reversed order.
	* and so on.
	* (Basically, I write the whole collision triangle in a single line and then reverse its order as that is how binary numbers work.)
	*
	* To check, If two layers can collide according to the collision field, the following logic happens inside "layerMaskAllowsCollision":
	* FlagField of length "l"
	* Layers: A with index "n", B with index "m"
	*
	* Step 1	: if n > m, swap n & m	-> Make sure they are in the correct order as I only store a collision flag triangle and not rectangle
	* Step 2	: find out at which index "i" the flag that represents the collision of AxB is located. To do that, I use the formular below.
	* Step 3	: create a checkFlag variable that is basically 2^i
	* Step 4	: bitwise AND the checkFlag variable with the bitfield, e.g.:
	*				0b 0000 0110		-> (part of the) FieldComponent
	*				0b 0000 0010	&	-> checkFlag
	*				============
	*				0b 0000 0010		-> result
	*			 if the result equals the checkFlag, the bitfield allows a collision between those two layers.
	*
	*
	* The formula to find the correct index:
	*
	* Lets look at this field
	*	0 1 2 3 4
	*	  5 6 7 8
	*	    9 A B
	*	      C D
	*	        E
	*	(F unused)
	*
	*	To get to the "7", we have to take the NUMBER (index+1) of end of the previous row (5),
	*	add the NUMBER (index + 1 -> 4) of the desired column and substract the NUMBER (index + 1, 2) of the desired row.
	*	The desired row will be called "n" from now on, the desired column "m"
	*
	*	The end of the previous row can be calculated without recursion using Pascal's Triangle:
	*	(n + 1) * L - ((n)^2 + n) / 2)
	*	with a Length L of 5,
	*	we get for n = ?, the result of
	*				0		5
	*				1		9
	*				2		C
	*				3		E
	*	which is exactly the first part of the formula.
	*
	*	The complete formula then is the following:
	*			"end of row NUMBER"					ADD "column"	SUB "row"
	*	i =		(n + 1) * L - ((n)^2 + n) / 2)		+ m				- n
	*
	*	Inside the "layerMaskAllowsCollision" method, I use a slightly altered formula to make things a bit faster.
	*/

	const static unsigned int m_s_c_CollisionLayerMaskFlagField = 0b000000001000110;// 0b1000110; //0b011000100000000;
	const static unsigned short m_s_c_CollisionLayers = 5;

	static bool layerMaskAllowsCollision(Collider& c1, Collider& c2);
};


/* The games collision layer mask supports up 5 layers
* S = Static Geometry
* F = Friendly Units (PlayerComponent, PlayerComponent Projectiles)
* E = EnemyComponent units (Enemies, EnemyComponent Projectiles)
* ?! = Not used at the moment
*
*		SFE?!
*	S	01100
*	F	 0100
*	E	  000
*	?	   00
*	!	    0
*
* This layer mask uses 15 bit so I use a 16 bit int to store the global layer mask.
* Everytime, before a complicated collision check is executed, I test if the two colliders may collide depending on this layerMask:
*
*					   SEF? !///
* Static Geometry = 0b0110 0000 -> Can collide with Friendly & EnemyComponent Units
* Friendly Units  = 0b1010 0000 -> Can collide with Static G.& EnemyComponent Units
*
*
*
* =====
* 2 Approaches:
*
* Am Liebsten, aber komplziertesten: 1 GlobalesFlagField, auf das zugegriffen wird, in dem man irgendwie fxE zu index 6 tracken kann und dann einfach nur noch das einzelne bit prüft.
*									  Inputs A und B müssen so getauscht werden, dass i(A) < i(B) gilt.
*
*									  S (=0) ende = (5)										= 1 * (5)								= 5
*									  F (=1) ende = S ende + S ende - 1						= 2 * (5) - 1							= 9
*									  E (=2) ende = S ende + S ende - 1 + S ende - 2		= 3 * (5) - 3							= 12
*									  ? (=3) ende =										    = 4 * (5) - 6							= 14
*									  ? (=4) ende =											= 5 * (5) - 10							= 15
*
*									  N (=n) ende =											= [n + 1] * (5) - [n * (n+1)) / 2]		= 5n + 5 - (n^2 + n ) / 2
*
*									  Außerdem: wir wollen den index bei 0 starten lassen, also -1 dahinter
*
*									  F x E = (F-1) ende + (E - F + 1)	-1
*									  E x ! = (E-1) ende + (! - E + 1)	-1
*									  ====
*
*									  für A mit kleinerem Index n als B mit Index m
*									  bei einem Flag FieldComponent der Länge l
*
*									  gilt es folgende flag zu überprüfen:
*
*									  A x B = (n-1) ende					+ (m - n + 1)	-1
*											= n * L		- ((n-1) * n) / 2)	+ (m - n + 1)	-1
*											= n * L		- (n^2 - n) / 2		+ (m - n + 1)	-1
*
*		SFE?!
*	S	01100
*	F	 0100
*	E	  000
*	?	   00
*	!	    0
*
*									  Test:
*
*									  E x ?:
*									  n = 2, m = 3
*
*									  2 * 5 - (2^2 - 2) / 2 + (3 - 2 + 1) -1
*											= 10 - 1 + 2 - 1= 10			-> check
*
*									  ? x !:
*									  n = 3, m = 4
*
*									  3 * 5 - (3^2 - 3) / 2 + (4 - 3 + 1) -1
*											= 15 - 3 + 2 - 1 = 13			-> check
*
*
*									 Finale, kurze Formel:
*									 n * L		- (n^2 - n) / 2		+ (m - n + 1)	-1
*								=	 n * L		- (n^2 - n) / 2		+ m - n
*								=	 (n - 1)  L - (n^2 - n) / 2		+ m
*								=	 (n - 1.5)L - (n^2) / 2			+ m
*
*
*/



