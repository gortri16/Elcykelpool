<?php
$servername = "localhost";
$username = "root";
$pasword = "elcykel";
$wordpress = "wordpress";
$conn = new mysqli($servername,$username,$pasword,$wordpress);
$current_user = wp_get_current_user();
if($conn->connect_error){
    die("Connection failed: " . $conn->connect_error);
}
$select = "SELECT cykelID FROM Orders WHERE userID = '$current_user->ID' AND active = TRUE";
$result = $conn->query($select);
if($result->num_rows>0){
  while ($row = $result->fetch_assoc()) {
   $cykelid = $row["cykelID"];
  }
}  
$opendoor = "UPDATE Cykel_Fack SET doorlocked = TRUE WHERE cykelID = $cykelid";
if($result2 = $conn->query($opendoor)){
    echo " dörren öppnas ";
}else{
	echo " fel ";
}
$conn->close();