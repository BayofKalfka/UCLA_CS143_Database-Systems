<!DOCTYPE html>
<html>
<head>
	<title>CS143 Project 1c</title>
	<?php
		include_once('header.php');
	?>
</head>



<body>

            <h1>Add a new Actor/Director</h1>
            <form method="GET" action="addActorDirector.php">
               <label class="radio-inline">
                    <input type="radio" checked="checked" name="identity" value="Actor">
                    Actor
                </label>
                <label class="radio-inline">
                    <input type="radio" name="identity" value="Director">Director
                </label>
                <div class="form-group">
                  <label for="first_name">First Name</label>
                  <input type="text" class="form-control" placeholder="Text input" name="fname">
                </div>
                <div class="form-group">
                  <label for="last_name">Last Name</label>
                  <input type="text" class="form-control" placeholder="Text input" name="lname">
                </div>
                <label class="radio-inline">
                    <input type="radio" name="sex" checked="checked" value="male">Male
                </label>
                <label class="radio-inline">
                    <input type="radio" name="sex" value="female">Female
                </label>
                <div class="form-group">
                  <label for="DOB">Date of Birth</label>
                  <input type="text" class="form-control" placeholder="Text input" name="dob">ie: 1999-09-09<br>
                </div>
                <div class="form-group">
                  <label for="DOD">Date of Death</label>
                  <input type="text" class="form-control" placeholder="Text input" name="dod">(leave blank if alive now)<br>
                </div>
                <input type="submit" value="Submit">
            </form>


<?php
/* define and initialize variables */
// $db: database variable used to represent the MySQL:CS143 database
// $query: used to store input query statement
// $rs: used to store retrieving results
$db = "";
$lookup_result = "";
$query = ""; 
//$query = create_query($_GET["identity"], $_GET["fname"], $_GET[""], $_GET[""], $_GET[""], $_GET[""]); 
$rs = ""; 
$fields = array("identity", "fname", "lname", "sex", "dob");
$error = false;
$error_message = "At least one field is missing!";

/* Establishing a Connection of PHP page and MySQL database CS143*/
$db = new mysqli('localhost', 'cs143', '', 'CS143');
if($db->connect_errno > 0){
	die('Unable to connect to database [' . $db->connect_error . ']');
}
else
    echo ("Connecting to database successfully!<br/>");

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
/* Check if dates are valid */
if (!$error)
{
	//if (strlen($_GET['dob']) != 8 || !ctype_digit($_GET['dob']))
	if (!preg_match("/\d{4}\-\d{2}-\d{2}/", $_GET['dob']))
	{
		$error = true;
		$error_message = "Invalid Date of Birth!";
	}
	else if (isset($_GET['dod']))
	{
		if (!empty($_GET['dod']))
		{
			if (!preg_match("/\d{4}\-\d{2}-\d{2}/", $_GET['dod']))
			{
				$error = true;
				$error_message = "Invalid Date of Death!";
			}
			//if ($_GET['dod'] < $_GET['dob'])
			//{
			//	$error = true;
			//	$error_message = "Date of Death cannot precede Date of Birth!";
				
			//}
		}
	}
}
/* MySQLi insertion */
//if (isset($GET_('submit')) {
	if ($error)
		echo $error_message;
	else{
		    // Look up MaxPersonID, assign a unique ID to new actor/director and increase the MaxPersonID value 
		    echo "No error before lookup!<br/>";
			$lookup_query = "SELECT id FROM MaxPersonID;";
			$lookup_result = $db->query($lookup_query);
			if ($db->query($lookup_query)) {
				$row =$lookup_result->fetch_assoc();
				$id = current($row)+1;
				echo "Newly added person id is: ".$id;
				echo "<br/>";
				$increase_query = "UPDATE MaxPersonID SET id=$id";
				$increase_result=$db->query($increase_query);
				if ($increase_result=$db->query($increase_query)) {
					echo "MaxPersonID updated successfully!<br/>";
				}
			}
			else
				echo "lookup failed!<br/>";
			//
			$query = insert_query($_GET["identity"],$id,$_GET["lname"],$_GET["fname"],$_GET["sex"],$_GET["dob"],$_GET["dod"]);
			$rs = $db->query($query);
			if (!($rs = $db->query($query))) {
				echo "New Content added successfully!<br/>";
			}else{
				echo "Add Content failed, please try again!<br/>";
				exit(1);
			}

	}
 
//}


//
//$query = add_query($_GET["identity"], $id, $_GET["lname"], $_GET["fname"], $_GET["sex"], $_GET["dob"], $_GET["dod"]);
//$rs = $db->query($query);
//if (!($rs = $db->query($query))){
//   echo "Add Content failed, please try again!<br/>";
//   exit(1);
//}else{
//   echo "New Content added successfully<br/>";
//}


/* add_query($identity,$id, $last, $first, $sex, $dob, $dod) */
function insert_query($identity, $id, $last, $first, $sex, $dob, $dod) {
if($identity == "Actor"){
	$insert_query = "INSERT INTO Actor VALUES ($id,'$last','$first','$sex','$dob','$dod');";
}else if($identity == "Director"){
	$insert_query = "INSERT INTO Director VALUES ($id,'$last','$first','$dob','$dod');";
}
return $insert_query;
}

$db->close();
?>

</body>
</html>
