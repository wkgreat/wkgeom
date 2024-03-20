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

// 2D

template <typename T>
class Point<T, 2>;

template <typename T>
class LineString<T, 2>;

template <typename T>
class LineRing<T, 2>;

template <typename T>
class Polygon<T, 2>;

template <typename T>
class Box<T, 2>;

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
class LineString<T, 2> : public Geometry<T, 2> {
protected:
  std::vector<Point<T, 2>> points;

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
  LineString() : Geometry<T, 2>() {}
  LineString(std::vector<Point<T, 2>>& points) : points(points) {
    spdlog::info("LineString<T,2> constructor called");
    if (!points.empty()) {
      this->setHasM(this->firstPoint().getHasM());
      this->empty_ = false;
    } else {
      this->empty_ = true;
    }
  }
  LineString(const LineString<T, 2>& line) : Geometry<T, 2>(line) {
    spdlog::info("LineString<T,2> copy called");
    this->empty_ = line.empty_;
    this->srid = line.srid;
    this->hasm = line.hasm;
    this->points = line.points;
  }
  LineString(LineString<T, 2>&& line) : Geometry<T, 2>(line) {
    spdlog::info("LineString<T,2> copy called");
    this->empty_ = line.empty_;
    this->srid = line.srid;
    this->hasm = line.hasm;
    this->points = line.points;
  }
  LineString<T, 2>& operator=(const LineString<T, 2>& line) {
    if (this == &line)
      return *this;
    else {
      Geometry<T, 2>::operator=(line);
      this->points = line.points;
      return *this;
    }
  }
  ~LineString() {}

  virtual Box<T, 2>* envelope() {
    T xmin, ymin, xmax, ymax;
    xmin = firstPoint().getX();
    xmax = firstPoint().getX();
    ymin = firstPoint().getY();
    ymax = firstPoint().getY();

    for (auto it = (this->points).begin() + 1; it < (this->points).end(); ++it) {
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
  }

  virtual std::string toWKT() { return this->toWKTWithTag("LINESTRING"); }

  Point<T, 2>& firstPoint() { return points.front(); }

  Point<T, 2>& lastPoint() { return points.back(); }

  int npoints() const { return points.size(); }

  Point<T, 2>& pointAt(const int i) { return points[i]; }

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

  template <typename U>
  friend std::ostream& operator<<(std::ostream& os, const LineString<U, 2>& line) {
    for (const Point<U, 2>& p : line.points) {
      os << p << ",";
    }
    return os;
  }
};

template <typename T>
class LineRing<T, 2> : public LineString<T, 2> {
public:
  LineRing() : LineString<T, 2>() {}
  LineRing(std::vector<Point<T, 2>>& points) : LineString<T, 2>(points) {
    if (this->firstPoint() != this->lastPoint()) {
      this->points.push_back(this->firstPoint());
    }
  }
  LineRing(const LineRing<T, 2>& ring) : LineString<T, 2>(ring) {}
  ~LineRing() {}

  virtual std::string toWKT() { return this->toWKTWithTag("LINERING"); }
};

template <typename T>
class Polygon<T, 2> : public Geometry<T, 2> {
protected:
  LineRing<T, 2> exterior;
  std::vector<LineRing<T, 2>> holes;

public:
  Polygon() : Geometry<T, 2>() {}
  Polygon(LineRing<T, 2> exterior, std::vector<LineRing<T, 2>>& holes)
      : exterior(exterior), holes(holes) {
    this->empty_ = this->exterior.empty();
    if (this->exterior.empty() > 0) {
      this->setHasM(this->exterior.getHasM());
    } else {
      this->setHasM(false);
    }
  }
  Polygon(const Polygon<T, 2>& poly) : Geometry<T, 2>(poly) {
    this->exterior = poly.exterior;
    this->holes = poly.holes;
  }
  Polygon& operator=(const Polygon& poly) {
    if (this == &poly) {
      return *this;
    } else {
      Geometry<T, 2>::operator=(poly);
      this->exterior = poly.exterior;
      this->holes = poly.holes;
    }
  }
  ~Polygon() {}

  virtual Box<T, 2>* envelope() { return this->exterior.envelope(); }
  virtual std::string toWKT() {
    std::ostringstream oss;
    if (this->empty()) {
      oss << "POLYGON EMPTY";
      return oss.str();
    } else {
      oss << "POLYGON";
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

  LineRing<T, 2>& getExterior() { return this->exterior; }
  std::vector<LineRing<T, 2>>& getHoles() { return this->holes; }
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
    this->exterior = exterior;
    this->holes = holes;
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

// 3D

template <typename T>
class Point<T, 3>;

template <typename T>
class LineString<T, 3>;

template <typename T>
class LineRing<T, 3>;

template <typename T>
class Polygon<T, 3>;

template <typename T>
class Box<T, 3>;

template <typename T>
class Point<T, 3> : public Geometry<T, 3> {
public:
  Point() : x(0), y(0), z(0), m(0) { this->setHasM(false); }
  Point(T x, T y, T z) : x(x), y(y), z(z), m(0) { this->setHasM(false); }
  Point(T x, T y, T z, T m) : x(x), y(y), z(z), m(m) { this->setHasM(true); }
  Point(const Point<T, 2>& p) : Geometry<T, 2>(p) {
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

template <typename T>
class LineString<T, 3> : public Geometry<T, 3> {
protected:
  std::vector<Point<T, 3>> points;

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
  LineString() : Geometry<T, 3>() {}
  LineString(std::vector<Point<T, 3>>& points) : points(points) {
    if (!points.empty()) {
      this->setHasM(this->firstPoint().getHasM());
      this->empty_ = false;
    } else {
      this->setHasM(true);
      this->empty_ = true;
    }
  }
  LineString(const LineString<T, 3>& line) : Geometry<T, 3>(line) { this->points = line.points; }
  ~LineString() {}

  virtual Box<T, 3>* envelope() {
    T xmin, ymin, zmin, xmax, ymax, zmax;
    xmin = firstPoint().getX();
    xmax = firstPoint().getX();
    ymin = firstPoint().getY();
    ymax = firstPoint().getY();
    zmin = firstPoint().getZ();
    zmax = firstPoint().getZ();

    for (auto it = (this->points).begin() + 1; it < (this->points).end(); ++it) {
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
    return new Box<T, 3>(xmin, ymin, zmin, xmax, ymax, zmax);
  }

  virtual std::string toWKT() { return this->toWKTWithTag("LINESTRING Z"); }

  Point<T, 3>& firstPoint() { return points.front(); }

  Point<T, 3>& lastPoint() { return points.back(); }

  int npoints() const { return points.size(); }

  Point<T, 3>& pointAt(const int i) { return points[i]; }

  std::string toPointArrayStr() {
    std::ostringstream oss;
    bool hasm = this->firstPoint().getHasM();
    oss << "(";
    int i = 0;
    for (; i < this->npoints() - 1; ++i) {
      oss << this->pointAt(i).getX() << " " << this->pointAt(i).getY() << " "
          << this.pointAt(i).getZ();
      if (hasm) {
        oss << " " << this->pointAt(i).getM();
      }
      oss << ",";
    }
    oss << this->pointAt(i).getX() << " " << this->pointAt(i).getY() << " "
        << this->pointAt(i).getZ();
    if (hasm) {
      oss << " " << this->pointAt(i).getM();
    }
    oss << ")";

    return oss.str();
  }

  template <typename U>
  friend std::ostream& operator<<(std::ostream& os, const LineString<U, 3>& line) {
    for (const Point<U, 2>& p : line.points) {
      os << p << ",";
    }
    return os;
  }
};

template <typename T>
class LineRing<T, 3> : public LineString<T, 3> {
public:
  LineRing() : LineString<T, 3>() {}
  LineRing(std::vector<Point<T, 3>>& points) : LineString<T, 3>(points) {
    if (this->firstPoint() != this->lastPoint()) {
      this->points.push_back(this->firstPoint());
    }
  }
  LineRing(const LineRing<T, 3>& ring) : LineString<T, 3>(ring) {}
  ~LineRing() {}

  virtual std::string toWKT() { return this->toWKTWithTag("LINERING Z"); }
};

template <typename T>
class Polygon<T, 3> : public Geometry<T, 3> {
protected:
  LineRing<T, 3> exterior;
  std::vector<LineRing<T, 3>> holes;

public:
  Polygon() : Geometry<T, 3>() {}
  Polygon(LineRing<T, 3> exterior, std::vector<LineRing<T, 3>>& holes)
      : exterior(exterior), holes(holes) {
    this->empty_ = this->exterior.empty();
    if (this->exterior.empty() > 0) {
      this->setHasM(this->exterior.getHasM());
    } else {
      this->setHasM(false);
    }
  }
  Polygon(const Polygon<T, 3>& poly) : Geometry<T, 3>(poly) {
    this->exterior = poly.exterior;
    this->holes = poly.holes;
  }
  ~Polygon() {}

  virtual Box<T, 3>* envelope() { return this->exterior.envelope(); }
  virtual std::string toWKT() {
    std::ostringstream oss;
    if (this->empty()) {
      oss << "POLYGON EMPTY";
      return oss.str();
    } else {
      oss << "POLYGON Z";
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

  LineRing<T, 3>& getExterior() { return this->exterior; }
  std::vector<LineRing<T, 3>>& getHoles() { return this->holes; }
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