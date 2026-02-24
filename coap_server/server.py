from coapthon.server.coap import CoAP
from db.init_db import init_database
from resources.sono_resource import SonoResource

class CoAPServer(CoAP):
    def __init__(self, host, port):
        super(CoAPServer, self).__init__((host, port))
        self.add_resource("sono/", SonoResource())

        print(f"Servidor CoAP iniciado em coap://{host}:{port}/sono")

def main():
    host = "localhost"
    port = 5683

    init_database()
    server = CoAPServer(host, port)

    try:
        server.listen(10)
    except KeyboardInterrupt:
        print("servidor finalizado")
        server.close()

if __name__ == "__main__":
    main()