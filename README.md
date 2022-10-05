# RogueLike

RogueLike is a game prototype inspired by the The Binding Of Isaac that I worked on in my second semester (early 2018).\
From a technical view, the interesting parts are that the game runs in a custom C++/OpenGL engine featuring
- gameobject component system
- realtime shadow mapping in forward rendering
- realtime batching and instancing
- custom physics implementation

See the original description below for more details on these topics or feel free to check out the project yourself!

## Game Features
### Self-Written Game Engine

- **GameObjects with Components**
    There are basic components for general purposes and special "UpdateComponents" & "RenderComponents" that can participate in the update & rendering systems.

	In general, I mostly used components and at some occasions systems - But most of the time, I decided against systems, as I prefer to implement systems in a complete manner.
	And for example doing a completely autonomous physics system requires a system that creates a scene oct tree for performance reasons. 
	Which I then also would have to incorporate in the render batch system, the pooling system, the render system in general and so on. 
	What I ended up with is a solution where I add components to gameobject manually and then manually add them to e.g. a vector of "dynamic geometry" which from there on handles the rendering & updating of the object.
	Still, I tried to incorporate a somewhat linear memory layout as often as possible, e.g. by storying all fields and all pooled objects (Enemies, Bullets) in a container with linear memory layout (c-Array, std::array or std::vector (given a size at initialization)). This helps the engine update those objects very fast as they are iterated in the same way they are in memory.
- **Transform Hierarchies**
    Supports position, rotation (both via quaternion & via euler) & scale as well as parenting
- **Custom Render Engine**
I use OpenGL with forward rendering for my game.

    Most common techniques I use are object indexing (Instead of just a vertex-, uv-, ..., buffer, I also use a index buffer to reduce bandwith when communicating with the GPU.
    *Special Techniques:*
    
    - **Mixed baked & realtime shadow mapping for point lights**
        Basic Concept: Each frame, each light takes 6 snapshots of the depth(!) of the scene in all directions and stores 
		them inside a cube map. Later on, shadow-receiving shaders sample the cube map at the correct direction and calculate whether it is the first surface hit by the light (lit) or not (inside shadow)
		    - This system currently support sp to 64 lights in total and can handle about 20-30 lights in 60 fps in my machine
    - **Realtime Batching/Instancing, Object Pooling**
		In the engine, I provide functionality to batch objects together, both in a object pooled and non-object pooled way.
		A Batch can be created for objects that share model and material.
		Then, I use instancing to render all of the meshes in one single draw call.
		- In my case, I send just one model of the batch to the GPU and then an array of 4x4 matrices that are used to transform all objects that are batched together. This is about the same amount of work for the GPU but also greatly decreases bandwith between CPU & GPU!
		- During the early stages of the production, I did some benchmarks:
		     - I rendered 2000 textured cubes with simple lighting (no shadows) in about 12 ms (~83 fps). Then, I switched to instanced rendering and rendered the same amount of cubes with the same material in about 1.2 ms (~830 fps).
		     - This is an performance gain of 900%!
	    - This supports static objects such as the fields that make up the map as well as pooled game objects like the enemies or bullets, for both cases optimizing how much data needs to be calculated each frame (none for the static objects, some for the dynamic ones)
	    - As a result, I need about 30 drawcalls to draw the scene once with about 20 of them being from shadowcasters.
	    Each type of field or entity takes a single drawcall, independent of whether just one instance (like the player) or 1500 (like the wall) get rendered with it.
    - To draw a shadow cube map, I have to render the scene 6 times from the point of each light in each direction (+x, -x, +y, -y, +z, -z). Even when using a very simple shader (writing only depth values), this costs alot! Therefore, I use:
        -  **Range Culling on Lights**
		Only Lights that are near to the player render, allowing for the world to hold up to 64 Lights (which, when rendered all at the same time, would result in ~15 fps)
- **Custom Physics Engine**
    - Currently supports 2D-Circle Colliders & 2D-BoxColliders. Basically not that far away from 3D collisions, just simplified for performance purposes as there is no need for 3D collision in this game.
	- *CollisionLayers*
The engine supports physics layers:		
E.g. the whole Map Geometry is on layer 1, all Friendly Units on layer 2 & all Hostile Units on layer 3.
		A Collision Matrix that is layed out like this determines whether objects can collide
        ```
			M F H
		M   0 1 1
		F     0 1
		H       0
        ```
    - This collision matrix would he saved as the single int ```0b[...]010110  ```
For the real engine, I currently use 6 layers:
*Map Geometry, Friendly_Ground_Units, Hostile_Ground_Units, Friendly_Flying_Units, Hostile_Flying_Units, Water*
To support all of the layers, I have tu use an int32 to store all possible calculations (21).
This means that the current system supports even more layers that I just dont use at the moment.
**All of this allows me to be really fast in collision detection!**:
To check, if two layers may collide, I just use bitwise & operation between with the layer matrix and the layer collision to check. More detailed Information about the maths behind that can be found in "PhysicsEngine.h"

- **Texture, Model & Material Manager**
	All of them story identifiers (strings) and combine them with data to be able to easily get a model in the code by calling "getModelByIdentifer("mesh_Player");"

	- MaterialManager:	System to load up shaders and build up materials on top of them to communicate with the shaders
	- ModelManager:		Custom .Obj loader to load up triangulated & indexed models. (Non Indexed .objs will be indexed during the loading process. models need to be triangulated though)
	- TextureManager:	uses the .lodepng Loader by Lode Vandevenne
- **Custom Shaders**
	- Base Shader supports being lit by up to 64 Lights at the same time, also is able to receive shadows. Can be used both instanced & non-instanced!
	    - Also supports moving UVs (e.g. for the water)
	- UI Shader is able to draw cutout sprites (support for transparent textures) onto the HUD
	- Billboard Shader (non used in this project) is able to draw a billboard such as a enemy healthbar or particle that is stuck in the 3D world.
	- Depth Shader is used to draw the scene depth into a texture for shadow mapping
	- HealthContainer Shader is used to display the players current health. It uses a gradient texture to blend between the empty and the full heart depending on
	the players current health. Result is that this HUD can even display things like 0.23 HP which would result in a 23% filled heartcontainer.
	Also has 2 small animations: 
		- If it is not fully filled or completly empty, the red heart-mass "waves" around.
		- If it is the last filled heart, it blinks in bright red to alarm the player

-  **5 Camera Modes (see "Controls")**
	- Camera Shake support for Camera Mode 1
		- Basic Idea: Whenever the player takes damage, he gets some "trauma". Trauma linearly sinks over time.
				Trauma gets used exponentially to shake the cameras focus point using Perlin Noise.
		- Result is that upon taking the damage, the trauma linearly goes from 1 to 0, while the shake amount scales exponentially:
        ```
			Time	Trauma		Shake Amount
			0		1			100%
			1		0.75		56%
			2		0.5			25%
			3		0.25		6%
			4		0			0%
        ```
        Also, this allows for (if usefull for the game) staking trauma. E.g. the player gets hit once and has a little bit of nausea and with each
			further hit in the near future, he gets exponentially more trauma until its is completely reduced to 0 again.

Random Map Generator
-
The game uses a custom map generator that is supposed to mimmick The Biding Of Isaacs dungeons.
it starts by placing down an empty room the player can spawn in and then puzzles random rooms against each other in a way that all doors match!
- It starts by loading up the room file ("graphics/rooms.png") which contains all rooms that can be generated.
Each pixel describes field inside a room. The red channel contains information about the field type, the green chanel about the entities on the field and the blue chanel about light information.
The fieldtype dexcribes how a field looks, which mesh it uses, whether it has a collider (and if so, on which collision layer) and so on.
- Currently there are 30 unique room types
- You can explore the map by using camera modes 4 or 5 to fly over the map

Enemies
-
- Rogue: Charges at you and does big melee damage
- Archer: Sts still but shoots you with moderate ranged damage

Notes:
=

This is not a complete game.
Currently it features no set goal / end of game / player death action.
Those things would have upped the scope by quite a bit as I would have had to create a secondary scene for which the engine currently is not
made yet. (The other option would be to just enable a HUD overlay but I want to do things right with this game and dont use bad workarounds)

Controls:
=
Camera:
-

1- Default Camera (focuses current room, also has a feature to shake on player damage)
2- Player Camera 1
3 - Player Camera 2
4 - Free Flight
  - JL -> Move Left/Right
  - KI -> Move Forward/Backward
  - UO -> Strive Up/Down
  - Hold Shift -> Go faster
  
 5 - Free Flight with paused gameplay

Player:
-

WASD				- Movement
Left Joystick		- Movement (highly recommended, tested with a XBox360 Controller)

Arrow Keys			- Shooting
Right Joystick		- Shooting
Controller Buttons	- Shooting

General:
-
Escape				- Toggle Camera Lock / Pause, Unpause Game


Dependencies:
=
This game uses:
glfw				- Open GL Library
glew				- Open GL Extention Wrapper
glm					- Math Library!
lodepng				- PNG Library

All of them should work on their own as they are provided withing the "Additional dependencies" folder or are part of the project itself.

Credits:
=
- The Game is heavily inspired by *The Binding Of Isaac*
- I used the 4 libraries stated in the dependencies section
- Modeling (Enemies, Rocks & Walls) - Charlotte Kügler
- Everything else (Programming, UVs, Textures) was created by me!
