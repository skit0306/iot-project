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

$sql = "SELECT * FROM rf_record";
$result = $conn->query($sql);

if ($result->num_rows > 0) {
    // Output data of each row
    echo "<table border='1'>
    <tr>
        <th>No</th>
		<th>Time</th>
        <th>CardID</th>
		<th>Authorized</th>
    </tr>";

    while ($row = $result->fetch_assoc()) {
        echo "<tr>
            <td>" . $row["no"] . "</td>
			<td>" . $row["time"] . "</td>
            <td>" . $row["cardID"] . "</td>
			<td>" . $row["authorized"] . "</td>
        </tr>";
    }

    echo "</table>";
} else {
    echo "0 results";
}

$conn->close();
?>
