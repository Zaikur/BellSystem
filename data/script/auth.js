/*
Quinton Nelson
3/17/2024
this file contains JQuery that authenticates the user and handles the login/logout button
*/

/**
 * The function `checkServerTokenMatch` verifies a server token by sending it to the server and
 * invoking a callback with the result.
 * @param callback - The `callback` parameter in the `checkServerTokenMatch` function is a function
 * that will be invoked with a boolean argument indicating whether the server token matches or not. The
 * callback function is called with `true` if the token verification is successful, and `false` if the
 * token verification fails.
 * @returns The `checkServerTokenMatch` function is not explicitly returning any value. It is making an
 * asynchronous AJAX request to verify a token and then invoking the provided callback function with
 * either `true` or `false` based on the token verification result.
 */
function checkServerTokenMatch(callback) {
    const authToken = getAuthToken();
    if (!authToken) {
        callback(false); // Immediately invoke callback with false
        return;
    }

    $.ajax({
        url: '/auth',
        type: 'GET',
        headers: { 'Authorization': authToken },
        success: function(response) {
            callback(true); // Token matches, invoke callback with true
        },
        error: function(xhr) {
            callback(false); // Token does not match, invoke callback with false
        }
    });
}


/**
 * The function `getAuthToken` retrieves the authentication token from the localStorage.
 * @returns The function `getAuthToken` is returning the value stored in the `authToken` key in the
 * localStorage.
 */
function getAuthToken() {
    return localStorage.getItem('authToken');
}

/**
 * The function `showLoginModal` displays a login modal and focuses on the password input field when
 * the modal is shown.
 */
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
            error: function(xhr, status, error) {
                if (xhr.status == 400 || xhr.status == 401) {
                    $('#wrongPassword').text('Invalid password. Please try again.');
                    $('#password').focus();
                }
            }
        });
    });
});
