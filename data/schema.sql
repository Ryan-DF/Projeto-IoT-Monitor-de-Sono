-- Schema do banco de dados do monitor de sono
-- Banco: SQLite

CREATE TABLE IF NOT EXISTS leituras_sono (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    device_id TEXT NOT NULL,
    temperatura REAL,
    umidade REAL,
    ruido_dbfs REAL,
    timestamp INTEGER NOT NULL
);
