# -*- coding: utf-8 -*-

import bpy
import bmesh
import os
import subprocess
import ctypes


DDS_COMPRESS_NONE = 0
DDS_COMPRESS_BC1 = 1        # DXT1  
DDS_COMPRESS_BC2 = 2        # DXT3  
DDS_COMPRESS_BC3 = 3        	 # DXT5  
DDS_COMPRESS_BC3N = 4       	 # DXT5n 
DDS_COMPRESS_BC4 = 5        	 # ATI1  
DDS_COMPRESS_BC5 = 6        	 # ATI2 
DDS_COMPRESS_RXGB = 7       	 # DXT5

DDS_MIPMAP_FILTER_DEFAULT = 0
DDS_MIPMAP_FILTER_NEAREST = 1
DDS_MIPMAP_FILTER_BOX = 2
DDS_MIPMAP_FILTER_TRIANGLE = 3
DDS_MIPMAP_FILTER_QUADRATIC = 4
DDS_MIPMAP_FILTER_BSPLINE = 5
DDS_MIPMAP_FILTER_MITCHELL = 6
DDS_MIPMAP_FILTER_LANCZOS = 7
DDS_MIPMAP_FILTER_KAISER = 8

DDS_MIPMAP_WRAP_DEFAULT = 0
DDS_MIPMAP_WRAP_MIRROR = 1
DDS_MIPMAP_WRAP_REPEAT = 2
DDS_MIPMAP_WRAP_CLAMP = 3

#load dependencies although they won't be explicitly called
upng_plugin_path = bpy.path.abspath("//")+"libupng64.dll"
upng_plugin = ctypes.CDLL(upng_plugin_path)

dds_plugin_path = bpy.path.abspath("//")+"ddsplugin64.dll"
dds_plugin = ctypes.CDLL(dds_plugin_path)

from shutil import copyfile

from rna_xml import rna2xml

file_name = bpy.path.basename(bpy.context.blend_data.filepath)

end = file_name.rfind(".")

#dist_path =  file_name[:end] +"-dist\\"
dist_path = ".\\"

dist_path_abs = bpy.path.abspath("//") + dist_path

#dist_path = ".\\" + dist_path

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


call_Compress_Texture2D_DXT = dds_plugin.Compress_Texture2D_DXT
call_Compress_Texture2D_DXT.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_int,ctypes.c_int,ctypes.c_int,ctypes.c_int]
call_Compress_Texture2D_DXT.restype = ctypes.c_int

class Vertex:
    def __init__(self):
        self.position = -1
        self.normal = -1
        self.tex_uv = -1
        self.lightmap_uv = -1

class Face:
    def __init__(self):
        self.vertex = []

def GetMaterialData(object,material_groups,materials,textures,material_textures):
    
    for index in material_groups.keys():
        
        slot = object.material_slots[index]
    
        material = slot.material
    
        if material.use_nodes == True:
                       
            nodes = material.node_tree.nodes
                             
            for node in nodes:
                if node.bl_idname == "ShaderNodeGroup":
                    #out.write("node group\n")
                    
                    node_tree = getattr(node,'node_tree')
                    
                    shader_name = getattr(node_tree,'name')
                    
                    if shader_name == "Standard shader" or shader_name == "Standard dielectric" or shader_name == "Standard metallic":
                        materials.items[material.name] = json_material(material.name,node,nodes,shader_name,textures,material_textures)
                                        
            #rna2xml(fw=material_file_handle.write,root_node=key,root_rna=material.node_tree)    

def WritePosition(positions,file_handle):
    for vertex in positions:
       # convert to OpenGL coordinate system
       
       x = round(vertex.co[0],7)
       y = round(vertex.co[2],7)
       z = -round(vertex.co[1],7)
        
       file_handle.write("v "+str(x)+" "+str(y)+" "+str(z)+"\n")

def WriteNormal(normals,file_handle):
    for normal in normals:
       # convert to OpenGL coordinate system
       
       x = round(normal[0],7)
       y = round(normal[2],7)
       z = -round(normal[1],7)
        
       file_handle.write("vn "+str(x)+" "+str(y)+" "+str(z)+"\n")
       
def WriteUV(coords,file_handle):
    for uv in coords:
        
        u = round(uv[0],5)
        v = round(1-uv[1],5)
        
        file_handle.write("vt "+str(u)+" "+str(v)+"\n")
       
def MapPosition(mesh,loop_index,position_map,pos_index,vertex_ref):
   
    vertex_index = mesh.loops[loop_index].vertex_index
            
    vertex = mesh.vertices[vertex_index]

    key = vertex_index

    unique_index = position_map.get(key,None)

    if unique_index is None:
        unique_count = len(pos_index)
        position_map[key] = unique_count
        unique_index = unique_count
        
        pos_index.append(vertex)

    vertex_ref.position = unique_index
    
def MapNormal(mesh,loop_index,normal_map,normal_index,vertex_ref,face):
        
    #vertex_index = mesh.loops[loop_index].vertex_index
            
    #normal = mesh.vertices[vertex_index].normal
    
    normal = mesh.loops[loop_index].normal

    #if not face.use_smooth:
    #    normal = face.normal
        
    #key = vertex_index,face.use_smooth

    #unique_index = len(normal_index)
    #normal_index.append(normal)    
    
    key = round(normal[0],7),round(normal[1],7),round(normal[2],7)
    
    unique_index = normal_map.get(key,None)

    if unique_index is None:
        unique_count = len(normal_index)
        normal_map[key] = unique_count
        unique_index = unique_count

        normal_index.append(normal)

    vertex_ref.normal = unique_index
    
def MapUV(mesh,loop_index,uv_cache,uv_buffer,diffuse_uv_map,lightmap_uv_map,vertex_ref):
    
    for uv_map in mesh.uv_layers:
        
        if uv_map.name not in [diffuse_uv_map,lightmap_uv_map]:
            print("warning : unknown uv map : ",uv_map.name)
            continue
        
        uv = uv_map.data[loop_index].uv                
         
        #unique_index = len(uv_buffer)
        
        #uv_buffer.append(uv) 
        key = (loop_index,uv_map.name)
        
        #key = round(uv[0],5),round(uv[1],5)                
        unique_index = uv_cache.get(key,None)
        
        if unique_index is None:
            unique_count = len(uv_buffer)
            uv_cache[key] = unique_count
            unique_index = unique_count
            
            uv_buffer.append(uv)
                
        if uv_map.name == diffuse_uv_map:
            vertex_ref.tex_uv = unique_index
        elif uv_map.name == lightmap_uv_map:
            vertex_ref.lightmap_uv = unique_index
            
    
def ExportCME(file,object,mesh,materials,textures,material_textures):
    
    file_handle = open(file,"w")

    #create a temporary copy that can be safely triangularized    
    temp_mesh = bmesh.new()
    
    temp_mesh.from_mesh(mesh)
    
    bmesh.ops.triangulate(temp_mesh,faces=temp_mesh.faces[:],quad_method=0,
    ngon_method=0)
    
    triangle_mesh = bpy.data.meshes.new("__cme_export__")
    
    temp_mesh.to_mesh(triangle_mesh)
    temp_mesh.free()

    triangle_mesh.calc_normals_split()
    
    material_groups = {}
    
    # group polygons by material
    
    for polygon in triangle_mesh.polygons:
      
        dest_index = material_groups.get(polygon.material_index,None)
        
        if dest_index is None:
            material_groups[polygon.material_index] = [polygon]
        else:
            dest_index.append(polygon)

    # gather material data
    
    GetMaterialData(object,material_groups,materials,textures,material_textures)
    
    diffuse_uv_map = None
    lightmap_uv_map = None
    
    for material in materials.items.values():
        if material.diffuse_uv_map is not None:
            diffuse_uv_map = material.diffuse_uv_map
        
        if material.lightmap_uv_map is not None:
            lightmap_uv_map = material.lightmap_uv_map
           
    print("diffuse uv map : ",diffuse_uv_map)
    print("lightmap uv map ; ",lightmap_uv_map)

    file_handle.write("#CME = Concept Engine Mesh\n")
    #file_handle.write("mtllib " + 
    file_handle.write("o " + mesh.name + "\n")
    
    position_cache = {}
    
    #index of position in the file
    position_index = []

    normal_cache = {}
    
    #index of normal in the file
    normal_index = []
    
    uv_cache = {}
    
    #index of uv in the file
    uv_index = []
    
    new_material_groups = {}
    
    print("mesh : ",mesh.name)
    print("loops : ",str(len(triangle_mesh.loops)))
    
    for material,faces in material_groups.items():
        
        new_material_groups[material] = []
        
        for face in faces:
            new_face = Face()
            
            for loop_index in face.loop_indices:
                
                vertex = Vertex()
                                
                MapPosition(triangle_mesh,loop_index,position_cache,position_index,vertex)
                MapNormal(triangle_mesh,loop_index,normal_cache,normal_index,vertex,face)
                MapUV(triangle_mesh,loop_index,uv_cache,uv_index,diffuse_uv_map,lightmap_uv_map,vertex)
                
                new_face.vertex.append(vertex)
                
            new_material_groups[material].append(new_face)
                
    del position_cache
    del normal_cache
    del uv_cache 
    
    print("original faces : ",len(mesh.polygons))
    print("triangulated faces : ",len(triangle_mesh.polygons))
    
    print("total vertices : ",len(mesh.vertices))
    print("total normals : ",len(triangle_mesh.loops))
    
    print("unique positions : ",len(position_index))
    print("unique normals : ",len(normal_index))
    print("unique uvs : ",len(uv_index))
 
    material_groups = new_material_groups
    
    WritePosition(position_index,file_handle)
    WriteNormal(normal_index,file_handle)
    WriteUV(uv_index,file_handle)
    
    del position_index
    del normal_index
    del uv_index
    
    for material,faces in material_groups.items():
        slot = object.material_slots[material]
            
        material_ref = slot.material

        file_handle.write("usemtl " + material_ref.name.replace(" ","_") + "\n")
        for face in faces:
           file_handle.write("f ")
           for vertex in face.vertex:                
               pos = vertex.position+1
               norm = vertex.normal+1
               
               uv = vertex.tex_uv+1
               lmap_uv = vertex.lightmap_uv+1
               
               file_handle.write(str(pos)+"/")
               
               if uv > 0:
                   file_handle.write(str(uv))
               
               if lmap_uv > 0:
                   if uv > 0:
                       file_handle.write("/")
                   file_handle.write(str(lmap_uv))

               file_handle.write("/")
                                  
               file_handle.write(str(norm) + " ")               
                
           file_handle.write("\n")
           
    bpy.data.meshes.remove(triangle_mesh)
    file_handle.close()
    print("mesh exported")

def Is_Newer(sourceFile,destFile):
   try:
       sourceTime = os.path.getmtime(sourceFile)
   except FileNotFoundError as f:
       return False
    
   try:
       destTime = os.path.getmtime(destFile)
   except FileNotFoundError as f:
       destTime = 0.0
    
   if (destTime < sourceTime):
       return True

   return False

def Compress_Texture2D_DXT(sourceFile,destFile,compression=None,normalMap=0,
                            mipmapFilter=DDS_MIPMAP_FILTER_DEFAULT,mipmapWrap=DDS_MIPMAP_WRAP_DEFAULT):
    #return     
    dest_path = destFile[:destFile.rfind('.')] + '.dds'   
    
    if not Is_Newer(sourceFile,dest_path):
        print("texture compress : ",dest_path, " is up to date")
        return
    
    print("compress texture")
    print("source = " , sourceFile)
    print("dest = " , dest_path)
    c_sourcefile = ctypes.c_char_p(sourceFile.encode())
    c_destfile = ctypes.c_char_p(dest_path.encode())
    
    result =call_Compress_Texture2D_DXT(c_sourcefile,c_destfile,ctypes.c_int(compression),ctypes.c_int(normalMap),ctypes.c_int(mipmapFilter),ctypes.c_int(mipmapWrap))
    
    print("result = " ,result)



def json_value(data):    
    if isinstance(data,str):
        return "\""+data+"\""
    elif isinstance(data,list):
        return json_list(data)
    elif data.__class__ == bool:
        if data:
            return "true"
        else:
            return "false"
    elif data is None:
        return "null"
    elif isinstance(data,(int,float)):
        return str(data)
   

def json_list(data): 
    if len(data) == 0:
        return "None"
    text = "[ "
    first = True
    
    for item in data:
        if not first:
            text = text + " , "
        text = text + json_value(item)
        first = False    

    text = text + " ]" 
    return text        

def json_pair(name,value):
    return json_value(name) + " : " + str(json_value(value))

class json_object:
    def __init__(self):
        pass
        
    def serialize(self,out,indent):
        pass
    
    def serialize_header(self,out,indent,title):
        indent_print(out,indent,quotes(title) + " : ")
        indent_print(out,indent,"{")
        
    def serialize_suffix(self,out,indent):
        indent_print(out,indent,"}")
        
class json_collection(json_object):
    
    def __init__(self,name):
        super(json_collection,self).__init__()
        
        self.name = name
        self.items = {}
        
    def serialize(self,out,indent):
        
        self.serialize_header(out,indent,self.name)
          
        first = True
        for name,texture in self.items.items():
            if not first:
                indent_print(out,indent+1,",")
                
            texture.serialize(out,indent+1)
            
            first = False
        
        self.serialize_suffix(out,indent)

class MaterialTexture:
    def __init__(self,name,path,metallic,roughness,wrapMode):
        self.name = name
        self.path = path
        
        self.metallic = metallic            
        self.roughness = roughness
        
        self.metallic_texture = False
        if isinstance(metallic,json_texture):
            self.metallic_texture = True

        self.roughness_texture = False
        if isinstance(roughness,json_texture):
            self.roughness_texture = True
            
        
            
        self.json_ref = json_texture(name,False,wrapMode,path,False,metallic.uv_map)
                    
    def make(self):
        pass
        
        
    
class json_texture(json_object):
   
    def __init__(self,name,sRGB,wrapMode,path,normalmap,uv_map):
        super(json_texture,self).__init__()
    
        self.uv_map = uv_map
        
        self.name = name
        self.sRGB = sRGB
        self.wrapMode = wrapMode
        self.normal_map = normalmap
        self.source_path = path
          
        start = path.rfind(".")
        
        extension = path[start+1:]
        
        #print("texture : extension : ",extension)
                
        if extension == "exr":
            print("exr mode : extension = ",extension)
            # Don't compress HDR images for now
            self.path = [name + path[start:]]    
            #self.path = [dist_tex_path + name + path[start:]]    
        else:
            print("default mode : extension = ",extension)

            self.path = [name + ".dds", name + path[start:]]
            #self.path = [dist_tex_path + name + ".dds", dist_tex_path + name + path[start:]]
    
    @staticmethod
    def get(node,normal_map):

        image = getattr(node,'image')

        path = bpy.path.abspath(image.filepath)
                
        end = image.name.rfind(".")
        name = image.name[0:end]
        
        #print ("texture : ",name)
        
        sRGB = True     
        if str(getattr(node,'color_space')) == 'NONE':
            sRGB = False
        
        wrapMode = "repeat"
        if node.extension == "CLIP":
            wrapMode = "clip"
            
        uv_map = None
            
        for input in node.inputs:
            if input.name == "Vector":
                for link in input.links:
                    if link.from_node.bl_idname == "ShaderNodeUVMap":
                        uv_map = link.from_node.uv_map
                        #print("uv map : ",uv_map,"\n")
                        
        uv_maps[uv_map] = True
                    
        #copy source texture to dist as backup if the file in dist is not up to date    
        start = path.rfind(".")  
        dest_path = bpy.path.abspath("//"+dist_tex_path + name + path[start:])
        
        if Is_Newer(path,dest_path):
            print("dist file out of date: ",dest_path)
            copyfile(path,dest_path)
                        
        json_texture.compress_dxt(path,dest_path,normal_map)
        
        return json_texture(name,sRGB,wrapMode,path,normal_map,uv_map)
    
    def serialize(self,out,indent):
        
        self.serialize_header(out,indent,self.name)
        
        indent_print(out,indent+1,json_pair("sRGB",self.sRGB) + ",")
        
        indent_print(out,indent+1,json_pair("wrapmode",self.wrapMode) + ",")

        if self.normal_map:
            
            indent_print(out,indent+1,json_pair("normalmap",self.normal_map) + ",")

        #indent_print(out,indent+1,json_pair("source_file",self.source_path) + ",")
            
        indent_print(out,indent+1,json_pair("file",self.path))
           
        self.serialize_suffix(out,indent)
        
    @staticmethod
    def compress_dxt(source,dest,normal_map):
        
        compression = DDS_COMPRESS_BC1
        if normal_map:
            #compression = DDS_COMPRESS_BC3N
            compression = DDS_COMPRESS_BC5
        
        Compress_Texture2D_DXT(source,dest,compression)
        
class json_material(json_object):
    
    def __init__(self,name,root_node,nodes,shader_name,textures,material_textures):
        super(json_material,self).__init__()
        
        self.shader = shader_name
        
        self.name = name.replace(" ","_")
        
        self.color = None
        self.normal = None
       
        self.dielectric_reflect = None
       
        self.roughness = None
        self.metallic = None
        
        if shader_name == "Standard metallic":
            self.metallic = 1.0
        
        self.material_tex = False
        self.material = None
        
        self.roughness_tex = False
        self.metallic_tex = False
        self.dielectric_tex = False
        
        self.lightmap = None
        
        self.diffuse_uv_map = None
        self.lightmap_uv_map = None
        
        self.vs_flags = []
        self.fs_flags = []
              
        inputs = getattr(root_node,'inputs')
        
        var = None
        
        #print ("processing material : ",name)
        
        for input in inputs:            
            #print("input : " + input.name + "\n")
            
            if input.name == "Color / Specular":
                var = 'color'
            elif input.name == "Normalmap":
                var = 'normal'
            elif input.name == "Roughness":
                var = 'roughness'
            elif input.name == 'Metallic':
                var = 'metallic'
            elif input.name == 'Lightmap':
                var = 'lightmap'
            elif input.name == "F0":
                var = 'metallic'
            else:
                print("unknown input : " + input.name)
                continue
                #var = 'dielectric_reflect'
                            
            if not input.is_linked:
                if var == 'color':
                    self.fs_flags.append("UNIFORM_COLOR")
                    color = input.default_value
                    self.color = [color[0],color[1],color[2]]
                if var == 'normal':
                    self.fs_flags.append("NO_NORMAL_MAP")               
                elif var == 'roughness':
                    self.roughness = input.default_value
                elif var == 'metallic':
                    self.metallic = input.default_value
                elif var == 'dielectric_reflect':
                    self.dielectric_reflect = input.default_value
        
            for link in input.links:
                             
                from_node = link.from_node
                from_socket = link.from_socket
                
                #print("from_node : " + str(from_node) + "\n")
                #print("from_socket : " + str(from_socket) + "\n")
                
                if from_node.bl_idname == "ShaderNodeTexImage":
                    normalmap = False
                    if var == 'normal':
                        normalmap = True
                    
                    texture = json_texture.get(from_node,normalmap)
                    setattr(self,var,texture) 
                    
                    if var == 'color' or var == 'normal' or var == 'lightmap':
                        textures.items[texture.name] = texture   
                    elif var == 'roughness':
                       self.roughness_tex = True
                    elif var == 'metallic':
                       self.metallic_tex = True      
                    elif var == 'dielectric_reflect':
                        self.dielectric_tex = True     

                    if var == 'lightmap':
                        self.fs_flags.append("LIGHTMAP_SCALAR")
                        self.lightmap_uv_map = texture.uv_map
                    else:
                        self.diffuse_uv_map = texture.uv_map                       


                    break
                elif from_node.bl_idname == "ShaderNodeSeparateRGB":
                    break
                elif from_node.bl_idname == "ShaderNodeRGB":
                   
                    color = from_node.color
                    setattr(self,var,[color.r,color.g,color.b])
                    
                    if var == 'color':
                        self.fs_flags.append("UNIFORM_COLOR")

                    break
                        
        if not (self.roughness_tex or self.metallic_tex or self.dielectric_tex):
            self.fs_flags.append("MATERIAL_UNIFORM") 
        else:
            self.fs_flags.append("MATERIAL_TEXTURE")
            
            materialTex = MaterialTexture(self.name+"-material",dist_tex_path+self.name+".dds",self.metallic,self.roughness,"repeat")
            self.material = materialTex.json_ref
            textures.items[materialTex.name] = self.material
            self.material_tex = True
            
                        
    def serialize(self,out,indent):
        self.serialize_header(out,indent,self.name)
        
        indent_print(out,indent+1,json_pair("vs","gbuffer.vs") + ",")
        indent_print(out,indent+1,json_pair("fs","gbuffer.fs") + ",")
        
        if self.vs_flags:
            indent_print(out,indent+1,json_pair("vs_flags",self.vs_flags) + ",")
       
        if len(self.fs_flags):
            indent_print(out,indent+1,json_pair("fs_flags",self.fs_flags) + ",")
        
        if isinstance(self.color,list):
            indent_print(out,indent+1,json_pair("diffuseColor",self.color) + ",")
        else:
            indent_print(out,indent+1,
            json_pair('diffuseTex',["tex2d",self.color.name]) + ",")
            
        if isinstance(self.lightmap,json_texture):
            indent_print(out,indent+1,
            json_pair('lightMap',["tex2d",self.lightmap.name]) + ",")
            
        if self.normal is not None:
            indent_print(out,indent+1,
            json_pair("normalMap",["tex2d",self.normal.name]) + ",")
    
        if self.material_tex:
            indent_print(out,indent+1,
            json_pair("materialTex",["tex2d",self.material.name]))   
        else:      
            if self.dielectric_reflect is not None:
                indent_print(out,indent+1,json_pair("dielectricReflect",self.dielectric_reflect) + ",")
            if self.roughness is not None:
                indent_print(out,indent+1,json_pair("materialRoughness",self.roughness) + ",")   
            if self.metallic is not None:
                indent_print(out,indent+1,json_pair("materialMetallic",self.metallic))   
          
        self.serialize_suffix(out,indent)   
        
    def serialize_flags(self,out,indent,name,flags):
        if len(flags) == 0:
            return
        text = "[ "
        first = True
        
        for flag in flags:
            if not first:
                text = text + " , "
            text = text + flag
            first = False    

        text = text + " ]"    
        indent_print(out,indent,quotes(name) + " : " + text + ",")
                          
def quotes(text):
    return "\"" + text + "\""

def indent_print(out,indent,text):
    padding = "\t" * indent
    
    out.write(padding + text + "\n")    
                
def print_node_info(out,nodes):    

    out.write("nodes class : " + str(nodes.__class__) + "\n")
        
    for node in nodes:           
        
        out.write("node :\n{\n")
        
        for attr in dir(node):
            if hasattr(node,attr):
                out.write("\t" + attr + " : " + str(getattr(node,attr)) + "\n")
            else:
                out.write("\t" + attr + "\n")
        
        out.write("}\n")

        tree = node.get('node_tree',None)
        if tree is not None:
            print_node_info(out,tree.nodes)
            
def list_collection_attribs(out,name,object):
    out.write(name + " : \n{\n")
    
    for param in dir(object):
        out.write("\t" + param + " : " + str(getattr(object,param)) + "\n")       
        
    out.write("}\n")
    
def list_object_attribs(out,name,object):
    out.write(name + " : \n{\n")
    
    for param,value in object.__dict__:
        out.write("\t" + param + " : " + str(value) + "\n")       
        
    out.write("}\n")
    
def print_mesh_data(out,mesh):
    out.write("mesh : " + mesh.name + "\n{\n")
    
    
    
    out.write("}\n")

material_file_handle = open(material_file_path,"w")

textures = json_collection("textures")

materials = json_collection("materials")

material_textures = []

uv_maps = {}

for key,object in bpy.data.objects.items():
         
    if (object.type == 'MESH'):
        print("mesh : ",key)
        
        mesh = object.data
                       
        ExportCME(mesh_file_path,object,mesh,materials,textures,material_textures)
        
indent_print(material_file_handle,0,"{")

textures.serialize(material_file_handle,1)   

indent_print(material_file_handle,1,",")

materials.serialize(material_file_handle,1)

indent_print(material_file_handle,0,"}")

material_file_handle.close()
        
print("material exported")
print("done")

