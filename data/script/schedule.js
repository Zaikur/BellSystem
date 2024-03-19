/*
Quinton Nelson
03/15/2024
this file contains JQuery to dynamically create the webpage for the schedule form
*/


$(document).ready(function() {
    const daysOfWeek = ["monday", "tuesday", "wednesday", "thursday", "friday", "saturday", "sunday"];
    const authToken = localStorage.getItem('authToken');
    let globalScheduleData = {};

    function init() {
        initScheduleForm();
        fetchCurrentSchedule();
    }

    // Initialize the schedule form with empty input fields
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

        
        if (!authToken) {
            alert("You are not authenticated. Please login first.");
            return;
        }

        console.log(scheduleData);  /****************************************************** */
        
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
                    alert("Session expired, please logout and login again.")
                } else {
                    alert("Failed to update schedule.");
                }
            }
        });
    });

    // Export and import schedule buttons event handlers
    $('#exportScheduleBtn').click(function() {
        if (Object.keys(globalScheduleData).length === 0) {
            alert("No schedule data to export.");
            return;
        }
        const scheduleData = JSON.stringify(globalScheduleData(), null, 2);
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
    
    $('#importScheduleFile').change(function(e) {
        const file = e.target.files[0];
        if (!file) return;
        const reader = new FileReader();
        reader.onload = function(e) {
            const scheduleData = JSON.parse(e.target.result);
            console.log("Imported schedule:", scheduleData); /****************************************************** */
            populateScheduleForm(scheduleData);
        };
        reader.readAsText(file);
    });
    
    // Copy Monday schedule to other days button event handler
    $('#copyMondayScheduleBtn').click(function() {
        const mondayTimes = $('#schedule-monday .ring-time').map(function() { return $(this).val(); }).get();
        daysOfWeek.forEach(day => {
            if (day !== "monday") {
                const $timesContainer = $(`#schedule-${day} .times-container`);
                $timesContainer.empty(); // Clear existing times
                mondayTimes.forEach(time => {
                    addRingTime(day, time); // Use your existing function to add times
                });
            }
        });
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
            headers: { 'Authorization': authToken }, // Assuming authToken is globally available
            success: function(scheduleData) {
                if (Object.keys(scheduleData).length > 0) {
                    console.log("Current schedule:", scheduleData); /****************************************************** */
                    globalScheduleData = scheduleData;
                    populateScheduleForm(scheduleData);
                }
            },
            error: function(xhr, status, error) {
                if (xhr.status === 401 || xhr.status === 403) {
                    alert("Session expired, please login.")
                }
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