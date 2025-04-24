<?php

if(isset($_GET["time"]) && isset($_GET["cardID"]) && isset($_GET["authorized"])) {
   $time = $_GET["time"]; // get time value from HTTP GET
   $cardID = $_GET["cardID"]; // get cardID value from HTTP GET
   $authorized = $_GET["authorized"]; // get cardID value from HTTP GET

   $servername = "localhost";
   $username = "Arduino";
   $password = "ArduinoGetStarted.com";
   $dbname = "db_3070";

   // Create connection
   $conn = new mysqli($servername, $username, $password, $dbname);
   // Check connection
   if ($conn->connect_error) {
      die("Connection failed: " . $conn->connect_error);
   }

   $sql = "INSERT INTO rf_record (time, cardID, authorized) VALUES ('$time', '$cardID', '$authorized')";

   if ($conn->query($sql) === TRUE) {
      echo "New record created successfully";
   } else {
      echo "Error: " . $sql . " => " . $conn->error;
   }

   $conn->close();
} else {
   echo "Time or cardID is not set";
}
?>
