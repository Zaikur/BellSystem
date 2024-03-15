// WebPage.cpp
#include "WebPage.h"

//This method generates a header for the page
String WebPage::header() {
    return "<!DOCTYPE html><html><head><title>" + pageType + "</title>" + css + "</head><body><h1 class='banner'>" + pageType + "</h1>";
}

//This method generates a navigation bar for the page
String WebPage::navigationBar() {
    return R"(
        <nav>
            <ul>
                <li><a href="/">Home</a></li>
                <li><a href="/schedule">Schedule</a></li>
                <li><a href="/settings">Settings</a></li>
                <li><a href="/about">About</a></li>
            </ul>
        </nav>
    )";
}

//This method generates a footer for the page
String WebPage::footer() {
    return "</body></html>";
}

//This method chooses the body content to be inserted into the page based on string parameter
//Test button does AJAX request to /ToggleRelay
String WebPage::body() {
    String page = "";
    
    if (pageType.equalsIgnoreCase("Home")) {
        Serial.println("Home page body created");
        page = R"(
            <h1>Welcome to [deviceName] ESP8266 Server!</h1>    //Replace [deviceName] with actual device name
            <p>Current Time: <span id="time">0:00</span></p>

            <!-- Ring Test Button -->
            <div>
                <h2>Test Ring Functionality</h2>
                <button id="testRingButton">Test Ring</button>
            </div>

            <!-- Countdown to Next Scheduled Ring -->
            <div>
                <h2>Next Scheduled Ring</h2>
                <p>Countdown: <span id="countdown">Loading...</span></p>
            </div>

            <!-- Error Display -->
            <div id="errorSection">
                <h2>System Messages</h2>
                <p>No errors currently reported.</p>
            </div>

            <script>
                // Ring Test Button functionality
                document.getElementById('testRingButton').addEventListener('click', function() {
                    var xhr = new XMLHttpRequest();
                    xhr.open("GET", "/TestRing", true); // Ensure you have a corresponding handler in your server code
                    xhr.onreadystatechange = function () {
                        if (xhr.readyState == 4 && xhr.status == 200) {
                            alert("Ring test initiated. Check device.");
                        }
                    };
                    xhr.send();
                });

                // Countdown and error display logic will be implemented here

                // Placeholder for updating current time, you can expand this with actual time sync logic
                function updateTime() {
                    var now = new Date();
                    document.getElementById('time').textContent = now.getHours() + ":" + now.getMinutes() + ":" + now.getSeconds();
                }
                setInterval(updateTime, 1000); // Update time every second

                // Placeholder for countdown logic
                // Placeholder for dynamically updating error messages
            </script>
        )";
    } 
    else if (pageType.equalsIgnoreCase("Settings")) { 
        Serial.println("Settings page body created");
        page = R"(
                <h1>Settings</h1>
                <form id="settingsForm" action="/SaveSettings" method="post">
                    <div>
                        <label for="deviceName">Device Name:</label>
                        <input type="text" id="deviceName" class="form-control" name="deviceName" placeholder="Device Name" required>
                        <small class="error" id="deviceNameError"></small>
                    </div>
                    <div>
                        <label for="ringDuration">Ring Duration (seconds):</label>
                        <input type="number" id="ringDuration" class="form-control" name="ringDuration" placeholder='Ring Duration (seconds)' required>
                        <small class="error" id="ringDurationError"></small>
                    </div>
                    <div>
                        <label for="settingsPassword">Settings Password:</label>
                        <input type="password" id="settingsPassword" class="form-control" name="settingsPassword" placeholder="Password" required>
                    </div>
                    <button type="submit" class="btn">Save</button>
                </form>

                <script>
                    document.getElementById('settingsForm').addEventListener('submit', function(event) {
                        // Prevent form submission for manual validation
                        event.preventDefault();

                        // Clear previous error messages
                        document.getElementById('deviceNameError').textContent = '';
                        document.getElementById('ringDurationError').textContent = '';

                        var deviceName = document.getElementById('deviceName').value;
                        var ringDuration = document.getElementById('ringDuration').value;

                        // Validate Device Name (alphanumeric and hyphens/underscores only)
                        if(!/^[a-zA-Z0-9-_]+$/.test(deviceName)) {
                            document.getElementById('deviceNameError').textContent = 'Invalid characters in device name.';
                            return; // Stop submission
                        }

                        // Validate Ring Duration (must be an integer greater than 0)
                        if(!(ringDuration > 0 && Math.floor(ringDuration) == ringDuration)) {
                            document.getElementById('ringDurationError').textContent = 'Ring duration must be a positive integer.';
                            return; // Stop submission
                        }

                        // Submit the form if validation passes
                        this.submit();
                    });
                </script>
        )";
    } 
    else if (pageType.equalsIgnoreCase("Change Password")) {
        Serial.println("Change Password page body created");
        page = R"(
                <h1>Change Password</h1>
                <form id="changePasswordForm" action="/ChangePassword" method="post">
                    <div>
                        <input type="password" id="oldPassword" class="form-control" name="oldPassword" placeholder="Old Password" required>
                    </div>
                    <div>
                        <input type="password" id="newPassword" class="form-control" name="newPassword" placeholder="New Password" required>
                        <small class="error" id="passwordStrengthError"></small>
                    </div>
                    <div>
                        <input type="password" id="confirmPassword" class="form-control" name="confirmPassword" placeholder="Confirm New Password" required>
                        <small class="error" id="passwordMatchError"></small>
                    </div>
                    <button type="submit" class="btn">Save</button>
                </form>

                <script>
                    document.getElementById('changePasswordForm').addEventListener('submit', function(event) {
                        // Prevent form submission for manual validation
                        event.preventDefault();

                        // Clear previous error messages
                        document.getElementById('passwordStrengthError').textContent = '';
                        document.getElementById('passwordMatchError').textContent = '';

                        var newPassword = document.getElementById('newPassword').value;
                        var confirmPassword = document.getElementById('confirmPassword').value;

                        // Check if newPassword meets strength criteria
                        if (newPassword.length < 8) {
                            document.getElementById('passwordStrengthError').textContent = 'Password must be at least 8 characters.';
                            return; // Stop submission
                        }

                        // Check if newPassword and confirmPassword match
                        if (newPassword !== confirmPassword) {
                            document.getElementById('passwordMatchError').textContent = 'Passwords do not match.';
                            return; // Stop submission
                        }

                        // Submit the form if validation passes
                        this.submit();
                    });
                </script>
        )";
    }
    else if (pageType.equalsIgnoreCase("Schedule")) { //Make Schedule page
        Serial.println("Schedule page body created");
        page = R"(
                <h1>Ring Schedule</h1>
                <form id="scheduleForm" action="/SaveSchedule" method="post">
                    <!-- Placeholder for dynamically generated schedule form -->
                </form>
                <button type="button" onclick='submitSchedule()'>Save Schedule</button>

                <script>
                    const daysOfWeek = ["Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"];

                    function initScheduleForm() {
                        let formContent = '';
                        daysOfWeek.forEach((day, index) => {
                            formContent += `
                                <div class="day-schedule" id="schedule-${day}">
                                    <h3>${day}</h3>
                                    <div>
                                        <input type="time" name="${day.toLowerCase()}RingTime[]" class="ring-time" required>
                                        <button type="button" onclick='addRingTime('${day.toLowerCase()}')'>Add Ring</button>
                                    </div>
                                    ${index > 0 ? `<div><input type="checkbox" onchange='copySchedule('${day.toLowerCase()}', '${daysOfWeek[index-1].toLowerCase()}')'> Use ${daysOfWeek[index-1]}'s schedule</div>` : ''}
                                </div>
                            `;
                        });
                        document.getElementById('scheduleForm').innerHTML = formContent;
                    }

                    function addRingTime(day) {
                        const scheduleDiv = document.getElementById(`schedule-${day}`);
                        const input = document.createElement("input");
                        input.type = "time";
                        input.name = `${day}RingTime[]`;
                        input.className = "ring-time";
                        input.required = true;
                        scheduleDiv.appendChild(input);
                    }

                    function copySchedule(day, previousDay) {
                        const currentDayInputs = document.querySelectorAll(`input[name="${day}RingTime[]"]`);
                        const previousDayInputs = document.querySelectorAll(`input[name="${previousDay}RingTime[]"]`);

                        // Remove current day's times
                        currentDayInputs.forEach(input => input.remove());

                        // Copy previous day's times
                        previousDayInputs.forEach(input => addRingTime(day, input.value));
                    }

                    function submitSchedule() {
                        // Here you would implement validation to ensure no two ring times are the same within a day
                        // Then submit the form
                        document.getElementById('scheduleForm').submit();
                    }

                    // Initialize the schedule form on page load
                    window.onload = initScheduleForm;
                </script>
        )";
    } 
    else if (pageType.equalsIgnoreCase("About")) { //Make About page
        Serial.println("About page body created");
        page = R"(
            <h1>About</h1>
            <p>ESP8266 Server</p>
        )";
    } 
    else if (pageType.equalsIgnoreCase("SettingsSaved")) { //Make SettingsSaved page
        Serial.println("Saved settings page body created");
        page = "<h1>Your settings have been saved</h1>";
    }
    else if (pageType.equalsIgnoreCase("ScheduleSaved")) {  //Make ScheduleSaved page
        Serial.println("Saved schedule page body created");
        page = "<h1>Your schedule has been saved</h1>";
    }
    else if (pageType.equalsIgnoreCase("ChangePasswordSaved")) {  //Make ChangePasswordSaved page
        Serial.println("Saved password page body created");
        page = "<h1>Your password has been changed</h1>";
    }
    else if (pageType.equalsIgnoreCase("Error")) {  //Make Error page
        Serial.println("Error page body created");
        page = "<h1>Oops! Something went wrong</h1>";
    }
    else {
        Serial.println("Invalid page type");
        page = "<h1>Invalid page type</h1>";
    }
    return page; // Return the page content
}

//This method generates the page by calling class methods and concatenating the results
String WebPage::generatePage() {
    // Example usage; your actual implementation might differ based on how you want to structure the page
    String pageContent = "This is dynamic content based on the page's purpose.";
    String completePage = header() + navigationBar() + body() + footer();
    return completePage;
}
