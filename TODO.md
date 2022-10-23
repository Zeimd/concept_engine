TODO
------------------------------------------
Automation

- [ ] Add environment map manager to automatize their rendering

- [ ] Design a scene file format in JSON

- [ ] Add scene manager for loading scene from file

- [ ] Use a configuration file to determine which scene to load

- [ ] Scene export script for Blender: world, env probes, objects, etc..

------------------------------------------
Bug fixes

- [ ] Envmap manager fails to load envmap when light-probe.fs has specularEnv or diffuseColor zeroed.

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


