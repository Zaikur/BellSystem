/*
Quinton Nelson
3/17/2024
this file contains JQuery that authenticates the user and handles the login/logout button
*/

// This function will check if the client-side token matches the server-side token
function checkServerTokenMatch(callback) {
    const authToken = getAuthToken();
    if (!authToken) {
        console.log("No auth token found.");
        callback(false); // Immediately invoke callback with false
        return;
    }

    console.log("Sending token for verification: ", authToken);

    $.ajax({
        url: '/auth',
        type: 'GET',
        headers: { 'Authorization': authToken },
        success: function(response) {
            console.log("Token verification success.");
            callback(true); // Token matches, invoke callback with true
        },
        error: function(xhr) {
            console.log("Token verification failed. Status: ", xhr.status);
            // Removed the second callback call in the error handler to prevent double invocation
            callback(false); // Token does not match, invoke callback with false
        }
    });
}


// This function returns the token from the browser's local storage
function getAuthToken() {
    return localStorage.getItem('authToken');
}

// This function will display a login modal, and after it's displayed it will focus on the password input
function showLoginModal() {
    $('#loginModal').modal('show').on('shown.bs.modal', function () {
        $('#password').focus();
    });
}

$(document).ready(function() {
    updateAuthButton();

    // This function will update the login/logout button based on the presence of a token
    // This function updates the login/logout button based on the token's presence and validity
    function updateAuthButton() {
        checkServerTokenMatch(function(tokenMatches) {
            if (tokenMatches) {
                // If authToken is verified, setup the button for logout
                $('#loginButton').text('Logout').off('click').on('click', function(event) {
                    event.preventDefault();
                    localStorage.removeItem('authToken');
                    window.location.href = '/'; // Redirect to index after logout
                });
            } else {
                // If authToken doesn't match, setup the button for login
                $('#loginButton').text('Login').off('click').on('click', function(event) {
                    event.preventDefault();
                    showLoginModal();
                });
            }
        });
    }


    $('#loginForm').on('submit', function(e) {
        e.preventDefault();
        var password = $('#password').val();

        $.ajax({
            type: "POST",
            url: "/completeLogin",
            data: { password: password },
            success: function(response) {
                var token = response.token; // Adjust based on the actual response structure
                localStorage.setItem('authToken', token);
                $('#loginModal').modal('hide');
                updateAuthButton();
            },
            error: function() {
                $('#loginMessage').text('Login failed. Please try again.');
            }
        });
    });
});
