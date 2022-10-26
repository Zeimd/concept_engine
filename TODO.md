concept_engine

TODO
------------------------------------------
Automation

- [ ] Design a scene file format in JSON

- [ ] Add scene manager for loading scene from file

- [ ] Use a configuration file to determine which scene to load

- [ ] Scene export script for Blender: world, env probes, objects, etc..

------------------------------------------
Bug fixes

- [ ] Envmap renders incorrectly on laptop. GPU driver difference?

- [ ] Envmap manager fails to load envmap when light-probe.fs has specularEnv or diffuseColor zeroed.
      NOTE: this is caused by compiler optimizing them away. It doesn't cause the program to crash, but when function result
            is checked, it gives an error which causes Envmap manager to fail

- [ ] Prevent lightmap-lit surfaces from being doubly lit by a diffuse environment map generated from the same scene.
      Stencil buffer or extra g-buffer fields?

------------------------------------------
Project structure

- [ ] Integrate dependencies ddsplugin and libupng to project

- [ ] Move blender scripts to "./scripts/blender" instead of assets/mesh. Differentiate by supported Blender version

- [ ] Add CMake support

- [ ] Check all dependecies and remove or replace 
      those that aren't compatible with MIT license

------------------------------------------
Environment maps

- [ ] Different cube map generation point and boundary shape center. Right now they are treated as equal.

- [ ] Oriented boundary shape option. This just requires that fragment world positions is further rotated into boundary
      shape's body frame

- [ ] Spherical boundary shape

------------------------------------------
Skybox

- [ ] Skybox rendering has different shader, so it can't be in env map manager. 
      NOTE: Env map manager can handle the environment part of the skybox

------------------------------------------
Graphics features

- [ ] Add area of effect boundary for environment maps (in addition to parallax correction boundary)

- [ ] Add light volumes to deferred lighting pass

- [ ] Add physically based light sources (take shape into account etc.)

- [ ] Add FreeType support for text printing

- [ ] Post effect: bloom

- [ ] Post effect: SSAO

- [ ] Post effect: Screen space reflections (dynamic objects only?)

- [ ] Post effect: Filmic tone mapping? (uncharted style?)

- [ ] Post effect: Screen space anti-alias

- [ ] Parallax mapping


------------------------------------------
Game logic

- [ ] Script language integration (Python or Lua?)


