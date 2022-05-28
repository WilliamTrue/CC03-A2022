/*
C. I: 26488388, Natanael Rojo
C. I: 29.854.297 William Mendoza
*/

//ESTA ES UNA NUEVA ENTRADA

#include <vector>
#include <tuple>
using namespace std;
using namespace Designar;


// Checks whether p is inside of the obstacle o.
inline bool is_inside(const Point2D& p, const Obstacle& o)
{
  UNUSED(p) // Remove this line.
  UNUSED(o) // Remove this line.
  
  // Write your solution here.  
  Segment s;
  //Vector2D v1, v2;
  auto pred = [p](GenSegment<Point2D> segment){
    Vector2D v1, v2;
    v1.set_x(segment.get_tgt_point().get_x() - segment.get_src_point().get_x());
    v1.set_y(segment.get_tgt_point().get_y() - segment.get_src_point().get_y());
    v2.set_x(p.get_x() - segment.get_src_point().get_x());
    v2.set_y(p.get_y() - segment.get_src_point().get_y());
    if (v1.cross_product(v2) > 0){
      return true;
    }
    return false;
  };
  
  return o.all_segment(pred);
}

// Checks whether p is inside of any of the obstacles in os.
inline bool is_inside(const Point2D& p, const SLList<Obstacle>& os)
{
  UNUSED(p) // Remove this line.
  UNUSED(os) // Remove this line.
  
  // Write your solution here.
  bool result;
  for (auto& element : os){
    result = is_inside(p, element);
    if (result){
      break;
    }
  }

  return result;
}

// Checks whether s intersects with the obstacle o.
inline bool intersects(const Segment& s, const Obstacle& o)
{
  // Write your solution here.
  vector<GenSegment<Point2D>> segments;
/*auto pred = [s](GenSegment<Point2D> segment){
  if (s.intersects_with(segment)){
    return true;
  }
  return false;
};*/
auto pred = [&segments](GenSegment<Point2D> segment){
  segments.push_back(segment);
  return true;
};
o.all_segment(pred);
for (auto& element : segments){
  if (s.intersects_with(element)){
    return true;
  }
}
return false;
}

// Checks whether s intersects with any of the obstacles in os.
inline bool intersects(const Segment& s, const SLList<Obstacle>& os)
{
  // Write your solution here.
  bool result;
  for (auto& obstacle : os){
    result = intersects(s, obstacle);
    if (result){
      break;
    }
  }
  return result;
}

// Translate the vector, dx units horizontally and dy units vertically.
inline Vector2D translate(const Vector2D& v, double dx, double dy)
{  
  // Write your solution here.
  Vector2D translated_vector;
  translated_vector.set_x(v.get_x() + dx);
  translated_vector.set_y(v.get_y() + dy);

  return translated_vector;
}

// Rotates the vector a degrees respect the origin (0.f, 0.f)
inline Vector2D rotate(const Vector2D& v, double a)
{
  // Write your solution here.
  Mat2D m(dcos(a), -dsin(a), dsin(a), dcos(a));
  return m * v;
}

// Rotates the vector a degrees respect u.
inline Vector2D rotate_around(const Vector2D& v, double a, const Vector2D& u)
{  
  // Write your solution here.
  Vector2D q;
  q.set_x(v.get_x() - u.get_x());
  q.set_y(v.get_y() - u.get_y());
  q = rotate(q, a);
  q = q + u;
  return q;
}

inline VisionArea build_vision_area(const Vector2D& p, const Vector2D& f, double r, double a)
{  
  // Write your solution here.
  Vector2D camera_position(p);
  Vector2D aux = f;
  aux.set_x(r * aux.get_x());
  aux.set_y(r * aux.get_y());
  Vector2D right_side = rotate(aux, -a);
  Vector2D left_side = rotate(aux, a);
    right_side = translate(right_side, p.get_x(), p.get_y());
  left_side = translate(left_side, p.get_x(), p.get_y());
  VisionArea vision_area = make_tuple(camera_position, right_side, left_side, r);
  return vision_area;
}

// Checks whether p is inside of va.
inline bool is_inside(const Point2D& p, const VisionArea& va)
{
  // Write your solution here.
  Vector2D pointVector;
  pointVector.set_x(p.get_x() - get<0>(va).get_x());
  pointVector.set_y(p.get_y() - get<0>(va).get_y());

  double squareRadious = (get<3>(va)*get<3>(va));
  bool radiousWithin = false; 
  bool areRightSide = false;
  bool areLeftSide = true;

  if ((pointVector.get_x()*pointVector.get_x() + pointVector.get_y()*pointVector.get_y()) <= squareRadious) {
    radiousWithin = true;
  }

  if ( (-get<2>(va).get_x()*pointVector.get_y() + get<2>(va).get_y()*pointVector.get_x()) > 0)
  {
    areRightSide = true;
  }

    if ( (-get<1>(va).get_x()*pointVector.get_y() + get<1>(va).get_y()*pointVector.get_x()) > 0)
  {
    areLeftSide = false;
  }

  if (radiousWithin && areRightSide && (areLeftSide == false))
  {
    return true;
  }
}
