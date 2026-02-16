from pathlib import Path
import sqlite3
import time

project_root = Path(__file__).resolve().parents[2]
db_path = project_root / "data" / "sono.db"

def get_db_connection():
    conn = sqlite3.connect(db_path)
    return conn

def insert_leitura(dados: dict):
    with get_db_connection() as conn:
        cursor = conn.cursor()
        cursor.execute("""
            INSERT INTO leituras_sono (device_id,  ruido_dbfs, temperatura, umidade)
            VALUES (?, ?, ?, ?)
        """, (
            dados["device_id"],
            dados["ruido_dbfs"],
            dados["temperatura"],
            dados["umidade"]
        ))