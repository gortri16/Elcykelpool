<?php
$servername = "localhost";
$username = "root";
$pasword = "elcykel";
$wordpress = "wordpress";
$conn = new mysqli($servername,$username,$pasword,$wordpress);


if($conn->connect_error){
  die("Connection failed: " . $conn->connect_error);
}
$current_user = wp_get_current_user();
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
$sql = "INSERT INTO Orders (userID,active,cykelID) VALUES('$current_user->ID',TRUE,'$cykelid')";
$result = $conn->query($sql);
if($result === TRUE){
  	  $uppCykelFalse = "UPDATE Cykel_Fack SET avaliable = FALSE WHERE cykelID = $cykelid";
      $conn->query($uppCykelFalse);
	  echo "Fack " .$fackid . ", du har blivit tilldelad cykel med ID " .$cykelid ;
}else{
    echo" Ingen cykel är tillgänlig ";
    }
$conn->close();