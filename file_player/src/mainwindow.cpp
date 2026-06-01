#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      my_ros_(new ROSThread(this, &mutex)),
      ui_(new Ui::MainWindow),
      play_flag_(false),
      pause_flag_(false),
      loop_flag_(false),
      stop_skip_flag_(true),
      slider_value_(0),
      slider_checker_(false)
{
  ui_->setupUi(this);
  my_ros_->start();

  connect(my_ros_, SIGNAL(StampShow(quint64)), this, SLOT(SetStamp(quint64)));
  connect(my_ros_, SIGNAL(StartSignal()), this, SLOT(Play()));

  connect(ui_->quitButton, SIGNAL(pressed()), this, SLOT(TryClose()));
  connect(ui_->pushButton, SIGNAL(pressed()), this, SLOT(FilePathSet()));
  connect(ui_->pushButton_2, SIGNAL(pressed()), this, SLOT(Play()));
  connect(ui_->pushButton_3, SIGNAL(pressed()), this, SLOT(Pause()));

  connect(ui_->doubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(PlaySpeedChange(double)));
  ui_->doubleSpinBox->setRange(0.01, 20.0);
  ui_->doubleSpinBox->setValue(1.0);
  ui_->doubleSpinBox->setSingleStep(0.01);

  connect(ui_->checkBox, SIGNAL(stateChanged(int)), this, SLOT(LoopFlagChange(int)));
  ui_->checkBox->setCheckState(loop_flag_ ? Qt::Checked : Qt::Unchecked);

  connect(ui_->checkBox_2, SIGNAL(stateChanged(int)), this, SLOT(StopSkipFlagChange(int)));
  ui_->checkBox_2->setCheckState(stop_skip_flag_ ? Qt::Checked : Qt::Unchecked);

  connect(ui_->checkBox_3, SIGNAL(stateChanged(int)), this, SLOT(AutoStartFlagChange(int)));
  ui_->checkBox_3->setCheckState(my_ros_->auto_start_flag_ ? Qt::Checked : Qt::Unchecked);

  connect(ui_->horizontalSlider, SIGNAL(sliderPressed()), this, SLOT(SliderPressed()));
  connect(ui_->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(SliderValueChange(int)));
  connect(ui_->horizontalSlider, SIGNAL(sliderReleased()), this, SLOT(SliderValueApply()));

  ui_->horizontalSlider->setRange(0, 10000);
  ui_->horizontalSlider->setValue(0);
}

MainWindow::~MainWindow()
{
  emit setThreadFinished(true);
  delete ui_;
  my_ros_->quit();
  if (!my_ros_->wait(500)) {
    my_ros_->terminate();
    my_ros_->wait();
  }
}

void MainWindow::RosInit(const rclcpp::Node::SharedPtr &node)
{
  node_ = node;
  my_ros_->ros_initialize(node_);
}

void MainWindow::TryClose()
{
  close();
}

void MainWindow::FilePathSet()
{
  play_flag_ = false;
  my_ros_->play_flag_ = false;
  ui_->pushButton_2->setText(QString::fromStdString("Play"));

  pause_flag_ = false;
  my_ros_->pause_flag_ = false;
  ui_->pushButton_3->setText(QString::fromStdString("Pause"));

  QFileDialog dialog;
  ui_->label->setText("Data is beging loaded.....");
  data_folder_path_ = dialog.getExistingDirectory();
  my_ros_->data_folder_path_ = data_folder_path_.toUtf8().constData();

  my_ros_->Ready();
  ui_->label->setText(data_folder_path_);
}

void MainWindow::SetStamp(quint64 stamp)
{
  ui_->label_2->setText(QString::number(stamp));
  if (!slider_checker_) {
    ui_->horizontalSlider->setValue(static_cast<int>(
        static_cast<float>(stamp - my_ros_->initial_data_stamp_) /
        static_cast<float>(my_ros_->last_data_stamp_ - my_ros_->initial_data_stamp_) * 10000));
  }
}

void MainWindow::Play()
{
  if (!my_ros_->play_flag_) {
    play_flag_ = true;
    my_ros_->play_flag_ = true;
    ui_->pushButton_2->setText(QString::fromStdString("End"));

    pause_flag_ = false;
    my_ros_->pause_flag_ = false;
    ui_->pushButton_3->setText(QString::fromStdString("Pause"));
  } else {
    play_flag_ = false;
    my_ros_->play_flag_ = false;
    ui_->pushButton_2->setText(QString::fromStdString("Play"));
  }
}

void MainWindow::Pause()
{
  if (!pause_flag_) {
    pause_flag_ = true;
    my_ros_->pause_flag_ = true;
    ui_->pushButton_3->setText(QString::fromStdString("Resume"));
  } else {
    pause_flag_ = false;
    my_ros_->pause_flag_ = false;
    ui_->pushButton_3->setText(QString::fromStdString("Pause"));
  }
}

void MainWindow::PlaySpeedChange(double value)
{
  my_ros_->play_rate_ = value;
}

void MainWindow::LoopFlagChange(int value)
{
  if (value == 2) {
    loop_flag_ = true;
    my_ros_->loop_flag_ = true;
  } else if (value == 0) {
    loop_flag_ = false;
    my_ros_->loop_flag_ = false;
  }
}

void MainWindow::StopSkipFlagChange(int value)
{
  if (value == 2) {
    stop_skip_flag_ = true;
    my_ros_->stop_skip_flag_ = true;
  } else if (value == 0) {
    stop_skip_flag_ = false;
    my_ros_->stop_skip_flag_ = false;
  }
}

void MainWindow::AutoStartFlagChange(int value)
{
  if (value == 2) {
    my_ros_->auto_start_flag_ = true;
  } else if (value == 0) {
    my_ros_->auto_start_flag_ = false;
  }
}

void MainWindow::SliderValueChange(int value)
{
  slider_value_ = value;
}

void MainWindow::SliderPressed()
{
  slider_checker_ = true;
}

void MainWindow::SliderValueApply()
{
  my_ros_->ResetProcessStamp(slider_value_);
  slider_checker_ = false;
}
