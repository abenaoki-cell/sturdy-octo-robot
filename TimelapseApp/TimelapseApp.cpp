#include "timelapseapp.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QTemporaryFile>

TimelapseApp::TimelapseApp(QWidget *parent) :
    QWidget(parent),
    captureTimer(new QTimer(this)),
    capturing(false) {
    setupUI();

    connect(startButton, &QPushButton::clicked, this, &TimelapseApp::startCapture);
    connect(stopButton, &QPushButton::clicked, this, &TimelapseApp::stopCapture);
    connect(selectFolderButton, &QPushButton::clicked, [this]() {
        saveDirectory = QFileDialog::getExistingDirectory(this, "Select Save Directory");
        statusLabel->setText("Save Directory: " + saveDirectory);
    });
    connect(captureTimer, &QTimer::timeout, this, &TimelapseApp::captureImage);

    camera.open(0, cv::CAP_DSHOW);
    if (!camera.isOpened()) {
        QMessageBox::critical(this, "Error", "Could not open camera.");
    }
}

TimelapseApp::~TimelapseApp() {
    if (camera.isOpened()) {
        camera.release();
    }
}

void TimelapseApp::setupUI() {
    // Layout and Widgets
    auto *layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel("Image Width:"));
    widthSpinBox = new QSpinBox(this);
    widthSpinBox->setRange(100, 4000);
    widthSpinBox->setValue(1920);
    layout->addWidget(widthSpinBox);

    layout->addWidget(new QLabel("Image Height:"));
    heightSpinBox = new QSpinBox(this);
    heightSpinBox->setRange(100, 4000);
    heightSpinBox->setValue(1080);
    layout->addWidget(heightSpinBox);

    layout->addWidget(new QLabel("Capture Interval (ms):"));
    intervalSpinBox = new QSpinBox(this);
    intervalSpinBox->setRange(100, 60000);
    intervalSpinBox->setValue(1000);
    layout->addWidget(intervalSpinBox);

    layout->addWidget(new QLabel("Image Format:"));
    formatComboBox = new QComboBox(this);
    formatComboBox->addItems({"JPG", "PNG", "BMP"});
    layout->addWidget(formatComboBox);

    selectFolderButton = new QPushButton("Select Folder", this);
    layout->addWidget(selectFolderButton);

    startButton = new QPushButton("Start Capture", this);
    layout->addWidget(startButton);

    stopButton = new QPushButton("Stop Capture", this);
    layout->addWidget(stopButton);

    statusLabel = new QLabel("Status: Ready", this);
    layout->addWidget(statusLabel);

    setLayout(layout);
}

void TimelapseApp::startCapture() {
    if (!camera.isOpened()) {
        QMessageBox::critical(this, "Error", "Camera is not initialized.");
        return;
    }

    imageWidth = widthSpinBox->value();
    imageHeight = heightSpinBox->value();
    captureInterval = intervalSpinBox->value();
    imageFormat = formatComboBox->currentText();

    if (saveDirectory.isEmpty()) {
        QMessageBox::warning(this, "Warning", "No directory selected.");
        return;
    }

    // タイムスタンプ付きディレクトリを生成
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    saveDirectory = QDir::toNativeSeparators(saveDirectory + "/" + timestamp);

    QDir dir(saveDirectory);
    if (!dir.exists()) {
        if (!dir.mkpath(saveDirectory)) {
            QMessageBox::critical(this, "Error", "Failed to create directory: " + saveDirectory);
            return;
        }
    }

    camera.set(cv::CAP_PROP_FRAME_WIDTH, imageWidth);
    camera.set(cv::CAP_PROP_FRAME_HEIGHT, imageHeight);

    capturing = true;
    captureTimer->start(captureInterval);
    statusLabel->setText("Status: Capturing...");
}


void TimelapseApp::stopCapture() {
    capturing = false;
    captureTimer->stop();
    statusLabel->setText("Status: Stopped.");
}

void TimelapseApp::captureImage() {
    if (!capturing || !camera.isOpened()) return;

    cv::Mat frame;
    camera >> frame;
    if (frame.empty()) return;

    QString filePath = saveDirectory + "/" +
                       QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + "." +
                       imageFormat.toLower();

    saveImage(frame, filePath);
}

void TimelapseApp::saveImage(const cv::Mat &frame, const QString &filePath) {
    QString nativeFilePath = QDir::toNativeSeparators(filePath);

    // OpenCVで画像をエンコード
    std::vector<uchar> buffer;
    std::vector<int> params;
    if (imageFormat == "JPG") params = {cv::IMWRITE_JPEG_QUALITY, 95};
    if (imageFormat == "PNG") params = {cv::IMWRITE_PNG_COMPRESSION, 3};

    std::string ext = "." + imageFormat.toLower().toStdString();
    if (!cv::imencode(ext, frame, buffer, params)) {
        QMessageBox::critical(this, "Error", "Failed to encode image.");
        return;
    }

    // QFileで保存
    QFile file(nativeFilePath);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Error", "Cannot write to: " + nativeFilePath);
        return;
    }
    file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    file.close();

    statusLabel->setText("Image saved to: " + nativeFilePath);
}
