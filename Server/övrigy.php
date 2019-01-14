<?php


require_once dirname(__FILE__) . '/../Phpmodbus/ModbusMaster.php';
$servername = "localhost";
$username = "root";
$pasword = "elcykel";
$wordpress = "wordpress";
$conn = new mysqli($servername,$username,$pasword,$wordpress);
if($conn->connect_error){
    die("Connection failed: " . $conn->connect_error);
}

// Create Modbus object
$modbus = new ModbusMaster("192.168.1.177", "TCP");
while(TRUE){
try {
    // FC 1
    $recData1 = $modbus->readCoils(1,62,1);
    $recData2 = $modbus->readCoils(1,62,1);
    $recdata3 = $modbus->readCoils(1,62,1);
    echo"$recData";
}
catch (Exception $e) {
    // Print error information if any
    echo $modbus;
    echo $e;
    exit;
}
  if($recData1 == True){
  $checkorder = "SELECT userID, active FROM Orders WHERE active = TRUE AND cykeldID = 1";
  $result = $conn->query($checkorder);
  if($result->num_rows>0){
    while ($row = $result->fetch_assoc()) {
      $userid = $row["userID"];
      $active = $row["active"];
      $flag = TRUE;
    }
  }else{
    echo "can't reach ";
  } 
  if($active == TRUE && $flag == TRUE){
    $updaterafack = "UPDATE Cykel_Fack SET avaliable = TRUE WHERE cykelID = 1";
    if($conn->query($updaterafack)){
        echo"fack uppdaterad";
    }else{
        echo"kunde inte uppdatera fack";
    }
    $updateraorder = "UPDATE Orders SET active = FALSE WHERE cykelID = 1 AND userID = $userid";
    if($conn->query($uppdateraorder)){
        echo"order är nu arkiverad";
    }else{
        echo"order kunde inte arkiveras";
    }
    $flag = FALSE;
  }
  }
  if($recData2 == TRUE){
    $checkorder2 = "SELECT userID, active FROM Orders WHERE active = TRUE AND cykeldID = 2";
    $result2 = $conn->query($checkorder2);
    if($result2->num_rows>0){
        while ($row2 = $result2->fetch_assoc()) {
        $userid2 = $row2["userID"];
        $active2 = $row2["active"];
        $flag2 = TRUE;
    }
    } else{
        echo "can't reach ";
    } 
    if($active2 == TRUE && $flag2 == TRUE){
        $updaterafack2 = "UPDATE Cykel_Fack SET avaliable = TRUE WHERE cykelID = 2";
        if($conn->query($updaterafack2)){
            echo"fack uppdaterad";
    }   else{
            echo"kunde inte uppdatera fack";
        }
        $updateraorder2 = "UPDATE Orders SET active = FALSE WHERE cykelID = 2 AND userID = $userid2";
        if($conn->query($uppdateraorder2)){
            echo"order är nu arkiverad";
        } else{
            echo"order kunde inte arkiveras";
            }
        $flag2 = FALSE; 
    }
  }
  if($recData3 == TRUE){
    $checkorder3 = "SELECT userID, active FROM Orders WHERE active = TRUE AND cykeldID = 3";
    $result3 = $conn->query($checkorder3);
    if($result3->num_rows>0){
        while ($row3 = $result3->fetch_assoc()) {
        $userid3 = $row3["userID"];
        $active3 = $row3["active"];
        $flag3 = TRUE;
    }
    } else{
        echo "can't reach ";
    } 
    if($active3 == TRUE && $flag3 == TRUE){
        $updaterafack3 = "UPDATE Cykel_Fack SET avaliable = TRUE WHERE cykelID = 3";
        if($conn->query($updaterafack3)){
            echo"fack uppdaterad";
    }   else{
            echo"kunde inte uppdatera fack";
        }
        $updateraorder3 = "UPDATE Orders SET active = FALSE WHERE cykelID = 3 AND userID = $userid3";
        if($conn->query($uppdateraorder3)){
            echo"order är nu arkiverad";
        } else{
            echo"order kunde inte arkiveras";
            }
        $flag3 = FALSE; 
    }
  
    }
}
// Print read data
echo "</br>Data:</br>";
var_dump($recData); 
echo "</br>";
$conn->close();