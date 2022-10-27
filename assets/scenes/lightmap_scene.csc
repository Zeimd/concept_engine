{
	"static_geometry" :
	{
		"world" :
		{
			"mesh" :
			{
				"file" : "lightmap-test.cme",
				"name" : "Room"
			}
		},
		"EnvProbes" :
		{
			"EnvProbe_1" :
			{
				"envMap" : "EnvProbe_1_0.exr",
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
			"envMap" : "envmap.bmp"
			"irradianceMap" : null
		}
	}
	"objects" :
	{
		"Cube" :
		{
			"mesh" :
			{
				"meshFile" : "cube.cme",
				"meshName" : "Cube"
			}
			"position" : [0.0, 1.0, 0.0],
			"rotationType" : "EULER_XYZ",
			"rotation" : [0.0, 0.0, 0.0]
		}
	}
}