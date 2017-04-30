<!DOCTYPE html>
<html>
<head>
	<title>CS143 Project 1c</title>
	<?php
		include_once('header.php');
	?>
</head>

<body>
		<h1>Show Actor Information</h1>
		<form action="showActor.php" method="GET">
		Actor ID: <input type="text" name="aid" placeholder="Text Input">
		<input type="submit" value="Submit" name="submit">
			
		</form>
<?php
/* define and initialize variables */
		$input=trim($_GET['aid']);
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
/* Check if aid is valid */
		if (!$error) {
				if (!ctype_digit($input)) {
				$error= true;
					$error_message="Invalid Actor ID!<br/>";
				}
		}
/* MySQLi select actor*/
		if($error)
			echo $error_message;
		else{
			echo "<h2>Show Actor</h2>";
			/* search actor query*/
			$select_query= "SELECT id,first,last,sex,dod,dob FROM Actor WHERE id=$input;";
			/*search $query in database*/
			/* Issuing Queries with error query handling */
			$select_rs=$db->query($select_query);
			if (!$select_rs) {
				echo "Show actor failed! Please try again! <br/>";
				exit(1);
			}
			/*output actor information*/
			$row=$select_rs->fetch_row();
			echo "<b>Actor ID: $row[0]</b><br/>";
			echo "<b>Name: $row[1] $row[2]</b><br/>";
			echo "<b>Sex: $row[3]</b><br/>";
			echo "<b>Date of Birth: $row[5]</b><br/>";
			if ($row[4]!="") {
           		echo "<b>Date of Death</b><br/>";
			}else {
				echo "<b>Date of Birth: N/A</b><br/>";
			}			
			/*free up query result*/
			$select_rs->free_result();


		/*Movies relevant*/
			echo "<br/><br/>";
			echo "<h2>Movies Involved</h2>";
			/*matching query*/
			$match_query="SELECT MA.role,M.title,M.year,M.id FROM MovieActor MA,Movie M WHERE MA.aid=$input AND M.id=MA.mid ORDER BY M.year;";
			$match_result=$db->query($match_query);
			/* Issuing Queries with error query handling */
  			
  			if (!$match_result) {
				echo "Show movie involved failed! Please try again! <br/>";
				exit(1);
			}
			/*generate link output*/
  			while ($row2=$match_result->fetch_row()) {
  					$mid=$row2[3];
  					$title=$row2[1];
  					$year=$row2[2];
  					$role=$row2[0];
  					echo  "<a href='http://localhost:1438/~cs143/P1C/www/showMovie.php?mid=$mid'>Movie: $title($year) &nbsp;&nbsp;&nbsp; Role: $role</a><br/>";
  				}
  			/*free up query result*/
  			$match_result->free_result();
		}
/* disconnect the PHP page and MySQLi database CS143*/

$db->close();
?>
</body>
</html>