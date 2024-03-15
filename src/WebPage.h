/*
Quinton Nelson
3/13/2024
This header file contains function declarations, and private variables used in the WebPages.cpp file to generate HTML pages.
*/
#ifndef WebPage_h
#define WebPage_h

#include <Arduino.h>

class WebPage {
public:
        WebPage(const String& title) : pageType(title) {}; // Constructor that sets the page title
        String generatePage(); // Generates the complete HTML page
    private:
        String title;
        String header();
        String navigationBar();
        String footer();
        String body(); // A method to append specific body content
        String pageType;

    //CSS styles to be applied to each page
    String css = R"(
        <style>
            /* Reset some basic elements */
            body, h1, h2, h3, p, form, input, button { margin: 0; padding: 0; box-sizing: border-box; }

            /* Basic styling */
            body {
                font-family: Arial, sans-serif;
                line-height: 1.6;
                padding: 20px;
                background-color: #f4f4f4;
                margin: 0 auto;
            }

            /* Section styles */
            .section {
                background-color: #ffffff;
                padding: 20px;
                margin-bottom: 20px;
                border-radius: 5px;
                box-shadow: 0 2px 4px rgba(0,0,0,0.1);
}

            /* Banner styles */
            .banner {
                background-color: #007bff;
                color: white;
                text-align: center;
                padding: 20px;
            }

            /* Form styles */
            form {
                margin: 20px 0;
            }

            form .form-control {
                margin-bottom: 20px;
                padding: 10px;
                font-size: 16px;
                width: 100%;
            }

            form .form-control:focus {
                border-color: #0056b3;
                box-shadow: 0 0 8px rgba(0, 86, 179, 0.1);
            }

            .btn:focus,
            .btn:active {
                outline: none;
                box-shadow: 0 0 8px rgba(0, 86, 179, 0.6);
            }

            /* Error styles */
            .error {
                color: #ff0000;
                margin-bottom: 15px;
                font-size: 14px;
            }


            /* Button styles */
            .btn {
                display: inline-block;
                background-color: #007bff;
                color: white;
                padding: 10px 20px;
                cursor: pointer;
                font-size: 16px;
                border: none;
                border-radius: 5px;
            }

            .btn:hover {
                background-color: #0056b3;
            }

            /* Navigation bar styles */
            ul, li { 
                    list-style: none; 
                }
                
                nav { 
                    display: flex; 
                    justify-content: center; 
                    background: #333; 
                }
                
                nav a { 
                    color: white; 
                    padding: 15px 20px; 
                    display: block; 
                    text-decoration: none; 
                }
                
                nav ul { display: flex; }

                nav li { margin: 0; }
                
                nav a:hover { background-color: #555; 
            }

            /* Ensure the page is responsive */
            @media (max-width: 768px) {
                body {
                    padding: 10px;
                }

                .banner {
                    padding: 10px;
                }

                nav ul { flex-direction: column; }

                nav a { 
                    text-align: center; 
                    padding: 10px; 
                    border-top: 1px solid #666;
                }
            }
        </style>
        )";

};

#endif