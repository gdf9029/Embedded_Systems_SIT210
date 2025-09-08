#
# Task 5.2C: Raspberry Pi GUI for LED Intensity Control using PWM
# This script creates a Tkinter GUI with three sliders to control the
# brightness of Red, Green, and Yellow LEDs.
#

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


for pin in led_pins.values():
    GPIO.setup(pin, GPIO.OUT)


# Create PWM instances for each LED pin with a frequency of 100 Hz
pwm_red = GPIO.PWM(led_pins["red"], 100)
pwm_green = GPIO.PWM(led_pins["green"], 100)
pwm_blue = GPIO.PWM(led_pins["yellow"], 100)

# Start PWM with an initial duty cycle of 0 (LEDs off)
pwm_red.start(0)
pwm_green.start(0)
pwm_blue.start(0)


win = tk.Tk()
win.title("PWM LED Intensity Controller")

# Update Functions
def update_red(duty_cycle):
    """Updates the duty cycle for the Red LED."""
    pwm_red.ChangeDutyCycle(float(duty_cycle))

def update_green(duty_cycle):
    """Updates the duty cycle for the Green LED."""
    pwm_green.ChangeDutyCycle(float(duty_cycle))

def update_yellow(duty_cycle):
    """Updates the duty cycle for the Yellow LED."""
    pwm_blue.ChangeDutyCycle(float(duty_cycle))

def close_app():
    """Stops PWM, cleans up GPIO, and closes the application."""
    pwm_red.stop()
    pwm_green.stop()
    pwm_blue.stop()
    GPIO.cleanup()
    win.destroy()


# Create a frame for the sliders for better organization
slider_frame = tk.Frame(win)
slider_frame.pack(pady=10, padx=20)

# Red LED Slider
tk.Label(slider_frame, text="Red Intensity").pack()
red_slider = tk.Scale(slider_frame, from_=0, to=100, orient=tk.HORIZONTAL, command=update_red)
red_slider.pack()

# Green LED Slider
tk.Label(slider_frame, text="Green Intensity").pack()
green_slider = tk.Scale(slider_frame, from_=0, to=100, orient=tk.HORIZONTAL, command=update_green)
green_slider.pack()

# Yellow LED Slider
tk.Label(slider_frame, text="Yellow Intensity").pack()
blue_slider = tk.Scale(slider_frame, from_=0, to=100, orient=tk.HORIZONTAL, command=update_yellow)
blue_slider.pack()

# Exit Button
tk.Button(win, text="Exit", command=close_app, bg="red", height=2, width=15).pack(pady=10)


# Set the protocol for window closing
win.protocol("WM_DELETE_WINDOW", close_app)
# Start the GUI event loop
win.mainloop()
