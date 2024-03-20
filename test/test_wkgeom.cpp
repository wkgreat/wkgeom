#include <catch2/catch_all.hpp>

#include <spdlog/spdlog.h>
#include "../wkgeom/wkgeom.h"

TEST_CASE("point init", "[point]") {
  spdlog::set_level(spdlog::level::trace);

  const double X = 1.0;
  const double Y = 2.0;
  const double M = 3.0;
  const int SRID = 4326;
  wk::wkgeom::Point<double, 2> p(X, Y, M);
  p.setSrid(SRID);
  REQUIRE(p.getX() == X);
  REQUIRE(p.getY() == Y);
  REQUIRE(p.getM() == M);
  REQUIRE(p.getSrid() == SRID);
}

TEST_CASE("point compare", "[point]") {
  spdlog::set_level(spdlog::level::trace);

  wk::wkgeom::Point<double, 2> p1(1.0, 2.0, 3.0);
  wk::wkgeom::Point<double, 2> p2(1.0, 2.0, 3.0);
  wk::wkgeom::Point<double, 2> p3(4.0, 5.0, 6.0);
  REQUIRE(p1 == p2);
  REQUIRE_FALSE(p1 == p3);

  std::cout << ">>>>> " << p1.toWKT() << std::endl;
}

TEST_CASE("linestring", "[linestring]") {
  spdlog::set_level(spdlog::level::trace);

  std::vector<wk::wkgeom::Point<double, 2>> points(5, wk::wkgeom::Point<double, 2>());

  points[0].setXYM(0, 1, 2);
  points[1].setXYM(3, 4, 5);
  points[2].setXYM(6, 7, 8);
  points[3].setXYM(9, 10, 11);
  points[4].setXYM(11, 12, 23);

  wk::wkgeom::LineString<double, 2> line(points);

  REQUIRE(line.npoints() == 5);

  std::cout << ">>>>> " << line.toWKT() << std::endl;
}

TEST_CASE("linering", "[linering]") {
  spdlog::set_level(spdlog::level::trace);

  std::vector<wk::wkgeom::Point<double, 2>> points(5, wk::wkgeom::Point<double, 2>());

  points[0].setXYM(0, 1, 2);
  points[1].setXYM(3, 4, 5);
  points[2].setXYM(6, 7, 8);
  points[3].setXYM(9, 10, 11);
  points[4].setXYM(11, 12, 23);

  wk::wkgeom::LineRing<double, 2> ring(points);

  REQUIRE(ring.npoints() == 6);
  REQUIRE(ring.firstPoint() == ring.lastPoint());

  std::cout << ">>>>> " << ring.toWKT() << std::endl;
}

TEST_CASE("polygon", "[polygon]") {
  spdlog::set_level(spdlog::level::trace);

  std::vector<wk::wkgeom::Point<double, 2>> points(5, wk::wkgeom::Point<double, 2>());

  points[0].setXYM(0, 1, 2);
  points[1].setXYM(3, 4, 5);
  points[2].setXYM(6, 7, 8);
  points[3].setXYM(9, 10, 11);
  points[4].setXYM(11, 12, 23);

  wk::wkgeom::LineRing<double, 2> ring(points);
  std::vector<wk::wkgeom::LineRing<double, 2>> holes(1, ring);
  wk::wkgeom::Polygon<double, 2> poly(ring, holes);

  std::cout << ">>>>> " << poly.toWKT() << std::endl;
}

TEST_CASE("box", "[box]") {
  spdlog::set_level(spdlog::level::trace);

  wk::wkgeom::Box<double, 2> box(117.0, 32.0, 118.0, 33.0);

  wk::wkgeom::LineRing<double, 2> exterior = box.getExterior();

  wk::wkgeom::Point<double, 2> p1(117.0, 32.0, 0.0);
  wk::wkgeom::Point<double, 2> p2(117.0, 33.0, 0.0);
  wk::wkgeom::Point<double, 2> p3(118.0, 33.0, 0.0);
  wk::wkgeom::Point<double, 2> p4(118.0, 32.0, 0.0);

  REQUIRE(exterior.pointAt(0) == p1);
  REQUIRE(exterior.pointAt(1) == p2);
  REQUIRE(exterior.pointAt(2) == p3);
  REQUIRE(exterior.pointAt(3) == p4);
  REQUIRE(exterior.pointAt(4) == p1);

  std::cout << ">>>>> " << exterior.toWKT() << std::endl;
}

TEST_CASE("envelope", "[envelope]") {
  spdlog::set_level(spdlog::level::trace);

  std::vector<wk::wkgeom::Point<double, 2>> points{{117, 32}, {118, 33}};
  wk::wkgeom::LineString<double, 2> line(points);
  wk::wkgeom::Box<double, 2>* b = line.envelope();
  wk::wkgeom::Box<double, 2> b2(117, 32, 118, 33);
  REQUIRE(*b == b2);
  delete b;

  std::cout << ">>>>> " << b2.toWKT() << std::endl;
}

TEST_CASE("3D Point", "[point3d]") {
  spdlog::set_level(spdlog::level::trace);

  wk::wkgeom::Point<double, 3> p(117, 32, 100);
  REQUIRE(wk::wkgeom::utils::dbl_equal(p.getX(), 117));
  REQUIRE(wk::wkgeom::utils::dbl_equal(p.getY(), 32));
  REQUIRE(wk::wkgeom::utils::dbl_equal(p.getZ(), 100));
}