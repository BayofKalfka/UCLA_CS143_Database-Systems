<!DOCTYPE html>
<html>
<head>
	<title>CS143 Project 1c</title>
	<?php
	include_once('header.php');
	?>
</head>


<body>

		<h1>Add a new Movie</h1>
        <form method="GET" action="addMovie.php">
        <label for="title">Title: </label>
		<input type="text" name="title" placeholder="Text input">
		<label for="company">Company: </label>
		<input type="text" name="company" placeholder="Text input">
		<label for="year">Year: </label>
		<input type="text" name="year" placeholder="Text input">
		<label for="mpaarating">MPAA Rating: <label>
		<select name = "mpaarating">
			<option value="G" checked="checked">G</option>
			<option value="PG">PG</option>
			<option value="PG-13">PG-13</option>
			<option value="R">R</option>
			<option value="NC-17">NC-17</option>
		</select>
		<br/><br/>
		<label for="genre">Genre :</label>
			<br/>
			<input type="checkbox" name="genre[]" value="Action">Action</input><br/>
			<input type="checkbox" name="genre[]" value="Adult">Adult</input><br/>
			<input type="checkbox" name="genre[]" value="Adventure">Adventure</input><br/>
			<input type="checkbox" name="genre[]" value="Animation">Animation</input><br/>
			<input type="checkbox" name="genre[]" value="Comedy">Comedy</input><br/>
			<input type="checkbox" name="genre[]" value="Crime">Crime</input><br/>
			<input type="checkbox" name="genre[]" value="Documentary">Documentary</input><br/>
			<input type="checkbox" name="genre[]" value="Drama">Drama</input><br/>
			<input type="checkbox" name="genre[]" value="Family">Family</input><br/>
			<input type="checkbox" name="genre[]" value="Fantasy">Fantasy</input><br/>
			<input type="checkbox" name="genre[]" value="Horror">Horror</input><br/>
			<input type="checkbox" name="genre[]" value="Musical">Musical</input><br/>
			<input type="checkbox" name="genre[]" value="Mystery">Mystery</input><br/>
			<input type="checkbox" name="genre[]" value="Romance">Romance</input><br/>
			<input type="checkbox" name="genre[]" value="Sci-Fi">Sci-Fi</input><br/>
			<input type="checkbox" name="genre[]" value="Short">Short</input><br/>
			<input type="checkbox" name="genre[]" value="Thriller">Thriller</input><br/>
			<input type="checkbox" name="genre[]" value="War">War</input><br/>
			<input type="checkbox" name="genre[]" value="Western">Western</input><br/>
					
			<br/>
			
			<input type="submit" name="submit" value="Submit"/>
			</form>

<?php
/* define and initialize variables */
$db = "";
$query = "";
$rs = "";
$fields = array("title","company","year","mpaarating","genre");
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

/* Check if year is valid */
if (!$error)
{
	if (strlen($_GET['year']) != 4 || !ctype_digit($_GET['year']))
	{
		$error = true;
		$error_message = "Invalid Year!";
	}
}

/* MySQLi insertion */
if ($error)
	echo $error_message;
else{
	echo "No error in user input!<br/>";
	/* Look up MaxPersonID */
	$lookup_query = "SELECT id FROM MaxMovieID;";
	$lookup_result = $db->query($lookup_query);
			if ($db->query($lookup_query)) {
				$row =$lookup_result->fetch_assoc();
				$id = current($row)+1;
				echo "Newly added movie id is: ".$id;
				echo "<br/>";
				$increase_query = "UPDATE MaxMovieID SET id=$id";
				$increase_result=$db->query($increase_query);
				if ($increase_result=$db->query($increase_query)) {
					echo "MaxMovieID updated successfully!<br/>";
				}
			}
			else
				echo "Lookup max movie id failed!<br/>";
	//$query = "INSERT INTO Movie VALUES ($id,"$GET_['title']","$GET_[year]",'$GET_[mpaarating]','$GET_[company]');";// include external variables into quote!!
			$genre=$_GET['genre'];
			$query = insert_query($id,$_GET["title"],$_GET["year"],$_GET["mpaarating"],$_GET["company"]);
			//$query2="INSERT INTO MovieGenre VALUES ($id,$genre);";
			$rs = $db->query($query);
			for($i=0;$i<count($genre);$i++){
				$genre_query="INSERT INTO MovieGenre VALUES ($id,'$genre[$i]');";
				$genre_rs=$db->query($genre_query);
				if(!$genre_rs)
					echo "something wrong!<br/>";

			}
			
			if ($rs){
				echo "New Content added successfully!<br/>";
			}else{
				echo "Add Content failed, please try again!<br/>";
				exit(1);
			}
}
/* function insert_query */
function insert_query($id,$title,$year,$mpaarating,$company){
	
	$query = "INSERT INTO Movie VALUES ($id,'$title','$year','$mpaarating','$company');";
	return $query;
}
/* disconnect the PHP page and Mysqli database CS143 */
$db->close();
?>			
</body>
</html>