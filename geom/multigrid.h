//**************************************************************************
// File:    multigrid.h                                                    *
// Content: Classes that constitute the multigrid                          *
// Author:  Sven Gross, Joerg Peters, Volker Reichelt, IGPM RWTH Aachen    *
// Version: 0.7                                                            *
// Date:    August, 1st, 2001                                              *
// Begin:   August, 3rd, 2000                                              *
//**************************************************************************

// TODO: Use information hiding, access control and const-qualification more
//       extensively to avoid accidental changes of the multigrid structure.

#ifndef DROPS_MULTIGRID_H
#define DROPS_MULTIGRID_H

#include <list>
#include <vector>
#include <algorithm>
#include <iostream>
#include "misc/utils.h"
#include "geom/boundary.h"
#include "geom/topo.h"
#include "num/unknowns.h"


namespace DROPS
{

// Classes for simplices in the triangulation
class BoundaryCL;
class VertexCL;
class EdgeCL;
class FaceCL;
class TetraCL;

// Containers for storage of the simplices
typedef GlobalListCL<VertexCL> MG_VertexContT;
typedef GlobalListCL<EdgeCL>   MG_EdgeContT;
typedef GlobalListCL<FaceCL>   MG_FaceContT;
typedef GlobalListCL<TetraCL>  MG_TetraContT;


//**************************************************************************
// Classes: FaceWrapperCL, RecycleBinCL                                    *
// Purpose: When the triangulation is modified, some simplices will have   *
//          to be removed in one step of the algorithm, but will be needed *
//          again some steps later. In order not to lose the information   *
//          in the 'Unknowns', we store those simplices in a 'RecycleBin'. *
//          To identify a simplex in the 'RecycleBin' we use its vertices. *
//          'Edges' and 'Tetras' know their vertices, but 'faces' do not.  *
//          Therefore, we provide a 'FaceWrapper' that adds its 'Vertices' *
//          to a 'Face'.                                                   *
//**************************************************************************

struct FaceWrapperCL
{
  public:
    const FaceCL*   face;
    const VertexCL* vert1;
    const VertexCL* vert2;

    FaceWrapperCL(const FaceCL* f, const VertexCL* v1, const VertexCL* v2)
        : face(f), vert1(v1), vert2(v2) {}
};


class RecycleBinCL
{
  public:
    typedef std::list<const EdgeCL*>  EdgeContT;
    typedef std::list<FaceWrapperCL>  FaceWrapperContT;
    typedef std::list<const TetraCL*> TetraContT;

  private:
    EdgeContT        _Edges;
    FaceWrapperContT _Faces;
    TetraContT       _Tetras;

  public:
    inline const EdgeCL*  FindEdge  (const VertexCL* v) const;
    inline const FaceCL*  FindFace  (const VertexCL* v1, const VertexCL* v2) const;
    inline const TetraCL* FindTetra (const VertexCL* v1, const VertexCL* v2, const VertexCL* v3) const;

    void Recycle (const EdgeCL* ep)  { _Edges.push_back(ep); } // ??? nicht alles doppelt speichern ???
    void Recycle (const FaceCL* fp, const VertexCL* v1, const VertexCL* v2)
        { _Faces.push_back( FaceWrapperCL(fp,v1,v2) ); }
    void Recycle (const TetraCL* tp) { _Tetras.push_back(tp); }

    void DebugInfo(std::ostream&) const;
};


/*******************************************************************
*   V E R T E X  C L                                               *
*******************************************************************/
/// \brief Represents vertices in the multigrid
/** Contains the geometric part ('_Coord', '_BndVerts') of a
    point in the multigrid as well as some topological ('_Level')
    information.
    It also stores some algorithmic information like '_RemoveMark'
    and the RecycleBins.                                          */
/*******************************************************************
*   V E R T E X  C L                                               *
*******************************************************************/
class VertexCL
{
    friend class MultiGridCL;

  public:
    typedef std::vector<BndPointCL>::iterator       BndVertIt;
    typedef std::vector<BndPointCL>::const_iterator const_BndVertIt;

  private:
    IdCL<VertexCL>           _Id;                                               // id of the vertex
    Point3DCL                _Coord;                                            // global coordinates of the vertex
    std::vector<BndPointCL>* _BndVerts;                                         // Parameterdarstellung dieses Knotens auf evtl. mehreren Randsegmenten
    RecycleBinCL*            _Bin;                                              // recycle-bin
    Uint                     _Level : 8;
    bool                     _RemoveMark;                                       // flag, if this vertex should be removed

  public:
    UnknownHandleCL          Unknowns;                                          ///< access to the unknowns on the vertex

  private:
    // RecycleBin
    bool                HasRecycleBin       () const { return _Bin; }           ///< check if recycle-bin is not empty
    const RecycleBinCL* GetRecycleBin       () const { return _Bin; }
          RecycleBinCL* GetCreateRecycleBin ()       { return HasRecycleBin() ? _Bin : (_Bin= new RecycleBinCL); }

// ===== Interface for refinement algorithm =====

  public:
    inline  VertexCL (const Point3DCL& Point3D, Uint FirstLevel);               ///< create a vertex by coordinate and level
    inline  VertexCL (const VertexCL&);                                         ///< Danger!!! Copying simplices might corrupt the multigrid structure!!!
    inline ~VertexCL ();                                                        ///< also deletes the recycle-bin and boundary information

    // Boundary
    inline void AddBnd  (const BndPointCL& BndVert);                            ///< add boundary-information
    inline void BndSort ();                                                     ///< sort boundary-segments

    // RemovementMarks
    bool IsMarkedForRemovement () const { return _RemoveMark; }                 ///< check if vertex is marked for removement
    void SetRemoveMark         ()       { _RemoveMark = true; }                 ///< set mark for removement
    void ClearRemoveMark       ()       { _RemoveMark = false; }                ///< clear mark for removement

    // RecycleBin
    void DestroyRecycleBin () { delete _Bin; _Bin= 0; }                         ///< empty recycle-bin

    void Recycle (const EdgeCL* ep)                                             ///< put a pointer to an edge into the recycle-bin of this vertex
      { GetCreateRecycleBin()->Recycle(ep); }
    void Recycle (const FaceCL* fp, const VertexCL* vp1, const VertexCL* vp2)   ///< put a pointer to a face into the recycle-bin of this vertex
      { GetCreateRecycleBin()->Recycle(fp,vp1,vp2); }
    void Recycle (const TetraCL* tp)                                            ///< put a pointer to a tetra into the recycle-bin of this vertex
      { GetCreateRecycleBin()->Recycle(tp); }

    /// Find an edge in the recycle-bin by the opposite vertex. Returns 0 if no edge is found.
    EdgeCL*  FindEdge  (const VertexCL* v) const
      { return HasRecycleBin() ? const_cast<EdgeCL*>(GetRecycleBin()->FindEdge(v))          : 0; }
    /// Find a face in the recycle-bin by the other vertices. Returns 0 if no face is found.
    FaceCL*  FindFace  (const VertexCL* v1, const VertexCL* v2) const
      { return HasRecycleBin() ? const_cast<FaceCL*>(GetRecycleBin()->FindFace(v1,v2))      : 0; }
    /// Find a tetra in the recycle-bin by the other vertices. Returns 0 if no tetra is found.
    TetraCL* FindTetra (const VertexCL* v1, const VertexCL* v2, const VertexCL* v3) const
      { return HasRecycleBin() ? const_cast<TetraCL*>(GetRecycleBin()->FindTetra(v1,v2,v3)) : 0; }

// ===== Public interface =====

    const IdCL<VertexCL>& GetId           () const { return _Id; }                          ///< get id of this vertex
    Uint                  GetLevel        () const { return _Level; }                       ///< get level of vertex (=first appearance in the multigrid)
    const Point3DCL&      GetCoord        () const { return _Coord; }                       ///< get coordinate of this vertex
    bool                  IsOnBoundary    () const { return _BndVerts; }                    ///< check if this vertex lies on domain boundary
    const_BndVertIt       GetBndVertBegin () const { return _BndVerts->begin(); }
    const_BndVertIt       GetBndVertEnd   () const { return _BndVerts->end(); }
    bool                  IsInTriang      (Uint TriLevel) const                             ///< check if the vertex can be found in a triangulation level
      { return  GetLevel() <= TriLevel; }

    // Debugging
    bool IsSane    (std::ostream&, const BoundaryCL& ) const;                               ///< check for sanity of this vertex
    void DebugInfo (std::ostream&) const;                                                   ///< get debug-information
};


//****************************************************************************
// Class:   EdgeCL                                                           *
// Purpose: Represents an edge in the multigrid                              *
//          The refinement algorithm works by manipulating the marks '_MFR'  *
//          of the edges. This is possible, because the refinement pattern   *
//          of a face/tetrahedron is determined by the pattern of its edges. *
//          Marking the edges ensures consistency between the neighbors.     *
//****************************************************************************

class EdgeCL
{
  public:
    typedef MG_VertexContT::LevelCont VertContT;
    typedef MG_EdgeContT::LevelCont   EdgeContT;

  private:
    SArrayCL<VertexCL*, 2> _Vertices;
    VertexCL*              _MidVertex;
    SArrayCL<BndIdxT, 2>   _Bnd;
    short int              _MFR;
    Uint                   _Level : 8;
    bool                   _RemoveMark;

  public:
    UnknownHandleCL Unknowns;

// ===== Interface for refinement =====

    inline EdgeCL (VertexCL* vp0, VertexCL* vp1, Uint Level, BndIdxT bnd0= NoBndC, BndIdxT bnd1= NoBndC);
    EdgeCL (const EdgeCL&); // Danger!!! Copying simplices might corrupt the multigrid structure!!!
    // default dtor

    void AddBndIdx(BndIdxT idx) { if (_Bnd[0]==NoBndC) _Bnd[0]= idx; else _Bnd[1]= idx; }

    // Midvertex
    VertexCL*   GetMidVertex   ()             { return _MidVertex; }
    void        SetMidVertex   (VertexCL* vp) { _MidVertex= vp; }
    void        RemoveMidVertex()             { _MidVertex= 0; }
    void        BuildMidVertex (VertContT&, const BoundaryCL&);
    inline void BuildSubEdges  (EdgeContT&, VertContT&, const BoundaryCL&);

    // Marks
    bool IsMarkedForRef       () const { return _MFR; }
    void IncMarkForRef        ()       { ++_MFR; }
    void DecMarkForRef        ()       { --_MFR; }
    void ResetMarkForRef      ()       { _MFR= 0; }
    bool IsMarkedForRemovement() const { return _RemoveMark; }
    void SetRemoveMark        ()       { _RemoveMark= true; }
    void ClearRemoveMark      ()       { _RemoveMark= false; }

    // etc.
    void RecycleMe   () const { _Vertices[0]->Recycle(this); }
    void SortVertices()       { if (_Vertices[1]->GetId() < _Vertices[0]->GetId()) std::swap(_Vertices[0],_Vertices[1]); }

// ===== Public interface =====

    Uint            GetLevel      ()                   const { return _Level; }
    const VertexCL* GetVertex     (Uint i)             const { return _Vertices[i]; }
    const VertexCL* GetMidVertex  ()                   const { return _MidVertex; }
    const VertexCL* GetNeighbor   (const VertexCL* vp) const { return vp==_Vertices[0] ? _Vertices[1] : _Vertices[0]; }
    bool            HasVertex     (const VertexCL* vp) const { return vp==_Vertices[0] || vp==_Vertices[1]; }
    bool            IsRefined     ()                   const { return _MidVertex; }
    bool            IsOnBoundary  ()                   const { return _Bnd[0] != NoBndC; }
    const BndIdxT*  GetBndIdxBegin()                   const { return _Bnd.begin(); }
    const BndIdxT*  GetBndIdxEnd  ()                   const { return IsOnBoundary() ? (_Bnd[1] == NoBndC ? _Bnd.begin()+1 : _Bnd.end() ) : _Bnd.begin(); }
    bool            IsInTriang    (Uint TriLevel)      const
        { return GetLevel() == TriLevel || ( GetLevel() < TriLevel && !IsRefined() ); }

    // Debugging
    bool IsSane    (std::ostream&) const;
    void DebugInfo (std::ostream&) const;
};


//**************************************************************************
// Class:   FaceCL                                                         *
// Purpose: Represents a face in the multigrid                             *
//          It can have neighbors on two levels: two regular ones (one per *
//          side) on level '_Level' and two green ones on the next.        *
//**************************************************************************

class FaceCL
{
  private:
    SArrayCL<const TetraCL*,4> _Neighbors;
    const BndIdxT              _Bnd;
    Uint                       _Level : 8;
    bool                       _RemoveMark;

  public:
    UnknownHandleCL Unknowns;

// ===== Interface for refinement =====

    FaceCL (Uint Level, BndIdxT bnd= NoBndC) : _Bnd(bnd), _Level(Level), _RemoveMark(false) {}
    FaceCL (const FaceCL&); // Danger!!! Copying simplices might corrupt the multigrid structure!!!
    // default dtor

    // RemovementMarks
    bool IsMarkedForRemovement() const { return _RemoveMark; }
    void SetRemoveMark        ()       { _RemoveMark= true; }
    void ClearRemoveMark      ()       { _RemoveMark= false; }

    // Neighbors
    void LinkTetra  (const TetraCL*);
    void UnlinkTetra(const TetraCL*);

    // Recycling
    void RecycleMe(VertexCL* vp0, const VertexCL* vp1, const VertexCL* vp2) const
        { vp0->Recycle(this,vp1,vp2); }

// ===== Public Interface =====

    Uint          GetLevel     ()              const { return _Level; }
    bool          IsOnNextLevel()              const { return _Neighbors[2] || _Neighbors[3]; }
    bool          IsOnBoundary ()              const { return _Bnd != NoBndC; }
    BndIdxT       GetBndIdx    ()              const { return _Bnd; }
    inline bool   IsRefined    ()              const;
    bool          IsInTriang   (Uint TriLevel) const
        { return GetLevel() == TriLevel || (GetLevel() < TriLevel && !IsRefined() ); }

    // Get simplex
    inline const VertexCL* GetVertex(Uint)       const;
    inline const EdgeCL*   GetEdge  (Uint)       const;
    inline const TetraCL*  GetTetra (Uint, Uint) const;

    // Neighboring tetras
           const TetraCL* GetSomeTetra     () const { return _Neighbors[0]; }
    inline bool           HasNeighborTetra (const TetraCL*)       const;
    inline const TetraCL* GetNeighborTetra (const TetraCL*)       const;
           const TetraCL* GetNeighInTriang (const TetraCL*, Uint) const;
    inline Uint           GetFaceNumInTetra(const TetraCL*)       const;

    // Debugging
    bool IsSane   (std::ostream&) const;
    void DebugInfo(std::ostream&) const;
};


//**************************************************************************
// Class:   TetraCL                                                        *
// Purpose: Represents a tetrahedron                                       *
//          This is probably the most important data structure of DROPS.   *
//          All major routines that work on the grid (i.e. the refinement  *
//          algorithm, the routines to set up a discretized system, the    *
//          error estimator) "do it tetra by tetra".                       *
//**************************************************************************

class TetraCL
{
friend class MultiGridCL;

  public:
    typedef SArrayCL<VertexCL*,NumVertsC>::iterator         VertexPIterator;
    typedef SArrayCL<VertexCL*,NumVertsC>::const_iterator   const_VertexPIterator;
    typedef SArrayCL<EdgeCL*,NumEdgesC>::iterator           EdgePIterator;
    typedef SArrayCL<EdgeCL*,NumEdgesC>::const_iterator     const_EdgePIterator;
    typedef SArrayCL<FaceCL*,NumFacesC>::iterator           FacePIterator;
    typedef SArrayCL<FaceCL*,NumFacesC>::const_iterator     const_FacePIterator;
    typedef SArrayCL<TetraCL*,MaxChildrenC>::iterator       ChildPIterator;
    typedef SArrayCL<TetraCL*,MaxChildrenC>::const_iterator const_ChildPIterator;
    typedef MG_VertexContT::LevelCont                       VertContT;
    typedef MG_EdgeContT::LevelCont                         EdgeContT;
    typedef MG_FaceContT::LevelCont                         FaceContT;
    typedef MG_TetraContT::LevelCont                        TetraContT;

  private:
    // static arrays for computations
    static SArrayCL<EdgeCL*, NumAllEdgesC> _ePtrs;
    static SArrayCL<FaceCL*, NumAllFacesC> _fPtrs;

    IdCL<TetraCL> _Id;
    Uint          _Level : 8;
    Uint          _RefRule : 8; // the actual refinement of the tetrahedron
    mutable Uint  _RefMark : 8; // the refinement-mark (e.g. set by the error estimator)

    // subsimplices, parent, children
    SArrayCL<VertexCL*,NumVertsC>    _Vertices;
    SArrayCL<EdgeCL*,NumEdgesC>      _Edges;
    SArrayCL<FaceCL*,NumFacesC>      _Faces;
    TetraCL*                         _Parent;
    SArrayCL<TetraCL*,MaxChildrenC>* _Children; // for leaves: NULL-pointer
// ??? TODO: Kann man das ohne Speicherluecken und ohne Fragmentieren hinkriegen ???

  public:
    UnknownHandleCL          Unknowns;

// ===== Interface for refinement =====

    inline  TetraCL (VertexCL*, VertexCL*, VertexCL*, VertexCL*, TetraCL*);
    TetraCL (const TetraCL&); // Danger!!! Copying simplices might corrupt the multigrid structure!!!
    inline ~TetraCL ();

    // access to children, vertices
    ChildPIterator GetChildBegin  () { return _Children->begin(); }
    ChildPIterator GetChildEnd    () { return _Children->begin() + GetRefData().ChildNum; }
    VertexCL*      GetVertMidVert (Uint i)
        { return IsMidVert(i) ? _Edges[EdgeOfMidVert(i)]->GetMidVertex() : _Vertices[i]; }

    // rules and marks
    void        SetRefRule          (Uint RefRule) { _RefRule = RefRule; }
    void        RestrictMark        ();
    inline void CommitRegRefMark    () const;
    inline void UnCommitRegRefMark  () const;
    inline void Close               ();
    void        ClearAllRemoveMarks ();

    // recycling
    void RecycleMe        () const { _Vertices[0]->Recycle(this); }
    void RecycleReusables ();

    // building children
    void        CollectEdges           (const RefRuleCL&, VertContT&, EdgeContT&, const BoundaryCL&);
    void        CollectFaces           (const RefRuleCL&, FaceContT&);
    inline void LinkEdges              (const ChildDataCL&);
    inline void LinkFaces              (const ChildDataCL&);
    inline void CollectAndLinkChildren (const RefRuleCL&, TetraContT&);

    void        UnlinkFromFaces        ()    { for (Uint face=0; face<NumFacesC; ++face) _Faces[face]->UnlinkTetra(this); }

    // used by builder
    void BuildEdges        (EdgeContT&);
    void BuildAndLinkFaces (FaceContT&);
    void SetFace           (Uint, FaceCL*);

//
// Public Interface
//
    const IdCL<TetraCL>& GetId       () const { return _Id; }
    Uint                 GetLevel    () const { return _Level; }

    Uint GetRefMark    () const { return _RefMark; }
    Uint GetRefRule    () const { return _RefRule; }
    inline const RefRuleCL& GetRefData () const;

// _RefMark is mutable
    void SetRegRefMark () const { _RefMark= RegRefMarkC; }
    void SetRemoveMark () const { _RefMark= RemoveMarkC; }
    void SetNoRefMark  () const { _RefMark= NoRefMarkC; }

    bool IsMarkEqRule   () const { return _RefMark == _RefRule; }
    bool IsUnrefined    () const { return _RefRule == UnRefRuleC; }
    bool IsRegularlyRef () const { return _RefRule == RegRefRuleC; }
    bool IsRegular      () const
        { return GetLevel()!=0 ? _Parent->GetRefRule() == RegRefRuleC : true; }

    bool IsMarkedForRef        () const
        { return _RefMark != NoRefMarkC && _RefMark != RemoveMarkC; }
    bool IsMarkedForRegRef     () const { return _RefMark == RegRefMarkC; }
    bool IsMarkedForRemovement () const { return _RefMark == RemoveMarkC; }
    bool IsMarkedForNoRef      () const { return _RefMark == NoRefMarkC; }

    const_VertexPIterator GetVertBegin ()   const { return _Vertices.begin(); }
    const_VertexPIterator GetVertEnd   ()   const { return _Vertices.end(); }
    const_EdgePIterator   GetEdgesBegin()   const { return _Edges.begin(); }
    const_EdgePIterator   GetEdgesEnd  ()   const { return _Edges.end(); }
    const_FacePIterator   GetFacesBegin()   const { return _Faces.begin(); }
    const_FacePIterator   GetFacesEnd  ()   const { return _Faces.end(); }
    const VertexCL*       GetVertex(Uint i) const { return _Vertices[i]; }
    const EdgeCL*         GetEdge  (Uint i) const { return _Edges[i]; }
    const FaceCL*         GetFace  (Uint i) const { return _Faces[i]; }

    bool           IsBndSeg        (Uint face) const { return _Faces[face]->IsOnBoundary(); }
    bool           IsNeighbor      (Uint face) const { return _Faces[face]->HasNeighborTetra(this); }
    BndIdxT        GetBndIdx       (Uint face) const { return _Faces[face]->GetBndIdx(); }
    const TetraCL* GetNeighbor     (Uint face) const { return _Faces[face]->GetNeighborTetra(this); }
    const TetraCL* GetNeighInTriang(Uint face, Uint trilevel) const
                                                { return _Faces[face]->GetNeighInTriang( this, trilevel); }
    const TetraCL*       GetParent     ()          const { return _Parent; }
    const_ChildPIterator GetChildBegin ()       const { return _Children->begin(); }
    const_ChildPIterator GetChildEnd   ()       const { return _Children->begin() + GetRefData().ChildNum; }
    const TetraCL*       GetChild      (Uint i) const { return (*_Children)[i]; }
    double               GetVolume     ()       const;
    double               GetNormal     (Uint face, Point3DCL& normal, double& dir) const;
    double               GetOuterNormal(Uint face, Point3DCL& normal)              const;
    bool                 IsInTriang    (Uint TriLevel) const
        { return GetLevel() == TriLevel || ( GetLevel() < TriLevel && IsUnrefined() ); }

    bool IsSane    (std::ostream&) const;
    void DebugInfo (std::ostream&) const;
};

Point3DCL GetBaryCenter(const EdgeCL&);
Point3DCL GetBaryCenter(const FaceCL&);
Point3DCL GetBaryCenter(const TetraCL&);
Point3DCL GetBaryCenter(const TetraCL& t, Uint face);

Point3DCL GetWorldCoord(const TetraCL&, const SVectorCL<3>&);
Point3DCL GetWorldCoord(const TetraCL&, Uint face, const SVectorCL<2>&);
// barycentric coordinates:
Point3DCL GetWorldCoord(const TetraCL&, const SVectorCL<4>&);

SVectorCL<3> FaceToTetraCoord(const TetraCL& t, Uint f, SVectorCL<2> c);

//**************************************************************************
// Classes that constitute a multigrid and helpers                         *
//**************************************************************************

class MultiGridCL;
class MGBuilderCL;

template <class SimplexT>
struct TriangFillCL;

template <class SimplexT>
class TriangCL
{
  public:
    typedef std::vector<SimplexT*> LevelCont;

    typedef SimplexT**                     ptr_iterator;
    typedef const SimplexT**         const_ptr_iterator;

    typedef ptr_iter<SimplexT>             iterator;
    typedef ptr_iter<const SimplexT> const_iterator;

  private:
    mutable std::vector<LevelCont> triang_;
    MultiGridCL&                   mg_;

    inline int  StdIndex    (int lvl) const;
    inline void MaybeCreate (int lvl) const;

  public:
    TriangCL (MultiGridCL& mg);

    void clear () { triang_.clear(); }
    Uint size  (int lvl= -1) const
        { MaybeCreate( lvl); return triang_[StdIndex( lvl)].size(); }

    ptr_iterator begin (int lvl= -1)
        { MaybeCreate( lvl); return &*triang_[StdIndex( lvl)].begin(); }
    ptr_iterator end   (int lvl= -1)
        { MaybeCreate( lvl); return &*triang_[StdIndex( lvl)].end(); }
    const_ptr_iterator begin (int lvl= -1) const
        { MaybeCreate( lvl); return const_cast<const_ptr_iterator>( &*triang_[StdIndex( lvl)].begin()); }
    const_ptr_iterator end   (int lvl= -1) const
        { MaybeCreate( lvl); return const_cast<const_ptr_iterator>( &*triang_[StdIndex( lvl)].end()); }

    LevelCont&       operator[] (int lvl)
        { MaybeCreate( lvl); return triang_[StdIndex( lvl)]; }
    const LevelCont& operator[] (int lvl) const
        { MaybeCreate( lvl); return triang_[StdIndex( lvl)]; }

};

typedef  TriangCL<VertexCL> TriangVertexCL;
typedef  TriangCL<EdgeCL>   TriangEdgeCL;
typedef  TriangCL<FaceCL>   TriangFaceCL;
typedef  TriangCL<TetraCL>  TriangTetraCL;

class BoundaryCL
{
  friend class MGBuilderCL;

  private:
    std::vector<BndSegCL*> _Bnd;

  public:
    typedef std::vector<BndSegCL*> SegPtrCont;

    // deletes the objects pointed to in _Boundary
    ~BoundaryCL();

    const BndSegCL* GetBndSeg(BndIdxT idx) const { return _Bnd[idx]; }
    BndIdxT         GetNumBndSeg()         const { return _Bnd.size(); }
};

class MultiGridCL
{

  friend class MGBuilderCL;

  public:
    typedef MG_VertexContT VertexCont;
    typedef MG_EdgeContT   EdgeCont;
    typedef MG_FaceContT   FaceCont;
    typedef MG_TetraContT  TetraCont;

    typedef VertexCont::LevelCont VertexLevelCont;
    typedef EdgeCont::LevelCont   EdgeLevelCont;
    typedef FaceCont::LevelCont   FaceLevelCont;
    typedef TetraCont::LevelCont  TetraLevelCont;

    typedef VertexCont::LevelIterator             VertexIterator;
    typedef EdgeCont::LevelIterator               EdgeIterator;
    typedef FaceCont::LevelIterator               FaceIterator;
    typedef TetraCont::LevelIterator              TetraIterator;
    typedef VertexCont::const_LevelIterator const_VertexIterator;
    typedef EdgeCont::const_LevelIterator   const_EdgeIterator;
    typedef FaceCont::const_LevelIterator   const_FaceIterator;
    typedef TetraCont::const_LevelIterator  const_TetraIterator;

    typedef TriangVertexCL::iterator             TriangVertexIteratorCL;
    typedef TriangEdgeCL::iterator               TriangEdgeIteratorCL;
    typedef TriangFaceCL::iterator               TriangFaceIteratorCL;
    typedef TriangTetraCL::iterator              TriangTetraIteratorCL;
    typedef TriangVertexCL::const_iterator const_TriangVertexIteratorCL;
    typedef TriangEdgeCL::const_iterator   const_TriangEdgeIteratorCL;
    typedef TriangFaceCL::const_iterator   const_TriangFaceIteratorCL;
    typedef TriangTetraCL::const_iterator  const_TriangTetraIteratorCL;

  private:
    BoundaryCL _Bnd;
    VertexCont _Vertices;
    EdgeCont   _Edges;
    FaceCont   _Faces;
    TetraCont  _Tetras;

    TriangVertexCL _TriangVertex;
    TriangEdgeCL   _TriangEdge;
    TriangFaceCL   _TriangFace;
    TriangTetraCL  _TriangTetra;

    void PrepareModify   () { _Vertices.PrepareModify(); _Edges.PrepareModify(); _Faces.PrepareModify(); _Tetras.PrepareModify(); }
    void FinalizeModify  () { _Vertices.FinalizeModify(); _Edges.FinalizeModify(); _Faces.FinalizeModify(); _Tetras.FinalizeModify(); }
    void AppendLevel     () { _Vertices.AppendLevel(); _Edges.AppendLevel(); _Faces.AppendLevel(); _Tetras.AppendLevel(); }
    void RemoveLastLevel () { _Vertices.RemoveLastLevel(); _Edges.RemoveLastLevel(); _Faces.RemoveLastLevel(); _Tetras.RemoveLastLevel(); }

    void ClearTriangCache () { _TriangVertex.clear(); _TriangEdge.clear(); _TriangFace.clear(); _TriangTetra.clear(); }

    void RestrictMarks (Uint Level) { std::for_each( _Tetras[Level].begin(), _Tetras[Level].end(), std::mem_fun_ref(&TetraCL::RestrictMark)); }
    void CloseGrid     (Uint);
    void UnrefineGrid  (Uint);
    void RefineGrid    (Uint);

  public:
    MultiGridCL (const MGBuilderCL& Builder);
    MultiGridCL (const MultiGridCL&); // Dummy
    // default ctor

    const BoundaryCL& GetBnd     () const { return _Bnd; }
    const VertexCont& GetVertices() const { return _Vertices; }
    const EdgeCont&   GetEdges   () const { return _Edges; }
    const FaceCont&   GetFaces   () const { return _Faces; }
    const TetraCont&  GetTetras  () const { return _Tetras; }

    const TriangVertexCL& GetTriangVertex () const { return _TriangVertex; }
    const TriangEdgeCL&   GetTriangEdge   () const { return _TriangEdge; }
    const TriangFaceCL&   GetTriangFace   () const { return _TriangFace; }
    const TriangTetraCL&  GetTriangTetra  () const { return _TriangTetra; }

    VertexIterator GetVerticesBegin (int Level=-1) { return _Vertices.level_begin( Level); }
    VertexIterator GetVerticesEnd   (int Level=-1) { return _Vertices.level_end( Level); }
    EdgeIterator   GetEdgesBegin    (int Level=-1)  { return _Edges.level_begin( Level); }
    EdgeIterator   GetEdgesEnd      (int Level=-1)  { return _Edges.level_end( Level); }
    FaceIterator   GetFacesBegin    (int Level=-1) { return _Faces.level_begin( Level); }
    FaceIterator   GetFacesEnd      (int Level=-1) { return _Faces.level_end( Level); }
    TetraIterator  GetTetrasBegin   (int Level=-1) { return _Tetras.level_begin( Level); }
    TetraIterator  GetTetrasEnd     (int Level=-1) { return _Tetras.level_end( Level); }
    const_VertexIterator GetVerticesBegin (int Level=-1) const { return _Vertices.level_begin( Level); }
    const_VertexIterator GetVerticesEnd   (int Level=-1) const { return _Vertices.level_end( Level); }
    const_EdgeIterator   GetEdgesBegin    (int Level=-1) const { return _Edges.level_begin( Level); }
    const_EdgeIterator   GetEdgesEnd      (int Level=-1) const { return _Edges.level_end( Level); }
    const_FaceIterator   GetFacesBegin    (int Level=-1) const { return _Faces.level_begin( Level); }
    const_FaceIterator   GetFacesEnd      (int Level=-1) const { return _Faces.level_end( Level); }
    const_TetraIterator  GetTetrasBegin   (int Level=-1) const { return _Tetras.level_begin( Level); }
    const_TetraIterator  GetTetrasEnd     (int Level=-1) const { return _Tetras.level_end( Level); }

    VertexIterator GetAllVertexBegin (int= -1     ) { return _Vertices.begin(); }
    VertexIterator GetAllVertexEnd   (int Level=-1) { return _Vertices.level_end( Level); }
    EdgeIterator   GetAllEdgeBegin   (int= -1     ) { return _Edges.begin(); }
    EdgeIterator   GetAllEdgeEnd     (int Level=-1) { return _Edges.level_end( Level); }
    FaceIterator   GetAllFaceBegin   (int= -1     ) { return _Faces.begin(); }
    FaceIterator   GetAllFaceEnd     (int Level=-1) { return _Faces.level_end( Level); }
    TetraIterator  GetAllTetraBegin  (int= -1     ) { return _Tetras.begin(); }
    TetraIterator  GetAllTetraEnd    (int Level=-1) { return _Tetras.level_end( Level); }
    const_VertexIterator GetAllVertexBegin (int= -1     ) const { return _Vertices.begin(); }
    const_VertexIterator GetAllVertexEnd   (int Level=-1) const { return _Vertices.level_end( Level); }
    const_EdgeIterator   GetAllEdgeBegin   (int= -1     ) const  { return _Edges.begin(); }
    const_EdgeIterator   GetAllEdgeEnd     (int Level=-1) const  { return _Edges.level_end( Level); }
    const_FaceIterator   GetAllFaceBegin   (int= -1     ) const { return _Faces.begin(); }
    const_FaceIterator   GetAllFaceEnd     (int Level=-1) const { return _Faces.level_end( Level); }
    const_TetraIterator  GetAllTetraBegin  (int= -1     ) const { return _Tetras.begin(); }
    const_TetraIterator  GetAllTetraEnd    (int Level=-1) const { return _Tetras.level_end( Level); }

    TriangVertexIteratorCL GetTriangVertexBegin (int Level=-1) { return _TriangVertex.begin( Level); }
    TriangVertexIteratorCL GetTriangVertexEnd   (int Level=-1) { return _TriangVertex.end( Level); }
    TriangEdgeIteratorCL   GetTriangEdgeBegin   (int Level=-1) { return _TriangEdge.begin( Level); }
    TriangEdgeIteratorCL   GetTriangEdgeEnd     (int Level=-1) { return _TriangEdge.end( Level); }
    TriangFaceIteratorCL   GetTriangFaceBegin   (int Level=-1) { return _TriangFace.begin( Level); }
    TriangFaceIteratorCL   GetTriangFaceEnd     (int Level=-1) { return _TriangFace.end( Level); }
    TriangTetraIteratorCL  GetTriangTetraBegin  (int Level=-1) { return _TriangTetra.begin( Level); }
    TriangTetraIteratorCL  GetTriangTetraEnd    (int Level=-1) { return _TriangTetra.end( Level); }
    const_TriangVertexIteratorCL GetTriangVertexBegin (int Level=-1) const { return _TriangVertex.begin( Level); }
    const_TriangVertexIteratorCL GetTriangVertexEnd   (int Level=-1) const { return _TriangVertex.end( Level); }
    const_TriangEdgeIteratorCL   GetTriangEdgeBegin   (int Level=-1) const { return _TriangEdge.begin( Level); }
    const_TriangEdgeIteratorCL   GetTriangEdgeEnd     (int Level=-1) const { return _TriangEdge.end( Level); }
    const_TriangFaceIteratorCL   GetTriangFaceBegin   (int Level=-1) const { return _TriangFace.begin( Level); }
    const_TriangFaceIteratorCL   GetTriangFaceEnd     (int Level=-1) const { return _TriangFace.end( Level); }
    const_TriangTetraIteratorCL  GetTriangTetraBegin  (int Level=-1) const { return _TriangTetra.begin( Level); }
    const_TriangTetraIteratorCL  GetTriangTetraEnd    (int Level=-1) const { return _TriangTetra.end( Level); }

    Uint GetLastLevel() const { return _Tetras.GetNumLevel()-1; }
    Uint GetNumLevel () const { return _Tetras.GetNumLevel(); }

    void Refine ();
    void Scale( double);
    void MakeConsistentNumbering();
    void SizeInfo(std::ostream&);
    void ElemInfo(std::ostream&, int Level= -1);

    bool IsSane (std::ostream&, int Level=-1) const;
};


template <class SimplexT>
struct TriangFillCL
{
  static void // not defined
  fill (MultiGridCL& mg, typename TriangCL<SimplexT>::LevelCont& c, int lvl);
};

template <>
struct TriangFillCL<VertexCL>
{
    static void fill (MultiGridCL& mg, TriangCL<VertexCL>::LevelCont& c, int lvl);
};

template <>
struct TriangFillCL<EdgeCL>
{
    static void fill (MultiGridCL& mg, TriangCL<EdgeCL>::LevelCont& c, int lvl);
};

template <>
struct TriangFillCL<FaceCL>
{
    static void fill (MultiGridCL& mg, TriangCL<FaceCL>::LevelCont& c, int lvl);
};

template <>
struct TriangFillCL<TetraCL>
{
    static void fill (MultiGridCL& mg, TriangCL<TetraCL>::LevelCont& c, int lvl);
};


#define DROPS_FOR_TRIANG_VERTEX( mg, lvl, it) \
for (DROPS::TriangVertexCL::iterator it( mg.GetTriangVertexBegin( lvl)), end__( mg.GetTriangVertexEnd( lvl)); it != end__; ++it)

#define DROPS_FOR_TRIANG_CONST_VERTEX( mg, lvl, it) \
for (DROPS::TriangVertexCL::const_iterator it( mg.GetTriangVertexBegin( lvl)), end__( mg.GetTriangVertexEnd( lvl)); it != end__; ++it)


#define DROPS_FOR_TRIANG_EDGE( mg, lvl, it) \
for (DROPS::TriangEdgeCL::iterator it( mg.GetTriangEdgeBegin( lvl)), end__( mg.GetTriangEdgeEnd( lvl)); it != end__; ++it)

#define DROPS_FOR_TRIANG_CONST_EDGE( mg, lvl, it) \
for (DROPS::TriangEdgeCL::const_iterator it( mg.GetTriangEdgeBegin( lvl)), end__( mg.GetTriangEdgeEnd( lvl)); it != end__; ++it)


#define DROPS_FOR_TRIANG_FACE( mg, lvl, it) \
for (DROPS::TriangFaceCL::iterator it( mg.GetTriangFaceBegin( lvl)), end__( mg.GetTriangFaceEnd( lvl)); it != end__; ++it)

#define DROPS_FOR_ALL_TRIANG_CONST_FACE( mg, lvl, it) \
for (DROPS::TriangFaceCL::const_iterator FaceCL* it( mg.GetTriangFaceBegin( lvl)), end__( mg.GetTriangFaceEnd( lvl)); it != end__; ++it)


#define DROPS_FOR_TRIANG_TETRA( mg, lvl, it) \
for (DROPS::TriangTetraCL::iterator it( mg.GetTriangTetraBegin( lvl)), end__( mg.GetTriangTetraEnd( lvl)); it != end__; ++it)

#define DROPS_FOR_TRIANG_CONST_TETRA( mg, lvl, it) \
for (DROPS::TriangTetraCL::const_iterator it( mg.GetTriangTetraBegin( lvl)), end__( mg.GetTriangTetraEnd( lvl)); it != end__; ++it)


class MGBuilderCL
{
  protected:
    MultiGridCL::VertexCont& GetVertices(MultiGridCL* _MG) const { return _MG->_Vertices; }
    MultiGridCL::EdgeCont&   GetEdges   (MultiGridCL* _MG) const { return _MG->_Edges; }
    MultiGridCL::FaceCont&   GetFaces   (MultiGridCL* _MG) const { return _MG->_Faces; }
    MultiGridCL::TetraCont&  GetTetras  (MultiGridCL* _MG) const { return _MG->_Tetras; }
    BoundaryCL::SegPtrCont&  GetBnd     (MultiGridCL* _MG) const { return _MG->_Bnd._Bnd; }
    void PrepareModify  (MultiGridCL* _MG) const { _MG->PrepareModify(); }
    void FinalizeModify (MultiGridCL* _MG) const { _MG->FinalizeModify(); }
    void AppendLevel    (MultiGridCL* _MG) const { _MG->AppendLevel(); }
    void RemoveLastLevel(MultiGridCL* _MG) const { _MG->RemoveLastLevel(); }

  public:
    // default ctor
    virtual ~MGBuilderCL() {}
    virtual void build(MultiGridCL*) const = 0;
};



//**************************************************************************
//  I n l i n e   f u n c t i o n s  of the classes above                  *
//**************************************************************************


// ********** RecycleBinCL **********

inline const EdgeCL* RecycleBinCL::FindEdge (const VertexCL* v) const
{
    for (EdgeContT::const_iterator it= _Edges.begin(), end= _Edges.end(); it!=end; ++it)
        if ( (*it)->GetVertex(1) == v ) return *it;
    return 0;
}


inline const FaceCL* RecycleBinCL::FindFace (const VertexCL* v1, const VertexCL* v2) const
{
    for(FaceWrapperContT::const_iterator it= _Faces.begin(), end= _Faces.end(); it!=end; ++it)
        if ( it->vert1 == v1 && it->vert2 == v2 ) return it->face;
    return 0;
}


inline const TetraCL* RecycleBinCL::FindTetra (const VertexCL* v1, const VertexCL* v2, const VertexCL* v3) const
{
    for(TetraContT::const_iterator it= _Tetras.begin(), end= _Tetras.end(); it!=end; ++it)
        if ( (*it)->GetVertex(1) == v1 && (*it)->GetVertex(2) == v2 && (*it)->GetVertex(3) == v3 )
            return *it;
    return 0;
}


// ********** VertexCL **********

inline VertexCL::VertexCL (const Point3DCL& Coord, Uint FirstLevel)
    : _Id(), _Coord(Coord), _BndVerts(0), _Bin(0), _Level(FirstLevel),
      _RemoveMark(false) {}


inline VertexCL::VertexCL (const VertexCL& v)
    : _Id(v._Id), _Coord(v._Coord),
      _BndVerts(v._BndVerts ? new std::vector<BndPointCL>(*v._BndVerts) : 0),
      _Bin(v._Bin ? new RecycleBinCL(*v._Bin) : 0), _Level (v._Level),
      _RemoveMark(v._RemoveMark), Unknowns(v.Unknowns) {}


inline VertexCL::~VertexCL ()
{
  delete _BndVerts;
  DestroyRecycleBin();
}

inline void VertexCL::AddBnd (const BndPointCL& BndVert)
{
    if (_BndVerts)
        _BndVerts->push_back(BndVert);
    else
        _BndVerts= new std::vector<BndPointCL>(1,BndVert);
}


inline void VertexCL::BndSort ()
    { std::sort(_BndVerts->begin(), _BndVerts->end(), BndPointSegLessCL()); }


// ********** EdgeCL **********

inline EdgeCL::EdgeCL (VertexCL* vp0, VertexCL* vp1, Uint Level, BndIdxT bnd0, BndIdxT bnd1)
    : _MidVertex(0), _MFR(0), _Level(Level), _RemoveMark(false)
{
    _Vertices[0]= vp0; _Vertices[1]= vp1;
    _Bnd[0]= bnd0; _Bnd[1]= bnd1;
}


inline EdgeCL::EdgeCL (const EdgeCL& e)
    : _Vertices(e._Vertices), _MidVertex(e._MidVertex), _Bnd(e._Bnd),
      _MFR(e._MFR), _Level(e._Level), _RemoveMark(e._RemoveMark),
      Unknowns(e.Unknowns) {}


inline void EdgeCL::BuildSubEdges(EdgeContT& edgecont, VertContT& vertcont, const BoundaryCL& Bnd)
{
    BuildMidVertex(vertcont, Bnd);
    edgecont.push_back( EdgeCL(_Vertices[0], GetMidVertex(), GetLevel()+1, _Bnd[0], _Bnd[1]) );
    edgecont.push_back( EdgeCL(GetMidVertex(), _Vertices[1], GetLevel()+1, _Bnd[0], _Bnd[1]) );
}

// ********** FaceCL **********

inline FaceCL::FaceCL (const FaceCL& f)
    : _Neighbors(f._Neighbors), _Bnd(f._Bnd), _Level(f._Level),
      _RemoveMark(f._RemoveMark), Unknowns(f.Unknowns) {}


inline bool FaceCL::IsRefined () const
{
    const TetraCL* const tp= GetSomeTetra();
    return RefinesFace( tp->GetRefRule(), GetFaceNumInTetra(tp) );
}


inline bool FaceCL::HasNeighborTetra (const TetraCL* tp) const
{
    if ( IsOnBoundary() ) return false;
    if ( tp->GetLevel() == GetLevel() ) return true;
//        return _Neighbors[0]==tp ? _Neighbors[1] : _Neighbors[0];
    Assert( IsOnNextLevel() && tp->GetLevel() == GetLevel()+1,
        DROPSErrCL("FaceCL::HasNeighborTetra: Illegal Level."), DebugRefineEasyC);
    return _Neighbors[2]==tp ? _Neighbors[3] : _Neighbors[2];
}

inline const TetraCL* FaceCL::GetNeighborTetra (const TetraCL* tp) const
{
    if ( tp->GetLevel()==GetLevel() )
        return _Neighbors[0]==tp ? _Neighbors[1] : _Neighbors[0];
    Assert( IsOnNextLevel() && tp->GetLevel() == GetLevel()+1,
        DROPSErrCL("FaceCL::GetNeighborTetra: Illegal Level."), DebugRefineEasyC);
    return _Neighbors[2]==tp ? _Neighbors[3] : _Neighbors[2];
}

inline Uint FaceCL::GetFaceNumInTetra (const TetraCL* tp) const
{
    for (Uint face=0; face<NumFacesC; ++face)
        if (tp->GetFace(face) == this) return face;
    throw DROPSErrCL("FaceCL::GetFaceNumInTetra: I'm not face of given tetra!");
}

inline const VertexCL* FaceCL::GetVertex (Uint v) const
{
    const TetraCL* const tp= GetSomeTetra();
    return tp->GetVertex( VertOfFace(GetFaceNumInTetra(tp), v) );
}

inline const EdgeCL* FaceCL::GetEdge (Uint e) const
{
    const TetraCL* const tp= GetSomeTetra();
    return tp->GetEdge( EdgeOfFace(GetFaceNumInTetra(tp), e) );
}

inline const TetraCL* FaceCL::GetTetra (Uint Level, Uint side) const
{
    if ( Level == GetLevel() ) return _Neighbors[side];
    else
    {
        Assert(IsOnNextLevel(), DROPSErrCL("FaceCL::GetTetra: No such level."), DebugRefineEasyC);
        return _Neighbors[side+2];
    }
}


// T e t r a C L

inline TetraCL::TetraCL (VertexCL* vp0, VertexCL* vp1, VertexCL* vp2, VertexCL* vp3, TetraCL* Parent)
    : _Id(), _Level(Parent==0 ? 0 : Parent->GetLevel()+1),
      _RefRule(UnRefRuleC), _RefMark(NoRefMarkC),
      _Parent(Parent), _Children(0)
{
    _Vertices[0] = vp0; _Vertices[1] = vp1;
    _Vertices[2] = vp2; _Vertices[3] = vp3;
}


inline TetraCL::TetraCL (const TetraCL& T)
    : _Id(T._Id), _Level(T._Level), _RefRule(T._RefRule),
      _RefMark(T._RefMark), _Vertices(T._Vertices), _Edges(T._Edges),
      _Faces(T._Faces), _Parent(T._Parent),
      _Children(T._Children ? new SArrayCL<TetraCL*,MaxChildrenC> (*T._Children) : 0),
      Unknowns(T.Unknowns) {}


inline TetraCL::~TetraCL()
{
    if (_Children) delete _Children;
}


inline void TetraCL::CommitRegRefMark() const
{
    for (const_EdgePIterator epiter(_Edges.begin()); epiter!=_Edges.end(); ++epiter)
        (*epiter)->IncMarkForRef();
}


inline void TetraCL::UnCommitRegRefMark() const
{
    for (const_EdgePIterator epiter(_Edges.begin()); epiter!=_Edges.end(); ++epiter)
        (*epiter)->DecMarkForRef();
}


inline void TetraCL::Close()
{
    Uint mask= 1;
    Uint newrefmark= 0;

    for (const_EdgePIterator edgep( _Edges.begin() ); edgep!=_Edges.end(); ++edgep, mask<<=1)
        if ( (*edgep)->IsMarkedForRef() ) newrefmark|= mask;
    switch (newrefmark)
    {
      case RegRefMarkC:
        _RefMark= GreenRegRefMarkC;
        return;

      case NoRefMarkC:
      // if this tetra was marked for removement, it can be removed,
      // i. e. we leave the mark for removement, so that RestrictMarks
      // will catch it in the next coarser level
        if (_RefMark!=RemoveMarkC) _RefMark= newrefmark;
        return;

      default:
        _RefMark= newrefmark;
        return;
    }
}

inline void TetraCL::LinkEdges (const ChildDataCL& childdat)
{
    for (Uint edge=0; edge<NumEdgesC; ++edge)
    {
        _Edges[edge]= _ePtrs[childdat.Edges[edge]];
    }
}

inline void TetraCL::LinkFaces (const ChildDataCL& childdat)
{
    for (Uint face=0; face<NumFacesC; ++face)
    {
        _Faces[face]= _fPtrs[childdat.Faces[face]];
        _Faces[face]->LinkTetra(this);
    }
}



inline const RefRuleCL& TetraCL::GetRefData () const
{
    return DROPS::GetRefRule(this->GetRefRule() & 63);
}

template <class SimplexT>
  TriangCL<SimplexT>::TriangCL (MultiGridCL& mg)
      : triang_( mg.GetNumLevel()), mg_( mg)
{}

template <class SimplexT>
  inline int
  TriangCL<SimplexT>::StdIndex(int lvl) const
{
    return lvl >= 0 ? lvl : lvl + mg_.GetNumLevel();
}

template <class SimplexT>
  inline void
  TriangCL<SimplexT>::MaybeCreate(int lvl) const
{
    const int level= StdIndex( lvl);
    Assert ( level >= 0 && level < static_cast<int>( mg_.GetNumLevel()),
        DROPSErrCL( "TriangCL::MaybeCreate: Wrong level."), DebugContainerC);
    if (triang_.size() != mg_.GetNumLevel()) {
        triang_.clear();
        triang_.resize( mg_.GetNumLevel());
    }
    if (triang_[level].empty())
        TriangFillCL<SimplexT>::fill( mg_, triang_[level], level);
}


void circumcircle(const TetraCL& t, Point3DCL& c, double& r);
void circumcircle(const TetraCL& t, Uint face, Point3DCL& c, double& r);

// calculates the transpose of the transformation  Tetra -> RefTetra
inline void GetTrafoTr( SMatrixCL<3,3>& T, double& det, const TetraCL& t)
{
    double M[3][3];
    const Point3DCL& pt0= t.GetVertex(0)->GetCoord();
    for(int i=0; i<3; ++i)
        for(int j=0; j<3; ++j)
            M[j][i]= t.GetVertex(i+1)->GetCoord()[j] - pt0[j];
    det=   M[0][0] * (M[1][1]*M[2][2] - M[1][2]*M[2][1])
         - M[0][1] * (M[1][0]*M[2][2] - M[1][2]*M[2][0])
         + M[0][2] * (M[1][0]*M[2][1] - M[1][1]*M[2][0]);

    T(0,0)= (M[1][1]*M[2][2] - M[1][2]*M[2][1])/det;
    T(0,1)= (M[2][0]*M[1][2] - M[1][0]*M[2][2])/det;
    T(0,2)= (M[1][0]*M[2][1] - M[2][0]*M[1][1])/det;
    T(1,0)= (M[2][1]*M[0][2] - M[0][1]*M[2][2])/det;
    T(1,1)= (M[0][0]*M[2][2] - M[2][0]*M[0][2])/det;
    T(1,2)= (M[2][0]*M[0][1] - M[0][0]*M[2][1])/det;
    T(2,0)= (M[0][1]*M[1][2] - M[1][1]*M[0][2])/det;
    T(2,1)= (M[1][0]*M[0][2] - M[0][0]*M[1][2])/det;
    T(2,2)= (M[0][0]*M[1][1] - M[1][0]*M[0][1])/det;
}


void MarkAll (MultiGridCL&);
void UnMarkAll (MultiGridCL&);

} // end of namespace DROPS

#endif
