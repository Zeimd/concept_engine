concept_engine

TODO
------------------------------------------
Automation

- [ ] Implement default resources list. These are always loaded regardless of scene.

- [ ] Design a scene file format in JSON

- [ ] Add scene manager for loading scene from file

- [ ] Use a configuration file to determine which scene to load

------------------------------------------
Toolchain

- [ ] Scene export script for Blender: world, env probes, objects, etc..

- [ ] Separate lightmap data from normal material information to allow material reuse

- [ ] Script for merging material files

- [ ] Move irradiance map generator to separate DLL
      NOTE: This allows use from python script

- [ ] Move conversion of CME mesh format to OpenGL suitable format to separate DLL
      NOTE: This allows use from python script

------------------------------------------
Debug

- [ ] Add timing to all loaders

------------------------------------------
Bug fixes

- [ ] Envmap renders incorrectly on laptop when using Intel(R) HD Graphics 4000. GPU driver difference?
      NOTE: this might be because layout modifier for vertex attribute and fragment output isn't supported.

- [ ] Envmap manager fails to load envmap when light-probe.fs has specularEnv or diffuseColor zeroed.
      NOTE: this is caused by compiler optimizing them away. It doesn't cause the program to crash, but when function result
            is checked, it gives an error which causes Envmap manager to fail

- [ ] Envmap produces odd results when viewing ceiling through a mirror rotated around Y-axis (vertical)

------------------------------------------
Project structure

- [ ] Integrate dependencies ddsplugin and libupng to project

- [ ] Move blender scripts to "./scripts/blender" instead of assets/mesh. Differentiate by supported Blender version

- [ ] Add CMake support

- [ ] Check all dependecies and remove or replace 
      those that aren't compatible with MIT license

------------------------------------------
Entities

- [ ] Should components provide their dependencies so that entity type's required
      components list can be generated?

- [ ] Entity type registry. From file and from code. 
      NOTE: For ensuring that dependencies get loaded and entities can be copied.

- [ ] Entity instantiation from code with JSON and STL containers.

------------------------------------------
Environment maps

- [ ] Ensure that same env map can't be loaded multiple times

- [ ] Use all available mip-map levels instead of only the first

- [ ] Different cube map generation point and parallax shape center. Right now they are treated as equal.

- [ ] Oriented parallax shape option. This just requires that fragment world positions is further rotated into boundary
      shape's body frame

- [ ] Spherical parallax shape

- [ ] Add area of effect boundary (in addition to parallax correction shape)

- [ ] Optionally draw env map's effect boundary instead of full screen quad
      NOTE: might also need negative of this to indicate areas where envmap should not render

- [ ] Add debug options to env probe shader, such as "diffuse only", "specular only"

- [ ] Prevent lightmap-lit surfaces from being doubly lit by a diffuse environment map generated from the same scene.
      Stencil buffer or extra g-buffer fields?

- [ ] Parallax correction for irradiance map?

------------------------------------------
Light sources

- [ ] Deferred shading: draw light's bounding box instead of full screen quad

- [ ] Add physically based light sources (take shape into account etc.)


------------------------------------------
Skybox

- [ ] Skybox rendering has different shader, so it can't be in env map manager. 
      NOTE: Env map manager can handle the environment part of the skybox

------------------------------------------
2d overlay

- [ ] Add 2d overlay

- [ ] FreeType support for text printing

------------------------------------------
Texture mapping

- [ ] Parallax mapping

------------------------------------------
Post effects 

- [ ] Adaptive brightness (human eye)

- [ ] Bloom

- [ ] SSAO

- [ ] Screen space reflections (dynamic objects only?)

- [ ] Filmic tone mapping? (uncharted style?)

- [ ] Screen space anti-alias

- [ ] Atmospheric (scattering?) blue tint for distant objects

------------------------------------------
Game logic

- [ ] Script language integration (Python or Lua?)


