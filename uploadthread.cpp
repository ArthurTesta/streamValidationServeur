#include "uploadthread.h"

UploadThread::UploadThread(int socketDescriptor, QString pathFile, QObject * parent) : socketDescriptor(socketDescriptor),pathFile(pathFile+"/"), QThread(parent){
    reading=false;
    clientConnection = new QTcpSocket();
    connect(clientConnection,SIGNAL(readyRead()),this,SLOT(readData()));
    if (!clientConnection->setSocketDescriptor(socketDescriptor)) {
        emit error(clientConnection->error());
        return;
    }
    qDebug() << "[SERV] Waiting for signal readyRead()" ;
    bdContent = new MediaList();
}

void UploadThread::run(){
    /*
      ARTHUR 27/11 :
      Bloque tant qu'il n'y a rien à lire
     */
    clientConnection->waitForReadyRead(-1);
}

void UploadThread::readData(){
    if(reading)return;
    reading=true;
    qDebug() << "[SERV] - READ";
    fileName=readQStringSock(clientConnection);
    qDebug() << "[SERV] - File" << pathFile+fileName;
    sendData(fileName);
}

void UploadThread::sendData(QString fileName){
    int code=-1;
    qDebug() << "[SERV] - SENDING DATA";
    if(bdContent->isMovieInDB(fileName)){
        qDebug() << "[SERV] - File FOUND" << pathFile+fileName;
        code=0;
        QString result("Sending stream");
        writeIntSock(&code,clientConnection);
        writeQStringSock(result, clientConnection);
    }else{

        QString result("Media is not in the server list");
        writeIntSock(&code,clientConnection);
        writeQStringSock(result, clientConnection);
    }
}

