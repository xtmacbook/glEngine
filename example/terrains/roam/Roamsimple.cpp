//
// ROAM Simplistic Implementation
// All code copyright Bryan Turner (Jan, 2000)
//                    brturn@bellsouth.net
//
// Based on the Tread Marks engine by Longbow Digital Arts
//                               (www.LongbowDigitalArts.com)
// Much help and hints provided by Seumas McNally, LDA.
//
#include <math.h>
#include <gls.h>
#include "landscape.h"
#include <cameraBase.h>
#include <shader.h>
#include <helpF.h>
// -------------------------------------------------------------------------------------------------
//	PATCH CLASS
// -------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------
// Split a single Triangle and link it into the mesh.
// Will correctly force-split diamonds.
//
void Patch::Split(TriTreeNode *tri)
{
	// We are already split, no need to do it again.
	if (tri->LeftChild)
		return;

	// If this triangle is not in a proper diamond, force split our base neighbor
	if ( tri->BaseNeighbor && (tri->BaseNeighbor->BaseNeighbor != tri) )
		Split(tri->BaseNeighbor);

	// Create children and link into mesh
	tri->LeftChild  = Landscape::AllocateTri();
	tri->RightChild = Landscape::AllocateTri();

	// If creation failed, just exit.
	if ( !tri->LeftChild )
		return;

	// Fill in the information we can get from the parent (neighbor pointers)
	tri->LeftChild->BaseNeighbor  = tri->LeftNeighbor;
	tri->LeftChild->LeftNeighbor  = tri->RightChild;

	tri->RightChild->BaseNeighbor  = tri->RightNeighbor;
	tri->RightChild->RightNeighbor = tri->LeftChild;

	// Link our Left Neighbor to the new children
	if (tri->LeftNeighbor != NULL)
	{
		if (tri->LeftNeighbor->BaseNeighbor == tri)
			tri->LeftNeighbor->BaseNeighbor = tri->LeftChild;
		else if (tri->LeftNeighbor->LeftNeighbor == tri)
			tri->LeftNeighbor->LeftNeighbor = tri->LeftChild;
		else if (tri->LeftNeighbor->RightNeighbor == tri)
			tri->LeftNeighbor->RightNeighbor = tri->LeftChild;
		else
			;// Illegal Left Neighbor!
	}

	// Link our Right Neighbor to the new children
	if (tri->RightNeighbor != NULL)
	{
		if (tri->RightNeighbor->BaseNeighbor == tri)
			tri->RightNeighbor->BaseNeighbor = tri->RightChild;
		else if (tri->RightNeighbor->RightNeighbor == tri)
			tri->RightNeighbor->RightNeighbor = tri->RightChild;
		else if (tri->RightNeighbor->LeftNeighbor == tri)
			tri->RightNeighbor->LeftNeighbor = tri->RightChild;
		else
			;// Illegal Right Neighbor!
	}

	// Link our Base Neighbor to the new children
	if (tri->BaseNeighbor != NULL)
	{
		if ( tri->BaseNeighbor->LeftChild )
		{
			tri->BaseNeighbor->LeftChild->RightNeighbor = tri->RightChild;
			tri->BaseNeighbor->RightChild->LeftNeighbor = tri->LeftChild;
			tri->LeftChild->RightNeighbor = tri->BaseNeighbor->RightChild;
			tri->RightChild->LeftNeighbor = tri->BaseNeighbor->LeftChild;
		}
		else
			Split( tri->BaseNeighbor);  // Base Neighbor (in a diamond with us) was not split yet, so do that now.
	}
	else
	{
		// An edge triangle, trivial case.
		tri->LeftChild->RightNeighbor = NULL;
		tri->RightChild->LeftNeighbor = NULL;
	}
}

// ---------------------------------------------------------------------
// Tessellate a Patch.
// Will continue to split until the variance metric is met.
//
void Patch::RecursTessellate( TriTreeNode *tri, const CameraBase*camera,
							 int leftX,  int leftY,
							 int rightX, int rightY,
							 int apexX,  int apexY,
							 int node )
{
	float TriVariance;
	int centerX = (leftX + rightX)>>1; // Compute X coordinate of center of Hypotenuse
	int centerY = (leftY + rightY)>>1; // Compute Y coord...

	if ( node < (1<<VARIANCE_DEPTH) )
	{
		// Extremely slow distance metric (sqrt is used).
		// Replace this with a faster one!

		float distance = 1.0f + sqrtf( SQR((float)centerX - camera->getPosition()[0]) +
									   SQR((float)centerY - camera->getPosition()[2]) );
		
		// Egads!  A division too?  What's this world coming to!
		// This should also be replaced with a faster operation.
		TriVariance = ((float)m_CurrentVariance[node] * MAP_SIZE * 2)/distance;	// Take both distance and variance into consideration
	}

	if ( (node >= (1<<VARIANCE_DEPTH)) ||	// IF we do not have variance info for this node, then we must have gotten here by splitting, so continue down to the lowest level.
		 (TriVariance > gFrameVariance))	// OR if we are not below the variance tree, test for variance.
	{
		Split(tri);														// Split this triangle.
		
		if (tri->LeftChild &&											// If this triangle was split, try to split it's children as well.
			((abs(leftX - rightX) >= 3) || (abs(leftY - rightY) >= 3)))	// Tessellate all the way down to one vertex per height field entry
		{
			RecursTessellate( tri->LeftChild, camera,  apexX,  apexY, leftX, leftY, centerX, centerY,    node<<1  );
			RecursTessellate( tri->RightChild, camera,rightX, rightY, apexX, apexY, centerX, centerY, 1+(node<<1) );
		}
	}
}

// ---------------------------------------------------------------------
// Render the tree.  Simple no-fan method.
//
void Patch::RecursRender( TriTreeNode *tri, int leftX, int leftY, int rightX, int rightY, int apexX, int apexY, std::vector<LVertex>&vertexs)
{
	if ( tri->LeftChild )					// All non-leaf nodes have both children, so just check for one
	{
		int centerX = (leftX + rightX)>>1;	// Compute X coordinate of center of Hypotenuse
		int centerY = (leftY + rightY)>>1;	// Compute Y coord...

		RecursRender( tri->LeftChild,  apexX,   apexY, leftX, leftY, centerX, centerY,vertexs );
		RecursRender( tri->RightChild, rightX, rightY, apexX, apexY, centerX, centerY ,vertexs);
	}
	else									// A leaf node!  Output a triangle to be rendered.
	{
		// Actual number of rendered triangles...
		gNumTrisRendered++;

		GLfloat leftZ  = m_HeightMap[(leftY *MAP_SIZE)+leftX ];
		GLfloat rightZ = m_HeightMap[(rightY*MAP_SIZE)+rightX];
		GLfloat apexZ  = m_HeightMap[(apexY *MAP_SIZE)+apexX ];

		 
		// Perform polygon coloring based on a height sample
		float fColor = (60.0f + leftZ) / 256.0f;
		if ( fColor > 1.0f )  fColor = 1.0f;

		math::V3f color(0.3, 0.5, 1.0);

		LVertex left_vertex;
		left_vertex.pos.x = leftX;
		left_vertex.pos.y = leftZ;
		left_vertex.pos.z = leftY;

		left_vertex.color = color;
		vertexs.push_back(left_vertex);

		LVertex right_vertex;
		right_vertex.pos.x = rightX;
		right_vertex.pos.y = rightZ;
		right_vertex.pos.z = rightY;
		right_vertex.color = color;
		vertexs.push_back(right_vertex);

		LVertex apex_vertex;
		apex_vertex.pos.x = apexX;
		apex_vertex.pos.y = apexZ;
		apex_vertex.pos.z = apexY;
		apex_vertex.color = color;
		vertexs.push_back(apex_vertex);
	}
}

// ---------------------------------------------------------------------
// Computes Variance over the entire tree.  Does not examine node relationships.
//
unsigned char Patch::RecursComputeVariance( int leftX,  int leftY,  unsigned char leftZ,
										    int rightX, int rightY, unsigned char rightZ,
											int apexX,  int apexY,  unsigned char apexZ,
											int node)
{
	//        /|\
	//      /  |  \
	//    /    |    \
	//  /      |      \
	//  ~~~~~~~*~~~~~~~  <-- Compute the X and Y coordinates of '*'
	//
	int centerX = (leftX + rightX) >>1;		// Compute X coordinate of center of Hypotenuse
	int centerY = (leftY + rightY) >>1;		// Compute Y coord...
	unsigned char myVariance;

	// Get the height value at the middle of the Hypotenuse
	unsigned char centerZ  = m_HeightMap[(centerY * MAP_SIZE) + centerX];

	// Variance of this triangle is the actual height at it's hypotenuse midpoint minus the interpolated height.
	// Use values passed on the stack instead of re-accessing the Height Field.
	myVariance = abs((int)centerZ - (((int)leftZ + (int)rightZ)>>1));

	// Since we're after speed and not perfect representations,
	//    only calculate variance down to an 8x8 block
	if ( (abs(leftX - rightX) >= 8) ||
		 (abs(leftY - rightY) >= 8) )
	{
		// Final Variance for this node is the max of it's own variance and that of it's children.
		myVariance = MAX( myVariance, RecursComputeVariance( apexX,   apexY,  apexZ, leftX, leftY, leftZ, centerX, centerY, centerZ,    node<<1 ) );
		myVariance = MAX( myVariance, RecursComputeVariance( rightX, rightY, rightZ, apexX, apexY, apexZ, centerX, centerY, centerZ, 1+(node<<1)) );
	}

	// Store the final variance for this node.  Note Variance is never zero.
	if (node < (1<<VARIANCE_DEPTH))
		m_CurrentVariance[node] = 1 + myVariance;

	return myVariance;
}

// -------------------------------------------------------------------------------------------------
//	PATCH CLASS
// -------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------
// Initialize a patch.
//
void Patch::Init( int heightX, int heightY, int worldX, int worldY, unsigned char *hMap )
{
	// Clear all the relationships
	m_BaseLeft.RightNeighbor = m_BaseLeft.LeftNeighbor = m_BaseRight.RightNeighbor = m_BaseRight.LeftNeighbor =
	m_BaseLeft.LeftChild = m_BaseLeft.RightChild = m_BaseRight.LeftChild = m_BaseLeft.LeftChild = NULL;

	// Attach the two m_Base triangles together
	m_BaseLeft.BaseNeighbor = &m_BaseRight;
	m_BaseRight.BaseNeighbor = &m_BaseLeft;

	// Store Patch offsets for the world and heightmap.
	m_WorldX = worldX;
	m_WorldY = worldY;

	// Store pointer to first byte of the height data for this patch.
	m_HeightMap = &hMap[heightY * MAP_SIZE + heightX];

	// Initialize flags
	m_VarianceDirty = 1;
	m_isVisible = 0;

	gl_obj.init();
}

// ---------------------------------------------------------------------
// Reset the patch.
//
void Patch::Reset()
{
	// Assume patch is not visible.
	m_isVisible = 0;

	// Reset the important relationships
	m_BaseLeft.LeftChild = m_BaseLeft.RightChild = m_BaseRight.LeftChild = m_BaseLeft.LeftChild = NULL;

	// Attach the two m_Base triangles together
	m_BaseLeft.BaseNeighbor = &m_BaseRight;
	m_BaseRight.BaseNeighbor = &m_BaseLeft;

	// Clear the other relationships.
	m_BaseLeft.RightNeighbor = m_BaseLeft.LeftNeighbor = m_BaseRight.RightNeighbor = m_BaseRight.LeftNeighbor = NULL;
}

// ---------------------------------------------------------------------
// Compute the variance tree for each of the Binary Triangles in this patch.
//
void Patch::ComputeVariance()
{
	// Compute variance on each of the base triangles...

	m_CurrentVariance = m_VarianceLeft;
	RecursComputeVariance(	0,          PATCH_SIZE, m_HeightMap[PATCH_SIZE * MAP_SIZE],
							PATCH_SIZE, 0,          m_HeightMap[PATCH_SIZE],
							0,          0,          m_HeightMap[0],
							1);

	m_CurrentVariance = m_VarianceRight;
	RecursComputeVariance(	PATCH_SIZE, 0,          m_HeightMap[ PATCH_SIZE],
							0,          PATCH_SIZE, m_HeightMap[ PATCH_SIZE * MAP_SIZE],
							PATCH_SIZE, PATCH_SIZE, m_HeightMap[(PATCH_SIZE * MAP_SIZE) + PATCH_SIZE],
							1);

	// Clear the dirty flag for this patch
	m_VarianceDirty = 0;
}

// ---------------------------------------------------------------------
// Discover the orientation of a triangle's points:
//
// Taken from "Programming Principles in Computer Graphics", L. Ammeraal (Wiley)
//
inline int orientation( int pX, int pY, int qX, int qY, int rX, int rY )
{
	int aX, aY, bX, bY;
	float d;

	aX = qX - pX;
	aY = qY - pY;

	bX = rX - pX;
	bY = rY - pY;

	d = (float)aX * (float)bY - (float)aY * (float)bX;
	return (d < 0) ? (-1) : (d > 0);
}

// ---------------------------------------------------------------------
// Set patch's visibility flag.
//
void Patch::SetVisibility(const CameraBase*camera)
{
	// Get patch's center point
	//int patchCenterX = m_WorldX + PATCH_SIZE / 2;
	//int patchCenterY = m_WorldY + PATCH_SIZE / 2;
	//
	//// Set visibility flag (orientation of both triangles must be counter clockwise)
	//m_isVisible = (orientation( eyeX,  eyeY,  rightX, rightY, patchCenterX, patchCenterY ) < 0) &&
	//			  (orientation( leftX, leftY, eyeX,   eyeY,   patchCenterX, patchCenterY ) < 0);
	m_isVisible = true;
}

// ---------------------------------------------------------------------
// Create an approximate mesh.
//
void Patch::Tessellate(const CameraBase*camera)
{
	// Split each of the base triangles
	m_CurrentVariance = m_VarianceLeft;
	RecursTessellate (	&m_BaseLeft, camera,
						m_WorldX,				m_WorldY+PATCH_SIZE,
						m_WorldX+PATCH_SIZE,	m_WorldY,
						m_WorldX,				m_WorldY,
						1 );
					
	m_CurrentVariance = m_VarianceRight;
	RecursTessellate(	&m_BaseRight, camera,
						m_WorldX+PATCH_SIZE,	m_WorldY,
						m_WorldX,				m_WorldY+PATCH_SIZE,
						m_WorldX+PATCH_SIZE,	m_WorldY+PATCH_SIZE,
						1 );
}

// ---------------------------------------------------------------------
// Render the mesh.
//
void Patch::Render()
{
	std::vector<LVertex> vertexs;
		
		RecursRender (	&m_BaseLeft,
			0,				PATCH_SIZE,
			PATCH_SIZE,		0,
			0,				0, vertexs);
		
		RecursRender(	&m_BaseRight,
			PATCH_SIZE,		0,
			0,				PATCH_SIZE,
			PATCH_SIZE,		PATCH_SIZE, vertexs);

		gl_obj.render(m_WorldX, m_WorldY, vertexs);
		
}

// -------------------------------------------------------------------------------------------------
//	LANDSCAPE CLASS
// -------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------
// Definition of the static member variables
//
int         Landscape::m_NextTriNode;
TriTreeNode Landscape::m_TriPool[POOL_SIZE];

// ---------------------------------------------------------------------
// Allocate a TriTreeNode from the pool.
//
TriTreeNode *Landscape::AllocateTri()
{
	TriTreeNode *pTri;

	// IF we've run out of TriTreeNodes, just return NULL (this is handled gracefully)
	if ( m_NextTriNode >= POOL_SIZE )
		return NULL;

	pTri = &(m_TriPool[m_NextTriNode++]);
	pTri->LeftChild = pTri->RightChild = NULL;

	return pTri;
}

// ---------------------------------------------------------------------
// Initialize all patches
//
void Landscape::Init(unsigned char *hMap)
{
	Patch *patch;
	int X, Y;

	// Store the Height Field array
	m_HeightMap = hMap;

	// Initialize all terrain patches
	for ( Y=0; Y < NUM_PATCHES_PER_SIDE; Y++)
		for ( X=0; X < NUM_PATCHES_PER_SIDE; X++ )
		{
			patch = &(m_Patches[Y][X]);
			patch->Init( X*PATCH_SIZE, Y*PATCH_SIZE, X*PATCH_SIZE, Y*PATCH_SIZE, hMap );
			patch->ComputeVariance();
		}
	GLPatch::initShader(nullptr);
}

// ---------------------------------------------------------------------
// Reset all patches, recompute variance if needed
//
void Landscape::Reset(const CameraBase* camera)
{
	 
	int X, Y;
	Patch *patch;

	// Set the next free triangle pointer back to the beginning
	SetNextTriNode(0);

	// Reset rendered triangle count.
	gNumTrisRendered = 0;

	// Go through the patches performing resets, compute variances, and linking.
	for ( Y=0; Y < NUM_PATCHES_PER_SIDE; Y++ )
		for ( X=0; X < NUM_PATCHES_PER_SIDE; X++)
		{
			patch = &(m_Patches[Y][X]);
			
			// Reset the patch
			patch->Reset();
			patch->SetVisibility(camera );
			
			// Check to see if this patch has been deformed since last frame.
			// If so, recompute the varience tree for it.
			if ( patch->isDirty() )
				patch->ComputeVariance();

			if ( patch->isVisibile() )
			{
				// Link all the patches together.
				if ( X > 0 )
					patch->GetBaseLeft()->LeftNeighbor = m_Patches[Y][X-1].GetBaseRight();
				else
					patch->GetBaseLeft()->LeftNeighbor = NULL;		// Link to bordering Landscape here..

				if ( X < (NUM_PATCHES_PER_SIDE-1) )
					patch->GetBaseRight()->LeftNeighbor = m_Patches[Y][X+1].GetBaseLeft();
				else
					patch->GetBaseRight()->LeftNeighbor = NULL;		// Link to bordering Landscape here..

				if ( Y > 0 )
					patch->GetBaseLeft()->RightNeighbor = m_Patches[Y-1][X].GetBaseRight();
				else
					patch->GetBaseLeft()->RightNeighbor = NULL;		// Link to bordering Landscape here..

				if ( Y < (NUM_PATCHES_PER_SIDE-1) )
					patch->GetBaseRight()->RightNeighbor = m_Patches[Y+1][X].GetBaseLeft();
				else
					patch->GetBaseRight()->RightNeighbor = NULL;	// Link to bordering Landscape here..
			}
		}

}

// ---------------------------------------------------------------------
// Create an approximate mesh of the landscape.
//
void Landscape::Tessellate(const CameraBase*camera)
{
	// Perform Tessellation
	int nCount;
	Patch *patch = &(m_Patches[0][0]);
	for (nCount=0; nCount < NUM_PATCHES_PER_SIDE*NUM_PATCHES_PER_SIDE; nCount++, patch++ )
	{
		if (patch->isVisibile())
			patch->Tessellate( camera);
	}
}

// ---------------------------------------------------------------------
// Render each patch of the landscape & adjust the frame variance.
//
void Landscape::Render(const CameraBase*camera)
{
	int nCount;
	GLPatch::preRender(camera);
	Patch *patch = &(m_Patches[0][0]);

	for (nCount=0; nCount < NUM_PATCHES_PER_SIDE*NUM_PATCHES_PER_SIDE; nCount++, patch++ )
	{
		if (patch->isVisibile())
			patch->Render();
	}

	GLPatch::postRender();
	// Check to see if we got close to the desired number of triangles.
	// Adjust the frame variance to a better value.
	if ( GetNextTriNode() != gDesiredTris )
		gFrameVariance += ((float)GetNextTriNode() - (float)gDesiredTris) / (float)gDesiredTris;

	// Bounds checking.
	if ( gFrameVariance < 0 )
		gFrameVariance = 0;
}

void GLPatch::init()
{
	glGenVertexArrays(1, &patch_Vao_);
	glBindVertexArray(patch_Vao_);

	glGenBuffers(1, &patch_Vbo_);
	glBindBuffer(GL_ARRAY_BUFFER, patch_Vbo_);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void GLPatch::render(int x, int z, const std::vector<LVertex>&data)
{
	glm::mat4 mode(1.0f);
	shader_->setMatrix4(model_loc_, 1, GL_FALSE, math::value_ptr(math::translateR(mode, math::V3f(x, 0.0, z))));
	glBindVertexArray(patch_Vao_);
	glBindBuffer(GL_ARRAY_BUFFER, patch_Vbo_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(LVertex) * data.size(), &data[0], GL_STATIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, data.size());
}

void GLPatch::destory()
{
	glDeleteBuffers(1, &patch_Vbo_);
	glDeleteVertexArrays(1, &patch_Vao_);
}

void GLPatch::preRender(const CameraBase*camera)
{
	shader_->turnOn();
	shader_->setMatrix4(proj_loc_, 1, GL_FALSE, math::value_ptr(camera->getProjectionMatrix()));
	shader_->setMatrix4(view_loc_, 1, GL_FALSE, math::value_ptr(camera->getViewMatrix()));
}

void GLPatch::postRender()
{
	shader_->turnOff();
}

bool GLPatch::initShader(const char*)
{
	shader_ = new Shader;
	//因为obj模型原因此处使用normal作为color
	char vertShder[] = "#version 330 core \n"
		"layout(location = 0) in vec3 position;"
		"layout(location = 1) in vec3 vcolor;"
		"layout(location = 2) in vec2 tex;"

		"out vec3 ocolor;"
		"out vec2 texCoord;"
		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"

		"void main()"
		"{"
		"	gl_Position = projection * view * model * vec4(position, 1.0f);"
		"	ocolor = vcolor;"
		"	texCoord = tex;"
		"}";

	char fragShader[] = "#version 330 core \n"
		"in vec3 ocolor;"
		"in vec2 texCoord;"
		"out vec4 color;"
		"uniform sampler2D hightmap;"
		"void main()"
		"{"
		"color = vec4(ocolor,1.0);"
		"}";

	shader_->loadShaderSouce(vertShder, fragShader, NULL);
	shader_->turnOn();

	proj_loc_ = shader_->getVariable("projection");
	view_loc_ = shader_->getVariable("view");
	model_loc_ = shader_->getVariable("model");

	return true;
}

Shader * GLPatch::shader_;

int GLPatch::proj_loc_;

int GLPatch::view_loc_;

int GLPatch::model_loc_;



