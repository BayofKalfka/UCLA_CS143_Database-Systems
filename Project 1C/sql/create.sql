/**/

/* create.sql */
/* This is a .sql file to create 9 tables required in the database */



CREATE TABLE Movie(
	id INT NOT NULL, 
	title VARCHAR(20) NOT NULL, 
	year INT, 
	rating VARCHAR(10), 
	company VARCHAR(50),
	PRIMARY KEY (id),
	-- check that the rating is valid
	-- check that the year is valid
	CHECK(rating IS NULL OR rating='G' OR rating='PG' OR rating='PG-13' OR rating='R' OR rating='NC-17'),
	CHECK(year > 999 AND year < 10000)
	)ENGINE=INNODB;  

CREATE TABLE Actor(
	id INT NOT NULL, 
	last VARCHAR(20), 
	first VARCHAR(20), 
	sex VARCHAR(6), 
	dob DATE NOT NULL, 
	dod DATE,
	-- set Actor.id as primary key
	-- check that the dod is null or dod is later than dob
	PRIMARY KEY (id),
	CHECK(dod IS NULL OR dod <= dob)
	)ENGINE=INNODB; 

CREATE TABLE Director(
	id INT NOT NULL, 
	last VARCHAR(20), 
	first VARCHAR(20), 
	dob DATE NOT NULL, 
	dod DATE,
	-- set Director.id as primary key
	-- check that the dod is null or dod is later than dob
	PRIMARY KEY (id),
	CHECK(dod IS NULL OR dod <= dob)
	)ENGINE=INNODB; 

CREATE TABLE MovieGenre(
	mid INT NOT NULL, 
	genre VARCHAR(20),
	-- link MovieGenre.mid to Movie.id
	FOREIGN KEY (mid) REFERENCES Movie(id)
	)ENGINE=INNODB; 

CREATE TABLE MovieDirector(
	mid INT NOT NULL, 
	did INT NOT NULL,
	-- link MovieDirector.mid to Movie.id
	-- link MovieDirector.did to Director.id
	FOREIGN KEY (mid) REFERENCES Movie(id),
	FOREIGN KEY (did) REFERENCES Director(id)
	)ENGINE=INNODB; 

CREATE TABLE MovieActor(
	mid INT NOT NULL, 
	aid INT NOT NULL, 
	role VARCHAR(20),
	-- link MovieActor.mid to Movie.id
	-- link MovieActor.did to Actor.id
	FOREIGN KEY (mid) REFERENCES Movie(id),
	FOREIGN KEY (aid) REFERENCES Actor(id)
	)ENGINE=INNODB;

CREATE TABLE Review(
	name VARCHAR(20), 
	time TIMESTAMP, 
	mid INT NOT NULL, 
	rating INT NOT NULL, 
	comment VARCHAR(500),
	-- link Review.mid to Movie.id
	FOREIGN KEY (mid) REFERENCES Movie(id),
	CHECK ((0 <= rating AND rating <= 5) OR rating IS NULL)
	)ENGINE=INNODB;

CREATE TABLE MaxPersonID(
	-- MaxPersonID.id cannot be null
	id INT NOT NULL
	)ENGINE=INNODB;

CREATE TABLE MaxMovieID(
	-- MaxMovieID.id cannot be null
	id INT NOT NULL
	)ENGINE=INNODB;


