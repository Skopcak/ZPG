"""Generate a procedural detail map and sky panorama for the texture exercises.

Requires numpy and Pillow. No external image assets or downloads are used.
The skybox uses the separate, unchanged JPEG images supplied by the teacher.
"""

from pathlib import Path
import numpy as np
from PIL import Image


def smoothstep(low, high, value):
    t = np.clip((value-low)/(high-low), 0, 1)
    return t*t*(3-2*t)


def sky_color(direction):
    direction = direction/np.linalg.norm(direction, axis=-1, keepdims=True)
    x, y, z = (direction[..., i] for i in range(3))
    height = np.clip(y, 0, 1)**0.55
    horizon = np.array([0.70, 0.83, 0.94])
    zenith = np.array([0.12, 0.34, 0.70])
    color = horizon + height[..., None]*(zenith-horizon)
    below = np.clip(-y, 0, 1)**0.35
    color = color*(1-below[..., None]) + np.array([0.08, 0.13, 0.18])*below[..., None]

    field = (np.sin(5*x+4*z)*np.cos(3*z-2*x)
             + 0.45*np.sin(13*x+8*z+3*y)
             + 0.18*np.sin(29*x-25*z+7*y)) / 1.63
    clouds = smoothstep(0.02, 0.55, field)
    clouds *= smoothstep(0.01, 0.13, y)*(1-smoothstep(0.70, 0.97, y))
    color = color*(1-0.80*clouds[..., None]) + np.array([0.96, 0.97, 0.98])*0.80*clouds[..., None]

    sun_direction = np.array([0.45, 0.50, -0.74])
    sun_direction /= np.linalg.norm(sun_direction)
    dot = np.sum(direction*sun_direction, axis=-1)
    halo = np.exp((dot-1)*35)*0.18
    disk = np.exp((dot-1)*2600)
    color += halo[..., None]*np.array([1.0, 0.72, 0.28])
    color = color*(1-disk[..., None]) + np.array([1.0, 0.98, 0.84])*disk[..., None]
    return np.uint8(np.clip(color, 0, 1)*255+0.5)


def generate(output):
    output = Path(output)
    output.mkdir(parents=True, exist_ok=True)

    longitude = ((np.arange(1024)+0.5)/1024-0.5)*2*np.pi
    latitude = (0.5-(np.arange(512)+0.5)/512)*np.pi
    lon, lat = np.meshgrid(longitude, latitude)
    directions = np.stack((np.cos(lat)*np.cos(lon), np.sin(lat),
                           np.cos(lat)*np.sin(lon)), axis=-1)
    Image.fromarray(sky_color(directions)).save(output/'sky_panorama.png')

    yy, xx = np.indices((64, 64))
    checker = ((xx//32 + yy//32) % 2).astype(bool)
    detail = np.where(checker[..., None], np.array([235, 222, 191]), np.array([60, 82, 58]))
    Image.fromarray(detail.astype('uint8')).save(output/'detail.png')
    print('Generated detail texture and sky panorama:', output)


if __name__ == '__main__':
    generate(Path(__file__).resolve().parents[1]/'project'/'Textures')
