
CREATE DATABASE IF NOT EXISTS diac_db;
USE diac_db;
CREATE TABLE IF NOT EXISTS candidaturas (
    cpf INT PRIMARY KEY,
    nome VARCHAR(100),
    cargo VARCHAR(30),
    tempo_contrato INT,
    salario FLOAT,
    clausula_rescisao FLOAT,
    dependentes INT,
    plano_saude VARCHAR(50)
);

