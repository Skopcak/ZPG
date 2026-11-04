"""Export a Blender scene to position/normal/UV triangle data for ZPG.

Run with Blender in background mode:
blender --background --disable-autoexec --python this_script.py -- input.blend output.h door
The image texture remains a separate PNG loaded by the application.
"""

import math
from pathlib import Path
import re
import sys

import bpy
from mathutils import Vector


def to_opengl(vector):
    """Convert Blender's Z-up coordinates to OpenGL Y-up coordinates."""
    return Vector((vector.x, vector.z, -vector.y))


def export_textured_model(source, destination, name):
    if not re.fullmatch(r'[A-Za-z_][A-Za-z0-9_]*', name):
        raise ValueError('The model name must be a C++ identifier.')
    bpy.ops.wm.open_mainfile(filepath=str(source))
    objects = sorted(
        (obj for obj in bpy.context.scene.objects if obj.type == 'MESH' and not obj.hide_render),
        key=lambda obj: obj.name,
    )
    if not objects:
        raise RuntimeError('The scene contains no visible mesh objects.')

    vertices = []
    graph = bpy.context.evaluated_depsgraph_get()
    for obj in objects:
        evaluated = obj.evaluated_get(graph)
        mesh = evaluated.to_mesh()
        try:
            mesh.calc_loop_triangles()
            uv_layer = mesh.uv_layers.active
            if uv_layer is None:
                raise RuntimeError(f'Mesh {obj.name} has no UV coordinates.')
            world = evaluated.matrix_world.copy()
            linear = world.to_3x3()
            determinant = linear.determinant()
            if abs(determinant) < 1e-12:
                raise RuntimeError(f'Mesh {obj.name} has a singular transform.')
            normal_matrix = linear.inverted().transposed()

            for triangle in mesh.loop_triangles:
                if triangle.area <= 0:
                    continue
                loops = tuple(triangle.loops)
                if determinant < 0:
                    loops = tuple(reversed(loops))
                for loop_index in loops:
                    loop = mesh.loops[loop_index]
                    position = to_opengl(world @ mesh.vertices[loop.vertex_index].co)
                    normal = to_opengl(normal_matrix @ mesh.corner_normals[loop_index].vector)
                    if normal.length_squared == 0:
                        normal = to_opengl(normal_matrix @ triangle.normal)
                    if normal.length_squared == 0:
                        raise RuntimeError(f'Mesh {obj.name} has an invalid normal.')
                    normal.normalize()
                    uv = tuple(uv_layer.data[loop_index].uv)
                    if not all(math.isfinite(v) for v in (*position, *normal, *uv)):
                        raise RuntimeError(f'Mesh {obj.name} contains non-finite vertex data.')
                    vertices.append((position, normal, uv))
        finally:
            evaluated.to_mesh_clear()

    if not vertices:
        raise RuntimeError('The scene contains no valid triangles.')
    minimum = Vector(tuple(min(vertex[0][axis] for vertex in vertices) for axis in range(3)))
    maximum = Vector(tuple(max(vertex[0][axis] for vertex in vertices) for axis in range(3)))
    center = (minimum + maximum) * 0.5
    scale = 2.0 / max(maximum-minimum)

    destination = Path(destination)
    destination.parent.mkdir(parents=True, exist_ok=True)
    with destination.open('w', encoding='utf-8', newline='\n') as stream:
        stream.write(
            '#pragma once\n\n'
            f'// Source: course model {Path(source).name}.\n'
            f'// Meshes: {", ".join(obj.name for obj in objects)}.\n'
            '// Exported by tools/export_textured_model_to_header.py.\n'
            '// Layout: position XYZ, normal XYZ, UV.\n'
            '// Centered, Y-up; maximum dimension is 2 units.\n\n'
            f'inline constexpr int {name}VertexCount = {len(vertices)};\n\n'
            f'inline constexpr float {name}Vertices[] = {{\n'
        )
        for position, normal, uv in vertices:
            values = (*(scale*(position-center)), *normal, *uv)
            stream.write('    ' + ', '.join(f'{value:.8f}f' for value in values) + ',\n')
        stream.write('};\n')
    print(f'Exported {len(objects)} meshes, {len(vertices)//3} triangles: {destination}')


if __name__ == '__main__':
    arguments = sys.argv[sys.argv.index('--')+1:]
    if len(arguments) != 3:
        raise SystemExit('Expected: input.blend output.h modelName')
    export_textured_model(Path(arguments[0]), Path(arguments[1]), arguments[2])
