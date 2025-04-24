<?php

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

$sql = "SELECT * FROM card";
$result = $conn->query($sql);

if ($result->num_rows > 0) {
    // Output data of each row
    echo "<table border='1'>
    <tr>
        <th>CardID</th>
    </tr>";

    while ($row = $result->fetch_assoc()) {
        echo "<tr>
            <td>" . $row["cardID"] . "</td>
        </tr>";
    }

    echo "</table>";
} else {
    echo "0 results";
}

$conn->close();
?>
