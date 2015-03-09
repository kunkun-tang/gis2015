#include <cstdio>
#include <cmath>
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

#define DEBUG_SHPOBJ(ind)                               \
  {                                                     \
    SHPObject *o = SHPReadObject(sh, ind);              \
    printf("ShapeId: %d\nParts: %d\nVertices: %d\n",    \
           o->nShapeId, o->nParts, o->nVertices);       \
    printf("X: [%f, %f]\n", o->dfXMin, o->dfXMax);      \
    for (int i = 0; i < o->nVertices; ++i)              \
      printf(" %f", o->padfX[i]);                       \
    printf("\nY: [%f, %f]\n", o->dfYMin, o->dfYMax);    \
    for (int i = 0; i < o->nVertices; ++i)              \
      printf(" %f", o->padfY[i]);                       \
    printf("\nM: [%f, %f]\n", o->dfMMin, o->dfMMax);    \
    for (int i = 0; i < o->nVertices; ++i)              \
      printf(" %f", o->padfM[i]);                       \
    printf("\n");                                       \
    SHPDestroyObject(o);                                \
  }

typedef int coord_t;

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

struct arc {
  int len;
  int maxspeed;

  arc(int l = 0, int v = 0)
      : len(l), maxspeed(v) { }
};

struct barrier {
  std::vector<vertex> V;
};

struct graph {
  std::vector<junction> V;
  std::vector<arc> E;
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
  DEBUG_SHPOBJ(0);
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
  printf("Done %s: %.4f sec\n\n", barr.c_str(), tmr.elapsed());
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
         junc.c_str(), nent, ntype);
#endif

  for (int i = 0; i < nent; ++i) {
    SHPObject *o = SHPReadObject(sh, i);
    g.V.push_back(junction(round(o->padfX[0]), round(o->padfY[0])));
    SHPDestroyObject(o);
  }

  SHPClose(sh);

#if defined(DEBUG)
  printf("Done %s: %.4f sec\n\n", junc.c_str(), tmr.elapsed());
  tmr.reset();
#endif

  /* --------------------------------------------------------------
   * Read road (arc)
   * --------------------------------------------------------------
   */
  sh = SHPOpen(road.c_str(), "r");
  SHPGetInfo(sh, &nent, &ntype, minb, maxb);

#if defined(DEBUG)
  printf("Read %s\n Number of Entities: %d\n ShapeType: %d\n",
         road.c_str(), nent, ntype);
#endif

#if (1 == DEBUG)
  DEBUG_SHPOBJ(2015);
#endif

  for (int i = 0; i < nent; ++i) {
    SHPObject *o = SHPReadObject(sh, i);
    double len = 0.;
    double x = o->padfX[0], y = o->padfY[0];

    for (int j = 1; j < o->nVertices; ++j) {
      double a = o->padfX[j] - x;
      double b = o->padfY[j] - y;
      len += sqrt(a * a + b * b);
      x = o->padfX[j];
      y = o->padfY[j];
    }

    arc e(round(len));
    g.E.push_back(e);
  }

  SHPClose(sh);

#if defined(DEBUG)
  printf("Done %s: %.4f sec\n\n", road.c_str(), tmr.elapsed());
  tmr.reset();
#endif

  /* --------------------------------------------------------------
   * Read turn
   * --------------------------------------------------------------
   */
  sh = SHPOpen(road.c_str(), "r");
  SHPGetInfo(sh, &nent, &ntype, minb, maxb);

#if defined(DEBUG)
  printf("Read %s\n Number of Entities: %d\n ShapeType: %d\n",
         turn.c_str(), nent, ntype);
#endif

#if (1 == DEBUG)
  DEBUG_SHPOBJ(2015);
#endif

  SHPClose(sh);

#if defined(DEBUG)
  printf("Done %s: %.4f sec\n\n", turn.c_str(), tmr.elapsed());
  tmr.reset();
#endif

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
