<?php
//establish connection to database
$servername = "localhost";
$username = "root";
$pasword = "elcykel";
$wordpress = "wordpress";
$conn = new mysqli($servername,$username,$pasword,$wordpress);

//check if connection was established
if($conn->connect_error){
  die("Connection failed: " . $conn->connect_error);
}
// get the current online user
$current_user = wp_get_current_user();
//select one of the available bikes and which fack it is in
$checkbikes = "SELECT cykelID,fackID FROM Cykel_Fack WHERE avaliable = TRUE";
$resultcheckbikes = $conn->query($checkbikes);
if($resultcheckbikes->num_rows>0){
  while ($row = $resultcheckbikes->fetch_assoc()) {
    $cykelid = $row["cykelID"];
    $fackid = $row["fackID"];
  }
}else{
  echo " ..";
}
//Create a order with the information of the current user,bike,status, and time
$sql = "INSERT INTO Orders (userID,active,cykelID) VALUES('$current_user->ID',TRUE,'$cykelid')";
$result = $conn->query($sql);
if($result === TRUE){
	//set bike as not available in the database
  	$uppCykelFalse = "UPDATE Cykel_Fack SET avaliable = FALSE WHERE cykelID = $cykelid";
      	$conn->query($uppCykelFalse);
	//message the user which bike he got and where to pick it up
	  echo "Fack " .$fackid . ", du har blivit tilldelad cykel med ID " .$cykelid ;
}else{
	//message the user that there are no available bikes
    echo" Ingen cykel är tillgänlig ";
    }
$conn->close();
