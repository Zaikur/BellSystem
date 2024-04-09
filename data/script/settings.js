/*
Quinton Nelson
03/15/2024
this file contains JQuery for the settings page
*/

$(document).ready(function() {
    // Get the MAC address from the server
    $('#macAddress').click(function() {
        checkServerTokenMatch(function(tokenMatches) {
            if (!tokenMatches) {
                showLoginModal();
                return;
            }

            $.ajax({
                url: '/getMacAddress',
                type: 'GET',
                success: function(response) {
                    $('#macAddressDiv').text(response);
                },
                error: function(xhr, status, error) {
                    console.error("Failed to fetch MAC address:", error);
                }
            })
        })
    });


    // Load settings from the server
    $('#settingsForm').submit(function(event) {
        // Prevent form submission for manual validation
        event.preventDefault();

        // Clear previous error messages
        $('#deviceNameError').text('');
        $('#ringDurationError').text('');
        $('#uniqueURLError').text('');

        var deviceName = $('#deviceName').val();
        var ringDuration = $('#ringDuration').val();
        var uniqueURL = $('#uniqueURL').val();

        // Validate Device Name (alphanumeric and hyphens/underscores only)
        if(!/^[a-zA-Z0-9-_]+$/.test(uniqueURL)) {
            $('#uniqueURLError').text('Invalid characters in device name.');
            return; // Stop submission
        }

        // Validate Ring Duration (must be an integer greater than 0)
        if(!(ringDuration > 0 && Math.floor(ringDuration) == ringDuration)) {
            $('#ringDurationError').text('Ring duration must be a positive integer.');
            return; // Stop submission
        }

        checkServerTokenMatch(function(tokenMatches) {
            if (!tokenMatches) {
                showLoginModal();
                return;
            }

            // Submit the form if validation passes
            // Make AJAX request with the token
            $.ajax({
                url: '/saveSettings',
                type: 'POST',
                headers: {
                    'Authorization': getAuthToken(),
                    'Content-Type': 'application/json'
                },
                data: JSON.stringify({
                    uniqueURL: uniqueURL,
                    deviceName: deviceName,
                    ringDuration: ringDuration
                }),
                success: function(response) {
                    if (response == "URL saved successfully, device will restart to apply changes") {
                        alert("Settings updated successfully! Device will restart to apply changes. \n" +
                                "Your new URL will be: " + uniqueURL + ".local \n" + 
                                "Redirecting to new url in 10 seconds...");
                        setTimeout(function() {
                            window.location.href = "http://" + uniqueURL + ".local";
                        }, 10000);
                    }
                    alert("Settings updated successfully!");
                },
                error: function(xhr, status, error) {
                    if (xhr.status == 403 || xhr.status == 401) {
                        showLoginModal();
                    } else {
                        // Handle other errors
                        alert("Failed to update settings.");
                    }
                }
            });
        });
    });
});

