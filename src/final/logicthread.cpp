#include "logicthread.h"
#include <QtCore>
#include <QDebug>

LogicThread::LogicThread()
{
    //Serial Test
    QextSerialPort * test = new QextSerialPort();
    test->setPortName("ttyUSB0");
    test->setBaudRate(BAUD115200);
    test->setDataBits(DATA_8);
    test->setParity(PAR_NONE);
    test->setStopBits(STOP_1);
    test->setFlowControl(FLOW_OFF);
    test->open(QIODevice::ReadWrite);
    if(!test->isOpen()) {
        qDebug("Error: Cannot connect to Serial port ttyUSB0, Doesnt exist or has incorrect permissions");
    } else {
        qDebug("Serial Device ttyUSB0 Test Successful!");
    }
    test->close();

    state = WAIT;
    lineReady = false;
    timedOut = false;
    moveEntered = false;
    endoOperation = false;
    portName = "ttyUSB0";
    /* QSERIALPORT LIB
    serial.setPortName(portName);
    serial.setBaudRate(QSerialPort::Baud115200);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);
    */

    serial.setPortName("ttyUSB0");
    serial.setBaudRate(BAUD115200);
    serial.setDataBits(DATA_8);
    serial.setParity(PAR_NONE);
    serial.setStopBits(STOP_1);
    serial.setFlowControl(FLOW_OFF);
    serial.setQueryMode(QextSerialPort::EventDriven);
    QObject::connect(&serial, SIGNAL(readyRead()), this, SLOT(slotLineReady()));
    serial.open(QIODevice::ReadWrite);
    // linux 'sudo chmod a+rw ttyUSB0'

    if(!serial.isOpen()) {
        qDebug("Error: Cannot connect to Serial port %s, Doesnt exist or has incorrect permissions", portName.toStdString().c_str());
    } else {
        qDebug("Serial Device %s Connected!", portName.toStdString().c_str());
        serial.moveToThread(this);
        serialDump();
    }
    sleep(1);

    lightLevel = -50;
    minRad = 10;
    maxRad = 40;
}

LogicThread::~LogicThread()
{
    serial.close();
}


void LogicThread::run()
{

    while(1)
    {
       // qDebug("While achieved!");

        switch(state)
        {
        case WAIT:
            // Waits for button input
            //qDebug("Bored in the Wait Space");
            break;

        case INIT:
            // Go Home
            serial.write(gantry_home('z'));
            qDebug("Write home to Z");
            if(serialIsOK(2000)) qDebug("Z Homed");
            serial.write(gantry_home('y'));
            qDebug("Write home to Y");
            if(serialIsOK(4000)) qDebug("Y Homed");
            serial.write(gantry_home('x'));
            qDebug("write home to X");
            if(serialIsOK(8000)) qDebug("X Homed");
            while(1); //TESSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSST
            emit sigCamDetect();
            break;

        case CAMDETECT:
            // Get Cam Stuff and Junk
            serial.write(gantry_linear_move(camCenterOffset_X, 0, 0, 9000));
            if(serialIsOK(5000)) qDebug("X in Cam Home Position");
            hitList.clear(); // ensure its empty before populating
            birdDetect();

            if(hitList.empty())
            {
                emit sigWait();
            }
            else
            {
                emit sigProcess();
            }
            break;

        case PROCESS:
            // process the hitlist
            if(hitList.empty())
            {
                qDebug("Hit List Empty");
                emit sigCamDetect();
                break;
            }
            else
            {

                if(nextTopHit())
                {
                    if(convertToReality())
                    {
                        qDebug("convert to reality is true");
                        qDebug("X: " + QString::number(toMoveX).toLatin1() + " Y: " + QString::number(toMoveY).toLatin1());
                        emit sigMoveToMushroom();
                        break;
                    }
                    else
                    {
                        qDebug("toMoveX and toMove Y are 0 yo");
                        emit sigCamDetect();
                        break;
                    }
                }
                else
                {
                    emit sigCamDetect();
                    break;
                }
            }
            break;

        case MOVETOMUSHROOM:
            //move to the shroom // NOT VALID, Need a better model of the workspace due to offset and different origins
            // 480/450 // 480 == xMM gantry, 450 pixels cropped : X AXIS
            // 440/350 // 480 == yMM gantry, 350 pixels cropped : Y AXIS
            // ratio xMM/pixelsX

            //serial.write(gantry_linear_move((toMoveX), (toMoveY*(1.47)), 9000));

            qDebug("got into movetomushroom");

                qDebug("convert to reality is true");
                qDebug("X: " + QString::number(toMoveX).toLatin1() + "Y: " + QString::number(toMoveY).toLatin1());
                serial.write(gantry_linear_move((toMoveX), (toMoveY), 9000));// PLACEHOLDER, DELETE WHEN READY
                msleep(2000);
                if(serialIsOK(10000)) qDebug("Moved Successfully to Mushroom x/y");
                emit sigEndoLocate();
                break;



        case ENDOLOCATE:
            // ENDO Alignment


            endoDetect();

            emit sigGetSeal();

            break;

        case GETSEAL:
            //

            emit sigMushroomDisplacement();
            break;

        case MUSHROOMDISPLACEMENT:
            //

            emit sigRaiseMushroom();
            break;

        case RAISEMUSHROOM:
            //

            emit sigMoveToBin();
            break;

        case MOVETOBIN:
            //

            emit sigProcess();
            break;
        }
        msleep(200);
    }
}

bool LogicThread::serialIsOK(int time)
{
    //QTimer::singleShot(time, this, SLOT(slotTimedOut()));
    qDebug("checking for ok");
    int i=0;
    while(1)
    {
        if(lineReady == true)
        {

            return true;

        }
        else if (timedOut == true)
        {
            qDebug("Timed Out");
            serialDump();
            emit sigWait();
            return false;
        }
        msleep(50);
        i++;
        if(i == (time/50)) timedOut = true;
    }

    /*
    QString lineFromSerial;
    QStringList splitLineFromSerial;
    QRegExp rx("(\\ |\\:)"); //Delimiters

    if(!waitforreadyread)
    {
        qDebug("Didnt Recieve OK from PCB. Resetting");
        serialDump();
        return(false);
    }
    else
    {
        if(serial.isReadable())
        {
            lineFromSerial = QString::fromStdString(serial.readLine().toStdString());

            if(lineFromSerial.isNull())
            {
                serialDump();
                return (false);
            }
            else
            {
                splitLineFromSerial = lineFromSerial.split(rx);
                if(QString::compare(splitLineFromSerial[0],"ok",Qt::CaseInsensitive))
                {
                    serialDump();
                    return(true);
                }else
                {
                    serialDump();
                    return(false);
                }
            }
        }
        else{
            return (false);
        }
    }
    */
}



void LogicThread::slotWaitEntered()
{
    qDebug("State Wait Entered");
    state = WAIT;
}

void LogicThread::slotInitEntered()
{
    qDebug("State Init Entered");
    state = INIT;

    //serial.moveToThread(this);

}

void LogicThread::slotCamDetectEntered()
{
    qDebug("State CamDetect Entered");
    state = CAMDETECT;
}

void LogicThread::slotProcessEntered()
{
    qDebug("State Process Entered");
    state = PROCESS;
}

void LogicThread::slotEndoLocateEntered()
{
    qDebug("State EndoLocate Entered");
    state = ENDOLOCATE;
}

void LogicThread::slotMoveToMushroomEntered()
{
    qDebug("State MoveToMushroom Entered");
    state = MOVETOMUSHROOM;
}

void LogicThread::slotGetSealEntered()
{
    qDebug("State GetSeal Entered");
    state = GETSEAL;
}

void LogicThread::slotMushroomDisplacementEntered()
{
    qDebug("State MushroomDisplacement Entered");
    state = MUSHROOMDISPLACEMENT;
}

void LogicThread::slotRaiseMushroomEntered()
{
    qDebug("State RaiseMushroom Entered");
    state = RAISEMUSHROOM;
}

void LogicThread::slotMoveToBinEntered()
{
    qDebug("State MoveToBin Entered");
    state = MOVETOBIN;
}

void LogicThread::serialDump()
{
    while (serial.canReadLine())
    {
     //serial.clear(QSerialPort::AllDirections);
     serial.flush();
    }
}

void LogicThread::slotCloseWindow()
{
    qDebug("Closing Serial Port");
    serial.close();
}

void LogicThread::slotBtnPushed()
{
    qDebug("Button Pushed!");
    emit sigInit();
}

void LogicThread::slotLineReady()
{
    qDebug("Line Ready");
    lineReady = true;
}

void LogicThread::slotTimedOut()
{
    timedOut = true;
}

void LogicThread::birdDetect()
{

    capWebcam.open(0);
    cv::Mat testFrame;

    for(int i = 0; i<20; i++)
    {
        capWebcam >> testFrame;
    }

    cv::Mat imgOriginal, imgHSV, imgThreshLow, imgThreshHigh, imgThresh, imgProcessed, imgDark;

    const int cannyT = 50;
    const int accumT = 30;

    std::vector<cv::Vec3f> v3fCircles, circleList, avgHitList;
    int frameNumber = 20, tolerance = 10;
    cv::Rect workEnvelope(100,20,475,460);
    cv::Rect imgCropped(50, 8, 338, 439);

    for(int i=0;i<frameNumber;i++) {

        capWebcam >> imgOriginal;

        imgOriginal = imgOriginal(workEnvelope);
        std::vector<cv::Point2f> quad_pts;
        std::vector<cv::Point2f> squre_pts;
        quad_pts.push_back(cv::Point2f(82,15));
        quad_pts.push_back(cv::Point2f(50, 442));
        quad_pts.push_back(cv::Point2f(360, 8));
        quad_pts.push_back(cv::Point2f(386, 442));

        squre_pts.push_back(cv::Point2f(50, 8));
        squre_pts.push_back(cv::Point2f(50, 447));
        squre_pts.push_back(cv::Point2f(388, 8));
        squre_pts.push_back(cv::Point2f(388, 447));

        cv::Mat transmtx = getPerspectiveTransform(quad_pts,squre_pts);
        cv::Mat transformed = cv::Mat::zeros(imgOriginal.rows, imgOriginal.cols, CV_8UC3);
        warpPerspective(imgOriginal, transformed, transmtx, imgOriginal.size());
        imgOriginal = transformed;
        imgOriginal = imgOriginal(imgCropped);
        imshow("imgorg", imgOriginal);

        imgOriginal.convertTo(imgDark,-1,1,lightLevel);
        cv::cvtColor(imgDark, imgHSV,CV_BGR2HSV);
        cv::inRange(imgHSV, cv::Scalar(0, 0, 100), cv::Scalar(255, 255, 255), imgThreshLow);
        cv::inRange(imgHSV, cv::Scalar(0, 0, 100), cv::Scalar(255, 255, 255), imgThreshHigh);
        cv::add(imgThreshLow, imgThreshHigh, imgThresh);
        //cv::GaussianBlur(imgThresh, imgThresh, cv::Size(3,3),0);
        //cv::Mat structuringElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3,3));
        //cv::dilate(imgThresh, imgThresh, structuringElement);
        //cv::erode(imgThresh, imgThresh, structuringElement);
        cv::HoughCircles(imgThresh, v3fCircles, CV_HOUGH_GRADIENT, 2, imgThresh.rows / 4, cannyT,accumT,minRad,maxRad);
        circleList.insert(circleList.end(), v3fCircles.begin(), v3fCircles.end());
    }
    capWebcam.release();

    if (circleList.size()!=0)
    {

    int w;
    foreach(cv::Vec3f c, circleList) {
        w = 0;

        for(int j = 0; j<(int)hitList.size(); j++) {
            if((((int)c[0]-tolerance)<=(int)hitList[j][0])&&((int)hitList[j][0]<=((int)c[0]+tolerance))&&
               (((int)c[1]-tolerance)<=(int)hitList[j][1])&&((int)hitList[j][1]<=((int)c[1]+tolerance))&&
               (((int)c[2]-tolerance)<=(int)hitList[j][2])&&((int)hitList[j][2]<=((int)c[2]+tolerance))) {
               hitNumbers[j]++;
               w++;
               avgHitList[j][0] += c[0];
               avgHitList[j][1] += c[1];
               avgHitList[j][2] += c[2];
               break;
               }
        }
        if (w==0){
            hitList.push_back(c);
            hitNumbers.push_back(1);
            avgHitList.push_back(c);
        }

    }
    for(int k = 0; k<(int)hitList.size(); k++)
    {
        hitList[k][0]= avgHitList[k][0]/hitNumbers[k];
        hitList[k][1]= avgHitList[k][1]/hitNumbers[k];
        hitList[k][2]= avgHitList[k][2]/hitNumbers[k];
    }

    QString hitListString;
     for(unsigned int i = 0; i<hitList.size();i++){
         hitListString += "Location: "+QString::number((int)hitList[i][0]).rightJustified(4, ' ')+","+QString::number((int)hitList[i][1]).rightJustified(4, ' ')+" Radius: "+QString::number(hitList[i][2], 'f',3).rightJustified(7, ' ')+", Hit Number: "+QString::number((int)hitNumbers[i]).rightJustified(2, ' ')+ "\n";
         cv::circle(imgOriginal, cv::Point((int)hitList[i][0],(int)hitList[i][1]),(int)hitList[i][2],cv::Scalar(0,0,255),3);
         cv::circle(imgOriginal, cv::Point((int)hitList[i][0],(int)hitList[i][1]),3,cv::Scalar(0,255,0),CV_FILLED);
     }

        qDebug(hitListString.toLatin1());



/*

while(!hitList.empty())
{


    if(hitList.empty()) break;

    imgOriginal = bluePoint;





    cv::Mat imgHSVBlue, imgThreshBlueLow, imgThreshBlueHigh, imgThreshBlue;
    std::vector<cv::Vec3f> v3fBlueCircles;
    cv::cvtColor(imgOriginal, imgHSVBlue,CV_BGR2HSV);


    cv::inRange(imgHSVBlue, cv::Scalar(110, 150, 150), cv::Scalar(130, 255, 255), imgThreshBlueLow);
        cv::inRange(imgHSVBlue, cv::Scalar(240, 100, 100), cv::Scalar(255, 255, 255), imgThreshBlueHigh);
        cv::add(imgThreshBlueLow, imgThreshBlueHigh, imgThreshBlue);
        cv::imshow("g", imgThreshBlue);
        msleep(500);
        //cv::HoughCircles(imgThreshBlue, v3fBlueCircles, CV_HOUGH_GRADIENT, 2, imgThreshBlue.rows / 4, cannyT,accumT,minRad,maxRad);

        std::vector<std::vector<cv::Point> > contours;

          cv::Mat bimage = imgThreshBlue;

          cv::findContours(bimage, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

          cv::Mat cimage = cv::Mat::zeros(bimage.size(), CV_8UC3);
          cv::RotatedRect box;

          for(size_t i = 0; i < contours.size(); i++)
          {
              size_t count = contours[i].size();
              if( count < 6 )
                  continue;

              cv::Mat pointsf;
              cv::Mat(contours[i]).convertTo(pointsf, CV_32F);
              box = cv::fitEllipse(pointsf);

              if( MAX(box.size.width, box.size.height) > MIN(box.size.width, box.size.height)*30 )
                  continue;
              cv::drawContours(cimage, contours, (int)i, cv::Scalar::all(255), 1, 8);

              cv::ellipse(cimage, box, cv::Scalar(0,0,255), 1, CV_AA);
              cv::ellipse(cimage, box.center, box.size*0.5f, box.angle, 0, 360, cv::Scalar(0,255,255), 1, CV_AA);
              cv::Point2f vtx[4];
              box.points(vtx);
              for( int j = 0; j < 4; j++ )
                  line(cimage, vtx[j], vtx[(j+1)%4], cv::Scalar(0,255,0), 1, CV_AA);
           // toMoveX = box.center.x;
           // toMoveY = box.center.y;
            //cv::circle(imgOriginal, cv::Point((int)box.center.x,(int)box.center.y),10,cv::Scalar(0,0,0),CV_FILLED);
              QString blueCenter;
            blueCenter = "Location: "+QString::number(box.center.x).rightJustified(4, ' ')+","+QString::number(box.center.y).rightJustified(4, ' ')+"\n";
              qDebug(blueCenter.toLatin1());
          }

         // cv::circle(imgOriginal, cv::Point((int)hitList[maxHit][0],(int)hitList[maxHit][1]),3,cv::Scalar(0,0,0),CV_FILLED); // draw blue circle on current max


          imshow("result", cimage);
          adjHitList.push_back(box.center);
          hitListOrdered.push_back(hitList[maxHit]);
          hitNumbersOrdered.push_back(hitNumbers[maxHit]);
          hitList.erase(hitList.begin() + maxHit);
          hitNumbers.erase(hitNumbers.begin() + maxHit);

}

*/
    /*
    QString BlueCircleString;
    BlueCircleString += "Location: "+QString::number((int)v3fBlueCircles[0][0]).rightJustified(4, ' ')+","+QString::number((int)v3fBlueCircles[0][1]).rightJustified(4, ' ')+" Radius: "+QString::number(v3fBlueCircles[0][2], 'f',3).rightJustified(7, ' ')+ "\n";
    qDebug(BlueCircleString.toStdString().c_str());

    //emit gotTargetCam((float)hitList[maxHit][0], (float)hitList[maxHit][1]);

    //ui->txtConsole->clear();
    //ui->txtConsole->appendPlainText(hitListString);
    qDebug(hitListString.toLatin1());

  */

    }
    else{
       // ui->txtConsole->clear();
        //ui->txtConsole->appendPlainText("Thar be no circles yo");
    }
    imgProcessed = imgThresh;

    QImage qimgOriginal = convertOpenCVMatToQtQImage(imgOriginal);
    QImage qimgProcessed = convertOpenCVMatToQtQImage(imgProcessed);

    emit updateCamUI(qimgOriginal, qimgProcessed);

    msleep(500);

}

bool LogicThread::nextTopHit()
{


    for(int i = 0;i<=(int)hitNumbers.size();i++){
        if (hitNumbers[i] == *std::max_element(hitNumbers.begin(),hitNumbers.end()) )
        {
            if(hitNumbers[i]<4)
            {
                qDebug("Hit List Empty, Redetecting");
                hitNumbers.clear();
                hitList.clear();
                return false;
            }

            else
            {

                maxHit= i;
               // cv::circle(imgOriginal, cv::Point((int)hitList[maxHit][0],(int)hitList[maxHit][1]),3,cv::Scalar(255,0,0),CV_FILLED); // draw blue circle on current max
               // QImage qimgOriginal = convertOpenCVMatToQtQImage(imgOriginal);
              //  emit updatelblOriginal(qimgOriginal);
                toMove = hitList[maxHit];
                hitList.erase(hitList.begin() + maxHit);
                hitNumbers.erase(hitNumbers.begin() + maxHit);
                toMoveX = toMove[0];
                toMoveY = toMove[1];
                qDebug("X: " + QString::number(toMoveX).toLatin1() + " Y: " + QString::number(toMoveY).toLatin1());
                return true;
            }
        }
    }


}


bool LogicThread::convertToReality()
{


    if (toMoveX == NULL || toMoveY == NULL)
    {
        qDebug("no toMoveX or toMoveY yo");
        return false;

    }
    else
    {

        toMoveX = toMoveX*xRatio;
        toMoveY = toMoveY*yRatio;
        toMoveX = xOffset-toMoveX-40;
        toMoveY = yOffset-toMoveY;
        return true;

    }


}

void LogicThread::endoDetect(){

    capEndo.open(1);
    cv::Mat testFrame2;

    for(int i = 0; i<20; i++){

      capEndo >> testFrame2;
      msleep(100);
    }

        imshow("test frame 2", testFrame2);
        sleep(5);


    cv::Mat imgOriginal, imgHSV, imgThreshLow, imgThreshHigh, imgThresh, imgProcessed, imgCropped, imgDark;

    const int cannyT = 50;
    const int accumT = 30;


    std::vector<cv::Vec3f> v3fCircles, circleList, hitListEndo, avghitListEndo;
    std::vector<int> hitNumbersEndo;
    int frameNumber = 20, tolerance = 10;
   // cv::Rect workEnvelope(0,0,450,350);

    for(int i=0;i<frameNumber;i++) {

        capEndo >> imgOriginal;

        //imgOriginal = imgOriginal(workEnvelope);
        imgOriginal.convertTo(imgDark,-1,1,lightLevel);
        cv::cvtColor(imgDark, imgHSV,CV_BGR2HSV);
        cv::inRange(imgHSV, cv::Scalar(0, 0, 100), cv::Scalar(255, 255, 255), imgThreshLow);
        cv::inRange(imgHSV, cv::Scalar(0, 0, 100), cv::Scalar(255, 255, 255), imgThreshHigh);
        cv::add(imgThreshLow, imgThreshHigh, imgThresh);
        //cv::GaussianBlur(imgThresh, imgThresh, cv::Size(3,3),0);
        //cv::Mat structuringElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3,3));
        //cv::dilate(imgThresh, imgThresh, structuringElement);
        //cv::erode(imgThresh, imgThresh, structuringElement);
        cv::HoughCircles(imgThresh, v3fCircles, CV_HOUGH_GRADIENT, 2, imgThresh.rows / 4, cannyT,accumT,minRad,maxRad);
        circleList.insert(circleList.end(), v3fCircles.begin(), v3fCircles.end());
    }
    imshow("EnDo ", imgThresh);


   // sleep(10);
   // capEndo.release();


    if (circleList.size()!=0)
    {

    int w;
    foreach(cv::Vec3f c, circleList) {
        w = 0;

        for(int j = 0; j<(int)hitListEndo.size(); j++) {
            if((((int)c[0]-tolerance)<=(int)hitListEndo[j][0])&&((int)hitListEndo[j][0]<=((int)c[0]+tolerance))&&
               (((int)c[1]-tolerance)<=(int)hitListEndo[j][1])&&((int)hitListEndo[j][1]<=((int)c[1]+tolerance))&&
               (((int)c[2]-tolerance)<=(int)hitListEndo[j][2])&&((int)hitListEndo[j][2]<=((int)c[2]+tolerance))) {
               hitNumbersEndo[j]++;
               w++;
               avghitListEndo[j][0] += c[0];
               avghitListEndo[j][1] += c[1];
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
            break;
        }
    }

    QString hitListEndoString;
    for(unsigned int i = 0; i<hitListEndo.size();i++){
        hitListEndoString += "Location: "+QString::number((int)hitListEndo[i][0]).rightJustified(4, ' ')+","+QString::number((int)hitListEndo[i][1]).rightJustified(4, ' ')+" Radius: "+QString::number(hitListEndo[i][2], 'f',3).rightJustified(7, ' ')+", Hit Number: "+QString::number((int)hitNumbersEndo[i]).rightJustified(2, ' ')+ "\n";
        cv::circle(imgOriginal, cv::Point((int)hitListEndo[i][0],(int)hitListEndo[i][1]),(int)hitListEndo[i][2],cv::Scalar(0,0,255),3);
        cv::circle(imgOriginal, cv::Point((int)hitListEndo[i][0],(int)hitListEndo[i][1]),3,cv::Scalar(0,255,0),CV_FILLED);
    }
    cv::circle(imgOriginal, cv::Point((int)hitListEndo[endoMaxHit][0],(int)hitListEndo[endoMaxHit][1]),3,cv::Scalar(255,0,0),CV_FILLED);

    qDebug(hitListEndoString.toLatin1());
    imshow(" heres the circle", imgOriginal);

    sleep(5);

    if (hitListEndo.size()>0)
    {
        //emit gotTargetEndo((float)hitListEndo[maxHit][0], (float)hitListEndo[maxHit][1]);
    }else{
        //goto the next highest in the list
    }
        //ui->txtConsole->clear();
        //ui->txtConsole->appendPlainText(hitListEndoString);
    }
    else{
        //ui->txtConsole->clear();
        //ui->txtConsole->appendPlainText("Thar be no circles yo");
    }
    imgProcessed = imgThresh;

  //  QImage qimgOriginal = convertOpenCVMatToQtQImage(imgOriginal);
    //QImage qimgProcessed = convertOpenCVMatToQtQImage(imgProcessed);

  //  emit updatelblOriginal(qimgOriginal);
    //ui->lblProcessed->setPixmap(QPixmap::fromImage(qimgProcessed));
    //ui->lblOriginal->setPixmap(QPixmap::fromImage(qimgOriginal));

}

QImage LogicThread::convertOpenCVMatToQtQImage(cv::Mat mat)
{
    if(mat.channels() == 1)
    {                                                           // if 1 channel (grayscale or black and white) image
        return QImage((uchar*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);     // return QImage
    } else if(mat.channels() == 3) {                            // if 3 channel color image
        cv::cvtColor(mat, mat, CV_BGR2RGB);                     // flip colors
        return QImage((uchar*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);       // return QImage
    } else {
        qDebug() << "in convertOpenCVMatToQtQImage, image was not 1 channel or 3 channel, should never get here";
    }
    return QImage();                                            // return a blank QImage if the above did not work
}

bool LogicThread::linear_move(char c, double x, int f) // move 1 axis
{
    QRegExp rx("(\\ |\\:)"); //Delimiters
    QString newLine;
    QStringList tempQueryList, newQuery;

    serial.write(gantry_linear_move(c, x, f));

    if(serialIsOK(10000))
    {
        qDebug("GOT OK: Linear Move");
        if (toupper(c) == 'X') desX = x;
        else if (toupper(c) == 'Y') desY = x;
        else if (toupper(c) == 'Z') desZ = x;

        serialDump();

        serial.write(gantry_get_pos());

        while (!serial.canReadLine());

        newLine = QString::fromStdString(serial.readLine().toStdString());
        qDebug(newLine.toLatin1()); // DEBUG ONLY

        newQuery = newLine.split(rx);

        while(!((tempQueryList[1].toFloat() == desX) && (tempQueryList[3].toFloat() == desY) && (tempQueryList[5].toFloat() == desZ)))
        {
            serial.write(gantry_get_pos());

            newLine = QString::fromStdString(serial.readLine().toStdString());
            newQuery = newLine.split(rx);

            qDebug("%s", newLine.toStdString().c_str()); // DEBUG ONLY

            msleep(100);

        }

        serialDump();

        currX = desX;
        currY = desY;
        currZ = desZ;
        desX = desY = desZ = -1;

    }else return (false);

}

bool LogicThread::linear_move(char c, double x, char cc, double y, int f) // move 2 axis
{
    serial.write(gantry_linear_move(x, y, f));
    if(serialIsOK(10000)) qDebug("GOT OK: Linear Move");
}

bool LogicThread::linear_move(char c, double x, char cc, double y, char ccc, double z, int f) // move 3 axis
{
    serial.write(gantry_linear_move(x, y, z, f));
    if(serialIsOK(10000)) qDebug("GOT OK: Linear Move");
}
