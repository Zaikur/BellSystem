/*
Quinton Nelson
3/15/2024
this file contains JQuery for the index page
Contains the following functions:
- updateTime: updates the time on the page every second
- fetchTodayRemainingRingTimes: fetches the remaining ring times for today and updates the countdown
- updateCountdown: updates the countdown to the next bell ring every second
- fetchServerMessages: requests status messages from the server
- updateMessageList: updates the list of server messages
- displayError: displays any error messages from the server
*/


$(document).ready(function() {
    // This function calls the server to initiate a ring test if user is authenticated
    $('#testRingButton').click(function() {
        checkServerTokenMatch(function(tokenMatches) {
            if (!tokenMatches) {
                showLoginModal();
                return;
            }
    
            $.ajax({
                url: "/ToggleRelay",
                type: "GET",
                // Include the token in the request headers
                headers: {
                    "Authorization": getAuthToken()
                },
                success: function(data, textStatus, xhr) {
                    alert("Ring test initiated. Check device.");
                },
                error: function(xhr, status, error) {
                    if (xhr.status === 401 || xhr.status === 403) {
                        // Handle unauthorized or forbidden response
                        showLoginModal();
                    } else {
                        alert("Error initiating ring test: " + error);
                    }
                }
            });
        });
    });

    // This method will update the time ther server sent every second
    // Verify that the server is sending the correct time
    function updateTime() {
        var initialTimeString = $('#time').text(); // Get the initial time string from the element
        var initialTime = new Date(initialTimeString); // Convert it to a Date object
    
        // Function to add seconds to a Date object
        function addSeconds(date, seconds) {
            return new Date(date.getTime() + seconds * 1000);
        }
    
        // Update the time by adding one second
        function update() {
            var updatedTime = addSeconds(initialTime, 1); // Add one second to the initial time
            var options = { weekday: 'long', year: 'numeric', month: 'long', day: 'numeric', hour: '2-digit', minute: '2-digit', second: '2-digit', hour12: true };
            var timeString = new Intl.DateTimeFormat('en-US', options).format(updatedTime);
            $('#time').text(timeString);
    
            initialTime = updatedTime; // Set the initialTime to the updated time for the next update
        }
    
        // Call update every second
        setInterval(update, 1000);
    }
    

    // This method will fetch the remaining ring times for today and update the countdown
    function fetchTodayRemainingRingTimes() {
        $.ajax({
            url: '/getTodayRemainingRingTimes',
            type: 'GET',
            success: function(response) {
                // Response is a comma-separated list of times "HH:MM,HH:MM,..."
                var times = response.split(',');
                // Filter out past times and sort
                var now = new Date();
                times = times.filter(function(timeStr) {
                    var todayStr = now.toISOString().split('T')[0]; // "YYYY-MM-DD"
                    var timeDate = new Date(todayStr + 'T' + timeStr + ':00');
                    return now < timeDate;
                });

                if (times.length > 0) {
                    updateCountdown(times[0]); // Start countdown to the next ring time
                } else {
                    $('#countdown').text("No more rings today");
                }
            },
            error: function(xhr, status, error) {
                console.error("Failed to fetch remaining ring times:", error);
            }
        });
    }

    // This method will update the countdown to next bell ring every second
    // Once countdown is reached, fetch new ring times and update countdown
    function updateCountdown(nextRingTime) {
        // Clear existing countdown interval if one exists
        if (window.countdownInterval) {
            clearInterval(window.countdownInterval);
        }

        console.log("Next ring time:", nextRingTime);

        // Convert next ring time to a Date object
        var now = new Date();
        var todayStr = now.toISOString().split('T')[0]; // "YYYY-MM-DD"
        var nextRingDate = new Date(todayStr + 'T' + nextRingTime + ':00');

        window.countdownInterval = setInterval(function() {
            var now = new Date().getTime();
            var distance = nextRingDate - now;

            if (distance < 0) {
                clearInterval(window.countdownInterval);
                fetchTodayRemainingRingTimes(); // Fetch new times and update countdown
                return;
            }

            var hours = Math.floor((distance % (1000 * 60 * 60 * 24)) / (1000 * 60 * 60));
            var minutes = Math.floor((distance % (1000 * 60 * 60)) / (1000 * 60));
            var seconds = Math.floor((distance % (1000 * 60)) / 1000);

            $('#countdown').text(hours + "h " + minutes + "m " + seconds + "s ");
        }, 1000);
    }

    function fetchServerMessages() {
        $.ajax({
            url: '/getServerMessages',
            type: 'GET',
            dataType: 'json',
            success: function(messages) { // Directly receive the array of messages
                var messageList = $('#messageList');
                messageList.empty(); // Clear the list before adding new messages
    
                messages.forEach(function(message) {
                    var listItem = $('<li class="list-group-item bg-dark text-white"></li>').text(message);
                    messageList.append(listItem);
                });
            },
            error: function(xhr, status, error) {
                console.error("Failed to fetch server messages:", error);
                $('#messageList').empty() // Clear the list
                                .append($('<li class="list-group-item bg-dark text-white"></li>').text("Failed to load system messages."));
            }
        });
    }

    updateTime(); // Update the time on the page
    fetchServerMessages(); // Fetch server messages when the page loads
    fetchTodayRemainingRingTimes(); // Fetch remaining ring times when the page loads

});