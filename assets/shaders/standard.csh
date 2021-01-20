# Concept Engine shader file

shader Standard
vs_forward null
fs_forward null
vs_deferred gbuffer.vs
fs_deferred gbuffer.fs

param diffuse_color
type tex2d
uniform diffuseTex

param normal_map
type null
fs_flag NO_NORMAL_MAP
type tex2d
uniform normalMap

param texture_tiling
uniform uvScale
type float2
default 1.0 1.0

param texture_offset
type float2
uniform uvOffset
default 0.0 0.0

param detail_normalmap
uniform detailNormalMap
type tex2d
default null

param detail_scale
uniform detailScale

type float
default 1.0
min 0.0
max 1.0

uniform materialTex
type tex2d

type float2
switch MATERIAL_UNIFORM
default 0.0 1.0
min 0.02 0.02
max 0.99 0.99

