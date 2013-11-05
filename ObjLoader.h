#ifndef OBJLOADER_H__
#define OBJLOADER_H__

/*!
** A 3d int vector
*/
typedef struct 
{
    int x, y, z;
}
ObjVector3I;

/*!
** A 2d float vector.
*/
typedef struct 
{
    float x, y;
}
ObjVector2F;

/*!
** A 3d float vector.
*/
typedef struct 
{
    float x, y, z;
}
ObjVector3F;
 
/*!
** A handle to a face.
*/
typedef const struct _ObjFace* ObjFacePtr;

/*!
** Retrieves the position indices of the [face] and stores them in [indices].
*/
void ObjFaceGetPositionIndices(
    ObjFacePtr face, 
    ObjVector3I* indices 
);

/*!
** Retrieves the position indices of the [face] and stores them in [indices].
*/
void ObjFaceGetNormalIndices(
    ObjFacePtr face, 
    ObjVector3I* indices 
);

/*!
** Retrieves the position indices of the [face] and stores them in [indices].
*/
void ObjFaceGetTexCoordIndices(
    ObjFacePtr face, 
    ObjVector3I* indices 
);

/*!
** Retrieves the material index of the face.
*/
void ObjFaceGetMaterialIndex(
    ObjFacePtr face,
    int* materialIndex
);

/*!
** A handle to a group of faces.
*/
typedef const struct _ObjGroup* ObjGroupPtr;

/*!
** Retrieves the face for a group.
**
** Stores the handle of the [i]'s face of [group] in [face].
*/
void ObjGroupGetFace(
    ObjGroupPtr group,
    ObjFacePtr* face,
    int i
);

/*!
** Gets the number of faces for a group.
*/
void ObjGroupGetNumFaces(
    ObjGroupPtr group,
    int* numFaces
);

/*!
** Gets the name for a group.
**
** @param group Group being queried.
** @param length Actual length of the name (without '\0').
** @param name Char array for storing the name of the [group].
** @param maxLength Length of the [name] array.
*/
void ObjGroupGetName(
    ObjGroupPtr group,
    int* length,
    char* name,
    int maxLength       
);

/*!
** A handle to a Object
*/
typedef const struct _ObjObject* ObjObjectPtr;

/*!
** Gets a group handle of for ab object.
**
** Stores the handle of the [i]'s group of [object] in [group].
*/
void ObjObjectGetGroup(
    ObjObjectPtr object,
    ObjGroupPtr* group,
    int i
);

/*!
** Gets the number of groups for an object.
*/
void ObjObjectGetNumGroups(
    ObjObjectPtr object,
    int* numGroups
);
 
/*!
** Gets the name for an object.
**
** @param group Object being queried.
** @param length Actual length of the name (without '\0').
** @param name Char array for storing the name of the [group].
** @param maxLength Length of the [name] array.
*/
void ObjObjectGetName(
    ObjObjectPtr object,
    int* length,
    char* name,
    int maxLength       
);

/*!
** A handle to an objFile
*/
typedef const struct _ObjFile* ObjFilePtr;

/*!
** Loads an .obj specified by its [filename] from a file.
*/
void ObjFileLoad(
    ObjFilePtr* file, 
    const char* filename
);

/*!
** Releases an .obj file.
*/
void ObjFileRelease(
    ObjFilePtr* file
);

/*!
** Gets the number of positions in the .obj file.
*/
void ObjFileGetNumPositions(
    ObjFilePtr file,
    int* numPositions
);

/*!
** Gets the position for index [i].
*/
void ObjFileGetPosition(
    ObjFilePtr file,
    ObjVector3F* position,
    int i
);

/*!
** Gets the number of normals in the .obj file.
*/
void ObjFileGetNumNormals(
    ObjFilePtr file,
    int* numNormal
);

/*!
** Gets the normal for index [i].
*/
void ObjFileGetNormal(
    ObjFilePtr file,
    ObjVector3F* normal,
    int i
);

/*!
** Gets the number of tex coordinates in the .obj file.
*/
void ObjFileGetNumTexCoords(
    ObjFilePtr file,
    int* numTexCoords
);

/*!
** Gets the tex coordinate for index [i].
*/
void ObjFileGetTexCoord(
    ObjFilePtr file,
    ObjVector2F* texCoords,
    int i
);

/*!
** Gets the number of faces in the .obj file.
*/
void ObjFileGetNumFaces(
    ObjFilePtr file,
    int* numFaces
);

/*!
** Gets the face for index [i].
*/
void ObjFileGetFace(
    ObjFilePtr file,
    ObjFacePtr* face,
    int i
);

/*!
** Gets the number of objects in the .obj file.
*/
void ObjFileGetNumObjects(
    ObjFilePtr file,
    int* numObjects
);

/*!
** Gets the object for index [i].
*/
void ObjFileGetObject(
    ObjFilePtr file,
    ObjObjectPtr* object,
    int i
);

#endif /* end of include guard: OBJLOADER_H__ */