<html>

<head>
<title>CS143 Project 1B</title>
</head>

<body> 

<h1>Web Query Interface</h1>
<p>(Created by Xiongfeng.Hu on 16/10/16. Copyright (c) 2016 Xiongfeng.Hu. All rights reserved.)</p>
<br>
<br>
<br>
Type an SQL query in the following box:
Example: <tt>SELECT * FROM Actor WHERE id=10;</tt><br>
<p></p>
<form  action="query.php" method="GET"> <!-- action="."-->
<textarea name="query" cols="60" rows="8"></textarea><br>
<input type="submit" value="Submit">
</form>
<p></p>
<p><small>Note: tables and fields are case sensitive. All tables in Project 1B are availale.</small>
</p>

<h2>Results from MySQL:</h2>
<html><body><table border=1 cellspacing=1 cellpadding=2><tr align="center">

<?php
/* define variables and set to empty values */
// $db: database variable used to represent the MySQL:CS143 database
// $query: used to store input query statement
// $rs: used to store retrieving results
$db = "";
$query = ""; 
$rs = ""; 

/* Establishing a Connection of PHP page and MySQL database CS143*/
$db = new mysqli('localhost', 'cs143', '', 'CS143');
if($db->connect_errno > 0){
	die('Unable to connect to database [' . $db->connect_error . ']');
}

/* Issuing Queries with error query handling */
$query = $_GET["query"];
$rs = $db->query($query);
if (!($rs = $db->query($query))){
   $errmsg = $db->error;
   echo "Query failed: $errmsg <br/>";
   exit(1);
}
else{
print 'Total results: ' . $rs->num_rows.'<br/>';
print 'Total clomuns: ' . $rs->field_count ;
//Return the names of columns
$k = 0;
while($k < $rs->field_count){
//finfo holds the column information for all columns: name, max_length...
$column_info = $rs->fetch_fields();
//$column_info = mysqli_fetch_fields($rs);
echo '<td>' . $column_info->name . '</td>';
$k = $k+1;
 }
/* Retrieving Results */
//fetch_assoc() returns the column values of the retrieved row in an associative array
while($row = $rs->fetch_assoc()) {
   echo '<tr align="center">';
   $count = count($row);
 	//Return the table row by row
    // The <tr> tag defines a row in an HTML table.
   $i = 0;
   while($i < $count){
   		//Return the current element in an array
   		$c_row = current($row);
   		//if any value is NULL (blank), replace it with N/A
		if($c_row==NULL)
			echo '<td>N/A</td>';
		else
			echo '<td>' . $c_row . '</td>';
				
		next($row);
		$i = $i + 1;
   }
}
}
?>
<html></html>
<br>



</body>
</html>