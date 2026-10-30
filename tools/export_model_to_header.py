"""Export selected Blender meshes to the ZPG position/color/normal layout.

Prepared with AI assistance. Run this file in Blender's Scripting workspace.
Supports opaque models with solid material colors. Image textures must be
baked to colors separately; they are not silently replaced by a flat color.
"""

from pathlib import Path
import math
import re

import bpy
from mathutils import Vector


# Change these settings before exporting each ship.
MODEL_NAME = "venator"
OUTPUT_DIRECTORY = Path(r"C:\Users\ASUS\source\repos\ZPG\project\Models")
MODEL_AUTHOR = "Digital Sock"
MODEL_SOURCE = "https://sketchfab.com/3d-models/venator-class-star-destroyer-af3648043a5b4fd394adfc32baee369e"
MODEL_LICENSE = "CC Attribution (see the original model page)"


def material_color(mesh, material_index):
    """Read the solid base color used by a triangle's material."""
    if material_index >= len(mesh.materials):
        return (0.8, 0.8, 0.8)

    material = mesh.materials[material_index]
    if material is None:
        return (0.8, 0.8, 0.8)

    color = tuple(material.diffuse_color)
    if material.node_tree is not None:
        shader = next(
            (node for node in material.node_tree.nodes
             if node.type == "BSDF_PRINCIPLED"),
            None,
        )
        if shader is None:
            raise RuntimeError(
                f"Material '{material.name}' needs a Principled BSDF shader."
            )

        base_color = shader.inputs["Base Color"]
        if base_color.is_linked:
            raise RuntimeError(
                f"Material '{material.name}' uses a texture or linked color. "
                "Bake or convert its colors before using this exporter."
            )
        if shader.inputs["Alpha"].is_linked or shader.inputs["Alpha"].default_value < 0.999:
            raise RuntimeError(
                f"Material '{material.name}' uses unsupported transparency."
            )
        color = tuple(base_color.default_value)

    if color[3] < 0.999:
        raise RuntimeError(f"Material '{material.name}' is not opaque.")
    return color[:3]


def export_selected_model(model_name, output_file):
    """Combine selected meshes, center them, and export triangle vertices."""
    if not re.fullmatch(r"[A-Za-z_][A-Za-z0-9_]*", model_name):
        raise ValueError("MODEL_NAME must be a valid C++ identifier.")
    if bpy.context.mode != "OBJECT":
        raise RuntimeError("Switch to Object Mode before running the exporter.")

    objects = sorted(
        (obj for obj in bpy.context.selected_objects if obj.type == "MESH"),
        key=lambda obj: obj.name,
    )
    if not objects:
        raise RuntimeError("Select all mesh parts of one ship before exporting.")

    vertices = []
    skipped_triangles = 0
    dependency_graph = bpy.context.evaluated_depsgraph_get()

    for obj in objects:
        evaluated = obj.evaluated_get(dependency_graph)
        mesh = evaluated.to_mesh()
        try:
            mesh.calc_loop_triangles()
            world_matrix = evaluated.matrix_world.copy()
            linear_matrix = world_matrix.to_3x3()
            if linear_matrix.determinant() == 0.0:
                raise RuntimeError(f"Object '{obj.name}' has a zero scale.")
            normal_matrix = linear_matrix.inverted().transposed()
            colors = {}

            for triangle in mesh.loop_triangles:
                # A zero-area triangle has no usable surface normal.
                if triangle.area <= 0.0:
                    skipped_triangles += 1
                    continue

                material_index = triangle.material_index
                if material_index not in colors:
                    colors[material_index] = material_color(mesh, material_index)
                color = colors[material_index]

                loops = triangle.loops
                if linear_matrix.determinant() < 0.0:
                    loops = tuple(reversed(loops))

                for loop_index in loops:
                    loop = mesh.loops[loop_index]
                    position = world_matrix @ mesh.vertices[loop.vertex_index].co
                    normal = normal_matrix @ mesh.corner_normals[loop_index].vector
                    if normal.length_squared == 0.0:
                        normal = normal_matrix @ triangle.normal
                    if normal.length_squared == 0.0:
                        raise RuntimeError(f"Object '{obj.name}' has an invalid normal.")
                    normal.normalize()

                    if not all(math.isfinite(value) for value in (*position, *color, *normal)):
                        raise RuntimeError(f"Object '{obj.name}' contains invalid vertex data.")
                    vertices.append((position.copy(), color, normal.copy()))
        finally:
            evaluated.to_mesh_clear()

    if not vertices:
        raise RuntimeError("The selected objects contain no valid triangles.")

    minimum = Vector(tuple(min(vertex[0][axis] for vertex in vertices) for axis in range(3)))
    maximum = Vector(tuple(max(vertex[0][axis] for vertex in vertices) for axis in range(3)))
    center = (minimum + maximum) * 0.5
    uniform_scale = 2.0 / max(maximum - minimum)

    output_file = Path(output_file)
    output_file.parent.mkdir(parents=True, exist_ok=True)
    temporary_file = output_file.with_suffix(output_file.suffix + ".tmp")
    try:
        with temporary_file.open("w", encoding="utf-8", newline="\n") as stream:
            stream.write(
                "#pragma once\n\n"
                f"// Model author: {MODEL_AUTHOR}\n"
                f"// Source: {MODEL_SOURCE}\n"
                f"// License: {MODEL_LICENSE}\n"
                "// Exported with an AI-assisted Blender script.\n"
                "// Layout: position XYZ, color RGB, normal XYZ.\n"
                "// Centered; maximum dimension is 2 Blender units.\n\n"
                f"inline constexpr int {model_name}VertexCount = {len(vertices)};\n\n"
                f"inline constexpr float {model_name}Vertices[] = {{\n"
            )
            for position, color, normal in vertices:
                values = (*(uniform_scale * (position - center)), *color, *normal)
                stream.write("    " + ", ".join(f"{value:.8f}f" for value in values) + ",\n")
            stream.write("};\n")
        temporary_file.replace(output_file)
    finally:
        temporary_file.unlink(missing_ok=True)

    print(f"Export complete: {output_file}")
    print(f"Meshes: {len(objects)}; triangles: {len(vertices) // 3}; vertices: {len(vertices)}")
    print(f"Skipped zero-area triangles: {skipped_triangles}")
    return len(vertices)


if __name__ == "__main__":
    export_selected_model(MODEL_NAME, OUTPUT_DIRECTORY / f"{MODEL_NAME}.h")
