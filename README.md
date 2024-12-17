# CS350-Portfolio

# Project Reflection

## Summary of the Projects

Over the course of this class, I completed two main projects using the TI CC3220S board and Code Composer Studio: a Morse Code project and a Thermostat project. The Morse Code project involved programming the board’s LED to blink in predefined Morse code patterns, effectively translating stored strings into a visual code sequence. The Thermostat project utilized the board’s temperature sensor to monitor and display the current ambient temperature, allowing for adjustments to a set point. If the actual temperature fell below the set point, a red LED would activate, simulating a basic heating system indicator. Together, these projects solved the problem of interfacing low-level hardware features—such as LEDs and temperature sensors—with logic that reflects real-world applications.

## What I Did Well

I excelled at breaking down each project into manageable steps. For the Morse Code application, I clearly defined the mapping of characters to dots and dashes, ensuring the LED blink patterns were consistent and accurate. With the Thermostat project, I effectively integrated sensor data and user input logic. I maintained clear documentation and comments, which made it easy to understand how data flowed from the sensor to the decision-making logic that controlled the LED output.

## Where I Could Improve

While both projects functioned as intended, I see room for improvement in refining code efficiency. In the Morse Code project, I could have implemented more modular functions or classes for translating characters to Morse code, thereby reducing repetition. For the Thermostat project, I could improve the UI elements—perhaps by adding a clearer output display or introducing more nuanced states (such as indicating when the temperature is exactly at the set point). Additionally, enhancing error handling and incorporating more robust testing frameworks could ensure the code is more reliable and adaptable in the long term.

## Tools and Resources Added to My Support Network

Throughout these projects, I incorporated official TI documentation, forums, and example code from the TI Resource Explorer. I also found community-driven Q&A platforms, like Stack Overflow, helpful for troubleshooting. As I move forward, these resources, combined with version control best practices (GitHub), will continue to form a strong support network. I’ve also begun subscribing to embedded systems newsletters and following related GitHub repositories, which will keep me updated on new tools and techniques.

## Transferable Skills

From these projects, I gained stronger embedded programming skills, including working directly with hardware peripherals and understanding the nuances of timing and resource constraints. The problem-solving techniques—such as decomposing complex tasks into smaller, testable components—are highly transferable to future coursework and real-world projects. I’ve also sharpened my debugging strategies, using tools like breakpoints, watch variables, and incremental testing to ensure code correctness and stability.

## Making the Projects Maintainable, Readable, and Adaptable

To ensure long-term maintainability, I focused on clear naming conventions, consistent code formatting, and detailed inline comments. Where possible, I segmented code into smaller functions, each responsible for a specific piece of functionality, making it easier to locate and fix issues. For adaptability, I used configurable constants and parameters, allowing for quick adjustments to LED blink rates, temperature thresholds, or Morse code mappings. This approach not only streamlines modifications but also positions the codebase to be extended to new features or migrated to different hardware platforms in the future.
