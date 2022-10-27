{
	"static_geometry" :
	{
		"mesh" :
		{
			"fileName" : "lightmap-test.cme",
			"meshName" : "Room"
		},
		"envProbe" :
		{
			"EnvProbe_1" :
			{
				"envMap" : "EnvProbe_1.exr",
				"irradianceMap" : null,
				"samplePos" : [0.0, 1.5, 0.0],
				"parallaxCorrection"
				{
					"shape" : "box"
					"centerPos" : [0.0, 1.5, 0.0],
					"sideHalf" : [4.0, 2.5, 4.0]
				}
				"effectiveArea" :
				{
					"shape" : "box"
					"centerPos" : [0.0, 1.5, 0.0],
					"sideHalf" : [4.0, 2.5, 4.0]
				}
			}
		},
		"skybox" :
		{
			"default" : "normal",
			"normal" :
			{
				"envMap" : "envmap.bmp",
				"irradianceMap" : null
			}
		}
	}
	"entityList" :
	{
		"Cube" :
		{
			"type" :
			{
				"file" : "cube.cen",
				"name" : "Cube"
			},

			"components" :
			[
				"Position3" : [0.0, 1.0, 0.0],
				"Rotation3" : ["EULER_XYZ", [0.0, 0.0, 0.0]]
			]
		}
	}
}