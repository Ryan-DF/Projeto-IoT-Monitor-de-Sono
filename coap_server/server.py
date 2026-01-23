from coapthon.server.coap import CoAP
from resources.sono_resource import SonoResource

class CoAPServer(CoAP):
    def __init__(self, host, port):
        super(CoAPServer, self).__init__((host, port))
        self.add_resource("sono/", SonoResource())

        print(f"Servidor CoAP iniciado em coap://{host}:{port}/sono")

def main():
    host = "0.0.0.0"
    port = 5683

    server = CoAPServer(host, port)

    try:
        server.listen(10)
    except KeyboarInterrupt:
        print("servidor finalizado")
        server.close()

if __name__ == "__main__":
    main()