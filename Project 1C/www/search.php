<!DOCTYPE html>
<html>
<head>
	<title>CS143 Project 1c</title>
	<?php
		include_once('header.php');
	?>
</head>

<body>
		<h1>Search Actors and Movies</h1>

		<form action="search.php" method="GET" >

			<input type="text" name="search" placeholder="Text Input">
			<input type="submit" value="submit" name="Submit">

			
		</form>
<?php
		/*define and initialize variables*/
		$error=false;
		$error_message="The search field is misssing!<br/>";
        /* get the input and break it into terms array by space*/
		$search=trim($_GET["search"]);
		$terms=explode(' ',$search);
		//$in=$terms[0];
		//echo $in;
		echo count($terms);

		/* establish connection with Mysqli CS143 database */
		$db = new mysqli('localhost', 'cs143', '', 'CS143');
		if($db->connect_errno > 0){
			die('Unable to connect to database [' . $db->connect_error . ']');
		}
		else
    		echo ("<br/>Connecting to database successfully!<br/>");
    	/* check input is not empty*/
    	if (isset($_GET['search']))
		{
			if (empty($_GET['search']))
				$error = true;
		}
		else
			$error = true;


		/*mysqli actor search result*/
		if ($error)
				echo $error_message;
		else {
			echo "<h2>Actor Results</h2>";

			/* search actor query*/
			$actor_query="SELECT id,last,first FROM Actor WHERE first LIKE '%$terms[0]%' OR last LIKE '%$terms[0]%';";

			/*add more WHERE clauses*/
			for ($i=1;$i<count($terms);$i++) 
			{
				$actor_where="(first LIKE '%$terms[0]%' OR last LIKE '%$terms[0]%') AND (first LIKE '%$terms[$i]%' OR last LIKE '%$terms[$i]%')";
				$actor_query= "SELECT id,last,first FROM Actor WHERE $actor_where;";
			}
			/*search $query in database*/
			/* Issuing Queries with error query handling */
			$actor_rs=$db->query($actor_query);
			if (!$actor_rs) {
   				echo "Search failed! Please try again! <br/>";
 	  			exit(1);
			}
			/*generate link output*/
			while ($actor_row = $actor_rs->fetch_row()) {
				$aid= $actor_row[0];
				//echo $aid."<br/>";
				$last =$actor_row[1];
				$first = $actor_row[2];
				$name = "$first $last";
				echo  "<a href='http://localhost:1438/~cs143/P1C/www/showActor.php?aid=$aid'>$name</a><br/>";
			}
			/*free up query result*/
			$actor_rs->free_result();
			//http://localhost:1438/~cs143/P1C/www/showActor.php

		}
			echo "<br/>";
			echo "<br/>";
		/*mysqli movie search result*/
		if ($error)
			echo $error_message;
		else {
			echo "<h2>Movie Results</h2>";
			/* search actor query*/
			$movie_query="SELECT id,title,year FROM Movie WHERE title LIKE '%$terms[0]%';";
			/*add more WHERE clauses to query*/
			for ($i=1;$i<count($terms);$i++) 
			{
				$movie_where="(title LIKE '%$terms[0]%')AND title LIKE '%$terms[$i]%'";
				$movie_query= "SELECT id,title,year FROM Movie WHERE $movie_where;";
			}
			/*search $query in database*/
			/* Issuing Queries with error query handling */
			$movie_rs=$db->query($movie_query);
			if (!$movie_rs) {
				echo "Search failed! Please try again! <br/>";
				exit(1);
			}
			/*generate link output*/
			while ($movie_row= $movie_rs->fetch_row()) {
				$mid=$movie_row[0];
				$title=$movie_row[1];
				$year=$movie_row[2];
				echo  "<a href='http://localhost:1438/~cs143/P1C/www/showMovie.php?mid=$mid'>$title($year)</a><br/>";

			}
			/*free up query result*/
			$movie_rs->free_result();
		}
			



$db->close();
?>
</body>
</html>