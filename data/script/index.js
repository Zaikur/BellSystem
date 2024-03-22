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
                // If token doesn't match, show login modal and stop further execution
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
        var now = new Date();
        var options = { weekday: 'long', year: 'numeric', month: 'long', day: 'numeric', hour: '2-digit', minute: '2-digit', second: '2-digit' };
        var timeString = new Intl.DateTimeFormat('en-US', options).format(now);
        $('#time').text(timeString);
    }

    setInterval(updateTime, 1000); // Update time every second

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
                    console.log(timeDate);
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
            console.log("Distance:", distance);
            console.log("Next ring date:", nextRingDate);

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
            success: function(response) {
                // Server messages are separated by new lines
                var messages = response.split('\n');
                updateMessageList(messages);
            },
            error: function(xhr, status, error) {
                console.error("Failed to fetch server messages:", error);
                displayError("Failed to load system messages.");
            }
        });
    }

    // This method will request status messages from the server
    function updateMessageList(messages) {
        var $messageList = $('#messageList');
        $messageList.empty(); // Clear existing messages
        
        messages.forEach(function(message) {
            if (message.trim().length > 0) { // Ensure the message is not just empty spaces
                var $li = $('<li class="list-group-item bg-dark text-white"></li>');
                $li.text(message);
                $messageList.append($li);
            }
        });

        if (messages.length === 0 || (messages.length === 1 && messages[0].trim().length === 0)) {
            // If no messages or only empty message
            var $li = $('<li class="list-group-item bg-dark text-white">No system messages.</li>');
            $messageList.append($li);
        }
    }

    // This method will display any error messages from the server
    function displayError(message) {
        $('#errorSection').text(message);
    }

    fetchServerMessages(); // Fetch server messages when the page loads
    fetchTodayRemainingRingTimes(); // Fetch remaining ring times when the page loads

});