import pandas as pd
import matplotlib.pyplot as plt

archivo_csv = 'pc2_recibidos.csv'

data = pd.read_csv(archivo_csv)

milisegundos = data['Muestreo']
valor_ldr = data['Valor']

plt.figure(figsize=(10, 6))
plt.plot(milisegundos, valor_ldr, marker='o', linestyle='-', color='b', label='Lecturas LDR')

plt.xlabel('Milisegundos')
plt.ylabel('Valor LDR')
plt.title('Lecturas del LDR en el tiempo')

plt.grid(True)
plt.legend()
plt.tight_layout()
plt.show()
