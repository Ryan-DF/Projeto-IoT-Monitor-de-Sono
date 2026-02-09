from pathlib import Path
import sqlite3


def init_database():
    # Caminhos base do projeto
    project_root = Path(__file__).resolve().parents[2]
    data_dir = project_root / "data"
    schema_path = data_dir / "schema.sql"
    db_path = data_dir / "sono.db"

    # Garante que a pasta data exista
    data_dir.mkdir(exist_ok=True)

    # Conecta (ou cria) o banco de dados
    conn = sqlite3.connect(db_path)
    cursor = conn.cursor()

    # Lê e executa o schema
    with open(schema_path, "r", encoding="utf-8") as schema_file:
        schema_sql = schema_file.read()
        cursor.executescript(schema_sql)

    conn.commit()
    conn.close()

    print("Banco de dados inicializado com sucesso.")


if __name__ == "__main__":
    init_database()