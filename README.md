<div align="center">

# 🤖 EdgeAI-Robot

### Robô Autônomo com Detecção de Obstáculos em Tempo Real

[![Python](https://img.shields.io/badge/Python-3.8+-3776AB?style=for-the-badge&logo=python&logoColor=white)](https://python.org)
[![C++](https://img.shields.io/badge/C++-17-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)](https://isocpp.org)
[![YOLOv8](https://img.shields.io/badge/YOLOv8-Ultralytics-FF6B35?style=for-the-badge)](https://ultralytics.com)
[![Webots](https://img.shields.io/badge/Webots-Simulator-E94560?style=for-the-badge)](https://cyberbotics.com)

> Sistema de navegação autônoma em robótica simulada que integra visão computacional (YOLOv8), controle reativo e sensores de proximidade via comunicação TCP entre Python e C++.

</div>

## 📌 Sobre o Projeto

O **EdgeAI-Robot** é um projeto de robótica autônoma que combina visão computacional com simulação no Webots.

O sistema utiliza o modelo **YOLOv8 pré-treinado no COCO Dataset** para detectar objetos em tempo real através da câmera do robô e toma decisões de navegação baseadas na posição do objeto detectado.

A comunicação entre **Python (IA / YOLOv8)** e **C++ (controle no Webots)** é feita via **TCP Socket**, permitindo um sistema modular e em tempo real.

---

## ⚙️ Como Funciona

### 🧠 Arquitetura Geral

```
[C++ Webots]
     │ frame da câmera
     ▼
[Python YOLOv8]
     │ detecção de objetos
     ▼
[Processamento de decisão]
     │ comando (LEFT / FRONT / RIGHT)
     ▼
[C++ Controller]
     │ motores
     ▼
[Robô em movimento]
```

---

### 📍 Lógica de Decisão (Campo de Visão)

```
┌─────────────┬─────────────┬─────────────┐
│  ESQUERDA   │    CENTRO   │   DIREITA   │
│             │             │             │
│ vira direita│  segue reto │ vira esquerda│
└─────────────┴─────────────┴─────────────┘
      0      1/3 frame     2/3 frame     total
```

---

### 🛡️ Sensores de Segurança (e-puck)

```
           ▲ FRENTE ▲
    ps7   │         │   ps0
    ps6    \  🤖  /    ps1
    ps5    /       \   ps2
           ▼  TRÁS  ▼
        ps4            ps3
```

---

## 🛠️ Tecnologias

| Tecnologia | Função |
|------------|--------|
| YOLOv8 | Detecção de objetos |
| Python | Inferência + servidor TCP |
| C++ | Controle do robô |
| Webots | Simulação robótica |
| OpenCV | Processamento de imagem |
| NumPy | Manipulação de dados |

---

## 📁 Estrutura do Projeto

```
EdgeAI-Robot/
├── edge_ai_inference/
│   └── inference.py
│
├── webots_robot/
│   └── controllers/
│       └── robot_controller/
│           ├── robot_controller.cpp
│           └── Makefile
│
│   
│
├── requirements.txt
└── README.md
```

---

## 🧠 Comportamento do Sistema

O robô opera como um sistema de navegação reativa baseado em visão computacional.

- Detecta objetos em tempo real
- Converte posição do objeto em decisão de movimento
- Usa sensores do e-puck como fallback de segurança
- Evita colisões automaticamente
- Funciona em ambiente simulado no Webots

---

## 🎯 Classes Detectadas (Teste Real)

O modelo YOLOv8 foi testado no COCO Dataset, porém o cenário usado no projeto foi controlado:

| Classe | Resultado |
|--------|----------|
| 🐶 dog | Detecção principal e desvio correto |
| 👤 person | Detectado corretamente em aproximação |
| 🐘 elephant | Detectado em testes de escala/distância |
| 🛏️ bed | Detectado como obstáculo estático |

> Observação: o comportamento varia com distância — objetos podem mudar de classe (ex: dog ↔ person dependendo da escala).

---

## ⚠️ Limitações

- Pequena latência entre detecção e ação
- Erros em classes semelhantes
- Variação de detecção por distância
- Sensibilidade a ângulo da câmera
- YOLO pode confundir classes em objetos pequenos ou distantes

---

## 🚀 Como Rodar

### Pré-requisitos
- Webots
- Python 3.8+

### Instalação

```bash
pip install ultralytics opencv-python numpy
```

### Execução

1. Inicie o servidor Python:

```bash
cd edge_ai_inference
python inference.py
```

2. Abra o Webots e dê Play

---

## 🎥 Demo (YouTube)

> https://www.youtube.com/watch?v=DcYX8LEDtm0


### 📌 Descrição curta do video:

O robô detecta obstáculos, toma decisões de navegação e evita colisões automaticamente em ambiente simulado.

---

## 🧩 Arquitetura Completa

```
┌──────────────────────────────────────────────┐
│                  WEBOTS                      │
│                                              │
│   e-puck 🤖                                  │
│      │ frame                                 │
│      ▼                                      │
│  C++ Controller ───── TCP ─────▶ Python YOLO │
│      ▲                                      │
│      └──────── comandos ─────────────────────┘
│                                              │
│   navegação autônoma em tempo real          │
└──────────────────────────────────────────────┘
```

---

## 👨‍💻 Autor

Feito com 🔥 por **Luis Filipe**

GitHub: https://github.com/seu-usuario  
LinkedIn: https://linkedin.com/in/seu-usuario

---

## 📄 Licença

MIT License

