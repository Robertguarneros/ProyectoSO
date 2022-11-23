DROP DATABASE IF EXISTS M01_Runner2057;
CREATE DATABASE M01_Runner2057;-- Creamos base de datos

USE M01_Runner2057;-- seleccionamos nuestra nueva BBDD

CREATE TABLE Jugador(
	Username VARCHAR(30) PRIMARY KEY NOT NULL,
	Passwd VARCHAR(40) NOT NULL,
	Nombre TEXT
)ENGINE = InnoDB;

CREATE TABLE Games(
	Game_ID INT NOT NULL AUTO_INCREMENT,
	Username_Player1 VARCHAR(30) NOT NULL,
	Username_Player2 VARCHAR(40) NOT NULL,
	Score_Player1 INT NOT NULL,
	Score_Player2 INT NOT NULL,
	PRIMARY KEY(Game_ID),
	FOREIGN KEY (Username_Player1) REFERENCES Jugador(Username),
	FOREIGN KEY (Username_Player2) REFERENCES Jugador(Username)
)ENGINE = InnoDB;

INSERT INTO Jugador VALUES('Roger01','password','Roger');
INSERT INTO Jugador VALUES('Sergi02','prueba01','Sergi');
INSERT INTO Jugador VALUES('Roberto03','12345678','Roberto');

INSERT INTO Games (Username_Player1, Username_Player2, Score_Player1, Score_Player2) VALUES ('Roger01','Sergi02',10,5);
INSERT INTO Games (Username_Player1, Username_Player2, Score_Player1, Score_Player2) VALUES ('Roberto03','Sergi02',5,10);
INSERT INTO Games (Username_Player1, Username_Player2, Score_Player1, Score_Player2) VALUES ('Roger01','Roberto03',0,4);
INSERT INTO Games (Username_Player1, Username_Player2, Score_Player1, Score_Player2) VALUES ('Roger01','Sergi02',20,5);
INSERT INTO Games (Username_Player1, Username_Player2, Score_Player1, Score_Player2) VALUES ('Roberto03','Sergi02',52,10);
INSERT INTO Games (Username_Player1, Username_Player2, Score_Player1, Score_Player2) VALUES ('Roger01','Roberto03',10,4);
