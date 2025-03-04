// polypoint.h
//
// PolyPoint class, encapsulating a D3DTLVERTEX

// PolyPoint2D
//
// really a D3DTLVERTEX with helper functions
//
// the coordinates are stored as (x/z,y/z,1-1/z,1/z)

#ifndef _POLYPOINT_
#define _POLYPOINT_

// we copy U,V using int copies; this is because u,v are uninitialized in
// a lot of cases.  we can only hope that the driver doesn't read U,V into
// the FPU because it's not practical to check every single point where a
// POLY_Point is initialized ...
#ifdef TARGET_DC
#define	INT_COPY_FLOAT(DST,SRC)		((DST)=(SRC))
#else
#define	INT_COPY_FLOAT(DST,SRC)		*((int*)&(DST)) = *((int*)&(SRC))
#endif

class PolyPoint2D : private D3DTLVERTEX	// don't even *think* about making this a public base-class
{
public:
	// Helper functions for setup:

	void	SetSC(float sx, float sy, float sz = 0);				// Set Screen coordinates

	void	SetUV(float& u, float& v);								// Set u,v
	void	SetUV2(float u, float v);								// Set u,v

	static std::uint32_t	MakeD3DColour(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a);		// make D3D colour value
	static std::uint32_t	ModulateD3DColours(std::uint32_t c1, std::uint32_t c2);					// modulate 2 D3D colour values

	void	SetColour(std::uint32_t d3d_col);								// Set colour
	void	SetColour(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a = 0xFF);

	void	SetSpecular(std::uint32_t d3d_col);								// Set specular colour
	void	SetSpecular(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a = 0xFF);

	/*
	D3DTLVERTEX	*GetTLVert()
	{
		return ( (D3DTLVERTEX *)this );
	}
	*/
};

inline void PolyPoint2D::SetSC(float _sx, float _sy, float _sz)
{
	sx  = _sx;
	sy  = _sy;
	sz  = _sz;
	rhw = 1.0F - _sz;
}

inline void PolyPoint2D::SetUV(float& u, float& v)
{
	INT_COPY_FLOAT(tu, u);
	INT_COPY_FLOAT(tv, v);
}

inline void PolyPoint2D::SetUV2(float u, float v)
{
	INT_COPY_FLOAT(tu, u);
	INT_COPY_FLOAT(tv, v);
}

inline std::uint32_t PolyPoint2D::MakeD3DColour(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a)
{
	return (std::uint32_t(a) << 24) | (std::uint32_t(r) << 16) | (std::uint32_t(g) << 8) | std::uint32_t(b);
}

inline void PolyPoint2D::SetColour(std::uint32_t d3d_col)
{
	color = d3d_col;
}

inline void PolyPoint2D::SetColour(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a)
{
	SetColour(MakeD3DColour(r,g,b,a));
}

inline void PolyPoint2D::SetSpecular(std::uint32_t d3d_col)
{
	specular = d3d_col;
}

inline void PolyPoint2D::SetSpecular(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a)
{
	SetSpecular(MakeD3DColour(r,g,b,a));
}

// NOTE: doesn't round properly, doesn't modulate alpha

inline std::uint32_t PolyPoint2D::ModulateD3DColours(std::uint32_t c1, std::uint32_t c2)
{
	std::uint32_t	res;

	res = ((c1 & 0xFF) * (c2 & 0xFF)) >> 8;
	c1 >>= 8;	c2 >>= 8;
	res |= ((c1 & 0xFF) * (c2 & 0xFF)) & 0xFF00;
	c1 >>= 8;	c2 >>= 8;
	res |= (((c1 & 0xFF) * (c2 & 0xFF)) & 0xFF00) << 8;
	res |= (c1 & 0xFF00) << 16;

	return res;
}

#endif
