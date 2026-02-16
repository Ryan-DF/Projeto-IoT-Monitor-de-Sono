import json
import time
from db.database import insert_leitura
from coapthon.resources.resource import Resource
from coapthon import defines    
from json import JSONDecodeError

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
        payload_str = request.payload

        # Parseamento do JSON
        try:
            dados = json.loads(payload_str)
        except JSONDecodeError:
            self.payload = "JSON inválido"
            self.code = defines.Codes.BAD_REQUEST.number
            return self

        # Validação dos dados
        try:
            validate_payload(dados)
        except (TypeError, ValueError) as e:
            self.payload = f"Erro de validação: {str(e)}"
            self.code = defines.Codes.BAD_REQUEST.number
            return self

        # Inserção no banco
        try:
            insert_leitura(dados)
        except sqlite3.Error as e:
            self.payload = "Erro interno ao salvar dados"
            self.code = defines.Codes.INTERNAL_SERVER_ERROR.number  
            return self  

        self.payload = "Dados recebidos com sucesso"
        self.code = defines.Codes.CREATED.number
        return self



def validate_payload(dados: dict):
    if not isinstance(dados, dict):
        raise TypeError("Formato invalido de JSON")
    chaves_obrigatorias = {"device_id", "ruido_dbfs", "temperatura", "umidade"}
    chaves_recebidas = set(dados.keys())
    chaves_faltantes = chaves_obrigatorias - chaves_recebidas

    if  chaves_faltantes:
        raise ValueError(f"Chave(s) não encontradas: {chaves_faltantes}.")

    chaves_esperadas = {
        "device_id": str,
        "ruido_dbfs": (int, float),
        "temperatura": (int, float),
        "umidade": (int, float),
    }

    for campo, tipo_correto in chaves_esperadas.items():
        valor = dados[campo]
        if not isinstance(valor, tipo_correto):
            raise ValueError(
                f"{campo} deve ser do tipo: "
                f"{tipo_correto if isinstance(tipo_correto, type) else 'int ou float'},"
                f"não {type(valor).__name__}"
            )
