<!DOCTYPE html>
<html>
<head>
	<title>CS143 Project 1c</title>
	<?php
		include_once('header.php');
	?>
</head>

<body>

		<h1>Add a comment here</h1>
		<form method="GET" action="addMovieComment.php">
		<label for="mid">Movie ID: </label>
		<input type="text" name="mid" placeholder="Text input"></input><br/>
		(Please enter the ID of the movie you want to comment! If you donot have it, please check on search page!)
		<br/><br/>
		<label for="rating">Rating: <label>
		<select name = "rating">
			<option value="0" checked="checked">0</option>
			<option value="1">1</option>
			<option value="2">2</option>
			<option value="3">3</option>
			<option value="4">4</option>
			<option value="5">5</option>
		</select><br/>
		<label for="name">Your Name: <label>
		<input type="text" name="name" placeholder="Text input"></input><br/>
		<label for="comment">Comment: </label><br>
		<textarea name="comment" cols="60" rows="8"></textarea></br>
        <br/>
			
		<input type="submit" name="submit" value="Submit"/>
		</form>

<?php
/* define and initialize variables */
$db = "";
$query = "";
$rs = "";
$fields = array("mid","comment","rating");
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
		if (empty($_GET[$key])){
			$error = true;
		    echo "begin001<br/>";
		}
	}
	else{
		$error = true;
	    echo "begin002<br/>";
	}
}
/* Check if mid is valid */
if (!$error)
{
	if (!ctype_digit($_GET['mid']))
	{
		$error = true;
		$error_message = "Invalid Movie ID!<br/>";
	}//else{ 
        /* check whether or not input mid exist in database*/
	//	$lookup_query = "SELECT id FROM Movie";
	//	$lookup_result = $db->query($lookup_query);
		//echo "begin003<br/>";
	//	$check_result=0;
	//	while ($db->query($lookup_query)) {
			//echo "begin004<br/>";
	//		while($row = $lookup_result->fetch_assoc()) {
	//			//echo "begin005<br/>";
	//			$count = count($row);
	//			$i = 0;
	//			while ($i < $count) {
					//echo "begin004<br/>";
	//				$c_row = current($row);
	//				while ($c_row == $_GET["mid"]) {
	//					$check_result=1;
	//				}
	//			}
	//		}
	//	}
		//echo $check_result;
		//echo "<br/>";
		//if (!$check_result) {
		    //echo "beigin006<br/>";
			//$error = true;
			//$error_message = "Movie ID not found!<br/>";
		//}else{
			//echo "end<br/>";
		//}
	//}
}

/* MySQLi insertion */
if ($error)
	echo $error_message;
else {
	echo "No error in user input!<br/>";
	$query = insert_query($_GET["name"],$_GET["mid"],$_GET["rating"],$_GET["comment"]);
	$rs = $db->query($query);
	if ($rs){
				echo "New Content added successfully!<br/>";
			}else{
				echo "Add Content failed, please try again!<br/>";
				exit(1);
			}
}
/* function insert_query */
function insert_query($name,$mid,$rating,$comment){
	
	$query = "INSERT INTO Review(name,mid,rating,comment) Values ('$name','$mid',$rating,'$comment');";
	return $query;
}	
/* disconnect the PHP page and MySQLi database CS143*/

$db->close();
?>
	
</body>
</html>