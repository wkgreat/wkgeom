#pragma once

#include <fmt/core.h>
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

template <typename T>
class Geometry<T, 2>;

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
class Geometry<T, 2> {
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
  ~Geometry() {}
  int getSrid() const { return this->srid; }
  void setSrid(int srid) { this->srid = srid; }
  bool getHasM() const { return this->hasm; }
  void setHasM(bool hasm) { this->hasm = hasm; }
  bool empty() const { return this->empty_; }
  virtual Box<T, 2>* envelope() = 0;
  virtual std::string toWKT() = 0;
};

template <typename T>
class Point<T, 2> : public Geometry<T, 2> {
public:
  Point() : x(0), y(0), m(0) {}
  Point(T x, T y) : x(x), y(y), m(0) {}
  Point(T x, T y, T m) : x(x), y(y), m(m) {}
  Point(const Point<T, 2>& p) : Geometry<T, 2>(p) { setXYM(p.getX(), p.getY(), p.getM()); }
  ~Point() {}
  T getX() const { return this->x; }
  T getY() const { return this->y; }
  T getM() const { return this->m; }
  void setX(int x) { this->x = x; }
  void setY(int y) { this->y = y; }
  void setM(int m) {
    this->setHasM(true);
    this->m = m;
  }
  void setXY(int x, int y) {
    this->x = x;
    this->y = y;
    this->m = 0;
    this->setHasM(false);
    this->srid = 0;
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
    if (!points.empty()) {
      this->setHasM(this->firstPoint().getHasM());
      this->empty_ = false;
    } else {
      this->empty_ = true;
    }
  }
  LineString(const LineString<T, 2>& line) : Geometry<T, 2>(line) { this->points = line.points; }
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

}  // namespace wk::wkgeom