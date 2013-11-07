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
                int mat = 0;

                ObjFaceGetPositionIndices(face, &posIndices);
                ObjFaceGetNormalIndices(face, &nrmIndices);
                ObjFaceGetTexCoordIndices(face, &tcIndices);
                ObjFaceGetMaterialIndex(face, &mat);


                printf("\t\t\t%d/%d/%d %d/%d/%d %d/%d/%d mat: %d\n", 
                    posIndices.x,
                    tcIndices.x,
                    nrmIndices.x,
                    posIndices.y,
                    tcIndices.y,
                    nrmIndices.y,
                    posIndices.z,
                    tcIndices.z,
                    nrmIndices.z,
                    mat
                );
            }
        }
    }

    // dump materials
    printf("Material\n");
    int numMaterials;
    ObjMaterial material;

    ObjFileGetNumMaterials(file, &numMaterials);

    for (int i = 0; i < numMaterials; i++) 
    {
        ObjFileGetMaterial(file, &material, i);
        printf("Name: %s\n", material.name);
        printf("shininess: %f \n", material.shininess);
        
        printf(
            "Ka: [%f %f %f]\n", 
            material.ambient.x, 
            material.ambient.y, 
            material.ambient.z
        );
        
        printf(
            "Kd: [%f %f %f]\n", 
            material.diffuse.x, 
            material.diffuse.y, 
            material.diffuse.z
        );
        
        printf(
            "Ks: [%f %f %f]\n", 
            material.specular.x, 
            material.specular.y, 
            material.specular.z
        );

        printf("map_Ka %s\n", material.ambientTex);
        printf("map_Kd %s\n", material.diffuseTex);
        printf("map_Ks %s\n", material.specularTex);
    }
}


int main(int argc, char const *argv[])
{
    ObjFilePtr file;
    ObjFileLoadWithPath(&file, "sibenik.obj", "Objs");

    dump(file);

    return 0;
}
