/*
Quinton Nelson
3/15/2024
this file contains JQuery for the change password page
*/

$(document).ready(function() {
    $('#changePasswordForm').submit(function(event) {
        event.preventDefault(); // Prevent form submission for manual validation

        $('#passwordStrengthError').text('');
        $('#passwordMatchError').text('');

        var newPassword = $('#newPassword').val();
        var confirmPassword = $('#confirmPassword').val();
        var oldPassword = $('#oldPassword').val();

        if (!checkServerTokenMatch) {
            showLoginModal();
            return;
        }

        if (newPassword.length < 8) {
            $('#passwordStrengthError').text('Password must be at least 8 characters.');
            return;
        }

        if (newPassword !== confirmPassword) {
            $('#passwordMatchError').text('Passwords do not match.');
            return; // Stop submission
        }

        checkServerTokenMatch(function(tokenMatches) {
            if (!tokenMatches) {
                // If token doesn't match, show login modal and stop further execution
                return;
            }

            // Make AJAX request with token to change password
            $.ajax({
                url: '/finalizePassword',
                type: 'POST',
                data: {
                    OldPassword: oldPassword,
                    NewPassword: newPassword
                },
                headers: {
                    'Authorization': getAuthToken()
                },
                success: function(response) {
                    alert("Password changed successfully. Please log back in.");
                    localStorage.removeItem('authToken');
                    showLoginModal(); // Redirect after successful password change
                },
                error: function(xhr) {
                    alert("Error changing password.");
                }
            });
        });        
    });
});

