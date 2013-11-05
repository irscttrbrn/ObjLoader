//------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ObjLoader.h"
//------------------------------------------------------------------------------
#define MAX_STRING_LENGTH 256
//------------------------------------------------------------------------------ THE ACTUAL STRUCTS
typedef struct _ObjFace
{
    ObjVector3I positionIndices;
    ObjVector3I normalIndices;
    ObjVector3I texCoordIndices;
    int materialIndex;
}
ObjFace;
//------------------------------------------------------------------------------
typedef struct _ObjGroup
{
    char name[MAX_STRING_LENGTH];
    ObjFace* faces;
    int numFaces;
}
ObjGroup;
//------------------------------------------------------------------------------
typedef struct _ObjObject
{
    char name[MAX_STRING_LENGTH];
    ObjGroup* groups;
    int numGroups;
}
ObjObject;
//------------------------------------------------------------------------------
typedef struct _ObjFile
{
    char name[256];

    ObjVector3F* positions;
    ObjVector3F* normals;
    ObjVector2F* texCoords;
    ObjObject* objects;
    ObjGroup* groups;
    ObjFace* faces;

    int numPositions;
    int numNormals;
    int numTexCoords;
    int numObjects;
    int numGroups;
    int numFaces;
}
ObjFile;
//------------------------------------------------------------------------------ GLOBAL LOADER STATE
static ObjObject* currentObject = NULL;
static ObjGroup* currentGroup = NULL;
//------------------------------------------------------------------------------ FILE PRIVATE HELPER FCTS DECLARATION
static ObjFile* createObjFile(const char* filename);
static void resetGlobalState();
static void prepareForProcessing(ObjFile* file);
static void updateFileInfo(ObjFile* file, const char* line);
static void updateFile(ObjFile* file, const char* line);
static void addPosition(ObjFile* file, const char* line);
static void addNormal(ObjFile* file, const char* line);
static void addTexCoord(ObjFile* file, const char* line);
static void addFace(ObjFile* file, const char* line);
static void addObject(ObjFile* file, const char* line);
static void addGroup(ObjFile* file, const char* line);
//------------------------------------------------------------------------------ PUBLIC INTERFACE'S DEFINITION
void ObjFileLoad(
    ObjFilePtr* file, 
    const char* filename
)
{
    resetGlobalState();

    // expect the worst
    *file = NULL;

    // open the .obj file
    FILE* f;
    f = fopen(filename, "r");
    
    if (NULL == f)
    {
        return;
    }

    // create the ObjFile object.
    ObjFile* objFile = createObjFile(filename);

    if (NULL == objFile)
    {
        return;
    }

    // get neccessary information about the file

    // until the end of the file, read the file in line by line
    char line[MAX_STRING_LENGTH];
    
    while (NULL != fgets(line, MAX_STRING_LENGTH, f))
    {
        updateFileInfo(objFile, line);
    }

    // alloc memory for positions, normals, texcoords, groups, faces and objects // TODO: check if all allocations
                                                                                 //       succeed
    objFile->positions = (ObjVector3F*)malloc(
            sizeof(ObjVector3F)*(objFile->numPositions + 1)
        );

    objFile->normals = (ObjVector3F*)malloc(
            sizeof(ObjVector3F)*(objFile->numNormals + 1)
        );

    objFile->texCoords = (ObjVector2F*)malloc(
            sizeof(ObjVector2F)*(objFile->numTexCoords + 1)
        );

    if (objFile->numFaces)
    {
        objFile->faces = (ObjFace*)malloc(sizeof(ObjFace)*objFile->numFaces);
    }

    objFile->objects = (ObjObject*)malloc(
            sizeof(ObjObject)*(objFile->numObjects + 1)
        );

    objFile->groups = (ObjGroup*)malloc(
            sizeof(ObjGroup)*(objFile->numObjects + 1 + objFile->numGroups)
        );

    // once again iterate through the file and copy the data
    rewind(f);
    prepareForProcessing(objFile);

    while (NULL != fgets(line, MAX_STRING_LENGTH, f))
    {
        updateFile(objFile, line);
    }    

    // clean up
    fclose(f);

    // return result
    *file = objFile;
}
//------------------------------------------------------------------------------
void ObjFileGetNumPositions(
    ObjFilePtr file, 
    int* numPositions
)
{
    *numPositions = file->numPositions;
}
//------------------------------------------------------------------------------
void ObjFileGetPosition(
    ObjFilePtr file,
    ObjVector3F* position,
    int i
)
{
    if (i >= file->numPositions || position == NULL || file == NULL)
    {
        return;
    }

    *position = file->positions[i];
}
//------------------------------------------------------------------------------
void ObjFileGetNumNormals(
    ObjFilePtr file, 
    int* numNormals
)
{
    if (file == NULL || numNormals == NULL)
    {
        return;
    }

    *numNormals = file->numNormals;
}
//------------------------------------------------------------------------------
void ObjFileGetNormal(
    ObjFilePtr file,
    ObjVector3F* normal,
    int i
)
{
    if (file == NULL || i >= file->numNormals)
    {
        return;
    }

    *normal = file->normals[i];
}
//------------------------------------------------------------------------------
void ObjFileGetNumTexCoords(
    ObjFilePtr file, 
    int* numTexCoords
)
{
    if (file == NULL || numTexCoords == NULL)
    {
        return;
    }

    *numTexCoords = file->numTexCoords;
}
//------------------------------------------------------------------------------
void ObjFileGetTexCoord(
    ObjFilePtr file,
    ObjVector2F* texCoord,
    int i
)
{
    if (file == NULL || i >= file->numTexCoords)
    {
        return;
    }

    *texCoord = file->texCoords[i];
}
//------------------------------------------------------------------------------
void ObjFileGetNumFaces(
    ObjFilePtr file,
    int* numFaces
)
{
    if (file == NULL || numFaces == NULL)
    {
        return;
    }

    *numFaces = file->numFaces;   
}
//------------------------------------------------------------------------------
void ObjFileGetFace(
    ObjFilePtr file,
    ObjFacePtr* face,
    int i
)
{
    if (file == NULL || face == NULL || i >= file->numFaces)
    {
        return;
    }
    
    *face = &file->faces[i];
}
//------------------------------------------------------------------------------
void ObjFileGetNumObjects(
    ObjFilePtr file,
    int* numObjects
)
{
    if (file == NULL)
    {
        return;
    }

    *numObjects = file->numObjects;
}
//------------------------------------------------------------------------------
void ObjFileGetObject(
    ObjFilePtr file,
    ObjObjectPtr* object,
    int i
)
{
    if (file == NULL || object == NULL || i >= file->numObjects)
    {
        return;
    }

    *object = &file->objects[i];
}
//------------------------------------------------------------------------------
void ObjObjectGetName(
    ObjObjectPtr object,
    int* length,
    char* name,
    int maxLength  
)
{
    if (name == NULL || object == NULL)
    {
        return;
    }

    if (length != NULL)
    {
        *length = strlen(object->name);
    }

    if (strlen(object->name) >= maxLength)
    {
        int i = 0;
        for (i = 0; i < maxLength - 1; ++i)
        {
            name[i] = object->name[i];
        }
        name[maxLength - 1] = '\0';

        return;
    }

    strcpy(name, object->name);
}
//------------------------------------------------------------------------------
void ObjObjectGetGroup(
    ObjObjectPtr object,
    ObjGroupPtr* group,
    int i
)
{
    if (object == NULL || group == NULL || i >= object->numGroups)
    {
        return;
    }

    *group = &object->groups[i];
}
//------------------------------------------------------------------------------
void ObjObjectGetNumGroups(
    ObjObjectPtr object,
    int* numGroups
)
{
    if (object == NULL || numGroups == NULL)
    {
        return;
    }

    *numGroups = object->numGroups;
}
//------------------------------------------------------------------------------
void ObjFaceGetPositionIndices(
    ObjFacePtr face, 
    ObjVector3I* indices 
)
{
    if (face == NULL || indices == NULL)
    {
        return;
    }
    memcpy(indices, &face->positionIndices, sizeof(ObjVector3I));
}
//------------------------------------------------------------------------------
void ObjFaceGetNormalIndices(
    ObjFacePtr face, 
    ObjVector3I* indices 
)
{
    if (face == NULL || indices == NULL)
    {
        return;
    }
    memcpy(indices, &face->normalIndices, sizeof(ObjVector3I));
}
//------------------------------------------------------------------------------
void ObjFaceGetTexCoordIndices(
    ObjFacePtr face, 
    ObjVector3I* indices 
)
{
    if (face == NULL || indices == NULL)
    {
        return;
    }

    memcpy(indices, &face->texCoordIndices, sizeof(ObjVector3I));
}
//------------------------------------------------------------------------------
void ObjFaceGetMaterialIndex(
    ObjFacePtr face,
    int* materialIndex
)
{
    if (face == NULL || materialIndex == NULL)
    {
        return;
    }

    *materialIndex = face->materialIndex;
}
//------------------------------------------------------------------------------
void ObjGroupGetFace(
    ObjGroupPtr group,
    ObjFacePtr* face,
    int i
)
{
    if (group == NULL || face == NULL || i > group->numFaces)
    {
        return;
    }

    *face = &group->faces[i];
}
//------------------------------------------------------------------------------
void ObjGroupGetNumFaces(
    ObjGroupPtr group,
    int* numFaces
)
{
    if (group == NULL || numFaces == NULL)
    {
        return;
    }

    *numFaces = group->numFaces;
}
//------------------------------------------------------------------------------
void ObjGroupGetName(
    ObjGroupPtr group,
    int* length,
    char* name,
    int maxLength       
)
{
    if (name == NULL || group == NULL)
    {
        return;
    }

    if (length != NULL)
    {
        *length = strlen(group->name);
    }

    if (strlen(group->name) >= maxLength)
    {
        int i = 0;
        for (i = 0; i < maxLength - 1; ++i)
        {
            name[i] = group->name[i];
        }
        name[maxLength - 1] = '\0';

        return;
    }

    strcpy(name, group->name);    
}
//------------------------------------------------------------------------------ FILE PRIVATE HELPER FCTS DEFINITION
ObjFile* createObjFile(const char* filename)
{
    // creates and inits an ObjFile
    
    // error checking
    if (strlen(filename) >= MAX_STRING_LENGTH)
    {
        return NULL;
    }
    
    ObjFile* objFile = (ObjFile*)malloc(sizeof(ObjFile));

    if (NULL == objFile)
    {
        return NULL;
    }

    // initialize
    strcpy(objFile->name, filename);    

    objFile->positions = NULL;
    objFile->normals = NULL;
    objFile->texCoords = NULL;
    objFile->objects = NULL;
    objFile->groups = NULL;
    objFile->faces = NULL;
    
    objFile->numPositions = 0;
    objFile->numNormals = 0;
    objFile->numTexCoords = 0;    
    objFile->numObjects = 0;
    objFile->numGroups = 0;
    objFile->numFaces = 0;

    return objFile;
}
//------------------------------------------------------------------------------
void prepareForProcessing(ObjFile* file)
{
    // set the zero's element of [positions] to the zero vector

    file->numPositions = 0;
    file->numNormals = 0;
    file->numTexCoords = 0;    
    file->numObjects = 0;
    file->numGroups = 0;
    file->numFaces = 0;    

    file->positions[0].x = 0.0f;
    file->positions[0].y = 0.0f;
    file->positions[0].z = 0.0f;
    file->numPositions++;

    file->normals[0].x = 0.0f;
    file->normals[0].y = 0.0f;
    file->normals[0].z = 0.0f;
    file->numNormals++;

    file->texCoords[0].x = 0.0f;
    file->texCoords[0].y = 0.0f;
    file->numTexCoords++;

    // add a default object to the object file
    strcpy(file->objects[0].name, "");
    file->objects[0].groups = &file->groups[0];
    file->objects[0].numGroups++;
    currentObject = &file->objects[0];

    strcpy(file->groups[0].name, "");
    file->groups[0].faces = NULL;
    file->groups[0].numFaces = 0;

    file->numObjects++;
    file->numGroups++;
}
//------------------------------------------------------------------------------
void updateFileInfo(ObjFile* file, const char* line)
{
    // counts the number of groups, objectes, faces, positions, normals, 
    // texcoords 

    // TODO: trim [line]

    if (line[0] == 'v' && line[1] == ' ')
    {
        file->numPositions++;
        return;
    }

    if (line[0] == 'v' && line[1] == 'n' && line[2] == ' ')
    {
        file->numNormals++;
        return;
    }

    if (line[0] == 'v' && line[1] == 't' && line[2] == ' ')
    {
        file->numTexCoords++;
        return;
    }

    if (line[0] == 'o' && line[1] == ' ')
    {
        file->numObjects++;
        return;
    }

    if (line[0] == 'g' && line[1] == ' ')
    {
        file->numGroups++;
        return;
    }

    if (line[0] == 'f' && line[1] == ' ')
    {
        file->numFaces++;
        return;
    }

}
//------------------------------------------------------------------------------
void updateFile(ObjFile* file, const char* line)
{
    // counts the number of groups, objectes, faces, positions, normals, 
    // texcoords 

    // TODO: trim [line]

    if (line[0] == 'v' && line[1] == ' ')
    {
        addPosition(file, line);
        return;
    }

    if (line[0] == 'v' && line[1] == 'n' && line[2] == ' ')
    {
        addNormal(file, line);
        return;
    }

    if (line[0] == 'v' && line[1] == 't' && line[2] == ' ')
    {
        addTexCoord(file, line);
        return;
    }

    if (line[0] == 'o' && line[1] == ' ')
    {
        addObject(file, line);
        return;
    }

    if (line[0] == 'g' && line[1] == ' ')
    {
        addGroup(file, line);
        return;
    }

    if (line[0] == 'f' && line[1] == ' ')
    {
        addFace(file, line);
        return;
    }

}
//------------------------------------------------------------------------------
void addPosition(ObjFile* file, const char* line)
{
    ObjVector3F v;
    int n = sscanf(line, "v %f %f %f", &v.x, &v.y, &v.z);

    if (3 != n)
    {
        return;
    }

    file->positions[file->numPositions] = v;
    file->numPositions++;
}
//------------------------------------------------------------------------------
void addNormal(ObjFile* file, const char* line)
{
    ObjVector3F v;
    int n = sscanf(line, "vn %f %f %f", &v.x, &v.y, &v.z);

    if (3 != n)
    {
        return;
    }

    file->normals[file->numNormals] = v;
    file->numNormals++;
}
//------------------------------------------------------------------------------
void addTexCoord(ObjFile* file, const char* line)
{
    ObjVector2F v;
    int n = sscanf(line, "vt %f %f", &v.x, &v.y);

    if (2 != n)
    {
        return;
    }

    file->texCoords[file->numTexCoords] = v;
    file->numTexCoords++;    
}
//------------------------------------------------------------------------------
void addFace(ObjFile* file, const char* line)
{
    ObjFace face;
    memset(&face, 0, sizeof(ObjFace));

    // case: f [p] [p] [p]
    int n = sscanf(
            line, 
            "f %u %u %u", 
            &face.positionIndices.x, 
            &face.positionIndices.y, 
            &face.positionIndices.z
        );
    
    if (n == 3)
    {
        file->faces[file->numFaces] = face;
        file->numFaces++;
        currentGroup->numFaces++;
        return;
    }

    // case: f [p]/[tc] [p]/[tc] [p]/[tc]
    n = sscanf(
            line, 
            "f %u/%u %u/%u %u/%u",
            &face.positionIndices.x, 
            &face.texCoordIndices.x,
            &face.positionIndices.y, 
            &face.texCoordIndices.y,
            &face.positionIndices.z, 
            &face.texCoordIndices.z
        );

    if (n == 6)
    {
        file->faces[file->numFaces] = face;
        file->numFaces++;
        currentGroup->numFaces++;
        return;
    }       

    // case: f [p]//[n] [p]//[n] [p]//[n]
    n = sscanf(
            line, 
            "f %u//%u %u//%u %u//%u",
            &face.positionIndices.x, 
            &face.normalIndices.x,
            &face.positionIndices.y, 
            &face.normalIndices.y,
            &face.positionIndices.z, 
            &face.normalIndices.z
        );
    
    if (n == 6)
    {
        file->faces[file->numFaces] = face;
        file->numFaces++;
        currentGroup->numFaces++;
        return;
    }

    // case: f [p]/[tc]/[n] [p]/[tc]/[n] [p]/[tc]/[n]
    n = sscanf(
            line, 
            "f %u/%u/%u %u/%u/%u %u/%u/%u",
            &face.positionIndices.x, 
            &face.texCoordIndices.x,
            &face.normalIndices.x,
            &face.positionIndices.y, 
            &face.texCoordIndices.y,
            &face.normalIndices.y,
            &face.positionIndices.z, 
            &face.texCoordIndices.z,
            &face.normalIndices.z
        );

    if (n == 9)
    {
        file->faces[file->numFaces] = face;
        file->numFaces++;
        currentGroup->numFaces++;
        return;
    }  

}
//------------------------------------------------------------------------------
void addObject(ObjFile* file, const char* line)
{
    char name[MAX_STRING_LENGTH];

    int n = sscanf(line, "o %s", name);

    if (n != 1)
    {
        return;
    }

    strcpy(file->objects[file->numObjects].name, name);
    file->objects[file->numObjects].numGroups = 0;
    file->objects[file->numObjects].groups = &file->groups[file->numGroups];
    strcpy(file->groups[file->numGroups].name, "");
    file->groups[file->numGroups].numFaces = 0;
    file->groups[file->numGroups].faces = &file->faces[file->numFaces];
    file->objects[file->numObjects].numGroups++;

    currentObject = &file->objects[file->numObjects];
    currentGroup = &file->groups[file->numGroups];

    file->numObjects++;
    file->numGroups++;

}
//------------------------------------------------------------------------------
void addGroup(ObjFile* file, const char* line)
{
    char name[MAX_STRING_LENGTH];

    int n = sscanf(line, "g %s", name);
    
    if (1 != n)
    {
        return;
    }

    strcpy(file->groups[file->numGroups].name, name);
    file->groups[file->numGroups].numFaces = 0;
    file->groups[file->numGroups].faces = &file->faces[file->numFaces];
    currentObject->numGroups++;
    currentGroup = &file->groups[file->numGroups];
    file->numGroups++;
}
//------------------------------------------------------------------------------
void resetGlobalState()
{
    currentObject = NULL;
    currentGroup = NULL;
}
//------------------------------------------------------------------------------





