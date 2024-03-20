#pragma once

#include <fmt/core.h>
#include <spdlog/spdlog.h>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include "wkgeom_utils.h"

namespace wk ::wkgeom {

template <typename T, int DIM>
class Geometry;

template <typename T, int DIM>
class Point;

template <typename T, int DIM>
class LineString;

template <typename T, int DIM>
class LineRing;

template <typename T, int DIM>
class Polygon;

template <typename T, int DIM>
class Box;

template <typename T, int DIM>
class Geometry {
protected:
  int srid = 0;
  bool hasm = false;
  bool empty_ = true;

public:
  Geometry() {}
  Geometry(const Geometry& g) {
    this->srid = g.srid;
    this->hasm = g.hasm;
    this->empty_ = g.empty_;
  }
  Geometry<T, DIM>& operator=(const Geometry<T, DIM>& g) {
    if (this == &g) {
      return *this;
    } else {
      this->srid = g.srid;
      this->hasm = g.hasm;
      this->empty_ = g.empty_;
      return *this;
    }
  };
  ~Geometry() {}
  int getSrid() const { return this->srid; }
  void setSrid(int srid) { this->srid = srid; }
  bool getHasM() const { return this->hasm; }
  void setHasM(bool hasm) { this->hasm = hasm; }
  bool empty() const { return this->empty_; }
  virtual Box<T, DIM>* envelope() = 0;
  virtual std::string toWKT() = 0;
};

template <typename T>
class Point<T, 2> : public Geometry<T, 2> {
public:
  Point() : x(0), y(0), m(0) { this->setHasM(false); }
  Point(T x, T y) : x(x), y(y), m(0) { this->setHasM(false); }
  Point(T x, T y, T m) : x(x), y(y), m(m) { this->setHasM(true); }
  Point(const Point<T, 2>& p) : Geometry<T, 2>(p) {
    this->x = p.x;
    this->y = p.y;
    this->setHasM(p.getHasM());
    if (p.getHasM()) {
      this->m = p.m;
    }
  }
  Point& operator=(const Point& p) {
    if (this == &p) {
      return *this;
    } else {
      Geometry<T, 2>::operator=(p);
      this->x = p.x;
      this->y = p.y;
      if (p.getHasM()) {
        this->m = p.m;
      }
      return *this;
    }
  }
  ~Point() {}
  T getX() const { return this->x; }
  T getY() const { return this->y; }
  T getM() const { return this->getHasM() ? this->m : 0; }
  void setX(int x) { this->x = x; }
  void setY(int y) { this->y = y; }
  void setM(int m) {
    this->setHasM(true);
    this->m = m;
  }
  void setXY(int x, int y) {
    this->x = x;
    this->y = y;
    this->setSrid(0);
  }
  void setXYM(int x, int y, int m) {
    this->x = x;
    this->y = y;
    this->m = m;
    this->setHasM(true);
    this->setSrid(0);
  }

  virtual Box<T, 2>* envelope() { return new Box<T, 2>(this->x, this->y, this->x, this->y); }
  virtual std::string toWKT() {
    std::string s;
    if (this->getHasM()) {
      s = fmt::format("POINT M({} {} {})", this->x, this->y, this->m);
    } else {
      s = fmt::format("POINT({} {})", this->x, this->y);
    }
    return s;
  }

  template <typename U>
  friend std::ostream& operator<<(std::ostream& os, const Point<U, 2>& p) {
    os << p.x << " " << p.y << " " << p.m;
    return os;
  }

  template <typename U>
  friend bool operator==(const Point<U, 2>& p1, const Point<U, 2>& p2) {
    bool b1 = utils::dbl_equal(p1.getX(), p2.getX());
    bool b2 = utils::dbl_equal(p1.getY(), p2.getY());
    bool b3 = utils::dbl_equal(p1.getM(), p2.getM());
    return b1 && b2 && b3;
  }

private:
  T x;
  T y;
  T m;
};

template <typename T>
class Point<T, 3> : public Geometry<T, 3> {
public:
  Point() : x(0), y(0), z(0), m(0) { this->setHasM(false); }
  Point(T x, T y, T z) : x(x), y(y), z(z), m(0) { this->setHasM(false); }
  Point(T x, T y, T z, T m) : x(x), y(y), z(z), m(m) { this->setHasM(true); }
  Point(const Point<T, 3>& p) : Geometry<T, 3>(p) {
    this->x = p.x;
    this->y = p.y;
    this->z = p.z;
    this->setHasM(p.getHasM());
    if (p.getHasM()) {
      this->m = p.m;
    }
  }
  ~Point() {}
  T getX() const { return this->x; }
  T getY() const { return this->y; }
  T getZ() const { return this->z; }
  T getM() const { return this->getHasM() ? this->m : 0; }
  void setX(int x) { this->x = x; }
  void setY(int y) { this->y = y; }
  void setZ(int z) { this->z = z; }
  void setM(int m) {
    this->setHasM(true);
    this->m = m;
  }
  void setXY(T x, T y) {
    this->x = x;
    this->y = y;
    this->m = 0;
    this->setSrid(0);
  }
  void setXYZ(T x, T y, T z) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->setSrid(0);
  }
  void setXYZM(T x, T y, T z, T m) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->setHasM(true);
    this->m = m;
    this->setSrid(0);
  }

  virtual Box<T, 3>* envelope() {
    return new Box<T, 3>(this->x, this->y, this->x, this->y, this->z, this->z);
  }
  virtual std::string toWKT() {
    std::string s;
    if (this->getHasM()) {
      s = fmt::format("POINT ZM({} {} {} {})", this->x, this->y, this->z, this->m);
    } else {
      s = fmt::format("POINT Z({} {} {})", this->x, this->y, this->z);
    }
    return s;
  }

  template <typename U>
  friend std::ostream& operator<<(std::ostream& os, const Point<U, 3>& p) {
    os << p.x << " " << p.y << " " << p.z;
    if (p.getHasM()) {
      os << " " << p.m;
    }
    return os;
  }

  template <typename U>
  friend bool operator==(const Point<U, 3>& p1, const Point<U, 3>& p2) {
    bool b1 = utils::dbl_equal(p1.getX(), p2.getX());
    bool b2 = utils::dbl_equal(p1.getY(), p2.getY());
    bool b3 = utils::dbl_equal(p1.getZ(), p2.getZ());
    bool b4 = p1.getHasM() == p2.getHasM();
    bool b5 = p1.getHasM() ? dbl_equal(p1.getM(), p2.getM()) : true;
    return b1 && b2 && b3 && b4 && b5;
  }

private:
  T x;
  T y;
  T z;
  T m;
};

template <typename T, int DIM>
class LineString : public Geometry<T, DIM> {
protected:
  std::vector<Point<T, DIM>> points;

  std::string toWKTWithTag(std::string tag) {
    std::ostringstream oss;
    if (this->empty()) {
      oss << fmt::format("{} EMPTY", tag);
    } else {
      bool hasm = this->firstPoint().getHasM();
      oss << tag;
      if (hasm) {
        oss << " M";
      }
      oss << this->toPointArrayStr();
    }
    return oss.str();
  }

public:
  LineString() : Geometry<T, DIM>() {}
  LineString(std::vector<Point<T, DIM>>& points) : points(points) {
    spdlog::trace("LineString<{},{}> constructor called", typeid(T).name(), DIM);
    if (!points.empty()) {
      this->setHasM(this->firstPoint().getHasM());
      this->empty_ = false;
    } else {
      this->empty_ = true;
    }
  }
  LineString(const LineString<T, DIM>& line) : Geometry<T, DIM>(line) {
    spdlog::trace("LineString<{},{}> copy called", typeid(T).name(), DIM);
    this->empty_ = line.empty_;
    this->srid = line.srid;
    this->hasm = line.hasm;
    this->points = line.points;
  }
  LineString(LineString<T, DIM>&& line) : Geometry<T, DIM>(line) {
    spdlog::trace("LineString<{},{}> move copy called", typeid(T).name(), DIM);
    this->empty_ = line.empty_;
    this->srid = line.srid;
    this->hasm = line.hasm;
    this->points = std::move(line.points);
  }
  LineString<T, DIM>& operator=(const LineString<T, DIM>& line) {
    spdlog::trace("LineString<{},{}> operator= called", typeid(T).name(), DIM);
    if (this == &line)
      return *this;
    else {
      Geometry<T, DIM>::operator=(line);
      this->points = line.points;
      return *this;
    }
  }
  LineString<T, DIM>& operator=(const LineString<T, DIM>&& line) {
    spdlog::trace("LineString<{},{}> move operator= called", typeid(T).name(), DIM);
    if (this == &line)
      return *this;
    else {
      Geometry<T, DIM>::operator=(line);
      this->points = std::move(line.points);
      return *this;
    }
  }
  ~LineString() {}

  virtual Box<T, DIM>* envelope() {
    if (DIM == 2) {
      T xmin, ymin, xmax, ymax;
      Point<T, 2>& p = reinterpret_cast<Point<T, 2>&>(firstPoint());
      xmin = p.getX();
      xmax = p.getX();
      ymin = p.getY();
      ymax = p.getY();

      std::vector<Point<T, DIM>>& ps = this->points;
      std::vector<Point<T, 2>>& p2s = reinterpret_cast<std::vector<Point<T, 2>>&>(ps);

      for (auto it = p2s.begin() + 1; it < p2s.end(); ++it) {
        T x = it->getX();
        T y = it->getY();
        if (x < xmin) {
          xmin = x;
        }
        if (x > xmax) {
          xmax = x;
        }
        if (y < ymin) {
          ymin = y;
        }
        if (y > ymax) {
          ymax = y;
        }
      }
      return new Box<T, 2>(xmin, ymin, xmax, ymax);
    } else if (DIM == 3) {
      T xmin, ymin, xmax, ymax, zmin, zmax;
      Point<T, 3>& p = reinterpret_cast<Point<T, 3>&>(firstPoint());
      xmin = p.getX();
      xmax = p.getX();
      ymin = p.getY();
      ymax = p.getY();
      zmin = p.getZ();
      zmax = p.getZ();

      std::vector<Point<T, DIM>>& ps = this->points;
      std::vector<Point<T, 3>>& p3s = reinterpret_cast<std::vector<Point<T, 3>>&>(ps);

      for (auto it = p3s.begin() + 1; it < p3s.end(); ++it) {
        T x = it->getX();
        T y = it->getY();
        T z = it->getZ();
        if (x < xmin) {
          xmin = x;
        }
        if (x > xmax) {
          xmax = x;
        }
        if (y < ymin) {
          ymin = y;
        }
        if (y > ymax) {
          ymax = y;
        }
        if (z < zmin) {
          zmin = z;
        }
        if (z > zmax) {
          zmax = z;
        }
      }
      return reinterpret_cast<Box<T, DIM>*>(new Box<T, 3>(xmin, ymin, zmin, xmax, ymax, zmax));
    } else {
      spdlog::error("invalid dim {}, dim should be 2 or 3;", DIM);
      return nullptr;
    }
  }

  virtual std::string toWKT() {
    if (DIM == 2)
      return this->toWKTWithTag("LINESTRING");
    else if (DIM == 3) {
      return this->toWKTWithTag("LINESTRING Z");
    } else {
      spdlog::error("invalid dim {}, dim should be 2 or 3;", DIM);
      return "";
    }
  }

  Point<T, DIM>& firstPoint() { return points.front(); }

  Point<T, DIM>& lastPoint() { return points.back(); }

  int npoints() const { return points.size(); }

  Point<T, DIM>& pointAt(const int i) { return points[i]; }

  std::string toPointArrayStr() {
    std::ostringstream oss;
    bool hasm = this->firstPoint().getHasM();
    oss << "(";
    int i = 0;
    for (; i < this->npoints() - 1; ++i) {
      oss << this->pointAt(i).getX() << " " << this->pointAt(i).getY();
      if (hasm) {
        oss << " " << this->pointAt(i).getM();
      }
      oss << ",";
    }
    oss << this->pointAt(i).getX() << " " << this->pointAt(i).getY();
    if (hasm) {
      oss << " " << this->pointAt(i).getM();
    }
    oss << ")";

    return oss.str();
  }

  template <typename U, int D>
  friend std::ostream& operator<<(std::ostream& os, const LineString<U, D>& line) {
    for (const Point<U, D>& p : line.points) {
      os << p << ",";
    }
    return os;
  }
};

template <typename T, int DIM>
class LineRing : public LineString<T, DIM> {
public:
  LineRing() : LineString<T, DIM>() {}
  LineRing(std::vector<Point<T, DIM>>& points) : LineString<T, DIM>(points) {
    if (this->firstPoint() != this->lastPoint()) {
      this->points.push_back(this->firstPoint());
    }
  }
  LineRing(const LineRing<T, DIM>& ring) : LineString<T, DIM>(ring) {}
  LineRing<T, DIM>& operator=(const LineRing<T, DIM>& ring) {
    LineString<T, DIM>::operator=(ring);
    return *this;
  }
  LineRing<T, DIM>& operator=(const LineRing<T, DIM>&& ring) {
    LineString<T, DIM>::operator=(std::move(ring));
    return *this;
  }
  ~LineRing() {}

  virtual std::string toWKT() {
    if (DIM == 2) {
      return this->toWKTWithTag("LINERING");
    } else if (DIM == 3) {
      return this->toWKTWithTag("LINERING Z");
    } else {
      spdlog::error("invalid dim {}, dim should be 2 or 3;", DIM);
      return "";
    }
  }
};

template <typename T, int DIM>
class Polygon : public Geometry<T, DIM> {
protected:
  LineRing<T, DIM> exterior;
  std::vector<LineRing<T, DIM>> holes;

public:
  Polygon() : Geometry<T, DIM>() {}
  Polygon(LineRing<T, DIM> exterior, std::vector<LineRing<T, DIM>>& holes)
      : exterior(exterior), holes(holes) {
    this->empty_ = this->exterior.empty();
    if (this->exterior.empty() > 0) {
      this->setHasM(this->exterior.getHasM());
    } else {
      this->setHasM(false);
    }
  }
  Polygon(const Polygon<T, DIM>& poly) : Geometry<T, DIM>(poly) {
    this->exterior = poly.exterior;
    this->holes = poly.holes;
  }
  Polygon<T, DIM>& operator=(const Polygon<T, DIM>& poly) {
    if (this == &poly) {
      return *this;
    } else {
      Geometry<T, DIM>::operator=(poly);
      this->exterior = poly.exterior;
      this->holes = poly.holes;
    }
  }
  Polygon<T, DIM>& operator=(const Polygon<T, DIM>&& poly) {
    if (this == &poly) {
      return *this;
    } else {
      Geometry<T, DIM>::operator=(poly);
      this->exterior = std::move(poly.exterior);
      this->holes = std::move(poly.holes);
    }
  }
  ~Polygon() {}

  virtual Box<T, DIM>* envelope() { return this->exterior.envelope(); }
  virtual std::string toWKT() {
    std::ostringstream oss;
    if (this->empty()) {
      oss << "POLYGON EMPTY";
      return oss.str();
    }
    if (DIM == 2) {
      oss << "POLYGON";
    } else if (DIM == 3) {
      oss << "POLYGON Z";
    } else {
      spdlog::error("invalid dim {}, dim should be 2 or 3;", DIM);
      return "";
    }
    if (this->getHasM()) {
      oss << " M";
    }
    oss << "(";
    oss << this->exterior.toPointArrayStr();
    for (auto h : this->holes) {
      oss << "," << h.toPointArrayStr();
    }
    oss << ")";

    return oss.str();
  }

  LineRing<T, DIM>& getExterior() { return this->exterior; }
  std::vector<LineRing<T, DIM>>& getHoles() { return this->holes; }
};

template <typename T>
class Box<T, 2> : public Polygon<T, 2> {
private:
  T xmin, ymin, xmax, ymax;

public:
  Box(T xmin, T ymin, T xmax, T ymax) : Polygon<T, 2>() {
    this->xmin = xmin;
    this->ymin = ymin;
    this->xmax = xmax;
    this->ymax = ymax;
    std::vector<Point<T, 2>> pts = {
        {xmin, ymin}, {xmin, ymax}, {xmax, ymax}, {xmax, ymin}, {xmin, ymin}};

    LineRing<T, 2> exterior(pts);
    std::vector<LineRing<T, 2>> holes(0);
    this->exterior = std::move(exterior);
    this->holes = std::move(holes);
    this->empty_ = exterior.empty();
    this->setHasM(exterior.getHasM());
  }
  Box(const Box<T, 2>& b) : Polygon<T, 2>(b) {}
  Box& operator=(const Box<T, 2>* b) { return Polygon<T, 2>::operator=(b); }
  ~Box() {}

  T getXmin() const { return this->xmin; }
  T getYmin() const { return this->ymin; }
  T getXmax() const { return this->xmax; }
  T getYmax() const { return this->ymax; }

  virtual Box<T, 2>* envelope() { return new Box<T, 2>(*this); }

  friend bool operator==(const Box<T, 2>& b1, const Box<T, 2>& b2) {
    return utils::dbl_equal(b1.getXmin(), b2.getXmin()) &&
           utils::dbl_equal(b1.getYmin(), b2.getYmin()) &&
           utils::dbl_equal(b1.getXmax(), b2.getXmax()) &&
           utils::dbl_equal(b1.getYmax(), b2.getYmax());
  }
};

// TODO box<T,3> is a cube
template <typename T>
class Box<T, 3> : public Geometry<T, 3> {
private:
  T xmin, ymin, zmin, xmax, ymax, zmax;

public:
  Box(T xmin, T ymin, T zmin, T xmax, T ymax, T zmax) {
    this->xmin = xmin;
    this->ymin = ymin;
    this->zmin = zmin;
    this->xmax = xmax;
    this->ymax = ymax;
    this->zmax = zmax;
    this->setHasM(false);
  }
  Box(const Box<T, 3>& b) : Geometry<T, 3>(b) {
    this->xmin = b.xmin;
    this->ymin = b.ymin;
    this->zmin = b.zmin;
    this->xmax = b.xmax;
    this->ymax = b.ymax;
    this->zmax = b.zmax;
    this->setHasM(false);
  }
  ~Box() {}

  T getXmin() const { return this->xmin; }
  T getYmin() const { return this->ymin; }
  T getZmin() const { return this->zmin; }
  T getXmax() const { return this->xmax; }
  T getYmax() const { return this->ymax; }
  T getZmax() const { return this->zmax; }

  virtual Box<T, 3>* envelope() { return new Box<T, 3>(*this); }
  virtual std::string toWKT() { return ""; }

  friend bool operator==(const Box<T, 3>& b1, const Box<T, 3>& b2) {
    return utils::dbl_equal(b1.getXmin(), b2.getXmin()) &&
           utils::dbl_equal(b1.getYmin(), b2.getYmin()) &&
           utils::dbl_equal(b1.getZmin(), b2.getZmin()) &&
           utils::dbl_equal(b1.getXmax(), b2.getXmax()) &&
           utils::dbl_equal(b1.getYmax(), b2.getYmax()) &&
           utils::dbl_equal(b1.getZmax(), b2.getZmax());
  }
};

}  // namespace wk::wkgeom