#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <algorithm>
#include <chrono>
#include <ctime>
#include <dirent.h>
#include <iostream>
#include <signal.h>
#include <string.h>

#include <QCloseEvent>
#include <QDateTime>
#include <QDoubleSpinBox>
#include <QErrorMessage>
#include <QFileDialog>
#include <QInputDialog>
#include <QMainWindow>
#include <QMutex>
#include <QProcess>
#include <QThread>
#include <QVector>

#include <rclcpp/rclcpp.hpp>

#include "ROSThread.h"

#define R2D 180/PI
#define D2R PI/180
#define POWER_CTR_DELAY 200000
#define INTENSITY_MIN 0.0
#define INTENSITY_MAX 100.0
#define INTENSITY_COLOR_MIN 0.0
#define INTENSITY_COLOR_MAX 1.0

using namespace std;

extern QMutex mutex;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  void RosInit(const rclcpp::Node::SharedPtr &node);

private slots:
  void TryClose();
  void FilePathSet();
  void Play();
  void Pause();
  void PlaySpeedChange(double value);
  void LoopFlagChange(int value);
  void StopSkipFlagChange(int value);
  void AutoStartFlagChange(int value);
  void SetStamp(quint64 stamp);
  void SliderValueChange(int value);
  void SliderPressed();
  void SliderValueApply();

signals:
  void setThreadFinished(bool);

private:
  QMutex mutex;
  ROSThread *my_ros_;
  Ui::MainWindow *ui_;
  QString data_folder_path_;
  bool play_flag_;
  bool pause_flag_;
  bool loop_flag_;
  bool stop_skip_flag_;
  int slider_value_;
  int slider_checker_;

  rclcpp::Node::SharedPtr node_;
};

#endif // MAINWINDOW_H
