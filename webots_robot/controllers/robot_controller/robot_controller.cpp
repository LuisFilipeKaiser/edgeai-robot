//Includes
#include <webots/Robot.hpp>
#include <webots/Motor.hpp>
#include <webots/DistanceSensor.hpp>
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <webots/Camera.hpp>

//Linka a biblioteca de sockets do Windows
#pragma comment(lib, "ws2_32.lib")

//Remove a necessidade de usar webots:: e std:: ao chamar classes e funções
using namespace webots;
using namespace std;

//Velocidade maxima pré-definida pelo modelo do e-puck
#define MAX_SPEED 6.28


int main(int argc, char **argv) {


Robot *robot = new Robot();
int timeStep = (int)robot->getBasicTimeStep();

WSADATA wsaData;                        //estrutura que guarda info do Winsock
WSAStartup(MAKEWORD(2,2), &wsaData);   //inicializa o Winsock versão 2.2

//Cria um socket TCP para comunicação com o servidor Python (YOLO)
SOCKET sock = socket (AF_INET, SOCK_STREAM, 0); 

//Configura endereço IP e porta do servidor Python e estabelece conexão TCP
sockaddr_in serverAddr;
serverAddr.sin_family = AF_INET;

serverAddr.sin_port = htons(5000);
inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

connect (sock, (sockaddr*)&serverAddr, sizeof(serverAddr));


Motor *leftMotor = robot->getMotor("left wheel motor");
Motor *rightMotor = robot->getMotor("right wheel motor");


leftMotor->setPosition(INFINITY);
rightMotor->setPosition(INFINITY);
leftMotor->setVelocity(0.0);
rightMotor->setVelocity(0.0);


Camera *camera = robot->getCamera("camera");
camera->enable(timeStep);

// Array dos sensores de distância do robô e-puck
DistanceSensor *ps[8];
char psName[4];

// Inicializa e ativa os 8 sensores de distância do robô e-puck
for(int i = 0;i < 8; i++){
  sprintf(psName, "ps%d", i);
  ps[i] = robot->getDistanceSensor(psName);
  ps[i]->enable(timeStep);
}


printf("EdgeAI-Robot iniciado com sucesso\n");
printf("e-puck pronto para desviar de obstaculos\n");




while (robot->step(timeStep) != -1) {

  //Leitura dos valores dos sensores de distância
  double psValue[8];
  for (int i = 0; i < 8; i++) {
    psValue[i] = ps[i]->getValue();
  }

  //Interpreta os valores dos sensores para detecção de obstáculos
  bool obstaculoFrente = psValue[0] > 80.0 || psValue[7] > 80.0;
  bool obstaculoEsquerda = psValue[5] > 80.0 || psValue[6] > 80.0;
  bool obstaculoDireita = psValue[1] > 80.0 || psValue[2] > 80.0;

  //Envio de imagens
  const unsigned char *image = camera->getImage();
  int width = camera->getWidth();
  int height = camera->getHeight();
  int imageSize = width * height * 4;
  send(sock, (const char*)image, imageSize, 0);

  //Recebe resposta do modelo YOLO via socket
  char buffer[32] = {0};
  int len = recv(sock, buffer, sizeof(buffer)-1, 0);

  std::string msg = "NONE:NONE";

  if (len > 0) {
    buffer[len] = '\0';
    msg = std::string(buffer);
  }

  //Parsing seguro
  std::string comando = msg;
  std::string objeto = "NONE";

  size_t pos = msg.find(":");
  if (pos != std::string::npos) {
    comando = msg.substr(0, pos);
    objeto = msg.substr(pos + 1);
  }

  if (objeto != "NONE" && objeto != "") {
    printf("COMANDO: %s | OBJETO: %s\n",
           comando.c_str(),
           objeto.c_str());
}

double baseSpeed = MAX_SPEED * 0.6;
double leftSpeed = baseSpeed;
double rightSpeed = baseSpeed;

// PRIORIDADE 1: sensores
if (obstaculoFrente) {
  // gira pra escapar
  leftSpeed  = MAX_SPEED * 0.6;
  rightSpeed = -MAX_SPEED * 0.3;

} else if (obstaculoDireita) {
  // desvia pra esquerda
  leftSpeed  = MAX_SPEED * 0.3;
  rightSpeed = MAX_SPEED * 0.6;

} else if (obstaculoEsquerda) {
  // desvia pra direita
  leftSpeed  = MAX_SPEED * 0.6;
  rightSpeed = MAX_SPEED * 0.3;

} 
// PRIORIDADE 2: YOLO
else {
  if (comando == "ESQUERDA") {
    // virar esquerda
    leftSpeed  = -MAX_SPEED * 0.3;
    rightSpeed = MAX_SPEED * 0.6;

  } else if (comando == "DIREITA") {
    // virar direita
    leftSpeed  = MAX_SPEED * 0.6;
    rightSpeed = -MAX_SPEED * 0.3;

  } else {
    // seguir reto
    leftSpeed = baseSpeed;
    rightSpeed = baseSpeed;
  }
}

leftMotor->setVelocity(leftSpeed);
rightMotor->setVelocity(rightSpeed);

  
}

//Desligando Conexões
closesocket(sock);
WSACleanup();

//Encerramento do Sistema
delete robot;
return 0;

}
