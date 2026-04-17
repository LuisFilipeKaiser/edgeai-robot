#Imports
import socket
from ultralytics import YOLO 
import cv2
import numpy as np



model = YOLO("yolov8n.pt") # Modelo YOLOv8 pré-treinado no COCO

#Conexão via socket entre Python e C++
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

server.bind(("localhost", 5000)) #Definindo a porta local de comunicação

server.listen(1) # Aguardando a conexão com Webots
print("Aguardando a conexão com o robo.....")

conn,addr = server.accept() #Conexão aceita com o Webots
print(f"Robô conectado: {addr}")

#Definindo os valores da camera com base nos valores da camera no Webots
width= 320 #Definindo a largura
height = 240 #Definindo a altura
imageSize = width * height * 4 #Tamanho total da imagem


cv2.namedWindow("EDGEAI-ROBOT", cv2.WINDOW_NORMAL)
cv2.setWindowProperty("EDGEAI-ROBOT", cv2.WND_PROP_TOPMOST, 1)

# Área mínima para considerar uma detecção válida (evita ruído/falsos positivos)
MIN_AREA = 1500

# Contador usado para medir estabilidade da detecção ao longo dos frames
# (ajuda a evitar mudanças bruscas de decisão)
stable_count = 0


#Recebimento de Frames
while True:
    data = b"" # buffer para acumular os bytes recebidos
    while len(data) < imageSize: # recebe até ter o frame completo
        packet = conn.recv(imageSize - len(data))
        if not packet:
            break
        data += packet

        

    # converte bytes pra imagem
    frame = np.frombuffer(data, dtype=np.uint8).reshape((height, width, 4))
    frame = cv2.cvtColor(frame, cv2.COLOR_BGRA2BGR)

    #Confiança Minima de Desvio
    results = model(frame, conf=0.5)

    comando = "FRENTE"
    largura = frame.shape[1]

    #Guarda objeto detectado
    best_area = 0
    best_x = None
    best_name = None

    # Processa detecções do YOLO (análise dos objetos no frame)
    for result in results:
        for box in result.boxes:

            # Classe detectada pelo YOLO (ex: pessoa, cachorro, etc.)
            cls = int(box.cls[0])
            name = model.names[cls]
            conf = float(box.conf[0])

            x1, y1, x2, y2 = map(int, box.xyxy[0])

            # centro e area
            area = (x2 - x1) * (y2 - y1)
            centro_objeto = (x1 + x2) / 2

            #Desenha bounding box e rótulo do objeto detectado
            cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 0), 2)

            label = f"{name} {conf:.2f}"

            cv2.putText(
                frame,
                label,
                (x1, y1 - 10),
                cv2.FONT_HERSHEY_SIMPLEX,
                0.5,
                (0, 255, 0),
                2
            )
            
            
            # Seleciona o maior objeto relevante
            if area > best_area and area > MIN_AREA:
                best_area = area
                best_x = centro_objeto
                best_name = name
        
   
    if best_area > MIN_AREA:
        stable_count += 1
    else:
        stable_count = 0

    #Decisao fora do loop
    margem = largura * 0.15

    # decisão de direção baseada na posição do objeto no frame
    if stable_count >= 3 and best_x is not None:
        if best_x < (largura / 2 - margem):
            comando = "DIREITA"
        elif best_x > (largura / 2 + margem):
            comando = "ESQUERDA"
        else:
            comando = "FRENTE"

        print(f"Obstáculo principal: {best_name}")#Mostra o objeto na terminal

   

    mensagem = f"{comando}:{best_name if best_name else 'NONE'}"
    conn.send(mensagem.encode()) # envia comando para o robô via TCP

    #Janela em tempo real com a imagem capturada pelo e-puck
    cv2.imshow("EDGEAI-ROBOT", frame)

    # manter janela ativa
    cv2.setWindowProperty("EDGEAI-ROBOT", cv2.WND_PROP_TOPMOST, 1)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

    


#Desligando sistema
cv2.destroyAllWindows()
conn.close()
server.close() 