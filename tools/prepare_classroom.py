"""Prepare the EB408 furniture in Blender, preserving the original course models.

blender --background --factory-startup --disable-autoexec --python tools/prepare_classroom.py
Run generate_whiteboard_texture.py first. Application.cpp loads Furniture.gltf.
Blender uses Z up; the glTF exporter converts it to OpenGL Y up.
"""

from pathlib import Path
import math
import bpy
from mathutils import Matrix, Vector

ROOT = Path(__file__).resolve().parents[1]
MODELS = ROOT / 'project/Models'
FLOOR = 0.0054605


def material(name, image_path):
    result = bpy.data.materials.new(name)
    result.use_nodes = True
    shader = result.node_tree.nodes.get('Principled BSDF')
    shader.inputs['Metallic'].default_value = 0
    shader.inputs['Roughness'].default_value = .75
    texture = result.node_tree.nodes.new('ShaderNodeTexImage')
    texture.image = bpy.data.images.load(str(image_path), check_existing=True)
    result.node_tree.links.new(texture.outputs['Color'], shader.inputs['Base Color'])
    return result


def append_mesh(folder, filename, object_name, texture_name):
    source = MODELS / folder / filename
    with bpy.data.libraries.load(str(source), link=False) as (available, loaded):
        if object_name not in available.objects:
            raise RuntimeError(f'{source}: missing {object_name}')
        loaded.objects = [object_name]
    obj = loaded.objects[0]
    bpy.context.collection.objects.link(obj)
    bpy.context.view_layer.update()
    # Bake the source transform, including the door's original centimetre scale.
    obj.data = obj.data.copy()
    obj.data.transform(obj.matrix_world)
    obj.matrix_world = Matrix.Identity(4)
    obj.data.update()
    bpy.context.view_layer.update()
    obj.data.materials.clear()
    obj.data.materials.append(material(folder, MODELS/folder/texture_name))
    return obj


def bounds(obj):
    corners = [obj.matrix_world @ Vector(corner) for corner in obj.bound_box]
    return (Vector(tuple(min(v[i] for v in corners) for i in range(3))),
            Vector(tuple(max(v[i] for v in corners) for i in range(3))))


def center_on_floor(obj):
    low, high = bounds(obj)
    obj.data.transform(Matrix.Translation((-(low.x+high.x)/2, -(low.y+high.y)/2, -low.z)))


def duplicate(source, name, x, z, rotation=0):
    obj = source.copy()
    obj.name = name
    bpy.context.collection.objects.link(obj)
    obj.location = (x, -z, FLOOR)
    obj.rotation_euler.z = rotation
    return obj


def create_board_content(board):
    # The board faces +X. Its right edge is +Y when viewed from the room.
    vertices = [(.057, -1.065, 1.115), (.057, 1.025, 1.115),
                (.057, 1.025, 2.185), (.057, -1.065, 2.185)]
    mesh = bpy.data.meshes.new('BoardContent')
    mesh.from_pydata(vertices, [], [(0, 1, 2, 3)])
    mesh.uv_layers.new(name='UVMap')
    for loop, uv in zip(mesh.uv_layers.active.data, [(0, 0), (1, 0), (1, 1), (0, 1)]):
        loop.uv = uv
    mesh.materials.append(material('Lecture7_slide36', MODELS/'whiteboard/Lecture7_slide36.png'))
    content = bpy.data.objects.new('Whiteboard_Lecture7_slide36', mesh)
    bpy.context.collection.objects.link(content)
    content.location = board.location.copy()


def main():
    bpy.ops.object.select_all(action='SELECT')
    bpy.ops.object.delete(use_global=False)
    table = append_mesh('table', 'table.blend', 'SM_Table', 'T_Table_BaseColor.png')
    chair = append_mesh('chair', 'OrangeChair.blend', 'OrangeChair', 'T_OrangeChair_BaseColor.png')
    center_on_floor(table)
    center_on_floor(chair)
    # Three rows, each with three two-seat desks. The aisle by the board stays clear.
    for row, x in enumerate([-.95, .65, 2.25], start=1):
        for column, z in enumerate([1.65, 3.48, 5.31], start=1):
            duplicate(table, f'Desk_{row}_{column}', x, z, math.pi/2)
            for seat, dz in enumerate([-.44, .44], start=1):
                duplicate(chair, f'Chair_{row}_{column}_{seat}', x+.68, z+dz, -math.pi/2)
    # Rotate the desk 90 degrees; the teacher sits with the side wall behind them.
    duplicate(table, 'TeacherDesk', -2.30, 1.10, math.pi/2)
    duplicate(chair, 'TeacherChair', -3.00, 1.10, math.pi/2)
    bpy.data.objects.remove(table, do_unlink=True)
    bpy.data.objects.remove(chair, do_unlink=True)

    board = append_mesh('whiteboard', 'Whiteboard.blend', 'SM_Whiteboard', 'T_Whiteboard_BaseColor.png')
    board.name = 'Whiteboard'
    board.location = (-3.72, -2.80, FLOOR)
    create_board_content(board)

    # The classroom already contains a frame; add only the leaf from Door.blend.
    door = append_mesh('door', 'Door.blend', 'SM_Door', 'T_door_basecolor.png')
    door.name = 'Door_in_existing_frame'
    low, high = bounds(door)
    size = high-low
    door.data.transform(Matrix.Translation(-low))
    door.data.transform(Matrix.Diagonal((1.045/size.x, 1, 2.34/size.z, 1)))
    door.location = (-3.480, -7.650, FLOOR+.015)

    # Textured Shrek figurine under the teacher's desk.
    bpy.ops.wm.obj_import(filepath=str(MODELS/'shrek/shrek.obj'), forward_axis='NEGATIVE_Z', up_axis='Y')
    shrek = next(obj for obj in bpy.context.selected_objects if obj.type == 'MESH')
    shrek.name = 'Shrek_under_teacher_desk'
    shrek.location = (-2.23, -1.10, FLOOR+.001)
    shrek.scale = (.28, .28, .28)
    shrek.rotation_euler.z = math.pi/2
    shrek.data.materials.clear()
    shrek.data.materials.append(material('Shrek', MODELS/'shrek/shrek.png'))

    # Export the furnishings as one multi-material asset; no new scene class is needed.
    bpy.ops.object.select_all(action='SELECT')
    output = MODELS/'EB408/Furniture.gltf'
    bpy.ops.export_scene.gltf(filepath=str(output), export_format='GLTF_SEPARATE',
        use_selection=True, export_yup=True, export_apply=True, export_keep_originals=True,
        export_animations=False, export_cameras=False, export_lights=False)

    # Also keep a complete editable Blender scene for checking placements.
    bpy.ops.import_scene.gltf(filepath=str(MODELS/'EB408/EB408.gltf'))
    bpy.ops.file.pack_all()
    bpy.context.preferences.filepaths.save_version = 0
    bpy.ops.wm.save_as_mainfile(filepath=str(MODELS/'EB408/EB408_furnished.blend'))
    print(f'Prepared 10 desks, 19 chairs, a door, a board and Shrek: {output}')


if __name__ == '__main__':
    main()
