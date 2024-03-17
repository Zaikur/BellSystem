/*
Quinton Nelson
3/17/2024
this file contains JQuery that authenticates the user and handles the login/logout button
*/

$(document).ready(function() {
    updateAuthButton();

    function updateAuthButton() {
        const authToken = localStorage.getItem('authToken');

        if (authToken) {
            // If authToken exists, change the button to "Logout"
            $('#loginButton').text('Logout').on('click', function() {
                localStorage.removeItem('authToken');
                window.location.reload();
            });
        } else {
            // If authToken doesn't exist, keep or set the button to "Login"
            $('#loginButton').text('Login').off('click').on('click', function() {
                window.location.href = '/login';
            });
        }
    }
});

