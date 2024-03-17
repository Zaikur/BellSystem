/*
Quinton Nelson
3/17/2024
this file contains JQuery that authenticates the user and handles the login/logout button
*/

$(document).ready(function() {
    // Responsive navigation
    $("#hamburger").click(function() {
        $("#navLinks").toggle(); // Toggle visibility of navigation links
    });

    // Check if the user is logged in
    const authToken = localStorage.getItem('authToken');
    if (!authToken) {
        // User is not logged in, the button will already be set to "Login" from the HTML
        $('#loginButton').click(function() {
            // Redirect to the login page
            window.location.href = '/login';
        });
    } else {
        // User is logged in, change to a logout button
        $('#loginButton').text('Logout').off('click').click(function() {
            // Clear the token and refresh the page
            localStorage.removeItem('authToken');
            window.location.reload(); // This refreshes the page, logging out the user
        });
    }
});
