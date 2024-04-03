#ifndef __HTML_H_
#define __HTML_H_

#include <Arduino.h>

const char html_setting[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html lang="en">

<head>
    <meta name="viewport" content="width=device-width, initial-scale=1" charset="utf-8">
    <title>BUS MONITOR</title>

    <style>
        body {
            background-color: #f1f1f1;
            margin: 0;
            padding: 0;
            font-family: Arial;
        }

        .body_container {
            max-width: 500px;
            margin: auto;
            padding: 20px;
            background-color: #fff;
            border-radius: 10px;
            box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1);
        }

        h1 {
            margin-top: 0;
            text-align: center;
        }

        hr {
            border: none;
            border-top: 1px solid #EAEAEA;
        }

        label {
            font-weight: bold;
            color: black;
        }
    </style>

    <script>
        function getData() {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    var response = this.responseText.split(",");
                    document.getElementById("lat").innerHTML = response[0];
                    document.getElementById("lng").innerHTML = response[1];
                    document.getElementById("kmh").innerHTML = response[2];

                    document.getElementById("direction").innerHTML = response[3];
                    document.getElementById("bus_now").innerHTML = response[4];
                    document.getElementById("bus_pre").innerHTML = response[5];

                    console.log(response);
                }
                else {
                    // document.getElementById("ssid_wifi").innerHTML = "#FAIL";
                    // document.getElementById("id").innerHTML = "#FAIL";
                }
            };
            xhttp.open("GET", "/data", true);
            xhttp.send();
        }
    </script>
</head>

<body>
    <div class="body_container">
        <h1>BUS MONITOR</h1>
        <hr>

        <p>
            <strong>LAT: </strong><span id="lat" class="value">#NULL</span>
        </p>

        <p>
            <strong>LNG: </strong><span id="lng" class="value">#NULL</span>
        </p>

        <p>
            <strong>Speed: </strong><span id="kmh" class="value">#NULL</span>
        </p>
        <hr>

        <p>
            <strong>Direction: </strong><span id="direction" class="value">#NULL</span>
        </p>

        <p>
            <strong>Now Bus Stop: </strong><span id="bus_now" class="value">#NULL</span>
        </p>

        <p>
            <strong>Pre Bus Stop: </strong><span id="bus_pre" class="value">#NULL</span>
        </p>
        <hr>
        
    </div>

    <script>
        getData();
        setInterval(getData, 1000);
    </script>
</body>

</html>
)rawliteral";

#endif