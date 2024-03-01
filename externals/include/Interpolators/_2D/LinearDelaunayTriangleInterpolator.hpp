#pragma once

#include "DelaunayTriangulationInterpolatorBase.hpp"

namespace _2D {

  template<class Real>
  class LinearDelaunayTriangleInterpolator : public DelaunayTriangulationInterpolatorBase<LinearDelaunayTriangleInterpolator<Real>>
  {
    public:

      using BASE = DelaunayTriangulationInterpolatorBase<LinearDelaunayTriangleInterpolator<Real>>;
      using VectorType = typename BASE::VectorType;
      using MapType = typename BASE::MapType; 

      using point_t = typename BASE::point_t;
      using triangle_t = typename BASE::triangle_t;

      // types used to view data as 2D coordinates
      using MatrixType    = typename BASE::MatrixType;
      using _2DVectorView = typename BASE::_2DVectorView;
      using _2DMatrixView = typename BASE::_2DMatrixView;

      // types used for 2x2 matrix algebra
      using Matrix33 = Eigen::Matrix<Real,3,3 >;
      using ColVector2 = Eigen::Matrix<Real,3,1 >;
      using RowVector2 = Eigen::Matrix<Real,1,3 >;

      using rtree_value_t = typename BASE::rtree_value_t;

      /* Geometric interpolation */
      Real operator()(Real x, Real y) const
      {
        point_t p{x,y};
        // search optimization: find all boxes that envelope a triangle and cover the point first
        std::vector<rtree_value_t> candidates;
        this->m_triangles_index.query(boost::geometry::index::covers(p), std::back_inserter(candidates) );
        for( auto& v: candidates)
        {
          size_t i = v.second;
          auto &t = this->m_xy_triangles[i];
          if( boost::geometry::covered_by(p,t) )
          {
            std::array<std::array<Real,3>,3> points;
            size_t j = 0;
            boost::geometry::for_each_point(t,[&i,&j,&points,this](const point_t& pp){
                if( j > 2 )
                  return;
                points[j][0] = pp[0];
                points[j][1] = pp[1];
                points[j][2] = this->getZData()[this->m_triangle_datapoints[i][j]];
                j++;
                });

            boost::geometry::subtract_point(points[1],points[0]);
            boost::geometry::subtract_point(points[2],points[0]);
            auto surface_normal = boost::geometry::cross_product(points[2],points[1]);

            // equation for a plane:
            // (\vec{r} - \vec{r}_0) \cdot \vec{n} = 0 = \vec{r} \cdot \vec{n} - \vec{r}_0 \cdot \vec{n} = nx*(rx-r0x) + ny*(ry-r0y) + nz*(rz-r0z)
            // nx*rx + ny*ry + nz*rz = \vec{n} \cdot \vec{r}_0
            //
            // we want to compute rz
            //
            // nz*rz = \vec{n} \cdot \vec{r}_0 - nx*rx - ny*ry
            // rz = (\vec{n} \cdot \vec{r}_0 - nx*rx - ny*ry)/nz
            Real z = (boost::geometry::dot_product( surface_normal, points[0] ) - p[0]*surface_normal[0] - p[1]*surface_normal[1])/surface_normal[2];

            return z;
          }
        }

        // do not extrapolate
        return 0;
      }

      // /* Barycentric interpolation */
      // Real operator()( Real x, Real y, bool) const
      // {
      //   point_t p{x,y};
      //   // search optimization: find all boxes that envelope a triangle and cover the point first
      //   std::vector<rtree_value_t> candidates;
      //   this->m_triangles_index.query(boost::geometry::index::covers(p), std::back_inserter(candidates) );
      //   for( auto& v: candidates)
      //   {
      //     size_t i = v.second;
      //     auto &t = this->m_xy_triangles[i];
      //     if( boost::geometry::covered_by(p,t) )
      //     {
      //       std::array<point_t,3> points;

      //       /*
      //         Performs a copy of the 3 point-triangulation for inplace modification
      //         Alternative (maybe cleaner) is to use boost:begin iterator on `t`
      //         and get the 3 first points of `t`
      //       */
      //       size_t j = 0;
      //       boost::geometry::for_each_point(
      //         t // closed ring with 4 points i.e. a triangle
      //         , [&j,&points](const point_t& pp)
      //           { // lambda expression
      //             if( j > 2 ) // don't store the 4th point, i.e. the 1st one again
      //               return;

      //             points[j] = pp; // deep copy since it's std::array
      //             j++;
      //           }
      //       );
      //       /* Barycenter coordinates (u,v,w) computation
      //         https://gamedev.stackexchange.com/a/23745
      //         Vector v0 = b - a, v1 = c - a, v2 = p - a;
      //         float d00 = Dot(v0, v0);
      //         float d01 = Dot(v0, v1);
      //         float d11 = Dot(v1, v1);
      //         float d20 = Dot(v2, v0);
      //         float d21 = Dot(v2, v1);
      //         float denom = d00 * d11 - d01 * d01;
      //         v = (d11 * d20 - d01 * d21) / denom;
      //         w = (d00 * d21 - d01 * d20) / denom;
      //         u = 1.0f - v - w;
      //       */
      //       point_t v0(points[0]);
      //       point_t v1(points[0]);
      //       point_t v2(points[0]);

      //       // subtract_point(u, v) = v - u and stores result in u 
      //       boost::geometry::subtract_point(v0, points[1]);
      //       boost::geometry::subtract_point(v1, points[2]);
      //       boost::geometry::subtract_point(v2, p);

      //       double d00 = boost::geometry::dot_product(v0, v0);
      //       double d01 = boost::geometry::dot_product(v0, v1);
      //       double d11 = boost::geometry::dot_product(v1, v1);
      //       double d20 = boost::geometry::dot_product(v2, v0);
      //       double d21 = boost::geometry::dot_product(v2, v1);

      //       double inverseDeterminant = 1. / (d00 * d11 - d01 * d01);
      //       double v = (d11 * d20 - d01 * d21) * inverseDeterminant; // L2
      //       double w = (d00 * d21 - d01 * d20) * inverseDeterminant; // L3
      //       double u = 1.0f - v - w; // L1

      //       Real z0 = this->getZData()[this->m_triangle_datapoints[i][0]];
      //       Real z1 = this->getZData()[this->m_triangle_datapoints[i][1]];
      //       Real z2 = this->getZData()[this->m_triangle_datapoints[i][2]];

      //       return (Real) z0 * u + z1 * v + z2 * w;
      //     }
      //   }

      //   return 0; // do not extrapolate
      // }

    protected:
      using BASE::m_xy_triangles;

    private:
      friend BASE;
      void setupInterpolator() {}

  };

}
