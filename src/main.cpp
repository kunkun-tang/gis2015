#include <cstdio>
#include <fstream>
#include <vector>
#include <unordered_map>

#include "shapefil.h"

#include "timeit.h"

/* Debug level
 * 0 : brief flow info
 * 1 : verbose
 */
#define DEBUG 1

namespace gis2015 {

typedef double coord_t;

struct vertex {
  coord_t x, y;

  vertex(coord_t a = 0, coord_t b = 0)
      : x(a), y(b) { }
};

struct junction : public vertex {
  std::unordered_map<int, int> C;

  junction(coord_t a = 0, coord_t b = 0)
      : vertex(a, b) { }
};

struct road {
  int len;
  int maxspeed;

  road(int l = 0, int v = 0)
      : len(l), maxspeed(v) { }
};

struct barrier {
  std::vector<vertex> V;
};

struct graph {
  std::vector<junction> V;
  std::vector<road> E;
  std::vector<barrier> B;
};

void
cons_graph(graph &g, const std::string &barr, const std::string &junc,
           const std::string &road, const std::string &turn)
{
  SHPHandle sh;

  int nent, ntype;
  double minb[4], maxb[4];

  /* --------------------------------------------------------------
   * Read barries (convex polygon)
   *
   * TODO: it's still not clear if barriers will be in one file or
   * separate files for each barrier.
   * --------------------------------------------------------------
   */
  sh = SHPOpen(barr.c_str(), "r");
  SHPGetInfo(sh, &nent, &ntype, minb, maxb);

#if defined(DEBUG)
  printf("Read %s\n Number of Entities: %d\n ShapeType: %d\n",
         barr.c_str(), nent, ntype);
#endif

#if (1 == DEBUG)
  {
    SHPObject *o = SHPReadObject(sh, 0);
    printf("ShapeId: %d\nParts: %d\nVertices: %d\n",
           o->nShapeId, o->nParts, o->nVertices);
    printf("X:");
    for (int i = 0; i < o->nVertices; ++i)
      printf(" %f", o->padfX[i]);
    printf("\nY:");
    for (int i = 0; i < o->nVertices; ++i)
      printf(" %f", o->padfY[i]);
    printf("\n");
    SHPDestroyObject(o);
  }
#endif

#if defined(DEBUG)
  Timer tmr;
#endif

  barrier b;
  for (int i = 0; i < nent; ++i) {
    SHPObject *o = SHPReadObject(sh, i);
    b.V.push_back(vertex(o->padfX[0], o->padfY[0]));
    SHPDestroyObject(o);
  }
  g.B.push_back(b);
  SHPClose(sh);

#if defined(DEBUG)
  printf("read %s: %.4f sec\n\n", barr.c_str(), tmr.elapsed());
  tmr.reset();
#endif

  /* --------------------------------------------------------------
   * Read junction (vertex, point)
   * --------------------------------------------------------------
   */
  sh = SHPOpen(junc.c_str(), "r");
  SHPGetInfo(sh, &nent, &ntype, minb, maxb);

#if defined(DEBUG)
  printf("Read %s\n Number of Entities: %d\n ShapeType: %d\n",
         barr.c_str(), nent, ntype);
#endif

  for (int i = 0; i < nent; ++i) {
    SHPObject *o = SHPReadObject(sh, i);
    g.V.push_back(junction(o->padfX[0], o->padfY[0]));
    SHPDestroyObject(o);
  }

  SHPClose(sh);

#if defined(DEBUG)
  printf("read %s: %.4f sec\n\n", junc.c_str(), tmr.elapsed());
  tmr.reset();
#endif

  /* --------------------------------------------------------------
   * Read road (edge, line)
   * --------------------------------------------------------------
   */
  sh = SHPOpen(road.c_str(), "r");
  SHPGetInfo(sh, &nent, &ntype, minb, maxb);

#if defined(DEBUG)
  printf("Read %s\n Number of Entities: %d\n ShapeType: %d\n",
         road.c_str(), nent, ntype);
#endif

#if (1 == DEBUG)
  {
    SHPObject *o = SHPReadObject(sh, 2000);
    printf("ShapeId: %d\nParts: %d\nVertices: %d\n",
           o->nShapeId, o->nParts, o->nVertices);
    printf("X:");
    for (int i = 0; i < o->nVertices; ++i)
      printf(" %f", o->padfX[i]);
    printf("\nY:");
    for (int i = 0; i < o->nVertices; ++i)
      printf(" %f", o->padfY[i]);
    printf("\n");
    SHPDestroyObject(o);
  }
#endif

  SHPClose(sh);
}

}

int main(int argc, char *argv[])
{
  using namespace gis2015;

#if defined(DEBUG)
  Timer tmr;
#endif

  graph g;
  cons_graph(g,
             "../data/LA_nd_barriers",
             "../data/LA_nd_Junctions",
             "../data/LA_nd_roads",
             "../data/LA_nd_turns");

#if defined(DEBUG)
  printf("cons_graph: %.4f sec\n", tmr.elapsed());
#endif

  return 0;
}
