#include "endo.h"
#include <QtCore>
#include <QDebug>

Endo::Endo()
{

    minRadEndo = 180;
    maxRadEndo = 200;
}

void Endo::endoDetect(){

    capEndo.open(0);
    cv::Mat testFrame2;

    for(int i = 0; i<20; i++){

      capEndo >> testFrame2;
      //msleep(100);
    }

    imshow("test frame 2", testFrame2); //awghjgsedtrfgjhkgfdch
    //sleep(5); //testwesxdrjhk.asdrftgjikfgh
    qDebug("showed test fram");

    cv::Mat imgOriginalEndo, imgHSVEndo, imgThreshLowEndo, imgThreshHighEndo, imgThreshEndo, imgProcessedEndo, imgCroppedEndo, imgDarkEndo;

    const int cannyT = 50;
    const int accumT = 30;


    std::vector<cv::Vec3f> v3fCirclesEndo, circleListEndo, hitListEndo, avghitListEndo;
    std::vector<int> hitNumbersEndo;
    int frameNumberEndo = 20, toleranceEndo = 30;
   // cv::Rect workEnvelope(0,0,450,350);

    for(int i=0;i<frameNumberEndo;i++) {

        capEndo >> imgOriginalEndo;

        int lightLevelEndo = -50;

        //imgOriginalEndo = imgOriginalEndo(workEnvelope);
        imgOriginalEndo.convertTo(imgDarkEndo,-1,1,lightLevelEndo);
        cv::cvtColor(imgDarkEndo, imgHSVEndo,CV_BGR2HSV);
        cv::inRange(imgHSVEndo, cv::Scalar(0, 0, 100), cv::Scalar(255, 255, 255), imgThreshLowEndo);
        cv::inRange(imgHSVEndo, cv::Scalar(0, 0, 100), cv::Scalar(255, 255, 255), imgThreshHighEndo);
        cv::add(imgThreshLowEndo, imgThreshHighEndo, imgThreshEndo);
        //cv::GaussianBlur(imgThreshEndo, imgThreshEndo, cv::Size(3,3),0);
        //cv::Mat structuringElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3,3));
        //cv::dilate(imgThreshEndo, imgThreshEndo, structuringElement);
        //cv::erode(imgThreshEndo, imgThreshEndo, structuringElement);
        cv::HoughCircles(imgThreshEndo, v3fCirclesEndo, CV_HOUGH_GRADIENT, 2, imgThreshEndo.rows / 4, cannyT,accumT,minRadEndo,maxRadEndo);
        circleListEndo.insert(circleListEndo.end(), v3fCirclesEndo.begin(), v3fCirclesEndo.end());
    }
    imshow("EnDothresh ", imgThreshEndo);


   // sleep(10);
   capEndo.release();

    int maxEndo;

    if ((int) circleListEndo.size()>=1)
    {

    int w;
    foreach(cv::Vec3f c, circleListEndo) {
        w = 0;

        for(int j = 0; j<(int)hitListEndo.size(); j++) {
            if((((int)c[0]-toleranceEndo)<=(int)hitListEndo[j][0])&&((int)hitListEndo[j][0]<=((int)c[0]+toleranceEndo))&&
               (((int)c[1]-toleranceEndo)<=(int)hitListEndo[j][1])&&((int)hitListEndo[j][1]<=((int)c[1]+toleranceEndo))&&
               (((int)c[2]-toleranceEndo)<=(int)hitListEndo[j][2])&&((int)hitListEndo[j][2]<=((int)c[2]+toleranceEndo))) {
               hitNumbersEndo[j]++;
               w++;
               avghitListEndo[j][0] += c[0];
               avghitListEndo[j][1] += c[1];
               //sleep(5);
               avghitListEndo[j][2] += c[2];
               break;
               }
        }
        if (w==0){
            hitListEndo.push_back(c);
            hitNumbersEndo.push_back(1);
            avghitListEndo.push_back(c);
        }

    }
    for(int k = 0; k<(int)hitListEndo.size(); k++)
    {
        hitListEndo[k][0]= avghitListEndo[k][0]/hitNumbersEndo[k];
        hitListEndo[k][1]= avghitListEndo[k][1]/hitNumbersEndo[k];
        hitListEndo[k][2]= avghitListEndo[k][2]/hitNumbersEndo[k];
    }
    int endoMaxHit;
    for(int i = 0;i<=(int)hitNumbersEndo.size();i++){
        if (hitNumbersEndo[i] == *std::max_element(hitNumbersEndo.begin(),hitNumbersEndo.end()) )
        {
            endoMaxHit= i;
            maxEndo = endoMaxHit;
            break;
        }
    }

    QString hitListEndoString;
    for(unsigned int i = 0; i<hitListEndo.size();i++){
        hitListEndoString += "Location: "+QString::number((int)hitListEndo[i][0]).rightJustified(4, ' ')+","+QString::number((int)hitListEndo[i][1]).rightJustified(4, ' ')+" Radius: "+QString::number(hitListEndo[i][2], 'f',3).rightJustified(7, ' ')+", Hit Number: "+QString::number((int)hitNumbersEndo[i]).rightJustified(2, ' ')+ "\n";
        cv::circle(imgOriginalEndo, cv::Point((int)hitListEndo[i][0],(int)hitListEndo[i][1]),(int)hitListEndo[i][2],cv::Scalar(0,0,255),3);
        cv::circle(imgOriginalEndo, cv::Point((int)hitListEndo[i][0],(int)hitListEndo[i][1]),3,cv::Scalar(0,255,0),CV_FILLED);
    }
    cv::circle(imgOriginalEndo, cv::Point((int)hitListEndo[endoMaxHit][0],(int)hitListEndo[endoMaxHit][1]),3,cv::Scalar(255,0,0),CV_FILLED);

    qDebug(hitListEndoString.toLatin1());
    imshow(" heres the circle", imgOriginalEndo);
    //qDebug() << imgOriginalEndo.rows();
    //qDebug() << imgOriginalEndo.cols();

    //sleep(5); //what is this i dont even srdghjghfdrhfdrfdrgth

    if (hitListEndo.size()>0)
    {
        //emit gotTargetEndo((float)hitListEndo[maxHit][0], (float)hitListEndo[maxHit][1]);
    }else{
        //goto the next highest in the list
    }
        //ui->txtConsole->clear();
        //ui->txtConsole->appendPlainText(hitListEndoString);
    }
    //else{
        //ui->txtConsole->clear();
        //ui->txtConsole->appendPlainText("Thar be no circles yo");
    //}
    //imgProcessedEndo = imgThreshEndo;

  //  QImage qimgOriginal = convertOpenCVMatToQtQImage(imgOriginalEndo);
    //QImage qimgProcessedEndo = convertOpenCVMatToQtQImage(imgProcessedEndo);

  //  emit updatelblOriginal(qimgOriginal);
    //ui->lblProcessed->setPixmap(QPixmap::fromImage(qimgProcessedEndo));
    //ui->lblOriginal->setPixmap(QPixmap::fromImage(qimgOriginal));

    cv::Mat image, BWimage;
    cv::cvtColor(imgOriginalEndo, BWimage, CV_BGR2GRAY);

    image = BWimage;

    cv::namedWindow("Image", CV_WINDOW_AUTOSIZE);
    imshow("Image", image);

    float centreX = hitListEndo[maxEndo][0], centreY = hitListEndo[maxEndo][1], radius = hitListEndo[maxEndo][2]+15;

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

    imshow("Top Left", TL);
    imshow("Top Right", TR);
    imshow("Bottom Left", BL);
    imshow("Bottom Right", BR);
    imshow("Top", T);
    imshow("Left", L);
    imshow("Bottom", B);
    imshow("Right", R);


    qDebug() << pos;
}


