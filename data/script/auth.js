/*
Quinton Nelson
3/17/2024
this file contains JQuery that authenticates the user and handles the login/logout button
*/

// This function will check if the client-side token matches the server-side token
function checkServerTokenMatch() {
    const authToken = getAuthToken();
    if (!authToken) {
        // No token present on the client-side, probably not logged in or token was removed
        showLoginModal();
        return false;
    }

    $.ajax({
        url: '/auth',
        type: 'GET',
        headers: { 'Authorization': authToken },
        success: function(response) {
            return true;
        },
        error: function(xhr) {
            if (xhr.status === 401) {
                // Token is invalid, prompt login without losing state
                showLoginModal();
            }
        }
    });
}

// This function returns the token from the browser's local storage
function getAuthToken() {
    return localStorage.getItem('authToken');
}

// This function will display a login modal
function showLoginModal() {
    $('#loginModal').modal('show');
}

$(document).ready(function() {
    updateAuthButton();

    // This function will update the login/logout button based on the presence of a token
    function updateAuthButton() {
        const authToken = getAuthToken();

        if (authToken) {
            // If authToken exists, setup the button for logout
            $('#loginButton').text('Logout').off('click').on('click', function(event) {
                event.preventDefault();
                localStorage.removeItem('authToken');
                window.location.href = '/'; // Redirect to index after logout
            });
        } else {
            // If authToken doesn't exist, setup the button for login
            $('#loginButton').text('Login').off('click').on('click', function(event) {
                event.preventDefault();
                showLoginModal();
            });
        }
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
                window.location.href = '/';
            },
            error: function() {
                $('#loginMessage').text('Login failed. Please try again.');
            }
        });
    });
});
