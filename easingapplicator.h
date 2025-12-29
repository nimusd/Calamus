#ifndef EASINGAPPLICATOR_H
#define EASINGAPPLICATOR_H

#include <algorithm>
#include <cmath>

/**
 * EasingApplicator - Shapes parameter transitions using easing functions
 *
 * Prescribed rather than emergent movement. Applies mathematical easing
 * functions to create smooth, stylized transitions.
 *
 * Parameters:
 * - startValue: Beginning of transition
 * - endValue: End of transition
 * - progress: Position in transition (0-1)
 * - easingSelect: Which easing function to use
 *
 * Output:
 * - easedValue: Result after applying easing function
 *
 * Processing:
 * - t_eased = easingFunction(progress)
 * - output = startValue + (endValue - startValue) × t_eased
 *
 * Use cases:
 * - Pitch glide between notes
 * - Formant sweeps for consonants
 * - Brightness bloom on attack
 */
class EasingApplicator
{
public:
    enum class EasingType {
        Linear = 0,      // No easing, constant speed
        QuadIn,          // Accelerating from zero velocity
        QuadOut,         // Decelerating to zero velocity
        QuadInOut,       // Acceleration until halfway, then deceleration
        CubicIn,         // Cubic acceleration
        CubicOut,        // Cubic deceleration
        CubicInOut,      // Cubic acceleration/deceleration
        SineIn,          // Sine wave acceleration
        SineOut,         // Sine wave deceleration
        SineInOut,       // Sine wave acceleration/deceleration
        Elastic,         // Elastic bounce at end
        Bounce,          // Bouncing ball effect
        Back             // Overshoot and come back
    };

    enum class EasingMode {
        In,      // Ease in (slow start)
        Out,     // Ease out (slow end)
        InOut    // Ease in and out
    };

    EasingApplicator();

    // Apply easing to interpolate between start and end
    double process(double startValue, double endValue, double progress);

    // Parameter setters
    void setEasingType(EasingType type);
    void setEasingSelect(int index);  // Select by index
    void setEasingMode(EasingMode mode);

    // Parameter getters
    EasingType getEasingType() const { return easingType; }
    EasingMode getEasingMode() const { return easingMode; }

private:
    double applyEasing(double t) const;
    double easeLinear(double t) const;
    double easeQuad(double t) const;
    double easeCubic(double t) const;
    double easeSine(double t) const;
    double easeElastic(double t) const;
    double easeBounce(double t) const;
    double easeBack(double t) const;

    EasingType easingType;
    EasingMode easingMode;
};

#endif // EASINGAPPLICATOR_H
