import json
import time
import requests
from coapthon.resources.resource import Resource
from coapthon import defines    

NODE_RED_URL = "http://localhost:1880/sono"

class SonoResource(Resource):
    def __init__(self, name="sono"):
        super(SonoResource, self).__init__(
            name,
            visible = True,
            observable = False,
            allow_children = False
        )
        self.payload = "dados de monitoramento"

    def render_POST(self, request):
        # CHAMADO QUANDO O SERVIDOR RECEBE UMA REQUISIÇÃO DE POST
        try:
            payload_str = request.payload

            # dados recebidos
            dados = json.loads(payload_str)
            # adiciona o tempo do recebimento aos dados
            dados["timestamp"] = int(time.time())

            print("dados recebidos via CoAP:", dados)

            response = requests.post(
                NODE_RED_URL,
                json=dados,
                timeout=2
            )
            if response.status_code != 201:
                print("Aviso: Node-RED respondeu com status", response.status_code)

            self.payload = "Dados recebidos e encaminhados"
            self.code = defines.Codes.CREATED.number
            return self

        except Exception as e:
            self.payload = f"Erro no payload{e}"
            self.code = defines.Codes.INTERNAL_SERVER_ERROR.number
            return self