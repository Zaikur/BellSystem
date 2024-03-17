/*
Quinton Nelson
3/15/2024
this file contains JQuery for the login page
Upon successful login the server will return a token that will be stored in local storage
The token will be used to authenticate future requests to the server
The user is then redirected to the index page
*/

$(document).ready(function() {
    $('#loginForm').on('submit', function(e) {
        e.preventDefault();
        var password = $('#password').val();

        $.ajax({
            type: "POST",
            url: "/completeLogin",
            data: { password: password },
            success: function(response) {
                var token = response.token; // Adjust based on the actual response structure
                console.log(token);
                localStorage.setItem('authToken', token);
                window.location.href = '/';
            },
            error: function() {
                $('#loginMessage').text('Login failed. Please try again.');
            }
        });
    });
});
