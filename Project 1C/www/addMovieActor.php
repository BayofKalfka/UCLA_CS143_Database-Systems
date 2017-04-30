<!DOCTYPE html>
<html>
<head>
	<title>CS143 Project 1c</title>
	<?php
		include_once('header.php');
	?>
</head>

<body>
		<h1>Add A Actor To A Movie</h1>
		<form method="GET" action= "addMovieActor.php">
        	Movie : <select name="mid">
        			<?php
        		 	/*Establishing a Connection of PHP page and MySQL database CS143*/
					$db = new mysqli('localhost', 'cs143', '', 'CS143');
					if($db->connect_errno > 0){
						die('Unable to connect to database [' . $db->connect_error . ']');
					}
					else
    					echo ("<br/>Connecting to database successfully!<br/>");
            	    /**/
                	$movie_query = "SELECT id, title, year FROM Movie ORDER BY title;";
               	 	$movie_result =$db->query($movie_query);
                	while ($movie_row = $movie_result->fetch_row()) {
                	
                		$mid = $movie_row[0];
                		$title=$movie_row[1];
                		$year=$movie_row[2];
                		echo "<option value='$mid'>$title($year)</option>";
                	
               	 	}
                	$db->close();
        			?>	
        			</select>	
			<br/><br/><br/>

			Actor  : <select name="aid">
        			<?php
        		 	/*Establishing a Connection of PHP page and MySQL database CS143*/
					$db = new mysqli('localhost', 'cs143', '', 'CS143');
					if($db->connect_errno > 0){
						die('Unable to connect to database [' . $db->connect_error . ']');
					}
					else
    					echo ("<br/>Connecting to database successfully!<br/>");
            	    /**/
                	$actor_query = "SELECT id,first,last FROM Actor ORDER BY id;";
               	 	$actor_result = $db->query($actor_query);
                	while ($actor_row = $actor_result->fetch_row()) {
                	
                		$aid = $actor_row[0];
                		$first=$actor_row[1];
                		$last=$actor_row[2];
                		$name = "$first $last";
                		echo "<option value='$aid'> $name</option>";
                	
               	 	}
                	$db->close();

        			?>	
        			</select>	
        			<br/><br/>
		 	Role: <input type="text" maxlength="60" name="role" placeholder="Text input"></input>
			  		<br/><br/>
					<input type="submit" name="submit" value="Submit"/>





		</form>

<?php
/* define and initialize variables */
$db = "";
$query = "";
$rs = "";
$fields = array("mid","aid","role");
$error = false;
$error_message = "At least one filed is missing!<br/>";
/* Establishing a Connection of PHP page and MySQL database CS143*/
$db = new mysqli('localhost', 'cs143', '', 'CS143');
if($db->connect_errno > 0){
	die('Unable to connect to database [' . $db->connect_error . ']');
}
else
    echo ("<br/>Connecting to database successfully!<br/>");
/* check required fields */
foreach ($fields as $key)
{
	if (isset($_GET[$key]))
	{
		if (empty($_GET[$key]))
			$error = true;
	}
	else
		$error = true;
}

/* MySQLi insertion */
if ($error)
	echo $error_message;
else{
	$mid = $_GET['mid'];
	$aid= $_GET['aid'];
	$role = $_GET['role'];
	$insert_query = "INSERT INTO MovieActor (mid, aid, role) VALUES ('$mid', '$aid', '$role');";
	$insert_result = $db->query($insert_query);
		if ($insert_result) {
				echo "New Content added successfully!<br/>";
			}else{
				echo "Add Content failed, please try again!<br/>";
				exit(1);
			}
}
/* disconnect the PHP page and MySQLi database CS143*/
$db->close();
?>



</body>
</html>