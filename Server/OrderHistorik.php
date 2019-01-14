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
$select = "SELECT orderID,dateCreated,dateEnded,cykelID,active FROM Orders WHERE userID = '$current_user->ID' ORDER BY orderID DESC LIMIT 10";
$result = $conn->query($select);
if($result->num_rows>0){
  while ($row = $result->fetch_assoc()) {
    echo  "Order Nr: " .$row["orderID"]. " Cykel Nr: " .$row["cykelID"]. "    Start : " .$row["dateCreated"] . "    Avslutad: " .$row["dateEnded"]. "\n";
  }
  }
  $conn->close();