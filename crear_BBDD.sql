DROP DATABASE IF EXISTS ProyectoSO;
CREATE DATABASE ProyectoSO;

USE ProyectoSO;

CREATE TABLE Jugador(
	Username VARCHAR(30) PRIMARY KEY NOT NULL,
	Passwd VARCHAR(40) NOT NULL,
	Nombre TEXT 
)ENGINE = InnoDB;

INSERT INTO Jugador VALUES('Roger01','password','Roger');
INSERT INTO Jugador VALUES('Sergi02','prueba01','');
INSERT INTO Jugador VALUES('Roberto03','12345678','Roberto');


