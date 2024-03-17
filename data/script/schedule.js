/*
Quinton Nelson
03/15/2024
this file contains JQuery to dynamically create the webpage for the schedule form
*/


$(document).ready(function() {
    const daysOfWeek = ["monday", "tuesday", "wednesday", "thursday", "friday", "saturday", "sunday"];

    function initScheduleForm() {
        daysOfWeek.forEach(day => {
            const $dayDiv = $('<div class="day-schedule" id="schedule-' + day + '"></div>');
            const $heading = $('<h3 class="day-heading">' + capitalizeFirstLetter(day) + '</h3>');
            const $timesContainer = $('<div class="times-container"></div>');
            const $addRingButton = $('<button type="button" class="add-ring-button">Add Ring Time</button>').data('day', day);

            $dayDiv.append($heading, $timesContainer, $addRingButton);
            $('#scheduleForm').append($dayDiv);
        });
    }

    $('body').on('click', '.add-ring-button', function() {
        const day = $(this).data('day');
        const $timesContainer = $(`#times-${day}`);
        const $timeInputContainer = $('<div class="form-group time-input-container"></div>');
        const $newTimeInput = $('<input type="time" class="form-control ring-time" required>');
        const $deleteButton = $('<button type="button" class="btn btn-danger btn-sm delete-time-button">Delete</button>');

        $timeInputContainer.append($newTimeInput, $deleteButton);
        $timesContainer.append($timeInputContainer);
    });

    $('body').on('click', '.delete-time-button', function() {
        $(this).closest('.time-input-container').remove();
    });

    $('#saveScheduleBtn').click(function() {
        let scheduleData = {};

        daysOfWeek.forEach(day => {
            const times = [];
            $('#schedule-' + day + ' .ring-time').each(function() {
                times.push($(this).val());
            });
            scheduleData[day] = times;
        });

        const authToken = localStorage.getItem('authToken');
        if (!authToken) {
            alert("You are not authenticated. Please login first.");
            return;
        }
        
        $.ajax({
            url: '/updateSchedule',
            type: 'POST',
            contentType: 'application/json',
            headers: { 'Authorization': authToken },
            data: JSON.stringify(scheduleData),
            success: function(response) {
                alert("Schedule updated successfully!");
            },
            error: function(xhr) {
                if (xhr.status === 401 || xhr.status === 403) {
                    window.location.href = '/login.html';
                } else {
                    alert("Failed to update schedule.");
                }
            }
        });
    });

    function capitalizeFirstLetter(string) {
        return string.charAt(0).toUpperCase() + string.slice(1);
    }

    initScheduleForm();
});

