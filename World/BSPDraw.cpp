#include "../Common.h"
#include "BSPTree.h"
#include "BezierPatch.h"
#include "../OpenGL.h"

void BSPTree::DrawFace( int faceIndex, bool transparent )
{
	static const int stride = sizeof( BSPVertex_t );

	if ( drawnFaces[ faceIndex ] )
		return;

	BSPFace_t *face = &faces[ faceIndex ];

	if ( face->type == FACE_BILLBOARD )
		return;

	if ( textures[ face->texture ].surfaceFlags & SURF_SKY )
		return;

	// Defer rendering of transparent faces
	bool hasTransparency = TextureManager::Instance()->HasTransparency( hTextures[ face->texture ] );
	if ( !transparent && hasTransparency )
	{
		transparentFaces[ numTransparentFaces ] = faceIndex;
		numTransparentFaces++;
		return;
	}

	BSPVertex_t *startVert = &vertices[ face->startVertIndex ];

	// Texture map layer
	glActiveTexture( GL_TEXTURE0_ARB );
	TextureManager::Instance()->BindTexture( hTextures[ face->texture ] );

	// Lightmap layer
	glActiveTexture( GL_TEXTURE1_ARB );
	if ( face->lightmap >= 0 )
	{
		TextureManager::Instance()->BindTexture( hLightmaps[ face->lightmap ] );
		glDisable( GL_LIGHTING );
	}
	else 
	{
	    // Faces without lightmaps get a bit of faux-lighting
		TextureManager::Instance()->UnbindTexture();
		glEnable( GL_LIGHTING );
	}

	if ( face->type == FACE_PATCH )
	{
		bezierFaces[ face->startIndex ].Draw();
	}
	else
	{
		// Texture map coordinates
		glClientActiveTexture( GL_TEXTURE0_ARB );
		glTexCoordPointer( 2, GL_FLOAT, stride, startVert->textureCoord );

		// Light map coordinates
		glClientActiveTexture( GL_TEXTURE1_ARB );
		glTexCoordPointer( 2, GL_FLOAT, stride, startVert->lightmapCoord );

		// Vertex and normal arrays
		glVertexPointer( 3, GL_FLOAT, stride, &startVert->position.x );
		glNormalPointer( GL_FLOAT, stride, &startVert->normal.x );
	
		// Draw the triangles
		glDrawElements( GL_TRIANGLES, face->numIndices, GL_UNSIGNED_INT, &indices[ face->startIndex ] );
	}

	// Reset state
	glActiveTexture( GL_TEXTURE0_ARB );

	drawnFaces[ faceIndex ] = true;
}

bool BSPTree::ClusterVisible( int current, int test )
{
	if ( !visData.vecs || current < 0 )
		return true;

	char vec = visData.vecs[ (current * visData.vectorSize) + (test >> 3) ];

	int result = vec & (1 << (test & 7));

	return (result != 0);
}

void BSPTree::DrawLeaf( int leafIndex, int cluster )
{
	BSPLeaf_t *leaf = &leafs[ leafIndex ];

	if ( !ClusterVisible( cluster, leaf->cluster ) )
		return;

	if ( !viewFrustum->BoxInFrustum( leaf->mins, leaf->maxs ) )
		return;

	int faceCount = leaf->numLeafFaces;
	while ( faceCount-- )
		DrawFace( leafFaces[ leaf->startLeafFace + faceCount ], false );
}

void BSPTree::DrawNode( int nodeIndex, Vector3D &pos, int cluster )
{
	int first, last;

	if ( nodeIndex < 0 )
	{
		DrawLeaf( -( nodeIndex + 1 ), cluster );
	}
	else
	{
		BSPNode_t *node = &nodes[ nodeIndex ];

		if ( !viewFrustum->BoxInFrustum( node->mins, node->maxs ) )
			return;

		// Draw in front-to-back order
		if ( planes[ node->plane ].PointDistance2( pos ) >= 0 )
		{
			first = node->front;
			last = node->back;
		}
		else
		{
			first = node->back;
			last = node->front;
		}

		DrawNode( first, pos, cluster );
		DrawNode( last, pos, cluster );
	}
}

void BSPTree::DrawModel( int modelIndex )
{
	BSPModel_t *model = &models[modelIndex];

	int faceCount = model->numFaces;
	while ( faceCount-- )
		DrawFace( model->startFaceIndex + faceCount, true );
}

int BSPTree::LeafAtPosition( Vector3D &pos )
{
	int	i = 0;

	while ( i >= 0 )
	{
		BSPNode_t *node = &nodes[i];

		if ( planes[ node->plane ].PointDistance2( pos ) >= 0 )
			i = node->front;
		else
			i = node->back;
	}

	return -( i + 1 );
}

bool BSPTree::PositionVisible( Vector3D &cam, Vector3D &test )
{
	return ClusterVisible( leafs[ LeafAtPosition( cam ) ].cluster, leafs[ LeafAtPosition( test ) ].cluster );
}

void BSPTree::EnableClientStates()
{
	// Normal and vertex arrays
	glEnableClientState( GL_NORMAL_ARRAY );
	glEnableClientState( GL_VERTEX_ARRAY );

	// Texture Unit 1
	glActiveTexture( GL_TEXTURE1_ARB );
	glEnable( GL_TEXTURE_2D );
	glClientActiveTexture( GL_TEXTURE1_ARB );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	// Texture Unit 0
	glActiveTexture( GL_TEXTURE0_ARB );
	glEnable( GL_TEXTURE_2D );
	glClientActiveTexture( GL_TEXTURE0_ARB );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
}

void BSPTree::DisableClientStates()
{
	// Texture Unit 1
	glActiveTexture( GL_TEXTURE1_ARB );
	glClientActiveTexture( GL_TEXTURE1_ARB );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisable( GL_TEXTURE_2D );

	// Texture Unit 0
	glActiveTexture( GL_TEXTURE0_ARB );
	glClientActiveTexture( GL_TEXTURE0_ARB );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glEnable( GL_TEXTURE_2D );

	// Normal and vertex arrays
	glDisableClientState( GL_NORMAL_ARRAY );
	glDisableClientState( GL_VERTEX_ARRAY );
}

void BSPTree::Draw( Vector3D &pos, Frustum *viewFrustum )
{
	int i;

	if ( !nodes )
		return;

	BSPLeaf_t *camLeaf = &leafs[ LeafAtPosition( pos ) ];
	this->viewFrustum = viewFrustum;

	for ( i = 0; i < numFaces; i++ )
		drawnFaces[i] = false;

	numTransparentFaces = 0;

	glEnable( GL_CULL_FACE );
	glCullFace( GL_FRONT );

	// Start drawing at the first node
	EnableClientStates();
	DrawNode( 0, pos, camLeaf->cluster );
	DisableClientStates();

	glDisable( GL_CULL_FACE );
}

void BSPTree::DrawTransparentFaces()
{
	if ( !transparentFaces )
		return;

	EnableClientStates();
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glDepthMask( GL_FALSE );

	// Because we've drawn the BSP in front-to-back order, the transparent faces array 
	// is also sorted in front-to-back order.
	// By going backwards through the array, we're drawing the transparent faces in back-to-front order.
	while ( numTransparentFaces-- )
		DrawFace( transparentFaces[ numTransparentFaces ], true );

	DisableClientStates();
	glDisable( GL_BLEND );
	glDepthMask( GL_TRUE );
}

