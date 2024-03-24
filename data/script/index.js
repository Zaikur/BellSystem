/*
Quinton Nelson
3/15/2024
this file contains JQuery for the index page
Contains functions to update the time on the page, fetch remaining ring times, and fetch server messages
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

    /**
     * The function `updateTime` retrieves an initial time string from an element, converts it to a Date
     * object, and then continuously updates the time displayed by adding one second at a time.
     */
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
    

    /**
     * The function fetches today's remaining ring times, filters out past times, and initiates a countdown
     * to the next ring time if available.
     * @returns The `fetchTodayRemainingRingTimes` function makes an AJAX request to the server to get the
     * remaining ring times for today. The response is expected to be a comma-separated list of times in
     * the format "HH:MM,HH:MM,...". The function then filters out past times, sorts the remaining times,
     * and starts a countdown to the next ring time if there are any remaining times. If
     */
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

    /**
     * The function `updateCountdown` sets up a countdown interval to display the time remaining until a
     * specified next ring time.
     * @param nextRingTime - The `nextRingTime` parameter in the `updateCountdown` function represents the
     * time of the next ring in the format "HH:MM" (hours and minutes). It is used to calculate the
     * countdown until that specific time.
     * @returns The `updateCountdown` function returns nothing explicitly. It sets up an interval to update
     * a countdown timer displayed on the page until a certain time is reached, at which point it fetches
     * new times and updates the countdown.
     */
    function updateCountdown(nextRingTime) {
        // Clear existing countdown interval if one exists
        if (window.countdownInterval) {
            clearInterval(window.countdownInterval);
        }

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

    /**
     * The function `fetchServerMessages` makes an AJAX request to fetch server messages and displays them
     * in a list on the webpage, handling errors appropriately.
     */
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