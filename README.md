# asbmemakeathon2021

## description:
This is the code behind the electronics of the AEROLYZER. This code allows for the calibration and reading of any unknown O2 source. In here is the state machine design implementation along with the algorithms to calculate an unknown O2 concentration. This must be initially calibrated to account for humidity and temperature readings that may differ around the world. After calibration, the user is allowed to input a new oxygen source and our device calculates the O2 concentration based on the time it takes for the ultrasonic sensor to recieve its signal.
