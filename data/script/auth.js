$(document).ready(function() {

    // Check if the user is logged in
    const authToken = localStorage.getItem('authToken');
    if (!authToken) {
        // User is not logged in, show login button
        $('#loginContainer').html('<button id="loginButton">Login</button>');
        
        $('#loginButton').click(function() {
            // Redirect to the login page
            window.location.href = '/login';
        });
    } else {
        // User is logged in, show logout button
        $('#loginContainer').html('<button id="logoutButton">Logout</button>');
        
        $('#logoutButton').click(function() {
            // Clear the token and refresh the page
            localStorage.removeItem('authToken');
            window.location.reload(); // This refreshes the page, logging out the user
        });
    }
});