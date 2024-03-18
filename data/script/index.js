/*
Quinton Nelson
3/15/2024
this file contains JQuery for the index page
*/


$(document).ready(function() {
    // This function calls the server to initiate a ring test if user is authenticated
    $('#testRingButton').click(function() {
        // Retrieve the stored token
        const authToken = localStorage.getItem('authToken');
        if (!authToken) {
            alert("You are not authenticated. Please login first.");
            return;
        }
    
        $.ajax({
            url: "/ToggleRelay",
            type: "GET",
            // Include the token in the request headers
            headers: {
                "Authorization": authToken
            },
            success: function(data, textStatus, xhr) {
                alert("Ring test initiated. Check device.");
            },
            error: function(xhr, status, error) {
                if (xhr.status === 401 || xhr.status === 403) {
                    // Handle unauthorized or forbidden response
                    alert("Authentication required. Please login.");
                } else {
                    alert("Error initiating ring test: " + error);
                }
            }
        });
    });

    // This method will update the time every second
    function updateTime() {
        var now = new Date();
        dateFormat(now, "dddd, mmmm dS, yyyy, h:MM:ss TT");
        $('#time').text(timeString);
    }

    setInterval(updateTime, 1000); // Update time every second

    // This method will update the countdown to next bell ring every second
    function updateCountdown() {
        var eventTime = new Date('March 20, 2024 12:00:00').getTime(); // Example event time
        var now = new Date().getTime();
        var distance = eventTime - now;

        var days = Math.floor(distance / (1000 * 60 * 60 * 24));
        var hours = Math.floor((distance % (1000 * 60 * 60 * 24)) / (1000 * 60 * 60));
        var minutes = Math.floor((distance % (1000 * 60 * 60)) / (1000 * 60));
        var seconds = Math.floor((distance % (1000 * 60)) / 1000);

        $('#countdown').text(days + "d " + hours + "h " + minutes + "m " + seconds + "s ");

        if (distance < 0) {
            clearInterval(countdownInterval);
            $('#countdown').text("EXPIRED");
        }
    }
    var countdownInterval = setInterval(updateCountdown, 1000);

    // This method will display any error messages from the server
    function displayError(message) {
        $('#errorSection').text(message);
    }
});