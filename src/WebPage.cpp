// WebPage.cpp
#include "WebPage.h"

//Constructor that sets the page title
WebPage::WebPage(const String& pageTitle) {
    title = pageTitle;
}

//This method generates a header for the page
String WebPage::header() {
    return "<!DOCTYPE html><html><head><title>" + title + "</title>" + css + "</head><body><h1 class='banner'>" + title + "</h1>";
}

//This method generates a navigation bar for the page
String WebPage::navigationBar() {
    return R"(
        <nav>
            <ul>
                <li><a href="/">Home</a></li>
                <li><a href="/settings">Settings</a></li>
                <li><a href="/about">About</a></li>
                <!-- Add more navigation items here -->
            </ul>
        </nav>
    )";
}

//This method generates a footer for the page
String WebPage::footer() {
    return "</body></html>";
}

//This method chooses the body content to be inserted into the page based on string parameter
String WebPage::body(const String& content) {
    String page = "";
    
    if (content == "home") {
        page = R"(
            <h1>Welcome to the ESP8266 Server!</h1>
            <p>Current Time: <span id="time">0:00</span></p>
            <h1>Toggle Relay</h1>
            <button id="relayButton">Toggle Relay</button>

            <script>
                document.getElementById('relayButton').addEventListener('click', function() {
                    var xhr = new XMLHttpRequest();
                    xhr.open("GET", "/toggleRelay", true);
                    xhr.onreadystatechange = function () {
                        if (xhr.readyState == 4 && xhr.status == 200) {
                            alert("Relay State: " + xhr.responseText);
                        }
                    };
                    xhr.send();
                });
            </script>
        )";
    }
    return page; // Return the page content
}

//This method generates the page by calling class methods and concatenating the results
String WebPage::generatePage() {
    // Example usage; your actual implementation might differ based on how you want to structure the page
    String pageContent = "This is dynamic content based on the page's purpose.";
    String completePage = header() + navigationBar() + body(pageContent) + footer();
    return completePage;
}
