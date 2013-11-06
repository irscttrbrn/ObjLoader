# A minimal Wavefront .obj loader #

## Features ##
- minimal
- reads positions, normals, texture coordinates
- reads in faces
    - groups them into objects and then groups
- supported material attributes (currently):
    - Ka, Kd, Ks
    - map_Ka, map_Kd, map_Ks,
    - Ns

## Notes ##
- Reads only one (the latest occuring) .mtl file
- Supports only triangle meshes

## Todos ##
- add bump maps as material attribute
- maybe support multiple mtl files
- maybe save .obj files