#ifndef MOSAICWORKER_H
#define MOSAICWORKER_H

#include <QObject>
#include <opencv2/opencv.hpp>
#include <vector>

class MosaicWorker : public QObject {
    Q_OBJECT

public:
    explicit MosaicWorker(const std::vector<cv::Mat>& scanvector, float platesize_x, float platesize_y, int m_img_width, int m_img_height, int overlap_x, int overlap_y);
    ~MosaicWorker();

signals:
    void progressChanged(int value);
    void finished(cv::Mat* result);

public slots:
    void process();

private:
    std::vector<cv::Mat> m_scanvector;
    float m_platesize_x;
    float m_platesize_y;
    int m_img_width;
    int m_img_height;
    int m_overlap_x;
    int m_overlap_y;
};

#endif // MOSAICWORKER_H
