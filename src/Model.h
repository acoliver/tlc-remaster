#pragma once

#include "env.h"
#include <allegro.h>

// Forward declare OpenGL types used in this header
// The actual GL includes are in Model.cpp
typedef unsigned int GLuint;
typedef double GLdouble;


#include <string>
#include <map>
#include <vector>

class ModelLoader;

class Model
{
public:
   virtual ~Model();

   void Draw();

   class Material
   {
   public:
      std::string name;
      std::string file;
      GLuint texture;
   };
   std::map<std::string,Material> Materials;

   class Vertex
   {
   public:
      double x, y, z;
   };
   std::vector<Vertex> Vertices;

   class TexCoord
   {
   public:
      double u, v;
   };
   std::vector<TexCoord> TexCoords;

   class Normal
   {
   public:
      double i, j, k;
   };
   std::vector<Normal> Normals;

   class Triangle
   {
   public:
      Triangle()
      {
         for (int i = 0; i < 3; i++)
         {
            Vertices[i] = TexCoords[i] = Normals[i] = -1;
         }
      }

      int Vertices[3];
      int TexCoords[3];
      int Normals[3];
   };

   class TriangleGroup
   {
   public:
      std::string Name;
      std::string MaterialName;
      std::vector<Triangle> Triangles;
   };
   std::vector<TriangleGroup> TriangleGroups;

   GLdouble scaleX, scaleY, scaleZ;

private:
   Model();

   friend class ModelLoader;
};
