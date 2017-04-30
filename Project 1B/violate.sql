
-- Three primary key constraints

--PRIMARY KEY (id) in Movie
INSERT INTO Movie VALUES (6,'Test',2012,'rating','None');
--ERROR 1062 (23000): Duplicate entry '6' for key 'PRIMARY'

--PRIMARY KEY (id) in Actor
INSERT INTO Actor VALUES (61018,'last','first','sex',1999-09-09,'');
--ERROR 1062 (23000): Duplicate entry '61018' for key 'PRIMARY'

--PRIMARY KEY (id) in Director
INSERT INTO Director VALUES (68626,'Last','First',1999-09-09,1999-10-09);
--ERROR 1062 (23000): Duplicate entry '68626' for key 'PRIMARY'

-- Six referentiaL INTEGRITY CONSTRAINTS 

--MovieGenre.mid references Movie.id
INSERT INTO MovieGenre VALUES (000,'');
--ERROR 1452 (23000): Cannot add or update a child row: a foreign key constraint fails (`CS143`.`MovieGenre`, CONSTRAINT `MovieGenre_ibfk_1` FOREIGN KEY (`mid`) REFERENCES `Movie` (`id`))

--MovieDirector.mid references Movie.id
INSERT INTO MovieDirector VALUES (000,68626);
--ERROR 1452 (23000): Cannot add or update a child row: a foreign key constraint fails (`CS143`.`MovieDirector`, CONSTRAINT `MovieDirector_ibfk_1` FOREIGN KEY (`mid`) REFERENCES `Movie` (`id`))

--MovieDirector.did references Director.id
INSERT INTO MovieDirector VALUES (6,000);
--ERROR 1452 (23000): Cannot add or update a child row: a foreign key constraint fails (`CS143`.`MovieDirector`, CONSTRAINT `MovieDirector_ibfk_1` FOREIGN KEY (`mid`) REFERENCES `Movie` (`id`))

--MovieActor.mid references Movie.id
INSERT INTO MovieActor VALUES (000,61018,'');
--ERROR 1452 (23000): Cannot add or update a child row: a foreign key constraint fails (`CS143`.`MovieActor`, CONSTRAINT `MovieActor_ibfk_1` FOREIGN KEY (`mid`) REFERENCES `Movie` (`id`))

--MovieActor.did references Actor.id
INSERT INTO MovieActor VALUES (6,000,'');
--ERROR 1452 (23000): Cannot add or update a child row: a foreign key constraint fails (`CS143`.`MovieActor`, CONSTRAINT `MovieActor_ibfk_1` FOREIGN KEY (`mid`) REFERENCES `Movie` (`id`))

--Review.mid references Movie.id
INSERT INTO Review VALUES ('','',000,5,'');
--ERROR 1452 (23000): Cannot add or update a child row: a foreign key constraint fails (`CS143`.`Review`, CONSTRAINT `Review_ibfk_1` FOREIGN KEY (`mid`) REFERENCES `Movie` (`id`))

--Three check constraints

--raring must be MPAA rating
INSERT INTO Movie VALUES (0000,'Test',2016,'WrongRating','None');
--year must be four-digit
INSERT INTO Movie VALUES (0000,'Test',666666,'WrongRating','None');
---rating must 0-5 integer
INSERT INTO Review VALUES ('','',6,10,'');
