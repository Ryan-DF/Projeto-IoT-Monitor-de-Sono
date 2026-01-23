import json
import time
import random
from coapthon.client.helperclient import HelperClient


SERVER_HOST = "127.0.0.10"
SERVER_PORT = 5683
RESOURCE = "sono"

DEVICE_ID = "sleep_01"
INTERVALO_SEGUNDOS = 5  # tempo entre envios


def gerar_ruido_dbfs():
    """
    Simula ruído ambiente durante o sono.
    """
    return round(random.uniform(-55.0, -35.0), 2)

def gerar_temperatura():
    """
    Simular temperatura durante o sono
    """
    return round(random.uniform(20, 30), 2)

def gerar_umidade():
    return round(random.uniform(50, 60), 2)

def main():
    client = HelperClient(server=(SERVER_HOST, SERVER_PORT))

    print("Iniciando simulador CoAP...")
    print(f"Enviando dados a cada {INTERVALO_SEGUNDOS} segundos\n")

    try:
        while True:
            payload = {
                "device_id": DEVICE_ID,
                "ruido_dbfs": gerar_ruido_dbfs(),
                "temperatura": gerar_temperatura(),
                "umidade": gerar_umidade()
            }

            response = client.post(
                RESOURCE,
                json.dumps(payload)
            )

            if response:
                print("Enviado:", payload, "| Resposta:", response.payload)
            else:
                print("Sem resposta do servidor")

            time.sleep(INTERVALO_SEGUNDOS)

    except KeyboardInterrupt:
        print("\nSimulador finalizado pelo usuário")

    finally:
        client.stop()


if __name__ == "__main__":
    main()