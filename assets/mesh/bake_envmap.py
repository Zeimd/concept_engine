# -*- coding: utf-8 -*-

import bpy
import bmesh
import os
import subprocess
import ctypes
import math

from shutil import copyfile

from rna_xml import rna2xml

file_name = bpy.path.basename(bpy.context.blend_data.filepath)

end = file_name.rfind(".")

#dist_path =  file_name[:end] +"-dist\\"
dist_path = ".\\"

dist_path_abs = bpy.path.abspath("//") + dist_path

print("dist_path = ",dist_path_abs)
print("dist_rel = ",dist_path)

material_file_path = bpy.path.abspath("//"+file_name[:end]+".cma")

mesh_file_path = bpy.path.abspath("//"+file_name[:end]+".cme")

dist_tex_path_abs = dist_path_abs + "..\\textures\\"

dist_tex_path = dist_path + "..\\textures\\"
#dist_tex_path = ""

#dist_tex_path_abs = dist_path_abs + "textures\\"

#dist_tex_path = dist_path + "textures\\"

print("dist_tex_path = ",dist_tex_path_abs)
print("dist_tex_rel = ",dist_tex_path)

if not os.path.exists(dist_path_abs):
    os.makedirs(dist_path_abs)
    
if not os.path.exists(dist_tex_path_abs):
    os.makedirs(dist_tex_path_abs)

faceCameras = ( (math.radians(90.0),math.radians(0.0),math.radians(-90.0)), (math.radians(90.0),math.radians(0.0),math.radians(90.0))\
    ,(math.radians(180.0),math.radians(0.0),math.radians(0.0)), (math.radians(0.0),math.radians(0.0),math.radians(0.0)),\
        (math.radians(-90.0),math.radians(0.0),math.radians(180.0)), (math.radians(90.0),math.radians(0.0),math.radians(0.0)) )

faceEulerMode = ('XYZ', 'XYZ', 'XYZ', 'XYZ', 'ZXY', 'XYZ')

def Render(camera, cubeSide, outputName):

    # store current scene data

    scene = bpy.context.scene

    print("scene = " + scene.name)

    # TODO: store old values and restore on exit

    oldCamera = scene.camera

    scene.camera = camera

    bpy.context.scene.render.filepath = "//render_out/"+outputName

    bpy.context.scene.render.resolution_percentage = 100
    bpy.context.scene.render.resolution_x = int(cubeSide)
    bpy.context.scene.render.resolution_y = int(cubeSide)

    image_settings = bpy.context.scene.render.image_settings

    image_settings.file_format = "OPEN_EXR"
    image_settings.exr_codec = "ZIP"
    image_settings.color_mode = "RGB"
    image_settings.color_depth = "16"

    #image_settings.file_format = "PNG"
    #image_settings.color_mode = "RGB"
    #image_settings.color_depth = "8"

    scene.update()

    #bpy.ops.render.view_show()

    bpy.ops.render.render(animation=False, write_still=True, use_viewport=False,scene=scene.name)

    #bpy.ops.render.view_cancel()

    scene.camera = oldCamera
    scene.update()

def BakeEnvProbe(probe):

    print("***** BakeEnvProbe: "+probe.name)

    print("Render cameras:")

    camera_order = ["+X_face0","-X_face1","+Y_face2","-Y_face3","+Z_face4","-Z_face5"]

    if ("cubeSide" not in probe):
        print("ERROR: " + probe.name + ": " + "Missing property: "+"cubeSide")
        return

    cubeSide = probe["cubeSide"]

    print("Cubemap size = " + str(cubeSide))

    camData = bpy.data.cameras.new(name="TempEnvProbeCamera")
    
    camData.angle = math.radians(90.0)

    tempCamera = bpy.data.objects.new(name="TempEnvProbeCamera",object_data=camData)

    tempCamera.location = probe.location

    for index in range(0,6):

        tempCamera.rotation_mode = faceEulerMode[index]

        tempCamera.rotation_euler = faceCameras[index]

        outputName = probe.name + "_" + str(index);

        print(outputName)

        Render(tempCamera, cubeSide, outputName)

    tempCamera.parent = None

    bpy.data.objects.remove(tempCamera)

print("**** object names:")

print("**** env probes list:")

for key,obj in bpy.data.objects.items():

    if "IsEnvProbe" in obj and obj["IsEnvProbe"] == 1.0:
        #print(obj.name)

        BakeEnvProbe(obj)
