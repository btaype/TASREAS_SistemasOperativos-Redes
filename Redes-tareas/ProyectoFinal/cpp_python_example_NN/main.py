import torch
import torch.nn as nn
import numpy as np
import comunicacion
import csv
import sys

# Clase de red neuronal dinámica
class RedDinamica(nn.Module):
    def __init__(self, n_entrada, n_salida):
        super().__init__()
        self.fc1 = nn.Linear(n_entrada, 4)
        self.fc2 = nn.Linear(4, 4)
        self.fc3 = nn.Linear(4, 4)
        self.fc4 = nn.Linear(4, n_salida)

        self.red = nn.Sequential(
            self.fc1,
            nn.ReLU(),
            self.fc2,
            nn.ReLU(),
            self.fc3,
            nn.ReLU(),
            self.fc4
        )

    def forward(self, x):
        return self.red(x)



comunicacion.iniciar_cliente("127.0.0.1", 7003)#ip#host

nombre = input("NOMBRE: ")
comunicacion.login(nombre)
n1, n2 = comunicacion.datosentrenamiento()   
print(f" Entradas: {n1}, Salidas: {n2}")
 
red = RedDinamica(n1, n2)

while (1):

    entrada = comunicacion.recibirMuestra()#recibe la muestra apra entrenar

    if not entrada:  
        print("⚠️  Muestra vacía detectada. Guardando pesos y cerrando...")
        matrices = [
            red.fc1.weight.detach().clone().numpy().astype('float32').tolist(),
            red.fc2.weight.detach().clone().numpy().astype('float32').tolist(),
            red.fc3.weight.detach().clone().numpy().astype('float32').tolist(),
            red.fc4.weight.detach().clone().numpy().astype('float32').tolist()
        ]

        
        for i, matriz in enumerate(matrices):
            with open(f"pesos_W{i+1}.csv", "w", newline="") as f:
                writer = csv.writer(f)
                writer.writerows(matriz)
        break
        
    salida = comunicacion.recibirMuestra()#recibe los resultados de la muetra

    x = torch.tensor(entrada, dtype=torch.float32).view(1, n1)
    y = torch.tensor(salida, dtype=torch.float32).view(1, n2)

    
    criterio = nn.MSELoss()
    optimizador = torch.optim.SGD(red.parameters(), lr=0.01)

    max_iter = 10000
    umbral_loss = 1e-5

    for epoca in range(max_iter):
        optimizador.zero_grad()
        pred = red(x)
        loss = criterio(pred, y)
        loss.backward()
        optimizador.step()

        if loss.item() < umbral_loss:
            break

    # ✅ Obtener matriz de pesos final
    print("📦 Matrices de pesos:")
    matriz_pesos_fc1 = red.fc1.weight.data
    matriz_pesos_fc2 = red.fc2.weight.data
    matriz_pesos_fc3 = red.fc3.weight.data
    matriz_pesos_fc4 = red.fc4.weight.data

    print("W1:", matriz_pesos_fc1)
    print("W2:", matriz_pesos_fc2)
    print("W3:", matriz_pesos_fc3)
    print("W4:", matriz_pesos_fc4)
    matrices = [
    red.fc1.weight.detach().clone().numpy().astype('float32').tolist(),
    red.fc2.weight.detach().clone().numpy().astype('float32').tolist(),
    red.fc3.weight.detach().clone().numpy().astype('float32').tolist(),
    red.fc4.weight.detach().clone().numpy().astype('float32').tolist()
]
    comunicacion.enviar_matricesPesos(matrices)   
    matrix=comunicacion.recibirMatricesPesos()
    with torch.no_grad():
        red.fc1.weight.copy_(torch.tensor(matrix[0], dtype=torch.float32))
        red.fc2.weight.copy_(torch.tensor(matrix[1], dtype=torch.float32))
        red.fc3.weight.copy_(torch.tensor(matrix[2], dtype=torch.float32))
        red.fc4.weight.copy_(torch.tensor(matrix[3], dtype=torch.float32))
    print("✅ Nuevos pesos asignados:")
    print("W1:", red.fc1.weight)
    print("W2:", red.fc2.weight)
    print("W3:", red.fc3.weight)
    print("W4:", red.fc4.weight)


