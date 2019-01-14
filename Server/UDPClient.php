<?php

// php /home/pi/PhpModbus/phpmodbus-master/UDPClient.php
//require_once dirname(__FILE__) . '/../Phpmodbus/ModbusMaster.php';
$servername = "localhost";
$username = "root";
$pasword = "elcykel";
$wordpress = "wordpress";
$conn = new mysqli($servername,$username,$pasword,$wordpress);
if($conn->connect_error){
    die("Connection failed: " . $conn->connect_error);
}
echo "UDP Client Startup \n";

//Send/receive UDP messages to/from this IP and port
// Arduino Uno
$server_ip = '192.168.1.177';
$server_port = 8888;
// My PC
//$server_ip = "192.168.1.1";
//$server_port = 51258;

$beat_period = 2;

$slaveID = 2;
$address3E = 0x3E;
$address40 = 0x40;
// Read Coils
$numCoils = 1;
// Write Single Coil
$coilValue = 0x01;

$errorCount = 0;
error_reporting(~E_WARNING);

$recData1 = 0;
$state = 0;

// Create a UDP socket
// AF_INET: IP4 address
if (!$socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP)) {
    $errorcode = socket_last_error();
    $errormsg = socket_strerror($errorcode);
    die("Couldn't create socket : [$errorcode] $errormsg \n");
}
echo "Socket created \n";

// Set socket timeout to 1 second
if (!socket_set_option($socket, SOL_SOCKET, SO_RCVTIMEO, array("sec"=>1,"usec"=>0))) {
    $errorcode = socket_last_error();
    $errormsg = socket_strerror($errorcode);
    die("Unable to set options on socket : [$errorcode] $errormsg \n");
}

// Bind the source address
if (!socket_bind($socket, "192.168.1.10", "8888")) {
    $errorcode = socket_last_error();
    $errormsg = socket_strerror($errorcode);
    die("Could not bind socket : [$errorcode] $errormsg \n");
}
echo "Socket bind OK \n";

while (1) {
    /*
    echo "------------------------------------------------------------------------ \n";
    echo "--------------------------- READ BIKE STATUS ---------------------------\n";
    echo "------------------------------------------------------------------------ \n";
    while (readCoils($slaveID, $address40, $numCoils) != true) {     // SlaveID, Coil starting address, number of coils
        $errorCount++;
        echo "errorCount: " . $errorCount . "\n";
        sleep($beat_period);
        if ($errorCount==5) {
            $errorCount = 0;
            break;
        }
    }
    sleep($beat_period);

    echo "------------------------------------------------------------------------ \n";
    echo "------------------------------ GIVE BIKE -------------------------------\n";
    echo "------------------------------------------------------------------------ \n";
    while (readCoils($slaveID, $address3E, $numCoils) != true) {     // SlaveID, Coil starting address, number of coils
        $errorCount++;
        echo "errorCount: " . $errorCount . "\n";
        sleep($beat_period);
        if ($errorCount==5) {
            $errorCount = 0;
            break;
        }
    }
    sleep($beat_period);

    echo "------------------------ \n";
    while (writeSingleCoil($slaveID, $address3E, $coilValue) != true) {     // SlaveID, Coil address, value: high=0x01, low=0x00
        $errorCount++;
        echo "errorCount: " . $errorCount . "\n";
        sleep($beat_period);
        if ($errorCount==5) {
            $errorCount = 0;
            break;
        }
    }
    sleep($beat_period);
    
    echo "------------------------ \n";
    while (readCoils($slaveID, $address3E, $numCoils) != true) {     // SlaveID, Coil starting address, number of coils
        $errorCount++;
        echo "errorCount: " . $errorCount . "\n";
        sleep($beat_period);
        if ($errorCount==5) {
            $errorCount = 0;
            break;
        }
    }
    sleep($beat_period);

    echo "------------------------------------------------------------------------ \n";
    echo "--------------------------- READ BIKE STATUS ---------------------------\n";
    echo "------------------------------------------------------------------------ \n";
    while (readCoils($slaveID, $address40, $numCoils) != true) {     // SlaveID, Coil starting address, number of coils
        $errorCount++;
        echo "errorCount: " . $errorCount . "\n";
        sleep($beat_period);
        if ($errorCount==5) {
            $errorCount = 0;
            break;
        }
    }
    sleep($beat_period);
    */



    
    /*
    while (readCoils($slaveID, $address40, $numCoils) != true) {     // SlaveID, Coil starting address, number of coils
        $errorCount++;
        echo "errorCount: " . $errorCount . "\n";
        sleep($beat_period);
        if ($errorCount==5) {
            $errorCount = 0;
            break;
        }
    }
    sleep($beat_period);

    // Get status on bike in garage
    while (readCoils($slaveID, $address40, $numCoils) != true) {     // SlaveID, Coil starting address, number of coils
        $errorCount++;
        echo "errorCount: " . $errorCount . "\n";
        sleep($beat_period);
        if ($errorCount==5) {
            $errorCount = 0;
            break;
        }
    }
    sleep($beat_period);

    if($recData1 == True && $bikeLeft = false){
        
        returnBike();
    }

    // If bike is in garage and return is made
    if($recData1 == True && $bikeLeased = false){
        
        returnBike();
    }
    */
    
    /*
    switch ($state) {
        case 0:
        echo "Seariching \n";
            // Search for orders
            if (opendoor()) {
                echo "Order found \n";
                $state = 1;
            }
            break;
        case 1:
        echo "Waiting for bike to leave \n";
            while (readCoils($slaveID, $address40, $numCoils) != true) {     // SlaveID, Coil starting address, number of coils
                $errorCount++;
                echo "errorCount: " . $errorCount . "\n";
                sleep($beat_period);
                if ($errorCount==5) {
                    $errorCount = 0;
                    break;
                }
            }
            sleep($beat_period);
            if ($recData1 == 0) {
                $state = 2;
            }
            break;
        case 2:
        echo "Waiting for bike to return \n";
            while (readCoils($slaveID, $address40, $numCoils) != true) {     // SlaveID, Coil starting address, number of coils
                $errorCount++;
                echo "errorCount: " . $errorCount . "\n";
                sleep($beat_period);
                if ($errorCount==5) {
                    $errorCount = 0;
                    break;
                }
            }
            sleep($beat_period);
            //if ($recData1 == 1) {
              //  echo " recdata = 1";
                //returnBike();
                //$state = 0;
           // }
            break;
        default:
            
            # code...
            break;
    }
    */

    //opendoor();

    returnBike();

    die();
    
}
    

function returnBike() {
    global $recData1, $servername, $username, $pasword, $wordpress, $conn, $beat_period, $slaveID, $address3E, $coilValue;
    
        echo " bike is returned \n";
        $checkorder = "SELECT userID, active FROM Orders WHERE cykelID = 2 AND active = TRUE";
        $result = $conn->query($checkorder);
        if($result->num_rows>0){
            echo "result success \n";
          while ($row = $result->fetch_assoc()) {
            $userid = $row["userID"];
            echo "$userid";
            $active = $row["active"];
            $flag = TRUE;
          }
          $bikeLeased = false;
        }else{
          echo "inga bokningar finns \n";
        } 
        if($active == TRUE && $flag == TRUE){
          $updaterafack = "UPDATE Cykel_Fack SET avaliable = TRUE WHERE cykelID = 2";
          if($conn->query($updaterafack)){
              echo"fack uppdaterad\n";
          }else{
              echo"kunde inte uppdatera fack\n";
          }
          $updaterafack = "UPDATE Cykel_Fack SET doorlocked = FALSE WHERE fackID = 2";
          if($conn->query($updaterafack)){
              echo"fack uppdaterad\n";
          }else{
              echo"kunde inte uppdatera fack\n";
          }
          $uppdateraorder = "UPDATE Orders SET active = FALSE WHERE cykelID = 2 AND userID = $userid";
          if($conn->query($uppdateraorder)){
              echo"order är nu arkiverad\n";
          }else{
              echo"order kunde inte arkiveras\n";
          }
          $flag = FALSE;
        }else{
            echo"ingen cykel är bokad\n";
        }
    
    $conn->close();
}
function opendoor(){
    global $recData1, $servername, $username, $pasword, $wordpress, $conn, $beat_period, $slaveID, $address3E, $coilValue;
    $isdoorOpened = "SELECT doorlocked FROM Cykel_Fack WHERE fackID = 2 ";
    $res = $conn->query($isdoorOpened);
    if($res->num_rows>0){
        while($rows = $res->fetch_assoc()){
        $opendoor = $rows["doorlocked"];
        }
    }
    if($opendoor == TRUE){
        echo "------------------------ \n";
        echo " open door \n";
        while (writeSingleCoil($slaveID, $address3E, $coilValue) != true) {     // SlaveID, Coil address, value: high=0x01, low=0x00
            $errorCount++;
            echo "errorCount: " . $errorCount . "\n";
            sleep($beat_period);
            if ($errorCount==5) {
               $errorCount = 0;
                echo "connection failed\n";
                break;
            }
            sleep($beat_period);
        }
        $closedoor = "UPDATE Cykel_Fack SET doorlocked = 0 WHERE fackID = 2";
        //if($closeres = $conn->query($closedoor)){
        //    echo "door closed";
        //}
        return true;
    }else{ 
        //echo "door is not open\n";
        return false;
    }
}

socket_close($socket);

// Read coils from slave
// SlaveID, Coil starting address, number of coils
function readCoils($id, $address, $number) {
    global $server_ip, $server_port, $socket, $recData1;
    $message = chr($id) . chr(0x01) . chr(0x00) . chr($address) . chr(0x00) . chr($number);

    print "Slave " . $id . ", read " . $number. " coil(s) at address " . dechex($address) . "\n";
    socket_sendto($socket, $message, strlen($message), 0, $server_ip, $server_port);
    //print "Time: " . date("%r") . "\n";

    $r = socket_recvfrom($socket, $buf, 512, 0, $server_ip, $server_port);
    $array = bin2hex($buf);
    //echo "Slave " . $id . ", full msg: " . $array . "\n";
    print "Slave " . $id . ", response: ";

    // Check if slave response is correct  
    if ($array[1] != 1) {    // SOH
        print "readCoilsError : SOH not found \n";
        return false;
    }
    if ($array[3] != $id) {     // slave ID
        print "readCoilsError : ID not correct \n";
        return false;
    }
    if ($array[5] != 0x01) {    // function
        print "readCoilsError : wrong function code \n";
        return false;
    }
    if ($array[7] != $number) {    // numCoils
        print "readCoilsError : number of coils returned not correct \n";
        return false;
    }
    if ($array[9] != 0x00 && $array[9] != 0x01) {    // value
        print "readCoilsError : coil value incorrect \n";
        return false;
    }
    print $array[9] . "\n";
    
    $recData1 = $array[9];
    //print "Inlämning: " . $recData1 . "\n";

    return true;
}

// Set coil value in slave
// SlaveID, Coil address, value: high=0x01, low=0x00
function writeSingleCoil($id, $address, $value) {
    global $server_ip, $server_port, $socket;
    $message = chr($id) . chr(0x05) . chr(0x00) . chr($address) . chr(0x00) . chr($value);

    print "Slave " . $id . ", write single coil value" . $value . " at address " . dechex($address) . "\n";
    socket_sendto($socket, $message, strlen($message), 0, $server_ip, $server_port);
    //print "Time: " . date("%r") . "\n";

    $r = socket_recvfrom($socket, $buf, 512, 0, $server_ip, $server_port);
    $array = bin2hex($buf);
    //echo "Slave " . $id . ", full msg: " . $array . "\n";
    print "Slave " . $id . ", response: ";

    // Check if slave response is correct  
    if ($array[1] != 0x01) {    // SOH
        print "writeSingleCoilError : SOH not found \n";
        return false;
    }
    if ($array[3] != $id) {     // slave ID
        print "writeSingleCoilError : ID not correct \n";
        return false;
    }
    if ($array[5] != 0x05) {    // function
        print "writeSingleCoilError : wrong function code \n";
        return false;
    }
    if ($array[7] != 0x00) {    // addressHi
        print "writeSingleCoilError : wrong address \n";
        return false;
    }
    print "value " . $value . " set" . "\n";

    return true;
}

?>