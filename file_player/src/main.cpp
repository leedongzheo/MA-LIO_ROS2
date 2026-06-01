#include "mainwindow.h"

#include <QApplication>
#include <QProcess>

#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>

#include <rclcpp/rclcpp.hpp>

int main(int argc, char *argv[])
{
  rclcpp::init(argc, argv);

  QApplication app(argc, argv);
  auto node = std::make_shared<rclcpp::Node>("file_player");

  MainWindow window;
  window.RosInit(node);
  window.show();

  const int ret = app.exec();
  rclcpp::shutdown();
  return ret;
}
