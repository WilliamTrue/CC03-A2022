#include <Definitions.h>
#include <Geom.h>
#include <Pathfinder.h>

void Agent::compute_a_path()
{
  // Look for a resource that is not in the camera vision area.
  
  uniform_int_distribution<size_t> dist{0, terrain->get_resources().size() - 1};
  
  tgt = const_cast<Resource*>(&terrain->get_resources().at(dist(rng)));
  
  if (tgt->is_taken() or tgt->is_seen_by_camera())
    {
      tgt = nullptr;
      return;
    }
  
  path = terrain->get_pathfinder()->search_path(position, tgt->get_position());
}

void Agent::update(double dt)
{
  if (dead)
    {
      return;
    }

  if (tgt == nullptr or tgt->is_taken() or tgt->is_seen_by_camera() or path.is_empty())
    {
      // Compute a new path.
      compute_a_path();
    }
  
  double expected_distance = from.distance_with(to);
  double current_distance  = from.distance_with(position);
  
  if (current_distance >= expected_distance)
    {
      from = to;
      position.set_x(from.get_x());
      position.set_y(from.get_y());

      if (path.is_empty())
        {
	  return;
        }

      // new target
      to = path.remove_first();      
    }  
  
  // Compute the velocity  
  Vector2D velocity(to.get_x() - from.get_x(), to.get_y() - from.get_y());
  velocity.normalize();
  velocity.scale(speed);
  
  // Move the agent.
  position.add_scaled_vector(velocity, dt);
}

void Camera::update(double dt)
{
  current_time += dt;
  
  if (current_time > ROT_TIME)
    {
      front = rotate(front, 2*vision_angle);
      front.normalize();
      current_time = 0;
    }

  auto va = build_vision_area(position, front, vision_radius, vision_angle);

  // Mark as seen all of the resources inside the vision area.
  terrain->get_resources().for_each([&va] (auto& r)
				    {
				      if (is_inside(r.get_position(), va))
					r.see();
				      else
					r.unsee();
				    });

  // Kill all agents inside the vision area.
  terrain->get_agents().for_each([this, &va] (auto& a)
				 {
				   if (not is_inside(a.get_position(), va))
				     {
				       return;
				     }

				   Segment s(a.get_position(), position);

				   if (!intersects(s, terrain->get_obstacles()))
				     {
				       a.kill();
				     }
				 });
}

