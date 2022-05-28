#include <canvas.H>
#include <QPainter>
#include <QDebug>

QPoint Canvas::to_screen_pos(const Vector2D & v)
{
  return QPoint(v.get_x() * SCALE, height() - v.get_y() * SCALE);
}

QPoint Canvas::to_screen_pos(const Point2D & p)
{
  return QPoint(p.get_x() * SCALE, height() - p.get_y() * SCALE);
}

void Canvas::paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  painter.fillRect(0, 0, width(), height(), Qt::darkGreen);

  painter.setPen(Qt::lightGray);
  pf.get_graph().for_each_arc([this, &painter] (auto a)
  {
    painter.drawLine(to_screen_pos(a->get_src_node()->get_info()),
                     to_screen_pos(a->get_tgt_node()->get_info())
                     );
  });

  painter.setBrush(Qt::red);
  painter.setPen(Qt::red);
  painter.drawEllipse(to_screen_pos(cam.get_position()), 5, 5);

  auto va = build_vision_area(cam.get_position(), cam.get_front(),
                              cam.get_vision_radius(), cam.get_vision_angle());

  double a = (get<1>(va) - get<0>(va)).angle_with(Vector2D(1,0)) * 180/PI;
  double al = 2*(get<1>(va) - get<0>(va)).angle_with(cam.get_front()) * 180/PI;
  QColor color(Qt::yellow);
  color.setAlpha(50);
  painter.setBrush(QBrush(color));
  painter.setPen(QPen(color));
  QRect rect((get<0>(va).get_x() - cam.get_vision_radius())*SCALE,
             (get<0>(va).get_y() - cam.get_vision_radius())*SCALE,
                          cam.get_vision_radius() * 2 * SCALE,
                          cam.get_vision_radius() * 2 * SCALE);
  painter.drawPie(rect, -a*16, al*16);

  painter.setBrush(Qt::darkYellow);
  painter.setPen(Qt::darkYellow);
  terrain.get_obstacles().for_each([this, &painter] (const Obstacle & o)
  {
      const size_t & sz = o.size();

      QPoint * points = new QPoint[sz];
      int i = 0;

      o.for_each_vertex([&i, this, points] (const Point2D & p)
      {
          points[i++] = to_screen_pos(p);
      });

      painter.drawPolygon(points, sz);

      delete [] points;
  });

  terrain.get_resources().for_each([this, &painter] (const Resource & r)
  {
      if (r.is_taken())
          return;

      if (r.is_seen_by_camera())
        {
          painter.setPen(Qt::red);
          painter.setBrush(Qt::red);
        }
      else
        {
          painter.setPen(Qt::cyan);
          painter.setBrush(Qt::cyan);
        }

      QPoint p = to_screen_pos(r.get_position());
      int RADIUS = int(Resource::RADIUS * SCALE) / 2;
      painter.drawRect(p.x() - RADIUS, p.y() - RADIUS, 2*RADIUS, 2*RADIUS);
  });

  painter.setPen(Qt::white);
  painter.setBrush(Qt::white);
  terrain.get_agents().for_each([this, &painter] (const Agent & a)
  {
      painter.drawEllipse(to_screen_pos(a.get_position()),
                          int(Agent::RADIUS * SCALE),
                          int(Agent::RADIUS * SCALE));
  });
}

Canvas::Canvas(QWidget *parent)
  : QWidget(parent), terrain(120, 70), pf(&terrain, 20, 12),
    cam(Vector2D(terrain.get_width()/2, terrain.get_height()/2), Vector2D(1,0),
        35, 30)
{
  terrain.set_camera(&cam);
  terrain.set_pathfinder(&pf);

  cam.set_terrain(&terrain);

  rng_t rng(get_random_seed());

  terrain.add_obstacle({{3,4},{6,4},{8,7},{11, 13},{6,11},{3,11}, {1,7}});
  terrain.add_obstacle({{23,24},{26,24},{26,31}});

  SLList<Point2D> ps;
  QuickHull<Obstacle> qh;

  for (int i = 0; i < 100; ++i)
    {
      Point2D p;
      p.set_x(random_uniform(rng, 70., 90.));
      p.set_y(random_uniform(rng, 40., 50.));
      ps.append(p);
    }

  terrain.add_obstacle(qh(ps));

  ps.clear();

  for (int i = 0; i < 100; ++i)
    {
      Point2D p;
      p.set_x(random_uniform(rng, 55., 65.));
      p.set_y(random_uniform(rng, 15., 25.));
      ps.append(p);
    }

  terrain.add_obstacle(qh(ps));

  pf.apply_obstacles();

  for (int i = 0; i < 30; ++i)
    {
      Vector2D pos;

      HashSet<Vector2D> pos_set;

      do
        {
          pos.set_x(random_uniform(rng, terrain.get_width()));
          pos.set_y(random_uniform(rng, terrain.get_height()));
        }
      while(is_inside(pos, terrain.get_obstacles()) or
            pos_set.insert(pos) == nullptr);

      terrain.add_resource(Resource(pos));
    }

  for (int i = 0; i < 18; ++i)
    {

      Vector2D pos;

      do
        {
          pos.set_x(random_uniform(rng, terrain.get_width()));
          pos.set_y(random_uniform(rng, terrain.get_height()));
        }
      while(is_inside(pos, terrain.get_obstacles()));

      Agent agent(pos, 0.01);
      agent.set_terrain(&terrain);
      agent.compute_a_path();
      terrain.add_agent(std::move(agent));
    }

  setFixedSize(terrain.get_width() * SCALE, terrain.get_height() * SCALE);

  connect(&timer, SIGNAL(timeout()), this, SLOT(slot_uptate()));
  move(0,0);

  elapsed_timer.start();
  timer.start(33);
}

void Canvas::slot_uptate()
{
  auto dt = elapsed_timer.elapsed();
  terrain.update(dt);
  elapsed_timer.restart();
  repaint();
}
