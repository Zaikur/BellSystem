/*
Quinton Nelson
3/17/2024
this file contains JQuery that authenticates the user and handles the login/logout button
*/

$(document).ready(function() {
    updateAuthButton();

    // This method will update the login/logout button text based on the presence of the authToken
    function updateAuthButton() {
        const authToken = localStorage.getItem('authToken');

        $('#loginButton').off('click').on('click', function(event) {
            event.preventDefault(); // Prevent the default action of the anchor tag
            if (authToken) {
                // If authToken exists, handle logout
                localStorage.removeItem('authToken');
                window.location.href = '/'; // Redirect to index after logout
            } else {
                // If authToken doesn't exist, handle login
                window.location.href = '/login'; // Redirect to the login page
            }
        });

        if (authToken) {
            // If authToken exists, change the button text to "Logout"
            $('#loginButton').text('Logout');
        } else {
            // If authToken doesn't exist, set or keep the button text to "Login"
            $('#loginButton').text('Login');
        }
    }
});
