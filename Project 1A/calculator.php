<html>

<head>
<title>Calculator</title>
</head>


<body>

<h1>Calculator</h1>
(Created by Xiongfeng.Hu on 02/10/16. Copyright (c) 2016 Xiongfeng.Hu. All rights reserved.)
<br>
<br>
<br>
Type an expression in the box below (e.g., 5+3*2).
<p></p>

<!-- use GET if The interaction is more like a question (i.e., it is a safe operation such as a query, read operation, or lookup.)  -->
<form method="GET"> 
        <input type="text" name="expr">
        <input type="submit" value="Calculate">
</form>
<p></p>

<ul>
    <li>Only numbers and +,-,* and / operators are allowed in the expression.
    </li><li>The evaluation follows the standard operator precedence.
    </li><li>The calculator does not support parentheses.
    </li><li>The calculator handles invalid input "gracefully". It does not output PHP error messages.
</li></ul>
Here are some(but not limit to) reasonable test cases:
<ol>
  <li> A basic arithmetic operation:  5+3*2=11 </li>
  <li> An expression with floating point or negative sign : 1-2/3 = 0.33333333333333, 3*1-5.5 = -2.5 </li>
  <li> Some typos inside operation (e.g. alphabetic letter): Invalid input expression 6kevin6+1 </li>
</ol>
<h2>Result</h2>

<?php
// define variables and set to empty values
$equ = $ans = ""; 
if($_GET["expr"]){
    test_input($_GET["expr"]);
//$equ =test_input($_GET["expr"]);
/*eval("\$ans = $equ ;");
echo $_GET["expr"];
echo( '<br>' );
echo "ans = ".$ans;*/
}

 function test_input($data) {
    $data = trim($data);//Remove whitespaces from both sides of a string
    $equ = $data;
    eval("\$ans = $equ ;");
   //$valid = " #^[^0-9]$# "；
   //if (!preg_match("/^[a-zA-Z ]*$/",$name))
   if (!preg_match("/^[\d\(\)\+\-\*\/\.]*$/",$data)) {
       $exprError = "Invalid Expression 001!";
       echo $exprError;
   }elseif (!$ans) {
       if (preg_match("/\/0/",$data)){
           $exprError = "Divison by zero! Not allowed!";
           echo $exprError;
       }else{
           $exprError = "Invalid Expression 002!";
           echo $exprError;
       }
   }else {
    //$equ = $data;
    //eval("\$ans = $equ ;");
    echo $data." = ".$ans;
}

}

?><br>


</body>
</html>
