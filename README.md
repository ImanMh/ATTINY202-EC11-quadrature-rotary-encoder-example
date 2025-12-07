# ATTiny202 Rotary Encoder with LED Feedback

A robust rotary encoder implementation for ATTiny202 microcontrollers using timer-based polling with visual LED feedback for rotation direction.

## Features

- **Timer-based encoder polling** - Uses high-frequency timer interrupts (10kHz) for reliable encoder reading
- **Active-low encoder support** - Configured for encoders with active-low outputs
- **LED feedback** - Visual indication of clockwise and counter-clockwise rotation
- **Robust debouncing** - Software debouncing through high-frequency sampling
- **No missed ticks** - Timer interrupts run independently of main loop
- **Configurable sampling rate** - Adjustable interrupt frequency via `INTERRUPT_PERIOD`

## Hardware Requirements

- ATTiny202 microcontroller (or compatible: ATTiny402, ATTiny204, ATTiny404)
- Rotary encoder (active-low configuration)
- 2 LEDs for rotation feedback
- Pull-up resistors (internal pull-ups are used)

## Pin Connections

| Component | Pin | Description |
|-----------|-----|-------------|
| Encoder Channel A | 1 | Encoder quadrature output A |
| Encoder Channel B | 2 | Encoder quadrature output B |
| LED (Clockwise) | 4 | Lights up on clockwise rotation |
| LED (Counter-clockwise) | 0 | Lights up on counter-clockwise rotation |

## Library Credit

This implementation is based on the excellent **John-Lluch Encoder library** which uses a timer-based polling approach instead of traditional pin-change interrupts. This method provides superior reliability and eliminates issues with contact bounce and missed encoder ticks.

**Original Library:** [John-Lluch/Encoder](https://github.com/John-Lluch/Encoder)  
**Author:** John Lluch  
**License:** Please refer to the original repository for license information

The library's timer-based approach solves common encoder problems:
- Contact bounce issues with pin-change interrupts
- Missed ticks when main loop is slow
- Need for hardware debouncing components

## How It Works

### Timer-Based Polling

Instead of using pin-change interrupts, this implementation uses a hardware timer (TCB0) that fires at 10kHz (every 0.1ms). The timer interrupt service routine (ISR) continuously polls the encoder state, providing:

1. **Natural debouncing** - High-frequency sampling filters out contact bounce
2. **No missed ticks** - Timer runs independently of main loop code
3. **Simple API** - Just call `encoder.delta()` in your loop

### Encoder State Machine

The encoder uses a quadrature decoder that tracks valid Gray code transitions:
- Valid transitions: `0→2→3→1→0` (clockwise) or `0→1→3→2→0` (counter-clockwise)
- Invalid transitions (both pins changing) are ignored
- Direction is determined by the transition sequence

### Active-Low Configuration

The encoder is configured for active-low operation:
- Pins are pulled HIGH when encoder is inactive (via `INPUT_PULLUP`)
- Pins go LOW when encoder contacts close
- Readings are inverted: `!digitalRead()` converts LOW=active to standard state representation

## Configuration

### Adjusting Sampling Frequency

The timer interrupt frequency can be adjusted by modifying `INTERRUPT_PERIOD` in `ATTINY202-encoder.ino`:

```cpp
#define INTERRUPT_PERIOD  25   // 10kHz (0.1ms) - Default
```

**Frequency Reference Table:**

| INTERRUPT_PERIOD | Frequency | Period | Use Case |
|-----------------|-----------|--------|----------|
| 12 | 20kHz | 0.05ms | Very fast encoders, minimal debounce |
| 25 | 10kHz | 0.1ms | **Default** - Good balance |
| 50 | 5kHz | 0.2ms | Slower, more debouncing |
| 100 | 2.5kHz | 0.4ms | Very slow, heavy debouncing |

### LED Blink Duration

Adjust the LED blink duration by modifying:

```cpp
#define LED_BLINK_DURATION_MS  100  // LED blink duration in milliseconds
```

## Usage

1. **Connect hardware** according to the pin connections table above
2. **Upload the sketch** to your ATTiny202
3. **Rotate the encoder** - LEDs will light up indicating rotation direction:
   - Pin 4 (LED_PIN_CW): Clockwise rotation
   - Pin 0 (LED_PIN_CCW): Counter-clockwise rotation

### Reading Encoder Movement

In your code, read encoder movement using:

```cpp
int delta = encoder.delta();  // Returns accumulated change since last call
// Positive = clockwise, Negative = counter-clockwise, 0 = no movement
```

## Technical Details

### Timer Configuration

- **Timer Used:** TCB0 (TCA0 is used by `millis()`)
- **CPU Clock:** 20MHz
- **Prescaler:** CLK_PER/2 (10MHz timer clock)
- **Default Period:** 1000 counts = 10kHz interrupt rate
- **Calculation:** 10,000,000 Hz / 10,000 Hz = 1000 counts

### Debouncing

- **Debounce Count:** 3 consecutive identical reads required
- **Debounce Time:** ~0.3ms at 10kHz sampling rate
- **Method:** Software debouncing through state validation

## File Structure

```
ATTINY202-encoder/
├── ATTINY202-encoder.ino  # Main sketch
├── Encoder.h              # Encoder library header
├── Encoder.cpp            # Encoder library implementation
└── README.md              # This file
```

## Compilation

This project is designed for use with:
- **Board:** ATTiny202 (megaTinyCore)
- **Core:** megaTinyCore 2.6.12 or compatible
- **CPU Frequency:** 20MHz

## Troubleshooting

### LEDs don't light up
- Check LED connections and polarity
- Verify encoder is properly connected
- Ensure pull-ups are enabled (code uses `INPUT_PULLUP`)

### Encoder not detected
- Verify encoder is active-low configuration
- Check pin connections (CH_A = pin 1, CH_B = pin 2)
- Try adjusting `INTERRUPT_PERIOD` if encoder is very fast/slow

### Timer conflicts
- TCB0 is used for encoder (TCA0 is used by `millis()`)
- If you need to use TCB0 elsewhere, modify `Encoder.cpp` to use a different timer

## License

This project is based on the John-Lluch Encoder library. Please refer to the original repository for license information and ensure compliance with the original library's terms.

## Author

**Iman Mohammadi**  
Email: iman.mohamadi.dev@gmail.com

## Acknowledgments

- **John Lluch** - Original Encoder library author ([GitHub](https://github.com/John-Lluch/Encoder))
- The timer-based polling approach that makes this implementation robust and reliable

