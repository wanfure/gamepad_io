# Vibration intensity, from 0.0 to 1.0. NaN and infinity are rejected.
float32 intensity

# Duration in milliseconds. Either field being zero stops vibration.
# A new valid command immediately replaces the current command.
uint32 duration_ms
