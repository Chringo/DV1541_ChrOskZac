bl_info = {
"name": "Export lights",
'author': '',
"version" : (1,1,0),
"location": "Import-Export",
"description": "",
"category": "Import-Export"
}

import bpy
import mathutils
import os.path
import struct
import math

from bpy.props import *
from bpy_extras.io_utils import ExportHelper, ImportHelper

def uint(i):
    return struct.pack('I',i)

def float32bit(i):
    return struct.pack("f",i)

def floatArr(file, indata):
    for i in indata:
        file.write(float32bit(i))

class ExportLight(bpy.types.Operator, ExportHelper):
    '''Export Light (.light)'''
    bl_idname = 'export_scene.lights'
    bl_label = 'Export light'
    bl_options = {'UNDO'}

    filename_ext = '.light'
    filter_glob = StringProperty(default='*.light', options={'HIDDEN'})

    def execute(self, context):
        scene = context.scene
        print('Exporting file', self.filepath)
        lightIndex = 0
        file = open(self.filepath, "wb")
        file.write(uint(len(bpy.data.objects)))
        for obj in bpy.data.objects:
            if obj.type == 'LAMP':
                #file.write(uint(lightIndex))
                #floatArr(file, obj.location)
                file.write(float32bit(obj.location[0]))
                file.write(float32bit(obj.location[2]))
                file.write(float32bit(-obj.location[1]))
                file.write(float32bit(0))
                floatArr(file, obj.data.color)
                #file.write(float32bit(0))
                file.write(float32bit(obj.data.distance))
                #floatArr(file, {0, 0, 0})
                lightIndex += 1
                print('Light')
        
        file.close()
        print("Done exporting light")
        return {'FINISHED'}


## register the operator to call the export script
def menu_func_exporter(self, context):
    self.layout.operator(ExportLight.bl_idname,text="Lights (.light)")
        
## the register function register all the UI elemets
def register():
    bpy.utils.register_module(__name__)
    
    ##bpy.types.INFO_MT_file_import.append(menu_func_importer)
    bpy.types.INFO_MT_file_export.append(menu_func_exporter)

## if we disable the script we unregister
def unregister():
    bpy.utils.unregister_module(__name__)
    
    ##bpy.types.INFO_MT_file_import.remove(menu_func_importer)
    bpy.types.INFO_MT_file_export.remove(menu_func_exporter)

if __name__ == "__main__":
    register()
