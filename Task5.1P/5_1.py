# Importing libraries
import tkinter as tk
import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

# Define GPIO pins for each LED
led_pins = {
    "red": 17,
    "green": 27,
    "yellow": 22
}

# Set up each pin as an output
GPIO.setup(led_pins["red"], GPIO.OUT)
GPIO.setup(led_pins["green"], GPIO.OUT)
GPIO.setup(led_pins["yellow"], GPIO.OUT)

# --- GUI DEFINITIONS ---
win = tk.Tk()
win.title("LED Controller")

# Variable to hold the radio button state
led_choice = tk.IntVar()
led_choice.set(0) # Initialize with no selection

# --- FUNCTIONS ---
def led_control():
    """Controls the LEDs based on the radio button selection."""
    choice = led_choice.get()
    
    # Turn all LEDs off first
    GPIO.output(led_pins["red"], GPIO.LOW)
    GPIO.output(led_pins["green"], GPIO.LOW)
    GPIO.output(led_pins["yellow"], GPIO.LOW)
    
    # Turn on the selected LED
    if choice == 1: # Red
        GPIO.output(led_pins["red"], GPIO.HIGH)
    elif choice == 2: # Green
        GPIO.output(led_pins["green"], GPIO.HIGH)
    elif choice == 3: # Yellow
        GPIO.output(led_pins["yellow"], GPIO.HIGH)

def close_app():
    """Cleans up GPIO and closes the application."""
    GPIO.cleanup()
    win.destroy()

# Buttons
# Radio Buttons for LED selection
tk.Radiobutton(win, text="Red LED", variable=led_choice, value=1, command=led_control).pack(anchor=tk.W)
tk.Radiobutton(win, text="Green LED", variable=led_choice, value=2, command=led_control).pack(anchor=tk.W)
tk.Radiobutton(win, text="Yellow LED", variable=led_choice, value=3, command=led_control).pack(anchor=tk.W)

# Exit Button
tk.Button(win, text="Exit", command=close_app, bg="red", height=1, width=10).pack()



# Set the protocol for window closing
win.protocol("WM_DELETE_WINDOW", close_app)
# Start the GUI event loop
win.mainloop()

