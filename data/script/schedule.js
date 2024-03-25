/*
Quinton Nelson
03/15/2024
this file contains JQuery to dynamically create the webpage for the schedule form
*/


$(document).ready(function() {
    const daysOfWeek = ["monday", "tuesday", "wednesday", "thursday", "friday", "saturday", "sunday"];
    let globalScheduleData = {};

    function init() {
        initScheduleForm();
        fetchCurrentSchedule();
    }

    /**
     * The function `initScheduleForm` dynamically creates a schedule form with days of the week, headings,
     * and buttons for adding ring times.
     */
    function initScheduleForm() {
        daysOfWeek.forEach(day => {
            const $dayDiv = $('<div class="day-schedule mb-4" id="schedule-' + day + '"></div>');
            const $heading = $('<h3 class="day-heading text-center">' + capitalizeFirstLetter(day) + '</h3>');
            const $timesContainer = $('<div class="times-container"></div>');
    
            const $buttonContainer = $('<div class="text-center mt-2"></div>');
            const $addRingButton = $('<button type="button" class="btn btn-primary add-ring-button">Add Ring Time</button>').data('day', day);
    
            $buttonContainer.append($addRingButton);
            $dayDiv.append($heading, $timesContainer, $buttonContainer); // Append the button container instead of the button directly
            $('#scheduleForm').append($dayDiv);
        });
    }
    
    // Add ring time input to the page
    $('body').on('click', '.add-ring-button', function() {
        const day = $(this).data('day');
        const $timesContainer = $(`#schedule-${day} .times-container`);
        const $timeInputContainer = $('<div class="form-group time-input-container d-flex justify-content-center"></div>');
        const $newTimeInput = $('<input type="time" class="form-control ring-time" required>');
        const $deleteButton = $('<button type="button" class="btn btn-danger btn-sm delete-time-button ml-2">Delete</button>');
    
        $timeInputContainer.append($newTimeInput, $deleteButton);
        $timesContainer.append($timeInputContainer);
    });
    
    
    // Delete ring time input from the page
    $('body').on('click', '.delete-time-button', function() {
        $(this).closest('.time-input-container').remove();
    });

    // Save schedule button event handler
    $('#saveScheduleBtn').click(function() {
        let scheduleData = {};

        daysOfWeek.forEach(day => {
            const times = [];
            $(`#schedule-${day} .ring-time`).each(function() {
                times.push($(this).val());
            });
            scheduleData[day] = times;
        });

        
        checkServerTokenMatch(function(tokenMatches) {
            if (!tokenMatches) {
                showLoginModal();
                return;
            }
        
            $.ajax({
                url: '/updateSchedule',
                type: 'POST',
                contentType: 'application/json',
                headers: { 'Authorization': getAuthToken() },
                data: JSON.stringify(scheduleData),
                success: function(response) {
                    globalScheduleData = scheduleData;
                    alert("Schedule updated successfully!");
                },
                error: function(xhr) {
                    if (xhr.status === 401 || xhr.status === 403) {
                        showLoginModal();
                    } else {
                        alert("Failed to update schedule.");
                    }
                }
            });
        });
    });

    // Export and import schedule buttons event handlers
    $('#exportScheduleBtn').click(function() {
        if (Object.keys(globalScheduleData).length === 0) {
            alert("No schedule data to export. Save the schedule first.");
            return;
        }
        const scheduleData = JSON.stringify(globalScheduleData, null, 2);
        const blob = new Blob([scheduleData], {type: "application/json"});
        const url = URL.createObjectURL(blob);
        const a = document.createElement('a');
        a.href = url;
        a.download = "schedule.json";
        document.body.appendChild(a);
        a.click();
        document.body.removeChild(a);
    });
    
    $('#importScheduleBtn').click(function() {
        $('#importScheduleFile').click();
    });
    
    // Import schedule file change event handler, reads the file and populates the schedule form
    // Clear existing schedules for all days before importing new schedule
    $('#importScheduleFile').change(function(e) {
        const file = e.target.files[0];
        if (!file) return;
        const reader = new FileReader();
        reader.onload = function(e) {
            // Clear existing schedules for all days before importing new schedule
            daysOfWeek.forEach(day => clearDaySchedule(day));
    
            const scheduleData = JSON.parse(e.target.result);
            populateScheduleForm(scheduleData);
        };
        reader.readAsText(file);
    });
    
    // Copy Monday schedule to other days button event handler
    $('#copyMondayScheduleBtn').click(function() {
        const mondayTimes = $('#schedule-monday .ring-time').map(function() { return $(this).val(); }).get();
        daysOfWeek.forEach(day => {
            if (day !== "monday") {
                clearDaySchedule(day); // Clear existing times
                mondayTimes.forEach(time => {
                    addRingTime(day, time); // Use your existing function to add times
                });
            }
        });
    });

    // Clear schedule button event handler
    $('#clearSchedule').click(function() {
        if (confirm('Are you sure you want to clear the entire schedule? This action cannot be undone.')) {
            // Clear each day's schedule
            daysOfWeek.forEach(day => clearDaySchedule(day));
        }
    });
    

    // Helper function to capitalize the first letter of a string
    function capitalizeFirstLetter(string) {
        return string.charAt(0).toUpperCase() + string.slice(1);
    }

    // Fetch the current schedule from the server and populate the form
    function fetchCurrentSchedule() {
        $.ajax({
            url: '/getSchedule',
            type: 'GET',
            dataType: 'json',
            success: function(scheduleData) {
                // Check if the response is not empty
                if (!scheduleData) {
                    console.error("No schedule:", "Empty response");
                    return;
                }
                if (Object.keys(scheduleData).length > 0) {
                    globalScheduleData = scheduleData;
                    populateScheduleForm(scheduleData);
                }
            },
            error: function(xhr, status, error) {
                console.error("Failed to fetch current schedule:", error);
            }
        });
    }
    
    function populateScheduleForm(scheduleData) {
        Object.keys(scheduleData).forEach(day => {
            const times = scheduleData[day];
            times.forEach(time => {
                addRingTime(day, time);
            });
        });
    }

    // Function to clear schedule for a specific day
    function clearDaySchedule(day) {
        $(`#schedule-${day} .times-container`).empty(); // Assuming you have a container for each day's times
    }

    function addRingTime(day, time = '') {
        const $timesContainer = $(`#schedule-${day} .times-container`);
        const $timeInputContainer = $('<div class="form-group time-input-container d-flex justify-content-center"></div>');
        const $newTimeInput = $(`<input type="time" class="form-control ring-time" required value="${time}">`);
        const $deleteButton = $('<button type="button" class="btn btn-danger btn-sm delete-time-button ml-2">Delete</button>');
    
        $timeInputContainer.append($newTimeInput, $deleteButton);
        $timesContainer.append($timeInputContainer);
    }

    init();
});