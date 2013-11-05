#include "ObjLoader.h"
#include <stdio.h>

static void dump(ObjFilePtr file)
{
    // dump positions
    int numPos;
    ObjVector3F pos;

    ObjFileGetNumPositions(file, &numPos);
    printf("Positions: \n");

    for (int i = 0; i < numPos; ++i)
    {
        ObjFileGetPosition(file, &pos, i);
        printf("[%f %f %f]\n", pos.x, pos.y, pos.z);
    }

    // dump normals
    int numNormals;
    ObjVector3F nrm;
    
    ObjFileGetNumNormals(file, &numNormals);
    printf("Normals\n");

    for (int i = 0; i < numNormals; ++i)
    {
        ObjFileGetNormal(file, &nrm, i);    
        printf("[%f %f %f]\n", nrm.x, nrm.y, nrm.z);
    }

    // dump tex coords
    int numTC;
    ObjVector2F texCoord;

    ObjFileGetNumTexCoords(file, &numTC);
    printf("TexCoordinates\n");

    for (int i = 0; i < numTC; ++i)
    {
        ObjFileGetTexCoord(file, &texCoord, i);
        printf("[%f %f]\n", texCoord.x, texCoord.y);
    }

    // dumping objects
    int numObjects;
    
    ObjFileGetNumObjects(file, &numObjects);
    printf("Objects\n\n");

    for (int i = 0; i < numObjects; ++i)
    {
        ObjObjectPtr object;
        char name[128];
        int numGroups;

        ObjFileGetObject(file, &object, i);
        ObjObjectGetName(object, NULL, name, 128);
        ObjObjectGetNumGroups(object, &numGroups);
        printf("\tName %s\n", name);
        printf("\tNumber of Groups %d \n", numGroups);
        
        ObjGroupPtr group;

        printf("\tGroups\n\n");
        for (int j = 0; j < numGroups; ++j)
        {
            char name[128];
            int numFaces;
            ObjObjectGetGroup(object, &group, j);
            ObjGroupGetName(group, NULL, name, 128);
            ObjGroupGetNumFaces(group, &numFaces);
            printf("\t\tName %s\n", name);
            printf("\t\tNumber of Faces %d\n", numFaces);

            for (int k = 0; k < numFaces; ++k)
            {
                ObjFacePtr face;
                ObjGroupGetFace(group, &face, k);
                
                ObjVector3I posIndices;
                ObjVector3I nrmIndices;
                ObjVector3I tcIndices;

                ObjFaceGetPositionIndices(face, &posIndices);
                ObjFaceGetNormalIndices(face, &nrmIndices);
                ObjFaceGetTexCoordIndices(face, &tcIndices);

                printf("\t\t\t%d/%d/%d %d/%d/%d %d/%d/%d\n", 
                    posIndices.x,
                    tcIndices.x,
                    nrmIndices.x,
                    posIndices.y,
                    tcIndices.y,
                    nrmIndices.y,
                    posIndices.z,
                    tcIndices.z,
                    nrmIndices.z
                );
            }
        }
    }

}


int main(int argc, char const *argv[])
{
    ObjFilePtr file;
    ObjFileLoad(&file, "Iseki2.obj");

    dump(file);

    return 0;
}