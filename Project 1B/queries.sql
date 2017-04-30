/* queries.sql*/
/* This is a .sql file running some SELECT statements */

/* Give the names of all the actors in the movie  'Die Another Day'*/
SELECT CONCAT_WS(' ', A.first, A.last) AS ActorsInDieAnotherDay
FROM Actor A, Movie M, MovieActor MA
WHERE A.id = MA.aid AND MA.mid = M.id AND M.title = 'Die Another Day';

/* Give the count of all the actors who acted in multiple movies */
SELECT COUNT(*) AS ActorsOfMultipleMovies 
FROM (SELECT COUNT(*)
FROM MovieActor
GROUP BY aid
HAVING COUNT(*) > 1
) AS b;

/* Give the count all the actors */
SELECT COUNT(id) AS CountOfActors
FROM Actor;