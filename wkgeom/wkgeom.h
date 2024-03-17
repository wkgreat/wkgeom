#pragma once

#include <fmt/core.h>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include "wkgeom_utils.h"

namespace wk ::wkgeom {

template <typename T>
class Box;

template <typename T>
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
  ~Geometry() {}
  int getSrid() const { return this->srid; }
  void setSrid(int srid) { this->srid = srid; }
  bool getHasM() const { return this->hasm; }
  void setHasM(bool hasm) { this->hasm = hasm; }
  bool empty() const { return this->empty_; }
  virtual Box<T>* envelope() = 0;
  virtual std::string toWKT() = 0;
};

template <typename T>
class Point : public Geometry<T> {
public:
  Point() : x(0), y(0), m(0) {}
  Point(T x, T y) : x(x), y(y), m(0) {}
  Point(T x, T y, T m) : x(x), y(y), m(m) {}
  Point(const Point<T>& p) : Geometry<T>(p) { setXYM(p.getX(), p.getY(), p.getM()); }
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

  virtual Box<T>* envelope() { return new Box(this->x, this->y, this->x, this->y); }
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
  friend std::ostream& operator<<(std::ostream& os, const Point<U>& p) {
    os << p.x << " " << p.y << " " << p.m;
    return os;
  }

  template <typename U>
  friend bool operator==(const Point<U>& p1, const Point<U>& p2) {
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
class LineString : public Geometry<T> {
protected:
  std::vector<Point<T>> points;

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
  LineString() : Geometry<T>() {}
  LineString(std::vector<Point<T>>& points) : points(points) {
    if (!points.empty()) {
      this->setHasM(this->firstPoint().getHasM());
      this->empty_ = false;
    } else {
      this->empty_ = true;
    }
  }
  LineString(const LineString<T>& line) : Geometry<T>(line) { this->points = line.points; }
  ~LineString() {}

  virtual Box<T>* envelope() {
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
    return new Box<T>(xmin, ymin, xmax, ymax);
  }

  virtual std::string toWKT() { return this->toWKTWithTag("LINESTRING"); }

  Point<T>& firstPoint() { return points.front(); }

  Point<T>& lastPoint() { return points.back(); }

  int npoints() const { return points.size(); }

  Point<T>& pointAt(const int i) { return points[i]; }

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
  friend std::ostream& operator<<(std::ostream& os, const LineString<U>& line) {
    for (const Point<U>& p : line.points) {
      os << p << ",";
    }
    return os;
  }
};

template <typename T>
class LineRing : public LineString<T> {
public:
  LineRing() : LineString<T>() {}
  LineRing(std::vector<Point<T>>& points) : LineString<T>(points) {
    if (this->firstPoint() != this->lastPoint()) {
      this->points.push_back(this->firstPoint());
    }
  }
  LineRing(const LineRing<T>& ring) : LineString<T>(ring) {}
  ~LineRing() {}

  virtual std::string toWKT() { return this->toWKTWithTag("LINERING"); }
};

template <typename T>
class Polygon : public Geometry<T> {
protected:
  LineRing<T> exterior;
  std::vector<LineRing<T>> holes;

public:
  Polygon() : Geometry<T>() {}
  Polygon(LineRing<T> exterior, std::vector<LineRing<T>>& holes)
      : exterior(exterior), holes(holes) {
    this->empty_ = this->exterior.empty();
    if (this->exterior.empty() > 0) {
      this->setHasM(this->exterior.getHasM());
    } else {
      this->setHasM(false);
    }
  }
  Polygon(const Polygon<T>& poly) : Geometry<T>(poly) {
    this->exterior = poly.exterior;
    this->holes = poly.holes;
  }
  ~Polygon() {}

  virtual Box<T>* envelope() { return this->exterior.envelope(); }
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

  LineRing<T>& getExterior() { return this->exterior; }
  std::vector<LineRing<T>>& getHoles() { return this->holes; }
};

template <typename T>
class Box : public Polygon<T> {
private:
  T xmin, ymin, xmax, ymax;

public:
  Box(T xmin, T ymin, T xmax, T ymax) : Polygon<T>() {
    this->xmin = xmin;
    this->ymin = ymin;
    this->xmax = xmax;
    this->ymax = ymax;
    std::vector<Point<T>> pts = {
        {xmin, ymin}, {xmin, ymax}, {xmax, ymax}, {xmax, ymin}, {xmin, ymin}};

    LineRing<T> exterior(pts);
    std::vector<LineRing<T>> holes(0);
    this->exterior = exterior;
    this->holes = holes;
    this->empty_ = exterior.empty();
    this->setHasM(exterior.getHasM());
  }
  Box(const Box<T>& b) : Polygon<T>(b) {}
  ~Box() {}

  T getXmin() const { return this->xmin; }
  T getYmin() const { return this->ymin; }
  T getXmax() const { return this->xmax; }
  T getYmax() const { return this->ymax; }

  virtual Box<T>* envelope() { return new Box<T>(*this); }

  friend bool operator==(const Box<T>& b1, const Box<T>& b2) {
    return utils::dbl_equal(b1.getXmin(), b2.getXmin()) &&
           utils::dbl_equal(b1.getYmin(), b2.getYmin()) &&
           utils::dbl_equal(b1.getXmax(), b2.getXmax()) &&
           utils::dbl_equal(b1.getYmax(), b2.getYmax());
  }
};

}  // namespace wk::wkgeom