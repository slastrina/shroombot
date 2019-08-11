#include <QCoreApplication>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core.hpp>
#include <iostream>
#include <QDebug>
#include <QVector>
#include <endo.h>

//float for centre x and y and radius

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
/*
    cv::Mat image, BWimage;
    image = cv::imread("Shroom.jpg");

    resize(image, image, cv::Size(image.cols/4, image.rows/4));
    cv::cvtColor(image, BWimage, CV_BGR2GRAY);
    image = BWimage;

    cv::namedWindow("Image", CV_WINDOW_AUTOSIZE);
    imshow("Image", image);




            //Get passed in an Mat image and a centre point and radius

    // Mat image
    float centreX = 480, centreY = 270, radius = 85;


    cv::Mat TL(image, cv::Rect(0,0, centreX-radius, centreY-radius));
    cv::Mat TR(image, cv::Rect(centreX+radius, 0, image.cols-(centreX+radius), centreY-radius));
    cv::Mat BL(image, cv::Rect(0, centreY+radius, centreX-radius, image.rows-(centreY+radius)));
    cv::Mat BR(image, cv::Rect(centreX+radius, centreY+radius, image.cols-(centreX+radius), image.rows-(centreY+radius)));
    cv::Mat T(image, cv::Rect(centreX-radius, 0, 2*radius, centreY-radius));
    cv::Mat B(image, cv::Rect(centreX-radius, centreY+radius, 2*radius, image.rows-(centreY+radius)));
    cv::Mat L(image, cv::Rect(0, centreY-radius, centreX-radius, 2*radius));
    cv::Mat R(image, cv::Rect(centreX+radius, centreY-radius, image.cols-(centreX+radius), 2*radius));



    double TLLevel = cv::mean(TL)[0] + cv::mean(TL)[1]  + cv::mean(TL)[2];
    double TRLevel = cv::mean(TR)[0] + cv::mean(TR)[1]  + cv::mean(TR)[2];
    double BLLevel = cv::mean(BL)[0] + cv::mean(BL)[1]  + cv::mean(BL)[2];
    double BRLevel = cv::mean(BR)[0] + cv::mean(BR)[1]  + cv::mean(BR)[2];
    double TLevel = cv::mean(T)[0] + cv::mean(T)[1]  + cv::mean(T)[2];
    double BLevel = cv::mean(B)[0] + cv::mean(B)[1]  + cv::mean(B)[2];
    double LLevel = cv::mean(L)[0] + cv::mean(L)[1]  + cv::mean(L)[2];
    double RLevel = cv::mean(R)[0] + cv::mean(R)[1]  + cv::mean(R)[2];


    std::vector<double> directions;


    directions.push_back(TLLevel + TLevel + TRLevel);
    directions.push_back(TLevel + TRLevel + RLevel);
    directions.push_back(TRLevel + RLevel + BRLevel);
    directions.push_back(RLevel + BRLevel + BLevel);
    directions.push_back(BRLevel + BLevel + BLLevel);
    directions.push_back(BLevel + BLLevel + LLevel);
    directions.push_back(BLLevel + LLevel + TLLevel);
    directions.push_back(LLevel + TLLevel + TLevel);

    int pos = 0;

    for(int i = 0; i < (int)directions.size()-1; i++)
    {
        if (directions[i+1] < directions[pos])
        {
            pos = i+1;
        }
    }


    qDebug() << pos;
*/
//Returns an int pos
/*
 * 0 = North
 * 1 = NorthEast
 * 2 = East
 * 3 = SouthEast
 * 4 = South
 * 5 = SouthWest
 * 6 = West
 * 7 = NorthWest
*/

    Endo e;

    e.Endo::endoDetect();

    return a.exec();
}

