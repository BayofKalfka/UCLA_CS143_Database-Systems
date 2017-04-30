<!DOCTYPE html>
<html>
<head>
	<title>CS143 Project 1c</title>
	<?php
		include_once('header.php');
	?>
</head>

<body>
		<h1>Show Movie Information</h1>
		<form action="showMovie.php" method="GET">
		Movie ID: <input type="text" name="mid" placeholder="Text Input">
		<input type="submit" value="Submit" name="submit">
			
		</form>
<?php
/* define and initialize variables */
		$input=trim($_GET['mid']);
		$error=false;
		$error_message="The search field is misssing!<br/>";
/* Establishing a Connection of PHP page and MySQL database CS143*/
		$db = new mysqli('localhost', 'cs143', '', 'CS143');
		if($db->connect_errno > 0){
			die('Unable to connect to database [' . $db->connect_error . ']');
		}
		else
    		echo ("<br/>Connecting to database successfully!<br/>");
/* check required fields */
		if (isset($input))
		{
			if (empty($input))
				$error = true;
		}
		else
			$error = true;
/* Check if mid is valid */
		if (!$error) {
				if (!ctype_digit($input)) {
				$error= true;
					$error_message="Invalid Movie ID!<br/>";
				}
		}
/* MySQLi select movie */
		if($error)
			echo $error_message;
		else{
			echo "<h2>Show Movie</h2>";
			/*1 search movie query*/
				$select_query= "SELECT id,title,year,rating,company FROM Movie WHERE id=$input;";
				//$select_query="SELECT   FROM Movie M, Review R, MovieDirector MD, MovieActor MA WHERE ";
				/*search $query in database*/
				/* Issuing Queries with error query handling */
				$select_rs=$db->query($select_query);
				if (!$select_rs) {
					echo "Show actor failed! Please try again! <br/>";
					exit(1);
				}

				/*output movie information*/
				$row1=$select_rs->fetch_row();
				echo "<b>Movie ID: $row1[0]</b><br/>";
				echo "<b>Title: $row1[1]($row1[2])</b><br/>";
				echo "<b>MPAA Rating: $row1[3]</b><br/>";
				echo "<b>Production Co: $row1[4]</b><br/>";
				//echo "<b>: $row[0]</b><br/>";

				/*free up query result*/
				$select_rs->free_result();

			/* 2 search movie genre query*/
				$query2="SELECT genre FROM MovieGenre WHERE mid=$input;";
				$rs2=$db->query($query2);
				if (!$rs2) {
					echo "Show genre failed! Please try again! <br/>";
					exit(1);
				}
				/*output genre information*/
				echo "<b>Genre: </b>";
				$row2=$rs2->fetch_row();
				for ($i=0;$i<count($row2);$i++){
					echo "<b>$row2[$i]&nbsp;</b>";
				}
				echo "<br/>";
				/*free up query result*/
				$rs2->free_result();

			/* 3 search movie director query*/
				$query3="SELECT last first FROM Director D, MovieDirector MD WHERE MD.mid=$input AND D.id=MD.did;";
				$rs3=$db->query($query3);
				if (!$rs3) {
					echo "Show director failed! Please try again! <br/>";
					exit(1);
				}
				/*output director information*/
				$row3=$rs3->fetch_row();
				echo "<b>Director: $row3[1] $row3[0]</b><br/>";
				/*free up query result*/
				$rs3->free_result();
				echo"<br/><br/>";

			/* 4 search movie actor query*/
				echo"<h2>Related Actors</h2>";
				$query4="SELECT A.id, A.first, A.last, MA.role FROM Actor A, MovieActor MA WHERE MA.mid=$input AND A.id=MA.aid ORDER BY A.id;";
				$rs4=$db->query($query4);
				/* Issuing Queries with error query handling */
				if (!$rs4) {
					echo "Show related actors failed! Please try again! <br/>";
					exit(1);
				}
				/*generate link output*/
				while ($row4=$rs4->fetch_row()) {
						$aid=$row4[0];
						$name= "$row4[1] $row4[2]";
						$role="$row4[3]";
						echo "<a href='http://localhost:1438/~cs143/P1C/www/showActor.php?aid=$aid'>$name (Role:$role)</a><br/>";
				}
				/*free up query result*/
				$rs4->free_result();
				echo"<br/>";
			/*5 user review */
				echo "<h2>User Review</h2>";
				/* score query*/	
				$query5="SELECT AVG(rating) FROM Review WHERE mid=$input;";
				$rs5=$db->query($query5);

				/* Issuing Queries with error query handling */
				if (!$rs5) {
					echo "Show related reviews failed! Please try again! <br/>";
					exit(1);
				}
				/*output avg score*/
				while ($row5=$rs5->fetch_row()) {
				echo "<b>Average Score: $row5[0]</b><br/>";	
				if(!$row5[0]){
					echo "Be the first one to comment!<br/>";
					}
				}
				/*add comment here*/
				echo "<a href='http://localhost:1438/~cs143/P1C/www/addMovieComment.php'>You can add comment here!</a><br/>";
				/*free up query result*/
				$rs5->free_result();
				echo"<br/>";
			/*6 user comment */
				echo "<h2>User Comment</h2>";
				$query6="SELECT time, name, rating, comment FROM Review WHERE mid=$input ORDER BY time DESC;";
				$rs6=$db->query($query6);
				/*output reviews*/
				while ($row6=$rs6->fetch_assoc()) {
					$name=$row6['name'];
					$time=$row6['time'];
					$comment=$row6['comment'];
					$rating=$row6['rating'];
					echo "<b>$name($time): </b><br/>";
  				    echo "<b>Rating: $rating</b><br/>";
					echo "<b>Comment: $comment</b><br/>";
					echo "<br/>";
				}
				/*free up query result*/
				$rs6->free_result();
				echo"<br/>";


		}


$db->close();
?>	
</body>
</html>