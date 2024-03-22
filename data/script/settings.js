/*
Quinton Nelson
03/15/2024
this file contains JQuery for the settings page
*/

$(document).ready(function() {
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
                data: {
                    uniqueURL: uniqueURL,
                    deviceName: deviceName,
                    ringDuration: ringDuration
                },
                success: function(response) {
                    // Handle success
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

