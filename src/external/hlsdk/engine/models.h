//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

//
// models.h - common model declarations for SW & GL engine and other dlls.
//

#ifndef MODELS_H
#define MODELS_H
#pragma once

//
// d*_t structures are on-disk representations
// m*_t structures are in-memory
//

#include "wad.h"

// Model versioning
#define STUDIO_VERSION			10
#define ALIAS_MODEL_VERSION		 6
#define ALIAS_VERSION			ALIAS_MODEL_VERSION // TODO: Merge these?

// Studio headers for model loading
#define IDSTUDIOHEADER			(('T'<<24)+('S'<<16)+('D'<<8)+'I') // little-endian "IDST"
#define IDSTUDIOSEQHEADER		(('Q'<<24)+('S'<<16)+('D'<<8)+'I') // little-endian "IDSQ"

#define STUDIO_NONE				0
#define STUDIO_RENDER			1
#define STUDIO_EVENTS			2

#define ALIAS_Z_CLIP_PLANE		5

// flags in finalvert_t.flags
#define ALIAS_LEFT_CLIP			0x0001
#define ALIAS_TOP_CLIP			0x0002
#define ALIAS_RIGHT_CLIP		0x0004
#define ALIAS_BOTTOM_CLIP		0x0008
#define ALIAS_Z_CLIP			0x0010
// !!! if this is changed, it must be changed in d_ifacea.h too !!!
#define ALIAS_ONSEAM			0x0020	// also defined in modelgen.h;
										//  must be kept in sync
#define ALIAS_XY_CLIP_MASK		0x000F

#define	ZISCALE					((float)0x8000)

#define CACHE_SIZE				32 // used to align key data structures

#define ALIAS_BASE_SIZE_RATIO	(1.0 / 11.0)

// normalizing factor so player model works out to about
//  1 pixel per triangle
#define MAX_LBM_HEIGHT			480
#define MAX_ALIAS_MODEL_VERTS	2000

#define MAX_DL_EDGES			64

#define MAX_MDL_VERTS			(MAX_DL_EDGES - 4) // 60
#define MAX_MDL_CLIP_VERTS		(MAX_DL_EDGES + 4) // 64

#define MAX_MOD_KNOWN			1024

// Background ambient sounds
#define NUM_AMBIENTS			4

#define NUMVERTEXNORMALS		162

struct texture_t;

struct mtexinfo_t
{
	float				vecs[2][4];	// [s/t] unit vectors in world space. 
	// [i][3] is the s/t offset relative to the origin.
	// s or t = dot(3Dpoint,vecs[i])+vecs[i][3]
	float				mipadjust;	// ?? mipmap limits for very small surfaces
	texture_t*			texture;
	int					flags;		// sky or slime, no lightmap or 256 subdivision
};

#define	SIDE_FRONT		 0
#define	SIDE_BACK		 1
#define	SIDE_ON			 2
#define	SIDE_NUM		 3
#define	SIDE_CROSS		-2

struct mplane_t
{
	vec3_t				normal;			// surface normal
	float				dist;			// closest appoach to origin
	unsigned char		type;			// for texture axis selection and fast side tests
	// PLANE_* constants
	unsigned char		signbits;		// signx + signy<<1 + signz<<1
	unsigned char		pad[2];
};

struct mnode_t
{
	// common with leaf
	int					contents;	// 0, to differentiate from leafs
	int					visframe;	// node needs to be traversed if current

#if defined(GLQUAKE)
	float				minmaxs[6];	// for bounding box culling
#else
	short				minmaxs[6];
#endif

	mnode_t*			parent;

	// node specific	
	mplane_t*			plane;
	mnode_t*			children[2];

	unsigned short		firstsurface;
	unsigned short		numsurfaces;
};

#if defined(GLQUAKE)

#define	VERTEXSIZE 7
struct glpoly_t
{
	glpoly_t*			next;
	glpoly_t*			chain;
	int					numverts;
	int					flags;							// for SURF_UNDERWATER
	float				verts[4][VERTEXSIZE];	// variable sized (xyz s1t1 s2t2)
};

#endif // GLQUAKE

struct decal_t;

struct msurface_t
{
	int					visframe;			// should be drawn when node is crossed

#if !defined(GLQUAKE)
	int					dlightframe;		// last frame the surface was checked by an animated light
	int					dlightbits;			// dynamically generated. Indicates if the surface illumination 
	// is modified by an animated light.
#endif

	mplane_t*			plane;				// pointer to shared plane
	int					flags;				// see SURF_ #defines

	int					firstedge;			// look up in model->surfedges[], negative numbers
	int					numedges;			// are backwards edges

#if !defined(GLQUAKE)
	struct surfcache_t* cachespots[MIP_LEVELS]; // surface generation data
#endif

	short				texturemins[2];		// smallest s/t position on the surface.
	short				extents[2];			// ?? s/t texture size, 1..512 for all non-sky surfaces

#if defined(GLQUAKE)
	int					light_s, light_t;	// gl lightmap coordinates

	glpoly_t*			polys;				// multiple if warped
	msurface_t*			texturechain;
#endif

	mtexinfo_t*			texinfo;

#if defined(GLQUAKE)
	// lighting info
	int					dlightframe;		// indicates the last frame in which dlights illuminated this surface
	int					dlightbits;			// indicates which dlights illuminates this surface.

	int					lightmaptexturenum;
#endif

	// lighting info
	unsigned char		styles[MAX_LIGHTMAPS]; // index into d_lightstylevalue[] for animated lights 
	// no one surface can be effected by more than 4 
	// animated lights.

#if defined(GLQUAKE)
	int					cached_light[MAX_LIGHTMAPS];// values currently used in lightmap
	qboolean			cached_dlight;				// true if dynamic light in cache
#endif

	// Baked light information, used for lightmaps
	color24*			samples;			// [numstyles*surfsize]

	decal_t*			pdecals;
};

typedef struct
{
	float				fv[3]; // viewspace x, y
} auxvert_t;

typedef struct
{
	char*	name;
	short	entityIndex;
	byte	depth;
	byte	flags;
	vec3_t	position;
} DECALLIST;

// Used in qsort for DecalDepthCompare callback.
typedef int (*qsortFunc_t)(const void*, const void*);

//#include "cl_entity.h"

#define MODEL_MAX_TEXTURE_NAME	16

#define MODEL_ANIM_CYCLE		2

struct texture_t
{
	char				name[MODEL_MAX_TEXTURE_NAME];

	unsigned int		width;
	unsigned int		height;

#if defined(GLQUAKE)
	int					gl_texturenum;
	msurface_t*			texturechain;
#endif

	int					anim_total;				// total tenths in sequence ( 0 = no)
	int					anim_min, anim_max;		// time for this frame min <=time< max
	texture_t*			anim_next;				// in the animation sequence
	texture_t*			alternate_anims;		// bmodels in frame 1 use these

	unsigned int		offsets[MIP_LEVELS];		// four mip maps stored

#if defined(GLQUAKE)
	unsigned char*		pPal;					// Pointer to palette entry
#else
	unsigned			paloffset;
#endif
};

struct decal_t
{
	decal_t				*pnext;			// linked list for each surface
	msurface_t			*psurface;		// Surface id for persistence / unlinking
#if defined(GLQUAKE)
	float				dx;			// Offsets into surface texture (in texture coordinates, so we don't need floats)
	float				dy;
	int 				scale;			// Pixel scale
	float				texture;		// Decal texture
	short				flags;			// Decal flags
#else
	short				dx;
	short				dy;
	short				texture;
	unsigned char 		scale;			// Pixel scale
	unsigned char		flags;			// Decal flags
#endif

	short				entityIndex;	// Entity this is attached to
};

struct efrag_t;

typedef struct
{
	// common with node
	int					contents;		// wil be a negative contents number
	int					visframe;		// node needs to be traversed if current

#if defined(GLQUAKE)
	float				minmaxs[6];		// for bounding box culling
#else
	short				minmaxs[6];
#endif

	mnode_t*			parent;

	// leaf specific
	unsigned char*		compressed_vis;
	efrag_t*			efrags;

	msurface_t**		firstmarksurface;
	int					nummarksurfaces;
	int					key;			// BSP sequence number for leaf's contents
	unsigned char		ambient_sound_level[NUM_AMBIENTS];
} mleaf_t;

typedef struct
{
	int					planenum;
	short				children[2];	// negative numbers are contents
} dclipnode_t;

typedef struct
{
	dclipnode_t*		clipnodes;
	mplane_t*			planes;
	int					firstclipnode;
	int					lastclipnode;
	vec3_t				clip_mins;
	vec3_t				clip_maxs;
} hull_t;

struct mvertex_t
{
	vec3_t				position;
};

struct medge_t
{
	unsigned short		v[2];
	unsigned int		cachededgeoffset;
};

//=============================================================================
// 
// 
// BRUSH MODELS
//
//	Structure definitions for the BSP file format.
// 
//=============================================================================

//
// GoldSource brush file limits
//

#define	MAX_MAP_MODELS			400
#define	MAX_MAP_BRUSHES			4096
#define	MAX_MAP_ENTITIES		1024
#define	MAX_MAP_ENTSTRING		(128*1024)
#define	MAX_MAP_PLANES			32767

#define	MAX_MAP_NODES			32767		// because negative shorts are contents
#define	MAX_MAP_CLIPNODES		32767
#define	MAX_MAP_LEAFS			8192
#define	MAX_MAP_VERTS			65535
#define	MAX_MAP_FACES			65535
#define	MAX_MAP_MARKSURFACES	65535
#define	MAX_MAP_TEXINFO			8192
#define	MAX_MAP_EDGES			256000
#define	MAX_MAP_SURFEDGES		512000
#define	MAX_MAP_TEXTURES		512
#define	MAX_MAP_MIPTEX			0x200000	// ~2.1 MB
#define	MAX_MAP_LIGHTING		0x200000	// ~2.1 MB
#define	MAX_MAP_VISIBILITY		0x200000	// ~2.1 MB

#define	MAX_MAP_PORTALS			65536

// BSP versioning
#define Q1BSP_VERSION			29
#define BSPVERSION				30
#define	TOOLVERSION				 2

// key / value pair sizes
#define	MAX_KEY					32
#define	MAX_VALUE				1024

// Lump is a data structure used to locate entries inside BSP file.
// Each entry contains its own data.
typedef struct
{
	int		fileofs, filelen;
} lump_t;

// Lump data for BSP file format
#define	LUMP_ENTITIES_BSHIFT	1	// bshift specific
#define	LUMP_PLANES_BSHIFT		0	// bshift specific
#define	LUMP_ENTITIES			0
#define	LUMP_PLANES				1
#define	LUMP_TEXTURES			2
#define	LUMP_VERTEXES			3
#define	LUMP_VISIBILITY			4
#define	LUMP_NODES				5
#define	LUMP_TEXINFO			6
#define	LUMP_FACES				7
#define	LUMP_LIGHTING			8
#define	LUMP_CLIPNODES			9
#define	LUMP_LEAFS				10
#define	LUMP_MARKSURFACES		11
#define	LUMP_EDGES				12
#define	LUMP_SURFEDGES			13
#define	LUMP_MODELS				14
#define	HEADER_LUMPS			15

#define DMIPTEX_INVALID_DATAOFS -1

// BSP format file header
typedef struct
{
	int			version;
	lump_t		lumps[HEADER_LUMPS];
} dheader_t;

typedef struct
{
	int			nummiptex;
	int			dataofs[4]; // [nummiptex]
} dmiptexlump_t;

typedef struct
{
	char		name[16];
	unsigned	width, height;
	unsigned	offsets[MIP_LEVELS]; // four mip maps stored
} miptex_t;

typedef struct
{
	float		point[3];
} dvertex_t;

// 0-2 are axial planes
#define	PLANE_X					0
#define	PLANE_Y					1
#define	PLANE_Z					2

//3-5 are non-axial planes snapped to the nearest
#define	PLANE_ANYX				3
#define	PLANE_ANYY				4
#define	PLANE_ANYZ				5

#define	CONTENTS_NONE			0
#define	CONTENTS_EMPTY			-1
#define	CONTENTS_SOLID			-2		// Solid object
#define	CONTENTS_WATER			-3
#define	CONTENTS_SLIME			-4
#define	CONTENTS_LAVA			-5
#define	CONTENTS_SKY			-6
#define	CONTENTS_ORIGIN			-7		// removed at csg time
#define	CONTENTS_CLIP			-8		// changed to contents_solid
#define	CONTENTS_CURRENT_0		-9
#define	CONTENTS_CURRENT_90		-10
#define	CONTENTS_CURRENT_180	-11
#define	CONTENTS_CURRENT_270	-12
#define	CONTENTS_CURRENT_UP		-13
#define	CONTENTS_CURRENT_DOWN	-14
#define CONTENTS_TRANSLUCENT	-15

// Alias model header
#define IDPOLYHEADER			(('O'<<24)+('P'<<16)+('D'<<8)+'I') // little-endian "IDPO"

typedef struct
{
	float		normal[3];
	float		dist;
	int			type;		// PLANE_X - PLANE_ANYZ ?remove? trivial to regenerate
} dplane_t;

typedef struct
{
	float		mins[3], maxs[3];
	float		origin[3];
	int			headnode[MAX_HULLS];
	int			visleafs; // not including the solid leaf 0
	int			firstface, numfaces;
} dmodel_t;

typedef struct
{
	int			planenum;
	short		children[2];	// negative numbers are -(leafs+1), not nodes
	short		mins[3];		// for sphere culling
	short		maxs[3];
	ushort		firstface;
	ushort		numfaces;	// counting both sides
} dnode_t;

typedef struct
{
	float		vecs[2][4];		// [s/t][xyz offset]
	int			miptex;
	int			flags;
} texinfo_t;

// sky or slime, no lightmap or 256 subdivision
#define	TEX_SPECIAL		1

// note that edge 0 is never used, because negative edge nums are used for
// counterclockwise use of the edge in a face
typedef struct
{
	unsigned short v[2];		// vertex numbers
} dedge_t;

typedef struct
{
	short		planenum;
	short		side;

	int			firstedge;		// we must support > 64k edges
	short		numedges;
	short		texinfo;

	// lighting info
	byte		styles[MAX_LIGHTMAPS];
	int			lightofs;		// start of [numstyles*surfsize] samples
} dface_t;

// Automatic ambient sounds
#define	AMBIENT_WATER	0
#define	AMBIENT_SKY		1
#define	AMBIENT_SLIME	2
#define	AMBIENT_LAVA	3

// leaf 0 is the generic CONTENTS_SOLID leaf, used for all solid areas
//  all other leafs need visibility info
typedef struct
{
	int			contents;
	int			visofs;				// -1 = no visibility info

	short		mins[3];			// for frustum culling
	short		maxs[3];

	ushort		firstmarksurface;
	ushort		nummarksurfaces;

	byte		ambient_level[NUM_AMBIENTS];
} dleaf_t;

// Angle axis definition
#define	ANGLE_UP	-1
#define	ANGLE_DOWN	-2

typedef enum
{
	ALIAS_SINGLE = 0,
	ALIAS_GROUP
} aliasframetype_t;

typedef enum
{
	ALIAS_SKIN_SINGLE = 0,
	ALIAS_SKIN_GROUP
} aliasskintype_t;

#include "spritegn.h"

// Q1 oldschool model format
typedef struct
{
	int			ident;
	int			version;
	vec3_t		scale;
	vec3_t		scale_origin;
	float		boundingradius;
	vec3_t		eyeposition;
	int			numskins;
	int			skinwidth;
	int			skinheight;
	int			numverts;
	int			numtris;
	int			numframes;
	synctype_t	synctype;
	int			flags;
	float		size;
} mdl_t;

typedef struct
{
	int			onseam;
	int			s;
	int			t;
} stvert_t;

typedef struct
{
	int			facesfront;
	int			vertindex[3];
} dtriangle_t;

#define DT_FACES_FRONT	0x0010

//
// This mirrors trivert_t in trilib.h, is present so Quake knows how to
// load this data
//

typedef struct
{
	byte		v[3];
	byte		lightnormalindex;
} trivertx_t;

typedef struct
{
	trivertx_t	bboxmin;	// lightnormal isn't used
	trivertx_t	bboxmax;	// lightnormal isn't used
	char		name[16];	// frame name from grabbing
} daliasframe_t;

typedef struct
{
	int			numframes;
	trivertx_t	bboxmin;	// lightnormal isn't used
	trivertx_t	bboxmax;	// lightnormal isn't used
} daliasgroup_t;

typedef struct
{
	int			numskins;
} daliasskingroup_t;

typedef struct
{
	float		interval;
} daliasinterval_t;

typedef struct
{
	float		interval;
} daliasskininterval_t;

typedef struct
{
	aliasframetype_t	type;
} daliasframetype_t;

typedef struct
{
	aliasskintype_t		type;
} daliasskintype_t;

#define SURF_NONE				0
#define SURF_PLANEBACK			(1 << 1)
#define SURF_DRAWSKY			(1 << 2) // Skybox polys
#define SURF_DRAWSPRITE			(1 << 3) // Sprite polys
#define SURF_DRAWTURB			(1 << 4) // Uses turbsin - for water waves
#define SURF_DRAWTILED			(1 << 5) // Textures with scroll
#define SURF_DRAWBACKGROUND		(1 << 6)
#define SURF_UNDERWATER			(1 << 7) // Water polys
#define SURF_DONTWARP			(1 << 8)

//=============================================================================
// 
// 
// SPRITE MODELS
//
// 
//=============================================================================

typedef struct
{
	int					width, height;

#if !defined(GLQUAKE)
	void*				pcachespot;
#endif

	float				up, down, left, right; // top left, top right, bottom right, bottom left

#if defined(GLQUAKE)
	int					gl_texturenum;
#endif

#if !defined(GLQUAKE)
	byte				pixels[4];
#endif
} mspriteframe_t;

typedef struct
{
	int					numframes;
	float*				intervals;
	mspriteframe_t*		frames[1];
} mspritegroup_t;

typedef struct
{
	spriteframetype_t	type;
	mspriteframe_t*		frameptr;
} mspriteframedesc_t;

// Sprite type
#define SPR_VP_PARALLEL_UPRIGHT		0
#define SPR_FACING_UPRIGHT			1
#define SPR_VP_PARALLEL				2
#define SPR_ORIENTED				3
#define SPR_VP_PARALLEL_ORIENTED	4

// Texture format
#define SPR_NORMAL					0
#define SPR_ADDITIVE				1
#define SPR_INDEXALPHA				2
#define SPR_ALPHTEST				3

typedef struct
{
	short				type;		// SPR_VP_PARALLEL_UPRIGHT, SPR_FACING_UPRIGHT, etc
	short				texFormat;	// SPR_NORMAL, SPR_ADDITIVE, etc
	int					maxwidth;
	int					maxheight;
	int					numframes;	// # sprite frames
	int					paloffset;	// offset from start of buffer to palette entries
	float				beamlength;	// remove?
	void*				cachespot;	// remove?
	mspriteframedesc_t	frames[1];	// anysize array for frames
} msprite_t;

//=============================================================================
//
// 
// ALIAS MODELS
//
//	Alias models are position independent, so the cache manager can move them.
// 
//
//=============================================================================

typedef struct
{
#if defined(GLQUAKE)
	int					firstpose;
	int					numposes;
	float				interval;
#else
	aliasframetype_t	type;
#endif // GLQUAKE
	trivertx_t			bboxmin;
	trivertx_t			bboxmax;
	int					frame;
	char				name[16];
} maliasframedesc_t;

typedef struct
{
	aliasskintype_t		type;
	void*				pcachespot;
	int					skin;
} maliasskindesc_t;

#if !defined(GLQUAKE)
typedef struct
{
	int					numskins;
	int					intervals;
	maliasskindesc_t	skindescs[1];
} maliasskingroup_t;
#endif

typedef struct
{
	trivertx_t			bboxmin;
	trivertx_t			bboxmax;
	int					frame;
} maliasgroupframedesc_t;

typedef struct
{
	int					numframes;
	int					intervals;
	maliasgroupframedesc_t frames[1];
} maliasgroup_t;

typedef struct
{
	int					facesfront;
	int					vertindex[3];
} mtriangle_t;

#define MAX_SKINS 32

typedef struct
{
#if defined(GLQUAKE)
	int					ident;
	int					version;
	vec3_t				scale;
	vec3_t				scale_origin;
	float				boundingradius;
	vec3_t				eyeposition;
	int					numskins;
	int					skinwidth;
	int					skinheight;
	int					numverts;
	int					numtris;
	int					numframes;
	synctype_t			synctype;
	int					flags;
	float				size;
	int					numposes;
	int					poseverts;
	int					posedata;	// numposes*poseverts trivert_t
	int					commands;	// gl command list with embedded s/t
	int					gl_texturenum[MAX_SKINS];
#else
	int					model;
	int					stverts;
	int					skindesc;
	int					triangles;
	int					palette;
#endif // GLQUAKE
	maliasframedesc_t	frames[1];	// variable sized
} aliashdr_t;

#define	MAX_ALIAS_VERTS			1024
#define	MAX_ALIAS_FRAMES		256
#define	MAX_ALIAS_TRIS			2048
extern	aliashdr_t* pheader;
extern	stvert_t	stverts[MAX_ALIAS_VERTS];
extern	mtriangle_t	triangles[MAX_ALIAS_TRIS];
extern	trivertx_t* poseverts[MAX_ALIAS_FRAMES];

//=============================================================================
// 
// 
//  Whole model
// 
// 
//=============================================================================

// Model type for model_t
typedef enum
{
	mod_bad = -1, // Note: This actually doesn't exist on GS, but it's a convenience
	mod_brush,
	mod_sprite,
	mod_alias,
	mod_studio
} modtype_t;

// Needload flags for model_t
typedef enum
{
	NL_PRESENT = 0,		// The model is already present
	NL_NEEDS_LOADED,	// The model needs to be loaded
	NL_UNREFERENCED		// The model is unreferenced
} needload_t;

#define MODEL_MAX_NAME 64

struct model_t
{
	char				name[MODEL_MAX_NAME];
	int					needload; // bmodels and sprites don't cache normally

	modtype_t			type;
	int					numframes;
	synctype_t			synctype;

	int					flags;

	//
	// volume occupied by the model
	//
	vec3_t				mins, maxs;
	float				radius;

	//
	// brush model
	//
	int					firstmodelsurface, nummodelsurfaces;

	int					numsubmodels;
	dmodel_t			*submodels;

	int					numplanes;
	mplane_t			*planes;

	int					numleafs; // number of visible leafs, not counting 0
	mleaf_t				*leafs;

	//
	// Continuous list of vertexes in memory
	//
	int					numvertexes;
	mvertex_t			*vertexes;

	int					numedges;
	medge_t				*edges;

	int					numnodes;
	mnode_t				*nodes;

	int					numtexinfo;
	mtexinfo_t			*texinfo;

	int					numsurfaces;
	msurface_t			*surfaces;

	int					numsurfedges;
	int					*surfedges;

	int					numclipnodes;
	dclipnode_t			*clipnodes;

	int					nummarksurfaces;
	msurface_t			**marksurfaces;

	hull_t				hulls[MAX_HULLS];

	// Table of heap-allocated texture pointers. These are allocated upon
	// texture loading.
	int					numtextures;
	texture_t			**textures;

	unsigned char		*visdata;

	color24				*lightdata;

	char				*entities;

	//
	// additional model data
	//
	cache_user_t		cache; // only access through Mod_Extradata
};

extern model_t* loadmodel;

// Generous model index for world
#define WORLD_MODEL 1

//============================================================================

#if !defined(GLQUAKE)
void SW_Mod_Init();
#endif

void		Mod_ClearAll();
void		Mod_Print();
mleaf_t*	Mod_PointInLeaf(vec_t* p, model_t* model);
model_t*	Mod_LoadModel(model_t* mod, qboolean crash, qboolean trackCRC);
void		Mod_LoadStudioModel(model_t* mod, void* buffer);
model_t*	Mod_FindName(qboolean trackCRC, const char* name);
model_t*	Mod_ForName(const char* name, qboolean crash, qboolean trackCRC);
void		Mod_MarkClient(model_t* pModel);
void		Mod_UnloadSpriteTextures(model_t* pModel);
qboolean	Mod_ValidateCRC(char* name, CRC32_t crc);
void		Mod_FillInCRCInfo(qboolean trackCRC, int model_number);
void		Mod_NeedCRC(char* name, qboolean needCRC);
void		Mod_SpriteTextureName(char* pszName, int nNameSize, const char* pcszModelName, int framenum);

#endif // MODELS_H
