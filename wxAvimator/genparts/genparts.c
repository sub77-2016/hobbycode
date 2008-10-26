/*
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Copyright (C) 2006 by Vinay Pulim.
 * All rights reserved.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include "../bvh.h"
#include "../Math3D.h"
#include "../glm.h"

using namespace Math3D;

// SL Avatar: scale = 136.882148 and yoffset = 0.0

void genPart(GLMmodel* model, BVHNode *node, double yOffset, double scale,
	     Matrix4 &xform, Matrix4 &rot)
{
  double value;
  GLMgroup *group;
  GLMtriangle *tri;
  int offs;

  if (node->type == BVH_END) return;
  Matrix4 new_xform, new_rot, m;
  new_xform = Matrix4().Translation(Vector4(-node->offset[0], 
					    -node->offset[1], 
					    -node->offset[2])) * xform;
  new_rot = rot;
  for (int i=0; i<node->numChannels; i++) {
    value = node->frame[0][i];
    switch(node->channelType[i]) {
    case BVH_XROT: 
      m.Rotation(-value * M_PI/180, Vector4(1, 0, 0));
      new_rot = m * new_rot;
      break;
    case BVH_YROT: 
      m.Rotation(-value * M_PI/180, Vector4(0, 1, 0));
      new_rot = m * new_rot;
      break;
    case BVH_ZROT: 
      m.Rotation(-value * M_PI/180, Vector4(0, 0, 1));
      new_rot = m * new_rot;
      break;
    case BVH_XPOS: 
      m.Translation(Vector4(-value, 0, 0));
      break;
    case BVH_YPOS: 
      m.Translation(Vector4(0, -value, 0));
      break;
    case BVH_ZPOS: 
      m.Translation(Vector4(0, 0, -value));
      break;
    }
    new_xform = m * new_xform;
  }
  if (!(group = glmFindGroup(model, node->name))) {
    fprintf(stderr, "Group '%s' not found while aligning to BVH\n",node->name);
  }
  else {
    printf("  else if (!strcmp(name, \"%s\")) {\n", group->name);
    printf("    glBegin(GL_TRIANGLES);\n");
    for (int t=0; t<group->numtriangles; t++) {
      tri = &(model->triangles[group->triangles[t]]);
      for (int v=0; v<3; v++) {
	offs = 3 * tri->nindices[v];
	Vector4 norm(model->normals[offs + 0],
		   model->normals[offs + 1],
		   model->normals[offs + 2]);
	norm = new_rot * norm;
	printf("      glNormal3f(%f, %f, %f);\n",norm[0], norm[1], norm[2]);

	offs = 3 * tri->vindices[v];
	Vector4 vert(model->vertices[offs + 0],
		     model->vertices[offs + 1],
		     model->vertices[offs + 2]);
	vert = m.Scale(scale, scale, scale) * vert;
	vert = m.Translation(Vector4(0, yOffset, 0)) * vert;
	vert = new_xform * vert;
	//	vert = new_xform * m.Scale(scale, scale, scale) * vert;
	printf("      glVertex3f(%f, %f, %f);\n",vert[0], vert[1], vert[2]);
      }
    }
    printf("    glEnd();\n");
    printf("  }\n");
  }
  for (int i=0; i<node->numChildren; i++) {
    genPart(model, node->child[i], yOffset, scale, new_xform, new_rot );
  }  
}

void genParts(char *name, GLMmodel *model, BVHNode *root, double yOffset, double scale)
{
  Matrix4 I;
  printf("#include <string.h>\n");
  printf("#include <FL/gl.h>\n");
  printf("#include \"SLParts.h\"\n\n");
  printf("void drawSL%sPart(char *name) {\n", name);
  printf("  if (!strcasecmp(name, \"End\")) {\n");
  printf("    return;\n");
  printf("  }\n");
  genPart(model, root, yOffset, scale, I, I);
  printf("}\n");
}

int main(int argc, char **argv)
{
  if (argc != 6)
    printf("Usage: genfigure <name> <obj file> <bvh file> <yOffset> <scale>\n");
  else {
    GLMmodel *model = glmReadOBJ(argv[2]);
    if (!model) exit(0);    
    glmFacetNormals(model);
    glmVertexNormals(model, 90);    
    genParts(argv[1], model, bvhRead(argv[3]),
	      atof(argv[4]), atof(argv[5]));
  }
  exit(0);
}
