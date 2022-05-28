# include <QApplication>
# include <canvas.H>

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  Canvas c;
  c.show();
  return app.exec();
}
