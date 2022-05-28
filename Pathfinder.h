/*
C. I: 26488388, Natanael Rojo
C. I:29.854.297 William Mendoza
*/

class Pathfinder
{
public:
  Pathfinder(Terrain* t, double num_hnodes, double num_vnodes): terrain(t)
  {
    // Write your solution here.
    real_t x_width = this->terrain->get_width()/num_hnodes;
    real_t y_hight = this->terrain->get_height()/num_vnodes;
    real_t medium_x = x_width/2;
    real_t medium_y = y_hight/2;
    
    auto pred1 = [&x_width, &y_hight, &medium_x, &medium_y](EuclideanGraph::Node* node_i, nat_t i, nat_t j){
      node_i->get_info().set_x((x_width * (j + 1)) - medium_x);
      node_i->get_info().set_y((y_hight * (i + 1)) - medium_y);
    };
    auto pred2 = [&x_width, &y_hight](EuclideanGraph::Arc*  arc){
      auto current_src_x = arc->get_src_node()->get_info().get_x();
      auto current_src_y = arc->get_src_node()->get_info().get_y();
      auto current_tgt_x = arc->get_tgt_node()->get_info().get_x();
      auto current_tgt_y = arc->get_tgt_node()->get_info().get_y();
      if (current_src_x == current_tgt_x and current_src_y != current_tgt_y){
        arc->get_info() = y_hight;
      } else if (current_src_y == current_tgt_y and current_src_x != current_tgt_x){
        arc->get_info() = x_width;
      } else {
        auto pitagoras = sqrt(Designar::pow(x_width, 2) + Designar::pow(y_hight, 2));
        arc->get_info() = pitagoras;
      }
    };
    this->graph = build_grid<EuclideanGraph>(num_hnodes, num_vnodes, pred1, pred2);
  }
  
  void apply_obstacles()
  {
    for (auto& obstacle : this->terrain->get_obstacles()){
      auto pred = [&obstacle](EuclideanGraph::Node* node){
      return is_inside(node->get_info(), obstacle);
    };
    auto pred2 = [&obstacle](EuclideanGraph::Arc* arc){
      Segment s(arc->get_src_node()->get_info(), arc->get_tgt_node()->get_info());
      return intersects(s, obstacle);
    };
      this->graph.remove_node_if(pred);
      this->graph.remove_arc_if(pred2);
    }
  }

  SLList<Point2D> search_path(const Point2D& s, const Point2D& t)
  {
    UNUSED(s) // Remove this line.
    UNUSED(t) // Remove this line.
    
    // Write your solution here.
    return SLList<Point2D>();
  }

  const EuclideanGraph & get_graph() const
  {
    // Write your solution here.
    return graph;
  }

private:  
  Terrain* terrain;
  EuclideanGraph graph;
};
