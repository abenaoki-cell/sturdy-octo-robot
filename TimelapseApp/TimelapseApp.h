#ifndef TIMELAPSEAPP_H
#define TIMELAPSEAPP_H

#include <QWidget>
#include <QTimer>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QVBoxLayout>
#include <opencv2/opencv.hpp>

class TimelapseApp : public QWidget {
    Q_OBJECT

public:
    explicit TimelapseApp(QWidget *parent = nullptr);
    ~TimelapseApp();

private slots:
    void startCapture();
    void stopCapture();
    void captureImage();

private:
    QTimer *captureTimer;
    cv::VideoCapture camera;
    QString saveDirectory;
    int imageWidth, imageHeight, captureInterval;
    QString imageFormat;
    bool capturing;

    // GUI Components
    QSpinBox *widthSpinBox;
    QSpinBox *heightSpinBox;
    QSpinBox *intervalSpinBox;
    QComboBox *formatComboBox;
    QPushButton *startButton;
    QPushButton *stopButton;
    QPushButton *selectFolderButton;
    QLabel *statusLabel;

    void setupUI();
    void saveImage(const cv::Mat &frame, const QString &filePath);
};

#endif // TIMELAPSEAPP_H
