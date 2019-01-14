<?php
//establish connection to database
$servername = "localhost";
$username = "root";
$pasword = "elcykel";
$wordpress = "wordpress";
$conn = new mysqli($servername,$username,$pasword,$wordpress);
// get the current online user
$current_user = wp_get_current_user();
//check if connection was established
if($conn->connect_error){
    die("Connection failed: " . $conn->connect_error);
}
//Check which bike that was ordered by current user.
$select = "SELECT cykelID FROM Orders WHERE userID = '$current_user->ID' AND active = TRUE";
$result = $conn->query($select);
if($result->num_rows>0){
  while ($row = $result->fetch_assoc()) {
   $cykelid = $row["cykelID"];
  }
}  
// open the door for the bike that was booked
$opendoor = "UPDATE Cykel_Fack SET doorlocked = TRUE WHERE cykelID = $cykelid";
if($result2 = $conn->query($opendoor)){
    echo " dörren öppnas ";
}else{
	echo " fel ";
}
$conn->close();
