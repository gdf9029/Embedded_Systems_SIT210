import speech_recognition as sr
import RPi.GPIO as GPIO
import time


LED_PIN = 18  # The GPIO pin the red LED is connected to.


def setup_gpio(): #This function will prepare the GPIO pin 18 (pin 12) as an output pin and set it to LOW (off). 
    GPIO.setwarnings(False)
    GPIO.setmode(GPIO.BCM)      # Use Broadcom pin-numbering scheme
    GPIO.setup(LED_PIN, GPIO.OUT) # Set LED pin as an output
    GPIO.output(LED_PIN, GPIO.LOW) # Turn LED off initially

def process_command(command):
    """Processes the voice command to control the LED."""
    print(f"Command received: '{command}'")

    # Use 'in' for flexible matching
    if "turn on the light" in command or "light on" in command:
        print("💡 Turning the light ON")
        GPIO.output(LED_PIN, GPIO.HIGH)
    elif "turn off the light" in command or "light off" in command:
        print("🌑 Turning the light OFF")
        GPIO.output(LED_PIN, GPIO.LOW)
    else:
        print("❓ Command not recognized.")

def main():
    # sr.Microphone is the microphone class from the speech_recognition library which makes the script open the microhppone and listen for audio input.
    setup_gpio()
    recognizer = sr.Recognizer()

    print("🤖 Voice control script is running. Say 'exit' to stop.")

    while True:
        try:
            # Use the microphone as the audio source
            with sr.Microphone() as source:
                print("\n🎤 Listening for a command...")
                recognizer.adjust_for_ambient_noise(source) # Adjust for noise which will distinguish my voice from background noise.
                audio = recognizer.listen(source)

            # Recognize speech using Google's Web Speech API which is a free Web Speech API which then converst the listened audio to text. Note that, this process will require an active internet connection. Also, a lower case conversion is done to make the command matching easier.
            command = recognizer.recognize_google(audio).lower()

            # Check for exit command
            if "exit" in command or "stop" in command:
                print("Exiting program. Goodbye!")
                break
            
            # Process the recognized command
            process_command(command)

        except sr.UnknownValueError:
            # This error means the API couldn't understand the audio
            print("Sorry, I did not understand that. Please try again.")
        except sr.RequestError as e:
            # This error is for issues with the API request (e.g., no internet)
            print(f"Could not request results from Google Speech Recognition service; {e}")
        except KeyboardInterrupt:
            # Allows you to stop the script with Ctrl+C
            print("\nProgram interrupted by user.")
            break

    # Clean up GPIO settings before exiting
    GPIO.cleanup()

if __name__ == "__main__":
    main()