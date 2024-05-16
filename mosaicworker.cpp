#include "mosaicworker.h"

MosaicWorker::MosaicWorker(const std::vector<cv::Mat>& scanvector, float platesize_x, float platesize_y,
                           int m_img_width, int m_img_height,
                           int overlap_x, int overlap_y)
    : m_scanvector(scanvector), m_platesize_x(platesize_x), m_platesize_y(platesize_y),
      m_img_width(m_img_width), m_img_height(m_img_height),
      m_overlap_x(overlap_x), m_overlap_y(overlap_y)
{
    std::cout << "m_scanvector size: " << m_scanvector.size() << std::endl;
    std::cout << "m_platesize_x: " << m_platesize_x << std::endl;
    std::cout << "m_platesize_y: " << m_platesize_y << std::endl;
    std::cout << "m_img_width: " << m_img_width << std::endl;
    std::cout << "m_img_height: " << m_img_height << std::endl;
    std::cout << "m_overlap_x: " << m_overlap_x << std::endl;
    std::cout << "m_overlap_y: " << m_overlap_y << std::endl;
}

MosaicWorker::~MosaicWorker() {
}

void MosaicWorker::process() {
    try {
           std::cout << "Thread started" << std::endl;

           // Check if m_scanvector is empty
           if (m_scanvector.empty()) {
               std::cerr << "Error: m_scanvector is empty." << std::endl;
               return;
           }

           // Print member variables
           std::cout << "m_scanvector size: " << m_scanvector.size() << std::endl;
           std::cout << "m_platesize_x: " << m_platesize_x << std::endl;
           std::cout << "m_platesize_y: " << m_platesize_y << std::endl;
           std::cout << "m_img_width: " << m_img_width << std::endl;
           std::cout << "m_img_height: " << m_img_height << std::endl;
           std::cout << "m_overlap_x: " << m_overlap_x << std::endl;
           std::cout << "m_overlap_y: " << m_overlap_y << std::endl;

           // Check if any of the dimensions are invalid
           if (m_img_width <= 0 || m_img_height <= 0 || m_overlap_x < 0 || m_overlap_y < 0) {
               std::cerr << "Error: Invalid dimensions." << std::endl;
               return;
           }

           // Calculate dimensions
           int wmax = static_cast<int>(m_platesize_x / m_img_width);
           int hmax = static_cast<int>(m_platesize_y / m_img_height);
           std::cout << "wmax: " << wmax << ", hmax: " << hmax << std::endl;

           // Check if Mimage allocation is successful
           //cv::Mat* Mimage = new cv::Mat(cv::Mat::zeros((m_img_height + m_overlap_y), (m_img_width - m_overlap_x), CV_8UC3));
            int pixl_image_width = m_scanvector.at(0).cols;
            int pixl_image_height = m_scanvector.at(0).rows;
            cv::Mat* Mimage = new cv::Mat(cv::Mat::zeros((hmax * (pixl_image_height - m_overlap_y) + m_overlap_y),
                                                         (wmax * (pixl_image_width - m_overlap_x) + m_overlap_x), CV_8UC3));
            std::cout<< "Mimage shapes:" <<Mimage->rows <<" "<< Mimage->cols << "\n";
            if (!Mimage) {
                std::cerr << "Error: Failed to allocate memory for Mimage." << std::endl;
                return;
            }

            float p_v = 0.0f;
            for (int i = 0; i < hmax; ++i) {
                for (int j = 0; j < wmax; ++j) {
                    p_v = (static_cast<float>((wmax) * i + j) / static_cast<float>((hmax) * (wmax))) * 100;
                    emit progressChanged(static_cast<int>(p_v));

                    int rows = m_scanvector.at(i * wmax + j).rows;
                    int cols = m_scanvector.at(i * wmax + j).cols;

                    for (int jj = 0; jj < rows - m_overlap_y; ++jj) {
                        for (int ii = 0; ii < cols - m_overlap_x; ++ii) {
                            int dst_y = jj + i * (pixl_image_height - m_overlap_y);
                            int dst_x = ii + j * (pixl_image_width - m_overlap_x);

                            if (dst_y < Mimage->rows && dst_x < Mimage->cols) {
                                Mimage->at<cv::Vec3b>(dst_y, dst_x) = m_scanvector.at(i * wmax + j).at<cv::Vec3b>(jj, ii);
                            }
                        }
                    }
                }
            }
    // Emit finished signal
    emit finished(Mimage);
        } catch (const std::exception& e) {
    std::cerr << "Exception in MosaicWorker::process(): " << e.what() << std::endl;
        } catch (...) {
    std::cerr << "Unknown exception in MosaicWorker::process()." << std::endl;
    }
}
