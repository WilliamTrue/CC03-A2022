#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QElapsedTimer>
#include <QTimer>

#include <../Definitions.h>
#include <../Geom.h>
#include <../Pathfinder.h>

class Canvas : public QWidget
{
  Q_OBJECT

  static constexpr int SCALE = 10;

  Terrain    terrain;
  Pathfinder pf;
  Camera     cam;

  QElapsedTimer elapsed_timer;
  QTimer timer;

  QPoint to_screen_pos(const Vector2D &);

  QPoint to_screen_pos(const Point2D &);

protected:
  void paintEvent(QPaintEvent *) override;

public:
  explicit Canvas(QWidget *parent = nullptr);

signals:

public slots:
  void slot_uptate();
};

#endif // CANVAS_H
