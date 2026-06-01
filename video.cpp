#include <opencv2/opencv.hpp>

int main()
{
    cv::VideoCapture cap(0);

    if (!cap.isOpened())
        return -1;

    cv::Mat frame;

    while (true)
    {
        cap >> frame;

        if (frame.empty())
            break;

        cv::imshow("Camera", frame);

        if (cv::waitKey(1) == 27) // ESC
            break;
    }

    return 0;
}
