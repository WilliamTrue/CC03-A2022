#pragma once

#ifdef DEBUG
#define LOG(...) printf(__VA_ARGS__); printf("\n");
#else
#define LOG(...)
#endif

#define UNUSED(name) (void)name;

# include <tuple>

using namespace std;

# include <math.H>
# include <random.H>
# include <buildgraph.H>
# include <vector2D.H>
# include <geometricalgorithms.H>

using namespace Designar;

// Type of graph for the pathfinder.
using EuclideanGraph = Graph<Point2D, double>;

// Alias for an obstacle.
using Obstacle = Polygon;

// Trigonometry of notable angles.
constexpr double sin0{0.};
constexpr double sin30{0.5};
constexpr double sin45{0.707106781};
constexpr double sin60{0.866025404};
constexpr double sin90{1.};
constexpr double cos0{1.};
constexpr double cos30{0.866025404};
constexpr double cos45{0.707106781};
constexpr double cos60{0.5};
constexpr double cos90{0.};

// Convert degrees to radians.
inline double degree_to_radian(double a)
{
  return a * PI / 180.;
}

// Compute sin of an angle in degrees.
inline double dsin(double a)
{
  if (real_equal(a, 0.))
    return sin0;

  if (real_equal(a, 30.))
    return sin30;

  if (real_equal(a, 45.))
    return sin45;

  if (real_equal(a, 60.))
    return sin60;

  if (real_equal(a, 90.))
    return sin90;

  return std::sin(degree_to_radian(a));
}

// Compute cos of an angle in degrees.
inline double dcos(double a)
{
  if (real_equal(a, 0.))
    return cos0;

  if (real_equal(a, 30.))
    return cos30;

  if (real_equal(a, 45.))
    return cos45;

  if (real_equal(a, 60.))
    return cos60;

  if (real_equal(a, 90.))
    return cos90;

  return std::cos(degree_to_radian(a));
}

// 2x2 transformation matrix.
struct Mat2D
{
  double m11;
  double m12;
  double m21;
  double m22;

  Mat2D(double _m11, double _m12, double _m21, double _m22)
    : m11{_m11}, m12{_m12}, m21{_m21}, m22{_m22} {}

  // Product with a column vector.
  Vector2D operator * (const Vector2D &v) const
  {
    return Vector2D(m11 * v.get_x() + m12 * v.get_y(),
		    m21 * v.get_x() + m22 * v.get_y());
  }
};

/** Alias for the vision area of a camera.
 *
 * It is defined by three counterclockwise points.
 *
 * Each point is defined as follow:
 *  - The first element represents the camera position.
 *  - The second element is the farthest point to the right of the area.
 *  - The third element is the farthest point to the left of the area.
 *  - The fourth element is the vision length (vision radius).
 */
using VisionArea = tuple<Vector2D, Vector2D, Vector2D, double>;


// Class terrain (forward declaRADIUSn).
class Terrain;


// Basic entity (Resource, Agent, Camera).
class Entity
{
public:
  Entity(const Vector2D& p): position{p}{}
  
  const Vector2D& get_position() const
  {
    return position;
  }

protected:
  Vector2D position;
};

// Represents a resource.
class Resource : public Entity
{
public:
  // Respawn time.
  static constexpr double RESPAWN_TIME = 10000; // milliseconds.
  
  // Radius to draw the resource.
  static constexpr double RADIUS = 1;

  Resource(): Entity{Vector2D()} {}
  
  Resource(const Vector2D& p)
    : Entity{p}, taken{false}, taken_time{0}, seen_by_camera{false} {}

  // Mark the resource as taken.
  void take()
  {
    taken = true;
  }

  bool is_taken() const
  {
    return taken;
  }

  bool is_seen_by_camera() const
  {
    return seen_by_camera;
  }

  void see()
  {
    seen_by_camera = true;
  }

  void unsee()
  {
    seen_by_camera = false;
  }

  /** Updates the resource.
   *
   * @param dt: The time elapsed since the last update.  
   */
  void update(double dt)
  {
    if (not taken) // Nothing to do.
      {
        return;
      }

    taken_time += dt;
    
    if (taken_time > RESPAWN_TIME)
      {
	taken = false;
	taken_time = 0;
	seen_by_camera = false;
      }
  }
 
private:
  bool taken;
  double taken_time;
  bool seen_by_camera;
};

// Represents a game agent.
class Agent : public Entity
{
public:
  static constexpr double RADIUS = 1;
  
  // Construye un agente en la posición p con rapidez s
  Agent(const Vector2D& p, double s)
    : Entity{p}, speed{s}, dead{false}, from{p}, to{p}, rng{get_random_seed()} {}

  bool is_dead() const
  {
    return dead;
  }

  void kill()
  {
    dead = true;
  }

  void set_terrain(Terrain* t)
  {
    terrain = t;
  }

  void compute_a_path();

  void update(double);

  const SLList<Point2D>& get_path() const
  {
    return path;
  }

private:
  Terrain* terrain;
  double speed;
  bool dead;

  Point2D from;
  Point2D to;
  SLList<Point2D> path;

  Resource* tgt{nullptr};

  mt19937 rng;
};

class Camera : public Entity
{
public:
  // Construye una cámara en la posición p apuntando a f.
  Camera(const Vector2D& p, const Vector2D& f, double r, double a)
    : Entity(p), front(f), vision_radius(r), vision_angle(a), current_time(0)
  {
    front.normalize(); // Garantiza vector unitario
  }

  double curr_time() const { return current_time; }

  const Vector2D& get_front() const
  {
    return front;
  }

  double get_vision_radius() const
  {
    return vision_radius;
  }

  double get_vision_angle() const
  {
    return vision_angle;
  }

  void set_terrain(Terrain* t)
  {
    terrain = t;
  }

  void update(double);

private:
  // Time to rotate.
  static constexpr double ROT_TIME = 5000; // en ms
  
  Vector2D front;
  double vision_radius;
  double vision_angle;
  Terrain* terrain;

  double current_time;   
};

// Class Pathfinder (forward declaration).
class Pathfinder;

// Game terrain.
class Terrain
{
public:
  Terrain(nat_t w, nat_t h) : width{w}, height{h} {}

  void add_obstacle(const Obstacle& o)
  {
    obstacles.append(o);
  }

  void add_obstacle(Obstacle&& o)
  {
    obstacles.append(std::move(o));
  }

  void add_resource(const Resource& r)
  {
    resources.append(r);
  }

  void add_resource(Resource&& r)
  {
    resources.append(std::move(r));
  }

  void add_agent(const Agent& a)
  {
    agents.append(a);
  }

  void add_agent(Agent&& a)
  {
    agents.append(std::move(a));
  }
  
  nat_t get_width() const
  {
    return width;
  }

  nat_t get_height() const
  {
    return height;
  }

  void set_pathfinder(Pathfinder* pf)
  {
    pathfinder = pf;
  }

  Pathfinder* get_pathfinder()
  {
    return pathfinder;
  }

  void set_camera(Camera* cam)
  {
    camera = cam;
  }

  Camera* get_camera()
  {
    return camera;
  }

  const SLList<Obstacle> get_obstacles() const
  {
    return obstacles;
  }
  
  const DynArray<Resource>& get_resources() const
  {
    return resources;
  }

  const SLList<Agent>& get_agents() const
  {
    return agents;
  }

  void update(double dt)
  {
    // Upate resources.
    resources.for_each([dt] (auto& r)
		       {
			 r.update(dt);
		       });
    
    // Update agents.
    agents.for_each([this, dt] (auto& a)
		    {
		      a.update(dt);

		      auto l = Agent::RADIUS + Resource::RADIUS;

		      // Check whether the resources can be taken.
		      resources.for_each([&a, l] (auto& r)
		       {
			 if (r.get_position().distance_with(a.get_position()) < l)
			   {
			     r.take();
			   }

		       });
		    });
    
    // Update the camera.
    camera->update(dt);

    // Remove dead agents.
    agents.remove_if([] (auto& a) { return a.is_dead(); });
  }

private:
  nat_t width;
  nat_t height;

  Pathfinder* pathfinder;

  SLList<Obstacle> obstacles;
  DynArray<Resource> resources;
  SLList<Agent> agents;

  Camera* camera;
  
};

